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
		std::lock_guard<std::recursive_mutex> lock(sInstance.mMutex);

		sInstance.mColList.emplace_back(std::make_unique<ColliderType>(args...));
		return Collider<ColliderType>(static_cast<ColliderType*>(sInstance.mColList.back().get()));
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
		std::unique_lock<std::recursive_mutex> mLock;
		std::list<std::unique_ptr<ICollider>>* mList;

		ColListAccessor() : mLock(sInstance.mMutex), mList(&sInstance.mColList) {}
	};

private:
	static Colliders sInstance;

	Colliders() {};
	~Colliders() {}
	Colliders(const Colliders& a) = delete;
	Colliders& operator=(const Colliders&) = delete;
	Colliders(const Colliders&& a) = delete;
	Colliders& operator=(const Colliders&&) = delete;

	std::recursive_mutex mMutex;
	std::list<std::unique_ptr<ICollider>> mColList;
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
	ColliderType* mData = nullptr;

	//�R���X�g���N�^
	Collider(ColliderType* data) : mData(data) {
		AddCount(data);
	}

public:
	//�f�X�g���N�^
	~Collider() {
		if (mData) {
			SubtractCount(mData);
			if (GetCount(mData) == 0) {
				//�S�Q�Ə��ŁA�폜���鏈����������
				Colliders::sInstance.mColList.remove_if(
					[&](std::unique_ptr<ICollider>& o) {
						return o.get() == mData;
					});
			}
		}
	}
	//�R�s�[�R���X�g���N�^
	Collider(const Collider& o) {
		if (this != &o) {
			if (mData) {
				SubtractCount(mData);
				if (GetCount(mData) == 0) {
					//�S�Q�Ə��ŁA�폜���鏈����������
					Colliders::sInstance.mColList.remove_if(
						[&](std::unique_ptr<ICollider>& o) {
							return o.get() == mData;
						});
				}
			}
		}
		mData = o.mData;
		AddCount(mData);
	}
	//�R�s�[������Z�q
	Collider& operator=(const Collider& o) {
		if (this != &o) {
			if (mData) {
				SubtractCount(mData);
				if (GetCount(mData) == 0) {
					//�S�Q�Ə��ŁA�폜���鏈����������
					Colliders::sInstance.mColList.remove_if(
						[&](std::unique_ptr<ICollider>& o) {
							return o.get() == mData;
						});
				}
			}
			mData = o.mData;
			AddCount(mData);
		}
		return *this;
	}
	//���[�u�R���X�g���N�^
	Collider(Collider&& o) noexcept {
		if (this != &o) {
			if (mData) {
				SubtractCount(mData);
				if (GetCount(mData) == 0) {
					//�S�Q�Ə��ŁA�폜���鏈����������
					Colliders::sInstance.mColList.remove_if(
						[&](std::unique_ptr<ICollider>& o) {
							return o.get() == mData;
						});
				}
			}
			mData = o.mData;
			o.mData = nullptr;
		}
	}
	//���[�u������Z�q
	Collider& operator=(Collider&& o) noexcept {
		if (this != &o) {
			if (mData) {
				SubtractCount(mData);
				if (GetCount(mData) == 0) {
					//�S�Q�Ə��ŁA�폜���鏈����������
					Colliders::sInstance.mColList.remove_if(
						[&](std::unique_ptr<ICollider>& o) {
							return o.get() == mData;
						});
				}
			}
			mData = o.mData;
			o.mData = nullptr;
		}
		return *this;
	}

	operator bool() const {
		return mData != nullptr;
	}

	ColliderType* operator->() {
		return mData;
	}

private:
	static std::recursive_mutex mMutex;
	static std::map<void*, size_t> mCountMap;

	static void AddCount(void* p) {
		std::lock_guard<std::recursive_mutex> lock(mMutex);
		mCountMap[p]++;
	}

	static void SubtractCount(void* p) {
		std::lock_guard<std::recursive_mutex> lock(mMutex);
		mCountMap[p]--;

		if (mCountMap[p] == 0) {
			mCountMap.erase(p);
		}
	}

	static size_t GetCount(void* p) {
		std::lock_guard<std::recursive_mutex> lock(mMutex);
		auto itr = mCountMap.find(p);
		if (itr == mCountMap.end()) {
			return 0;
		}
		return mCountMap[p];
	}
};

template<typename ColliderType>
std::map<void*, size_t> Collider<ColliderType>::mCountMap = std::map<void*, size_t>();

template<typename ColliderType>
std::recursive_mutex Collider<ColliderType>::mMutex;