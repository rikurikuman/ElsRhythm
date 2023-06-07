#pragma once
#include <functional>
#include <string>
#include "Vector3.h"

class ICollider;

struct CollisionInfo {
	ICollider* hitCollider = nullptr; //�Փ˂�������
	bool hasClosestPos = false; //�ŋߐړ_���������Ă��邩
	Vector3 closestPos; //�ŋߐړ_(�����_�ŗǂ���)
	bool hasDistance = false; //�����������Ă��邩
	float distance = 0; //�Փ˓_�ւ̋���
	bool hasInter = false; //��_�������Ă��邩
	Vector3 inter; //��_
};

class Colliders;
class ICollider {
	friend class Colliders;
public:
	virtual ~ICollider() = default;

	virtual std::string GetTypeIndentifier() = 0;
	virtual void Update() {} //�K�v�Ȏq�����g���Ă�����
	virtual bool Collide(ICollider* hit, CollisionInfo* outputInfo) = 0;

	//�Փ˂������肪���邩�ǂ����𓾂�
	bool HasCollision() {
		return hasCollision;
	}

	bool active = false;
	std::function<void(CollisionInfo)> onCollision;

private:
	bool hasCollision = false;
};