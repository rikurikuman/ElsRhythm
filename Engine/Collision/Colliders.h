#pragma once
#include "ICollider.h"
#include <mutex>
#include <map>
#include <memory>
#include <list>
#include <Windows.h>

template <typename ColliderType>
class Collider;

// �e��R���C�_�[�ꊇ�Ǘ��N���X

class Colliders final {
	template <typename ColliderType>
	friend class Collider;
public:
	template <typename ColliderType, typename ...Args>
	static Collider<ColliderType> Create(Args ...args) {
		std::lock_guard<std::recursive_mutex> lock(instance.mutex);

		instance.colList.emplace_back(std::make_unique<ColliderType>(args...));
		return Collider<ColliderType>(static_cast<ColliderType*>(instance.colList.back().get()));
	}

	static void Update();

	//�S�R���C�_�[���A�N�e�B�u�ɂ���
	static void AllActivate();
	//�S�R���C�_�[���A�N�e�B�u�ɂ���
	static void AllInactivate();

	//Colliders�N���X���Ǘ�����e�R���C�_�[�̃��X�g�ւ̃A�N�Z�X�p�N���X
	//�O������͂��̃N���X����Ă̂݃A�N�Z�X�ł���
	class ColListAccessor {
		friend class Colliders;
	public:
		std::unique_lock<std::recursive_mutex> lock;
		std::list<std::unique_ptr<ICollider>>* list;

		ColListAccessor() : lock(instance.mutex), list(&instance.colList) {}
	};

private:
	static Colliders instance;

	Colliders() {};
	~Colliders() {}
	Colliders(const Colliders& a) = delete;
	Colliders& operator=(const Colliders&) = delete;
	Colliders(const Colliders&& a) = delete;
	Colliders& operator=(const Colliders&&) = delete;

	std::recursive_mutex mutex;
	std::list<std::unique_ptr<ICollider>> colList;
};

/*
	Colliders���Ǘ�����e��R���C�_�[���̂��Q�Ƃ���A
	�X�}�[�g�|�C���^���ǂ��ȃN���X�B
	"Collider<SphereCollider>"�݂����Ȋ����Ŏg���B
	�����J�E���g�������A���̃N���X�̐�����+1, �j����-1���āA
	0�ɂȂ�����R���C�_�[���̂��j������B
	(Colliders�C���X�^���X�������̒���Collider�C���X�^���X������Ɗ�Ȃ��͓̂���)
	(�ꉞ���v�Ȃ͂�������)
*/

template <typename ColliderType>
class Collider final {
	friend class Colliders;
public:
	//�R���X�g���N�^
	Collider() {}

private: //Colliders����Ȃ��ƐV�K�����͂ł��Ȃ��悤��(�R�s�[�Ƃ��͂�����)
	ColliderType* data = nullptr;

	//�R���X�g���N�^
	Collider(ColliderType* data) : data(data) {
		AddCount(data);
	}

public:
	//�f�X�g���N�^
	~Collider() {
		if (data) {
			SubtractCount(data);
			if (GetCount(data) == 0) {
				//�S�Q�Ə��ŁA�폜���鏈����������
				Colliders::instance.colList.remove_if(
					[&](std::unique_ptr<ICollider>& o) {
						return o.get() == data;
					});
			}
		}
	}
	//�R�s�[�R���X�g���N�^
	Collider(const Collider& o) {
		if (this != &o) {
			if (data) {
				SubtractCount(data);
				if (GetCount(data) == 0) {
					//�S�Q�Ə��ŁA�폜���鏈����������
					Colliders::instance.colList.remove_if(
						[&](std::unique_ptr<ICollider>& o) {
							return o.get() == data;
						});
				}
			}
		}
		data = o.data;
		AddCount(data);
	}
	//�R�s�[������Z�q
	Collider& operator=(const Collider& o) {
		if (this != &o) {
			if (data) {
				SubtractCount(data);
				if (GetCount(data) == 0) {
					//�S�Q�Ə��ŁA�폜���鏈����������
					Colliders::instance.colList.remove_if(
						[&](std::unique_ptr<ICollider>& o) {
							return o.get() == data;
						});
				}
			}
			data = o.data;
			AddCount(data);
		}
		return *this;
	}
	//���[�u�R���X�g���N�^
	Collider(Collider&& o) noexcept {
		if (this != &o) {
			if (data) {
				SubtractCount(data);
				if (GetCount(data) == 0) {
					//�S�Q�Ə��ŁA�폜���鏈����������
					Colliders::instance.colList.remove_if(
						[&](std::unique_ptr<ICollider>& o) {
							return o.get() == data;
						});
				}
			}
			data = o.data;
			o.data = nullptr;
		}
	}
	//���[�u������Z�q
	Collider& operator=(Collider&& o) noexcept {
		if (this != &o) {
			if (data) {
				SubtractCount(data);
				if (GetCount(data) == 0) {
					//�S�Q�Ə��ŁA�폜���鏈����������
					Colliders::instance.colList.remove_if(
						[&](std::unique_ptr<ICollider>& o) {
							return o.get() == data;
						});
				}
			}
			data = o.data;
			o.data = nullptr;
		}
		return *this;
	}

	operator bool() const {
		return data != nullptr;
	}

	ColliderType* operator->() {
		return data;
	}

private:
	static std::recursive_mutex mutex;
	static std::map<void*, size_t> countMap;

	static void AddCount(void* p) {
		std::lock_guard<std::recursive_mutex> lock(mutex);
		countMap[p]++;
	}

	static void SubtractCount(void* p) {
		std::lock_guard<std::recursive_mutex> lock(mutex);
		countMap[p]--;

		if (countMap[p] == 0) {
			countMap.erase(p);
		}
	}

	static size_t GetCount(void* p) {
		std::lock_guard<std::recursive_mutex> lock(mutex);
		auto itr = countMap.find(p);
		if (itr == countMap.end()) {
			return 0;
		}
		return countMap[p];
	}
};

template<typename ColliderType>
std::map<void*, size_t> Collider<ColliderType>::countMap = std::map<void*, size_t>();

template<typename ColliderType>
std::recursive_mutex Collider<ColliderType>::mutex;