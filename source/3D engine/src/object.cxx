#include "object.h"

#include <algorithm>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//System handles

static ObjectHandler g_objectHandler;
static ModelHandler g_modelHandler;

ObjectHandler *getObjectHandler(){return &g_objectHandler;}
ModelHandler *getModelHandler(){return &g_modelHandler;}

//Model importer
Assimp::Importer imp;

//ModelHandler helper functions

bool processPathDelimiters(const char **path)
{
    std::string process = *path;
    char *newString;

    if(process.find('\\') == std::string::npos)
        return false;

    newString = new char [process.size()];

    for(int a = 0; a < process.size(); a++)
    {
        if(process[a] != '\\')
            newString[a] = process[a];
        else
            newString[a] = '/';
    }

    *path = const_cast<const char*>(newString);
    return true;
}

void processMesh(aiMesh *mesh, const aiScene *scene, Model &mod)
{
    Mesh &tmp = mod.meshes.emplace_back();
    unsigned int numVertices = mesh->mNumVertices,
                 numFaces    = mesh->mNumFaces,
                 numIndices  = 0;

    tmp.vertices.resize(numVertices);
    for(int a = 0; a < numVertices; a++)
    {
        tmp.vertices[a].x = mesh->mVertices[a].x;
        tmp.vertices[a].y = mesh->mVertices[a].y;
        tmp.vertices[a].z = mesh->mVertices[a].z;
    }

    for(int a = 0; a < numFaces; a++)
    {
        numIndices = mesh->mFaces[a].mNumIndices;
        for(int b = 0; b < numIndices; b++)
            tmp.indices.push_back(mesh->mFaces[a].mIndices[b]);
    }
}

void processNode(aiNode *node, const aiScene *scene, Model &mod)
{
    for(unsigned int a = 0; a < node->mNumMeshes; a++)
        processMesh(scene->mMeshes[node->mMeshes[a]], scene, mod);

    for(unsigned int a = 0; a < node->mNumChildren; a++)
        processNode(node->mChildren[a], scene, mod);
}

//Mesh methods

void Mesh::reloadBuffers()
{
    if(vertexBuffer != 0 || indexBuffer != 0)
    {
        glDeleteBuffers(1, &vertexBuffer);
        glDeleteBuffers(1, &indexBuffer);
    }

    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData
    (
        GL_ARRAY_BUFFER,
        sizeof(Vertex) * vertices.size(),
        &vertices.front(),
        GL_STATIC_DRAW
    );

    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData
    (
        GL_ELEMENT_ARRAY_BUFFER,
        sizeof(unsigned short) * indices.size(),
        &indices.front(),
        GL_STATIC_DRAW
    );

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

//Model methods

Model::~Model()
{
    for(Mesh &it : meshes)
    {
        glDeleteBuffers(1, &it.vertexBuffer);
        glDeleteBuffers(1, &it.indexBuffer);
    }
}

void Model::operator=(Model &in)
{
    meshes = in.meshes;
}

void Model::reloadBuffers()
{
    for(int a = 0; a < meshes.size(); a++)
        meshes[a].reloadBuffers();
}

//ModelHandler methods

Model *ModelHandler::loadModel(const char* path, const char* name)
{
    if(models[name].meshes.size() != 0)
        return &models[name];

    Model tmp;
    const aiScene *scene = imp.ReadFile
    (
        path,
        aiProcess_Triangulate |
        aiProcess_FlipUVs
    );

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        printf("Assimp error: %s\n", imp.GetErrorString());
        return nullptr;
    }

    processNode(scene->mRootNode, scene, tmp);
    models[name] = tmp;

    reloadBuffers();

    return &models[name];
}

void ModelHandler::reloadBuffers()
{
    for(auto it = models.begin(); it != models.end(); it++)
        it->second.reloadBuffers();
}

//ObjectHandler methods and static member

int ObjectHandler::currentUUID = 0;

int ObjectHandler::createObject(const char *modelName)
{
    objects.emplace_back();
    Object &tmp = objects.back();
    strcpy(tmp.model, modelName);
    tmp.uuid = currentUUID++;
    return tmp.uuid;
}

int ObjectHandler::createObject(const char *modelPath, const char *modelName)
{
    if(g_modelHandler.loadModel(modelPath, modelName) != nullptr)
        return createObject(modelName);
    return -1;
}

void ObjectHandler::destroyObject(int uuid)
{
    auto it = getObjectIterator(uuid);
    if(it == objects.end())
        return;
    objects.erase(it);
}

Object *ObjectHandler::getObject(int uuid)
{
    auto it = getObjectIterator(uuid);
    if(it == objects.end())
        return nullptr;
    return &(*it);
}

std::vector<Object>::iterator ObjectHandler::getObjectIterator(int uuid)
{
    for(auto it = objects.begin(); it != objects.end(); it++)
    {
        if((*it).uuid == uuid)
            return it;
    }
    return objects.end();
}
