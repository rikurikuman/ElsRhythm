#pragma once
#include <list>
#include <memory>

#include "IParticle.h"

class ParticleManager
{
public:
	static void Register(std::shared_ptr<IParticle> ptr);
	static void Clear();

	static void Update();
	static void Draw();

private:
	ParticleManager();
	~ParticleManager() = default;
	ParticleManager(const ParticleManager& a) = delete;
	ParticleManager& operator=(const ParticleManager&) = delete;

	std::list<std::shared_ptr<IParticle>> mParticleList;

	static ParticleManager* GetInstance();
};

