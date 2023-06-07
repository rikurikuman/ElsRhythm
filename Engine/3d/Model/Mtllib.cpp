#include "Mtllib.h"
#include <fstream>
#include <sstream>
#include <cassert>
#include "Util.h"

using namespace std;

Mtllib Mtllib::Load(std::string filepath, std::string filename)
{
    Mtllib data;

    ifstream file;
    file.open((filepath + filename).c_str());
    if (file.fail()) {
        assert(0);
    }

    Material loading;

    string line = "";
    while (getline(file, line)) {
        istringstream line_stream(line);

        string key;
        getline(line_stream, key, ' ');

        if (key[0] == '\t') {
            key.erase(key.begin());
        }

        if (key == "newmtl") { //�}�e���A����`�i�����O�͂�������j
            if (loading != Material()) {
                data.materials.push_back(loading);
            }
            loading = Material();
            line_stream >> loading.name;
        }

        if (key == "Ka") { //����
            line_stream >> loading.ambient.x;
            line_stream >> loading.ambient.y;
            line_stream >> loading.ambient.z;
        }

        if (key == "Kd") { //�g�U���ˌ�
            line_stream >> loading.diffuse.x;
            line_stream >> loading.diffuse.y;
            line_stream >> loading.diffuse.z;
        }

        if (key == "Ks") { //���ʔ��ˌ�
            line_stream >> loading.specular.x;
            line_stream >> loading.specular.y;
            line_stream >> loading.specular.z;
        }

        if (key == "map_Kd") { //�e�N�X�`���w��
            string texfile;
            line_stream >> texfile;
            loading.texture = TextureManager::Load(filepath + texfile);
        }
    }

    data.materials.push_back(loading);
    return data;
}
