#include "Colliders.h"
#include <Util.h>
#include <vector>

Colliders Colliders::instance;

void Colliders::Update()
{
    std::lock_guard<std::recursive_mutex> lock(instance.mutex);

    //二重判定防止用なのです
    std::map <ICollider*, std::vector<ICollider*>> hitted;

    //2回回すの嫌だから何かいい方法考えたい所
    for (std::unique_ptr<ICollider>& ptr : instance.colList) {
        ptr->hasCollision = false;
    }

    //総当たりチェック！！！！！
    for (std::unique_ptr<ICollider>& ptr : instance.colList) {
        if (!ptr->active) continue;
        ptr->Update();
        for (std::unique_ptr<ICollider>& ptr2 : instance.colList) {
            if (ptr == ptr2) continue;
            if (!ptr2->active) continue;
            if (Util::IndexOf(hitted[ptr.get()], ptr2.get()) != -1) continue;

            CollisionInfo info;
            if (ptr->Collide(ptr2.get(), &info)) {
                hitted[ptr.get()].push_back(ptr2.get());
                hitted[ptr2.get()].push_back(ptr.get());

                ptr->hasCollision = true;
                if(ptr->onCollision) ptr->onCollision(info);

                ptr2->hasCollision = true;
                if (ptr2->onCollision) {
                    CollisionInfo copy = info;
                    copy.hitCollider = ptr2.get();
                    ptr2->onCollision(copy);
                }
            }
        }
    }
}

void Colliders::AllActivate()
{
    std::lock_guard<std::recursive_mutex> lock(instance.mutex);
    for (std::unique_ptr<ICollider>& ptr : instance.colList) {
        ptr->active = true;
    }
}

void Colliders::AllInactivate()
{
    std::lock_guard<std::recursive_mutex> lock(instance.mutex);
    for (std::unique_ptr<ICollider>& ptr : instance.colList) {
        ptr->active = false;
    }
}