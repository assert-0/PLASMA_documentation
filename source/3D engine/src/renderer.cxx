#include "renderer.h"

#include "runtime.h"
#include "object.h"

#include <vector>
#include <sstream>
#include <string>
#include <fstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

//Pi constants

const double PI = 3.141592653589793238463;
const double PI2 = 6.283185307179586476925;

//System handles and globals

static Renderer g_renderer;

glm::mat4 g_MVP;
glm::mat4 g_projectionMat, g_viewMat;
GLuint g_shaderID;
GLuint g_matrixUniformID;
GLuint g_colorUniformID;

GLuint g_vertexArrayID;

Renderer *getRenderer(){return &g_renderer;}

static Runtime *g_sys;
static ModelHandler *g_mod;

//Renderer helper functions

void createVertexArray()
{
    if(g_vertexArrayID != 0)
        glDeleteVertexArrays(1, &g_vertexArrayID);

    glGenVertexArrays(1, &g_vertexArrayID);
    glBindVertexArray(g_vertexArrayID);
}

void loadShaders(const char *vertexPath, const char *fragmentPath)
{
    GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	std::ifstream istr;
	std::stringstream ss;
	const char *vertexSourcePtr = nullptr, *fragmentSourcePtr = nullptr;
	std::string vertexSource, fragmentSource;

    auto loadShaderResource =
    [&](const char *fileName,
        const char *&resBuffPtr,
        std::string &resBuff)
    {
        istr.open(fileName, std::ios::binary);
        if(!istr.is_open())
        {
            printf("Error opening %s!\n", fileName);
            return false;
        }
        ss << istr.rdbuf();
        resBuff = ss.str();
        resBuffPtr = resBuff.c_str();
        ss.str("");
        ss.clear();
        istr.close();
        return true;
    };

    assert
    (
        loadShaderResource
        (
            vertexPath,
            vertexSourcePtr,
            vertexSource
        )
    );
    assert
    (
        loadShaderResource
        (
            fragmentPath,
            fragmentSourcePtr,
            fragmentSource
        )
    );
    //File loading error

    std::string errorMessage;
    int errorLen = 0;
    GLint glRet = GL_FALSE;

    auto compileShader =
    [&](GLuint shaderID, const char *path, const char *sourcePtr) -> bool
    {
        printf("Compiling shader: %s... ", path);
        glShaderSource(shaderID, 1, &sourcePtr, NULL);
        glCompileShader(shaderID);

        glGetShaderiv(shaderID, GL_COMPILE_STATUS, &glRet);
        glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &errorLen);
        if(errorLen > 0)
        {
            errorMessage.resize(errorLen + 1);
            glGetShaderInfoLog
            (
                shaderID,
                errorLen,
                NULL,
                &(errorMessage.front())
            );
            printf("\nCompilation error:\n%s\n", errorMessage.c_str());
            return false;
        }

        printf("Done!\n");
        return true;
    };

    assert
    (
        compileShader
        (
            vertexShaderID,
            vertexPath,
            vertexSourcePtr
        )
    );
    assert
    (
        compileShader
        (
            fragmentShaderID,
            fragmentPath,
            fragmentSourcePtr
        )
    );
    //Compilation error

    printf("Linking program... ");
	g_shaderID = glCreateProgram();
	glAttachShader(g_shaderID, vertexShaderID);
	glAttachShader(g_shaderID, fragmentShaderID);
	glLinkProgram(g_shaderID);

    glGetProgramiv(g_shaderID, GL_LINK_STATUS, &glRet);
	glGetProgramiv(g_shaderID, GL_INFO_LOG_LENGTH, &errorLen);

	if(errorLen > 0)
    {
        errorMessage.resize(errorLen + 1);
		glGetProgramInfoLog
		(
            g_shaderID,
            errorLen,
            NULL,
            &(errorMessage.front())
        );
		printf("\nLinking error:\n%s\n", errorMessage.c_str());
	}

	assert(errorLen == 0);
	//Linking error

	printf("Done!\n");

	glDetachShader(g_shaderID, vertexShaderID);
	glDetachShader(g_shaderID, fragmentShaderID);

	glDeleteShader(g_shaderID);
	glDeleteShader(g_shaderID);
}

void renderMesh(Mesh &in, float r, float g, float b)
{
    glUseProgram(g_shaderID);
    glBindVertexArray(g_vertexArrayID);

    glUniformMatrix4fv(g_matrixUniformID, 1, GL_FALSE, &(g_MVP[0][0]));
    glUniform3f(g_colorUniformID, r, g, b);

    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, in.vertexBuffer);
    glVertexAttribPointer
    (
        0,                  // attribute 0. No particular reason for 0,
                            // but must match the layout in the shader.
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );

    for(int a = 0; a < in.vertices.size(); a++)
    {
        in.vertices[a] = Vertex();
        in.indices[a] = 0;
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, in.indexBuffer);

    glDrawElements
    (
        GL_TRIANGLES,
        in.indices.size(),
        GL_UNSIGNED_SHORT,
        (void*)0
    );

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(0);
    glBindVertexArray(0);
    glUseProgram(0);
}

void calculateForwardAndRigthVectors
(
    glm::vec3 &forward,
    glm::vec3 &right,
    double hAngle,
    double vAngle
)
{
    forward.x = cos(vAngle) * sin(hAngle);
    forward.y = sin(vAngle);
    forward.z = cos(vAngle) * cos(hAngle);

    right.x = sin(hAngle - PI / 2.0);
    right.y = 0;
    right.z = cos(hAngle - PI / 2.0);
}

glm::mat4 calculateViewMatrix(float x, float y, float z, double hAngle, double vAngle)
{
    glm::mat4 ret;
    glm::vec3 direction;
    glm::vec3 right;
    glm::vec3 up;

    calculateForwardAndRigthVectors(direction, right, hAngle, vAngle);
    direction.x += x;
    direction.y += y;
    direction.z += z;

    up = glm::cross(right, direction);

    ret = glm::lookAt
    (
        glm::vec3(x, y, z),
        direction,
        up
    );

    return ret;
}

glm::mat4 calculateProjectionMatrix
(
    float FOV,
    float maxRenderDistance
)
{
    glm::mat4 ret;
    int screenW, screenH;

    g_sys->getScreenDimensions(screenW, screenH);

    ret = glm::perspective
    (
        float(FOV),
        float(screenW) / float(screenH),
        1.0f, maxRenderDistance
    );

    return ret;
}

glm::mat4 calculateModelMatrix(float x, float y, float z, float scale)
{
    glm::mat4 translateMat, scaleMat, ret;

    translateMat = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
    scaleMat = glm::scale(glm::mat4(1.0f), glm::vec3(scale));

    ret = translateMat * scaleMat;

    return ret;
}

//Renderer method implementations

Renderer::Renderer()
{
    ZeroMemory(this, sizeof(*this));
    FOV = glm::radians(70.0);
    maxRenderDistance = 10000.0f;
    g_sys = getRuntime();
    g_mod = getModelHandler();
}

Renderer::~Renderer()
{
    glDeleteProgram(g_shaderID);
    printf("Released shaders!\n");
}

void Renderer::renderSingleObject(Object* obj)
{
    if(!obj->visible)
        return;

    glm::mat4 modelMat;
    std::vector<Mesh> &meshes = g_mod->getModel(obj->model)->meshes;

    modelMat = calculateModelMatrix(obj->x, obj->y, obj->z, obj->scale);

    g_MVP = g_projectionMat * g_viewMat * modelMat;

    for(int a = 0; a < meshes.size(); a++)
        renderMesh(meshes[a], obj->r, obj->g, obj->b);
}

void Renderer::renderMultipleObjects(Object* objAr, int count)
{
    for(int a = 0; a < count; a++)
    {
        renderSingleObject(&objAr[a]);
    }
}

void Renderer::recalculateMatrices()
{
    g_projectionMat = calculateProjectionMatrix(FOV, maxRenderDistance);
    g_viewMat = calculateViewMatrix(camX, camY, camZ, camHorAngle, camVerAngle);
}

void Renderer::initShaders()
{
    createVertexArray();
    loadShaders("shaders/main.vertexshader", "shaders/main.fragmentshader");
    g_matrixUniformID = glGetUniformLocation(g_shaderID, "MVP");
    g_colorUniformID = glGetUniformLocation(g_shaderID, "OBJECT_COLOR");
}
