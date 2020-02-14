#pragma once

#include <vector>
#include <map>
#include <string>
#include <windows.h>

#include <GL/glew.h>

//Base struct declarations

struct Vertex
{
    float x, y, z;
};

struct Mesh
{
    Mesh(){ZeroMemory(this, sizeof(*this));}

    void reloadBuffers();

    std::vector<Vertex> vertices;
    std::vector<unsigned short> indices;
    GLuint vertexBuffer;
    GLuint indexBuffer;

    //Not used in this version - part of texture support.
    std::vector<float> uvs;

    //Not used in this version - part of lighting support.
    std::vector<float> normals;
};

struct Model
{
    Model(){ZeroMemory(this, sizeof(*this));}
    ~Model();

    void operator=(Model &in);

    void reloadBuffers();

    std::vector<Mesh> meshes;
};

struct Object
{
    Object(){ZeroMemory(this, sizeof(*this));}

    float x, y, z;
    float scale;
    bool visible;
    char model[256];
    int uuid;

    //Not used in this version - part of rotation support.
    float yaw, pitch, roll;

    //Specific to first primitive engine version - should be changed to textures later.
    float r, g, b;
};

//Handler declarations

class ModelHandler
{
public:
    ModelHandler(){}

    ~ModelHandler(){}

    Model *loadModel(const char *path, const char *name);

    Model *getModel(const char *name){return &models[name];}

    void destroyModel(const char *name){models.erase(name);}

    std::map<std::string, Model> *getModels(){return &models;}

private:
    void reloadBuffers();

    std::map<std::string, Model> models;
};

class ObjectHandler
{
public:
    ObjectHandler(){}

    ~ObjectHandler(){}

    int createObject(const char *modelName);
    int createObject(const char *modelPath, const char *modelName);

    void destroyObject(int uuid);

    Object *getObject(int uuid);

    std::vector<Object> *getObjects(){return &objects;}

private:
    std::vector<Object>::iterator getObjectIterator(int uuid);

    std::vector<Object> objects;
    static int currentUUID;
};

//Handler getters

ObjectHandler *getObjectHandler();
ModelHandler *getModelHandler();
