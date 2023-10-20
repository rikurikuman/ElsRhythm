#include "ParticleManager.h"

#include <functional>
#include "Util.h"

ParticleManager* ParticleManager::GetInstance()
{
    static ParticleManager instance;
    return &instance;
}

void ParticleManager::Register(std::shared_ptr<IParticle> ptr)
{
    ParticleManager* instance = GetInstance();
    instance->mParticleList.push_back(ptr);
}

void ParticleManager::Clear()
{
    ParticleManager* instance = GetInstance();

    std::vector<std::function<void()>> clearFuncs;

    for (auto& pt : instance->mParticleList) {
        std::function<void()> clearFunc = pt->GetBatchClearFunc();
        if (clearFunc) {
            bool hit = false;
            for (auto func : clearFuncs) {
                if (clearFunc.target<void()>() == func.target<void()>()) {
                    hit = true;
                    break;
                }
            }
            if (!hit) {
                clearFuncs.push_back(clearFunc);
                clearFunc();
            }
        }
    }

    instance->mParticleList.clear();
}

void ParticleManager::Update()
{
    ParticleManager* instance = GetInstance();

    for (auto itr = instance->mParticleList.begin(); itr != instance->mParticleList.end();) {
        IParticle* pt = itr->get();
        pt->Update();
        if (pt->mRemoveFlag) {
            itr = instance->mParticleList.erase(itr);
            continue;
        }
        itr++;
    }
}

void ParticleManager::Draw()
{
    ParticleManager* instance = GetInstance();

    std::vector<std::function<void()>> batchDrawFuncs;

    for (auto itr = instance->mParticleList.begin(); itr != instance->mParticleList.end();) {
        IParticle* pt = itr->get();
        pt->Draw();

        std::function<void()> batchDrawFunc = pt->GetBatchDrawFunc();

        if (batchDrawFunc) {
            bool hit = false;
            for (auto& func : batchDrawFuncs) {
                if (batchDrawFunc.target<void()>() == func.target<void()>()) {
                    hit = true;
                    break;
                }
            }
            if(!hit) batchDrawFuncs.push_back(batchDrawFunc);
        }
        itr++;
    }

    for (auto& func : batchDrawFuncs) {
        func();
    }
}

ParticleManager::ParticleManager() {

}