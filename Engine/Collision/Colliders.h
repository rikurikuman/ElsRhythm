#pragma once
#include "ICollider.h"
#include <mutex>
#include <map>
#include <memory>
#include <list>
#include <Windows.h>

template <typename ColliderType>
class Collider;

// 各種コライダー一括管理クラス

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

	//全コライダーをアクティブにする
	static void AllActivate();
	//全コライダーを非アクティブにする
	static void AllInactivate();

	//Collidersクラスが管理する各コライダーのリストへのアクセス用クラス
	//外部からはこのクラスを介してのみアクセスできる
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
	Collidersが管理する各種コライダー実体を参照する、
	スマートポインタもどきなクラス。
	"Collider<SphereCollider>"みたいな感じで使う。
	内部カウントを持ち、このクラスの生成で+1, 破棄で-1して、
	0になったらコライダー実体も破棄する。
	(Collidersインスタンスより寿命の長いColliderインスタンスがいると危ないのは内緒)
	(一応大丈夫なはずだけど)
*/

template <typename ColliderType>
class Collider final {
	friend class Colliders;
public:
	//コンストラクタ
	Collider() {}

private: //Collidersを介さないと新規生成はできないように(コピーとかはええで)
	ColliderType* data = nullptr;

	//コンストラクタ
	Collider(ColliderType* data) : data(data) {
		AddCount(data);
	}

public:
	//デストラクタ
	~Collider() {
		if (data) {
			SubtractCount(data);
			if (GetCount(data) == 0) {
				//全参照消滅、削除する処理をここに
				Colliders::instance.colList.remove_if(
					[&](std::unique_ptr<ICollider>& o) {
						return o.get() == data;
					});
			}
		}
	}
	//コピーコンストラクタ
	Collider(const Collider& o) {
		if (this != &o) {
			if (data) {
				SubtractCount(data);
				if (GetCount(data) == 0) {
					//全参照消滅、削除する処理をここに
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
	//コピー代入演算子
	Collider& operator=(const Collider& o) {
		if (this != &o) {
			if (data) {
				SubtractCount(data);
				if (GetCount(data) == 0) {
					//全参照消滅、削除する処理をここに
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
	//ムーブコンストラクタ
	Collider(Collider&& o) noexcept {
		if (this != &o) {
			if (data) {
				SubtractCount(data);
				if (GetCount(data) == 0) {
					//全参照消滅、削除する処理をここに
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
	//ムーブ代入演算子
	Collider& operator=(Collider&& o) noexcept {
		if (this != &o) {
			if (data) {
				SubtractCount(data);
				if (GetCount(data) == 0) {
					//全参照消滅、削除する処理をここに
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