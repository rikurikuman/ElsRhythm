#include "Material.h"
#include <fstream>
#include <sstream>
#include <cassert>
#include "Util.h"

using namespace std;

bool Material::operator==(const Material& o) const
{
    return name == o.name
        && texture == o.texture
        && color == o.color
        && ambient == o.ambient
        && diffuse == o.diffuse
        && specular == o.specular;
}

bool Material::operator!=(const Material& o) const
{
    return !(*this == o);
}

void Material::Transfer(MaterialBuffer* target)
{
	target->color = color;
	target->ambient = ambient;
	target->diffuse = diffuse;
	target->specular = specular;
}