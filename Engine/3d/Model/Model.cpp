#include "Model.h"
#include <fstream>
#include <sstream>
#include <cassert>
#include "Util.h"
#include "Mtllib.h"
#include "Matrix4.h"

using namespace std;

bool ModelData::operator==(const ModelData& o) const
{
    return name == o.name && vertexs == o.vertexs && indices == o.indices && material == o.material;
}

bool ModelData::operator!=(const ModelData& o) const
{
    return !(*this == o);
}

void ModelData::CalcSmoothedNormals()
{
    map<Vector3, vector<Vector3>> smoothData;

    for (VertexPNU v : vertexs) {
        smoothData[v.pos].push_back(v.normal);
    }

    for (VertexPNU& v : vertexs) {
        vector<Vector3> normals = smoothData[v.pos];

        Vector3 result;
        for (Vector3 n : normals) {
            result += n;
        }
        result /= static_cast<float>(normals.size());
        result.Normalize();

        v.normal = result;
    }

    /*assert(indices.size() % 3 == 0);
    for (unsigned int i = 0; i < indices.size() / 3; i++) {
        unsigned int index0 = indices[i * 3 + 0];
        unsigned int index1 = indices[i * 3 + 1];
        unsigned int index2 = indices[i * 3 + 2];

        Vector3 p0 = vertexs[index0].pos;
        Vector3 p1 = vertexs[index1].pos;
        Vector3 p2 = vertexs[index2].pos;

        Vector3 v1 = p1 - p0;
        Vector3 v2 = p2 - p0;

        Vector3 normal = v1.Cross(v2);
        normal.Normalize();

        smoothData[p0].push_back(normal);
        smoothData[p1].push_back(normal);
        smoothData[p2].push_back(normal);
    }

    for (auto itr = smoothData.begin(); itr != smoothData.end(); itr++) {
        Vector3 target = itr->first;
        vector<Vector3> normals = itr->second;

        Vector3 result = { 0, 0, 0 };
        for (Vector3 n : normals) {
            result += n;
        }
        result /= static_cast<float>(normals.size());
        result.Normalize();

        for (VertexPNU& v : vertexs) {
            if (v.pos == target) {
                v.normal = result;
            }
        }
    }*/
}

ModelHandle Model::Load(string filepath, string filename, ModelHandle handle, bool smooth)
{
    ModelManager* instance = ModelManager::GetInstance();
    Model model;

    if (filepath[filepath.size() - 1] != '/') {
        filepath += "/";
    }

    model.path = filepath + filename;

    ifstream file;
    file.open((filepath + filename).c_str());
    if (file.fail()) {
        return "PreRegistedModel_Empty";
    }

    if (handle.empty()) {
        handle = "NoNameHandle_" + model.path;
    }

    std::unique_lock<std::recursive_mutex> lock(instance->mutex);
    if (instance->modelMap.find(handle) != instance->modelMap.end()
        && instance->modelMap[handle].path == (filepath + filename)) {
        return handle;
    }
    lock.unlock();
    ModelData loading;

    vector<Material> materialList;
    vector<Vector3> vertPosList;
    vector<Vector2> vertTexcoordList;
    vector<Vector3> vertNormalList;

    int cou = 0;

    string line = "";
    while (getline(file, line)) {
        istringstream line_stream(line);

        string key;
        getline(line_stream, key, ' ');

        if (key == "#") {
            continue;
        }

        if (key == "o") { //おなまえ
            if (loading != ModelData()) {
                if (smooth) loading.CalcSmoothedNormals();
                loading.vertexBuff.Init(loading.vertexs);
                loading.indexBuff.Init(loading.indices);
                model.data.emplace_back(std::make_shared<ModelData>(loading));
            }

            loading = ModelData();
            line_stream >> loading.name;
        }

        if (key == "v") { //頂点座標
            Vector3 pos;
            line_stream >> pos.x;
            line_stream >> pos.y;
            line_stream >> pos.z;

            vertPosList.emplace_back(pos.x, pos.y, pos.z);
        }

        if (key == "vt") { //頂点UV
            Vector2 texcoord;
            line_stream >> texcoord.x;
            line_stream >> texcoord.y;
            vertTexcoordList.emplace_back(texcoord.x, 1.0f - texcoord.y);
        }

        if (key == "vn") { //頂点法線
            Vector3 normal;

            line_stream >> normal.x;
            line_stream >> normal.y;
            line_stream >> normal.z;

            vertNormalList.emplace_back(normal.x, normal.y, normal.z);
        }

        if (key == "f") { //インデックス
            vector<string> indexs = Util::StringSplit(line_stream.str(), " ");

            vector<VertexPNU> _vertices;
            for (int i = 1; i < indexs.size(); i++) {
                vector<string> indexText = Util::StringSplit(indexs[i], "/");

                bool ok = false;
                VertexPNU vertex;
                if (indexText.size() >= 1) {
                    int index = atoi(indexText[0].c_str()) - 1;
                    if (index >= 0 && vertPosList.size() > index) {
                        vertex.pos = vertPosList[index];
                        ok = true;
                    }
                }
                if (indexText.size() >= 2) {
                    int index = atoi(indexText[1].c_str()) - 1;
                    if (index >= 0 && vertTexcoordList.size() > index) {
                        vertex.uv = vertTexcoordList[index];
                    }
                }
                if (indexText.size() >= 3) {
                    int index = atoi(indexText[2].c_str()) - 1;
                    if (index >= 0 && vertNormalList.size() > index) {
                        vertex.normal = vertNormalList[index];
                    }
                }

                if (ok) {
                    _vertices.emplace_back(vertex);
                } 
            }

            if (_vertices.size() >= 3) {
                for (int i = 0; i < _vertices.size() - 2; i++) {
                    int indexB = 1 + i;
                    int indexC = 2 + i;

                    //めちゃくちゃ重いから一回廃止
                    /*size_t a = Util::IndexOf(model.vertexs, _vertices[0]);
                    size_t b = Util::IndexOf(model.vertexs, _vertices[indexB]);
                    size_t c = Util::IndexOf(model.vertexs, _vertices[indexC]);*/

                    size_t a = -1;
                    size_t b = -1;
                    size_t c = -1;

                    if (a == -1) {
                        loading.vertexs.emplace_back(_vertices[0]);
                        a = loading.vertexs.size() - 1;
                    }
                    if (b == -1) {
                        loading.vertexs.emplace_back(_vertices[indexB]);
                        b = loading.vertexs.size() - 1;
                    }
                    if (c == -1) {
                        loading.vertexs.emplace_back(_vertices[indexC]);
                        c = loading.vertexs.size() - 1;
                    }

                    loading.indices.emplace_back((UINT)a);
                    loading.indices.emplace_back((UINT)b);
                    loading.indices.emplace_back((UINT)c);
                }
            }
        }

        if (key == "mtllib") { //マテリアルテンプレートライブラリ
            string mfilename;
            line_stream >> mfilename;
            Mtllib lib = Mtllib::Load(filepath, mfilename);

            for (Material& mat : lib.materials) {
                materialList.push_back(mat);
            }
        }

        if (key == "usemtl") {
            if (loading.material != Material()) {
                string oldname = loading.name;
                if (smooth) loading.CalcSmoothedNormals();
                loading.vertexBuff.Init(loading.vertexs);
                loading.indexBuff.Init(loading.indices);
                model.data.emplace_back(std::make_shared<ModelData>(loading));
                loading = ModelData();
                loading.name = oldname + "_ChangeMaterial";
            }
            string mtlName;
            line_stream >> mtlName;
            for (Material& mtl : materialList) {
                if (mtl.name == mtlName) {
                    loading.material = mtl;
                }
            }
        }
    }

    if (smooth) loading.CalcSmoothedNormals();
    loading.vertexBuff.Init(loading.vertexs);
    loading.indexBuff.Init(loading.indices);
    model.data.emplace_back(std::make_shared<ModelData>(loading));

    lock.lock();
    instance->modelMap[handle] = model;
    lock.unlock();
    return handle;
}

ModelHandle Model::Register(ModelHandle handle, Model model)
{
    ModelManager* instance = ModelManager::GetInstance();
    std::lock_guard<std::recursive_mutex> lock(instance->mutex);

    instance->modelMap[handle] = model;
    return handle;
}

aiMatrix4x4 GetNodeAbsoluteTransform(const aiNode* node) {
    aiMatrix4x4 transform = node->mTransformation;

    if (node->mParent != nullptr) {
        aiMatrix4x4 pTransform = GetNodeAbsoluteTransform(node->mParent);
        transform = pTransform * transform;
    }

    return transform;
}

void NodeTraverse(const aiScene* scene, const aiNode* node, Model* model, const vector<Material>* materials) {
    for (UINT meshIndex = 0; meshIndex < node->mNumMeshes; meshIndex++) {
        ModelData data;

        aiMesh* mesh = scene->mMeshes[node->mMeshes[meshIndex]];

        //おなまえドットコム
        aiString _name = mesh->mName;
        data.name = _name.C_Str();

        //トランスフォーム
        aiMatrix4x4 transform = GetNodeAbsoluteTransform(node);

        //頂点
        vector<VertexPNU> vertices;

        for (UINT posIndex = 0; posIndex < mesh->mNumVertices; posIndex++) {
            Vector3 pos = { 0, 0, 0 };
            Vector3 norm = { 0, 0, 0 };
            Vector2 uv = { 0, 0 };

            aiVector3D _pos = mesh->mVertices[posIndex];
            _pos *= transform;
            pos = Vector3(_pos.x, _pos.y, _pos.z);

            if (mesh->HasNormals()) {
                aiVector3D _norm = mesh->mNormals[posIndex];

                aiVector3D _matpos;
                aiVector3D _matrotAxis;
                ai_real _matrotang;
                aiVector3D _matsca;
                transform.Decompose(_matsca, _matrotAxis, _matrotang, _matpos);

                aiMatrix4x4 rotateMat;
                aiMatrix4x4::Rotation(_matrotang, _matrotAxis, rotateMat);
                _norm *= rotateMat;
                _norm.Normalize();
                norm = Vector3(_norm.x, _norm.y, _norm.z);
            }

            if (mesh->HasTextureCoords(0)) {
                aiVector3D _uv = mesh->mTextureCoords[0][posIndex];
                uv = Vector2(_uv.x, _uv.y);
            }

            VertexPNU vert;
            vert.pos = pos;
            vert.normal = norm;
            vert.uv = uv;
            vertices.push_back(vert);
        }

        for (VertexPNU& vert : vertices) {
            data.vertexs.push_back(vert);
        }

        //面(インデックス)
        for (UINT faceIndex = 0; faceIndex < mesh->mNumFaces; faceIndex++) {
            aiFace face = mesh->mFaces[faceIndex];
            for (UINT vertIndex = 0; vertIndex < face.mNumIndices; vertIndex++) {
                data.indices.push_back(face.mIndices[vertIndex]);
            }
        }

        if (materials->size() > mesh->mMaterialIndex) {
            data.material = materials->at(mesh->mMaterialIndex);
        }

        data.vertexBuff.Init(data.vertexs);
        data.indexBuff.Init(data.indices);
        model->data.push_back(make_shared<ModelData>(data));
    }

    for (UINT childIndex = 0; childIndex < node->mNumChildren; childIndex++) {
        NodeTraverse(scene, node->mChildren[childIndex], model, materials);
    }
}

ModelHandle Model::LoadWithAIL(std::string directoryPath, std::string filename, ModelHandle handle)
{
    ModelManager* instance = ModelManager::GetInstance();
    Model model;

    if (directoryPath[directoryPath.size() - 1] != '/') {
        directoryPath += "/";
    }

    model.path = directoryPath + filename;

    if (handle.empty()) {
        handle = "UnnamedModelHandle_" + model.path;
    }

    std::unique_lock<std::recursive_mutex> lock(instance->mutex);
    if (instance->modelMap.find(handle) != instance->modelMap.end()
        && instance->modelMap[handle].path == (model.path)) {
        return handle;
    }
    lock.unlock();

    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(model.path,
        aiProcess_ConvertToLeftHanded |
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_SortByPType |
        aiProcess_GenSmoothNormals |
//        aiProcess_FixInfacingNormals |
        aiProcess_CalcTangentSpace
    );

    if (scene == nullptr) {
        OutputDebugStringA(importer.GetErrorString());
        return "";
    }

    model.name = scene->mName.C_Str();

    //マテリアル
    vector<Material> materials;

    for (UINT materialIndex = 0; materialIndex < scene->mNumMaterials; materialIndex++) {
        Material material;
        aiMaterial* _material = scene->mMaterials[materialIndex];

        aiString str;
        _material->Get(AI_MATKEY_NAME, str);
        material.name = str.C_Str();

        aiColor3D color;

        color = { 0, 0, 0 };
        _material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
        material.diffuse = Vector3(color.r, color.g, color.b);
        
        color = { 0, 0, 0 };
        _material->Get(AI_MATKEY_COLOR_AMBIENT, color);
        material.ambient = Vector3(color.r, color.g, color.b);

        color = { 0, 0, 0 };
        _material->Get(AI_MATKEY_COLOR_SPECULAR, color);
        material.specular = Vector3(color.r, color.g, color.b);

        float opacity = 0.0f;
        _material->Get(AI_MATKEY_OPACITY, opacity);
        material.color.a = opacity;

        if (_material->GetTextureCount(aiTextureType_DIFFUSE) >= 1) {
            //_material->GetTexture(aiTextureType_DIFFUSE, 0, &str);
            _material->Get(AI_MATKEY_TEXTURE_DIFFUSE(0), str);

            string path = str.C_Str();
            string texHandle = "";

            if (!material.name.empty()) {
                texHandle = model.path + "_" + material.name + "_Tex";
            }
            else {
                texHandle = "UnnamedMaterial_Tex";
            }

            material.texture = "";
            if (path.empty()) {
                material.texture = "";
            }
            else {
                const aiTexture* embedTex = scene->GetEmbeddedTexture(path.c_str());
                if (embedTex != nullptr) {
                    if (embedTex->mHeight == 0) {
                        material.texture = TextureManager::Load(embedTex->pcData, embedTex->mWidth, path, texHandle);
                    }
                    else {
                        Color* data = new Color[embedTex->mWidth * embedTex->mHeight];

                        for (UINT i = 0; i < embedTex->mWidth * embedTex->mHeight; i++) {
                            aiTexel texel = embedTex->pcData[i];
                            //data[i] = Color(texel.r, texel.g, texel.b, texel.a);
                        }

                        material.texture = TextureManager::Create(data, embedTex->mWidth, embedTex->mHeight, path, texHandle);
                        
                        delete[] data;
                    }
                }
                else {
                    if (path[0] == '*') {
                        string index = path.substr(1, path.size() - 1);
                        aiTexture* tex = scene->mTextures[atoi(index.c_str())];
                        
                        if (tex->mHeight == 0) {
                            material.texture = TextureManager::Load(tex->pcData, tex->mWidth, path, texHandle);
                        }
                        else {
                            Color* data = new Color[tex->mWidth * tex->mHeight];

                            for (UINT i = 0; i < tex->mWidth * tex->mHeight; i++) {
                                aiTexel texel = tex->pcData[i];
                                //data[i] = Color(texel.r, texel.g, texel.b, texel.a);
                            }

                            material.texture = TextureManager::Create(data, tex->mWidth, tex->mHeight, path, texHandle);

                            delete[] data;
                        }
                    }
                    else {
                        material.texture = TextureManager::Load(directoryPath + path, texHandle);
                    }
                }
            }
        }

        materials.push_back(material);
    }

    //ルートノードからぜーんぶメッシュをぶち込んでモデルにする
    NodeTraverse(scene, scene->mRootNode, &model, &materials);

    lock.lock();
    instance->modelMap[handle] = model;
    lock.unlock();
    return handle;
}

Model* ModelManager::Get(ModelHandle handle)
{
    ModelManager* instance = GetInstance();
    std::lock_guard<std::recursive_mutex> lock(instance->mutex);

    if (instance->modelMap.find(handle) == instance->modelMap.end()) {
        return &instance->modelMap["PreRegisteredModel_Empty"];
    }

    return &instance->modelMap[handle];
}

void ModelManager::Init()
{
    Model model;
    ModelData data;

    data.vertexs.push_back(VertexPNU({ -1, -1,  1 }, { 0, 0, 0 }, { 0, 1 })); //0
    data.vertexs.push_back(VertexPNU({  1, -1,  1 }, { 0, 0, 0 }, { 1, 1 })); //1
    data.vertexs.push_back(VertexPNU({ -1, -1, -1 }, { 0, 0, 0 }, { 0, 1 })); //2
    data.vertexs.push_back(VertexPNU({  1, -1, -1 }, { 0, 0, 0 }, { 1, 1 })); //3
    data.vertexs.push_back(VertexPNU({  0,  1,  0 }, { 0, 0, 0 }, { 0.5f, 0.0f })); //4

    data.indices.push_back(2);
    data.indices.push_back(3);
    data.indices.push_back(1);
    data.indices.push_back(2);
    data.indices.push_back(1);
    data.indices.push_back(0);
    data.indices.push_back(2);
    data.indices.push_back(4);
    data.indices.push_back(3);
    data.indices.push_back(3);
    data.indices.push_back(4);
    data.indices.push_back(1);
    data.indices.push_back(1);
    data.indices.push_back(4);
    data.indices.push_back(0);
    data.indices.push_back(0);
    data.indices.push_back(4);
    data.indices.push_back(2);

    VertexPNU::CalcNormalVec(data.vertexs, data.indices);

    data.vertexBuff.Init(data.vertexs);
    data.indexBuff.Init(data.indices);
    model.data.emplace_back(std::make_shared<ModelData>(data));
    modelMap["PreRegisteredModel_Empty"] = model;
}
