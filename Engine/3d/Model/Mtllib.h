#pragma once
#include "Material.h"
#include <vector>

class Mtllib
{
public:
	std::vector<Material> materials;

	static Mtllib Load(std::string filepath, std::string filename);
};

