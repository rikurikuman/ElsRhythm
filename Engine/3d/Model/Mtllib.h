#pragma once
#include "Material.h"
#include <vector>

class Mtllib
{
public:
	std::vector<Material> mMaterials;

	static Mtllib Load(std::string filepath, std::string filename);
};

