#include "test_OpenGL_main.h"
#include "test_OpenGL_event.h"
#include "runtime.h"
#include <cstdio>
#include <cmath>
#include <sstream>
#include <string>
#include <fstream>
#include <cassert>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

const double PI = 3.141592653589793238463;
const double PI2 = 6.283185307179586476925;

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Vars and system handle getter.
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

enum MOVE_DIRECTIONS
{
    MOVE_FORWARD = 1,
    MOVE_BACKWARD = 2,
    MOVE_RIGHT = 4,
    MOVE_LEFT = 8
};

enum KEYS
{
    UP,
    DOWN,
    RIGHT,
    LEFT,
    W,
    A,
    S,
    D
};

static GLuint g_vertexArrayID = 0;
static GLuint g_vertexBuffer = 0;
static GLuint g_colorBuffer = 0;
static GLuint g_programID = 0;
static GLuint g_matrixID = 0;

static glm::mat4 g_projectionMat;
static glm::mat4 g_viewMat;
static glm::mat4 g_modelMat;
static glm::mat4 g_MVP;

static int g_screenW, g_screenH;
static Runtime *g_sys;

static double g_timeElapsed;

static float g_FOW = glm::radians(75.0f);
static float g_minRenderDistance = 1.0f, g_maxRenderDistance = 10000000000.0f;
static double g_mouseSensitivity = 0.00004;
static double g_cameraSpeed = 10.0;

static float g_camX = 0, g_camY = 0, g_camZ = -100;
static double g_horizontalAngle = 0.0, g_verticalAngle = 0.0;

static float g_cubeX = 0, g_cubeY = 0, g_cubeZ = 0;
static float g_cubeScale = 1.0;

static int g_moveDirection;

static const GLfloat
g_cube[] =
{
    -1.0f,-1.0f,-1.0f, // triangle 1 : begin
    -1.0f,-1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f, // triangle 1 : end
    1.0f, 1.0f,-1.0f, // triangle 2 : begin
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f, // triangle 2 : end
    1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
    1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,
    1.0f,-1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f,-1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f,-1.0f, 1.0f
},
g_cubeColors[]
{
    0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 1.0f,
    0.0f, 1.0f, 1.0f,
    0.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    1.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 1.0f,
    0.0f, 1.0f, 1.0f,
    0.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
};

static bool g_initialized = false;

static void acquireSystemHandle()
{
    g_sys = getRuntime();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Shader compilation.
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void loadShaders(const char *vertexPath, const char *fragmentPath)
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
        printf("Compiling shader: %s\n", path);
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
            printf("%s\n", errorMessage.c_str());
            return false;
        }
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

    printf("Linking program\n");
	g_programID = glCreateProgram();
	glAttachShader(g_programID, vertexShaderID);
	glAttachShader(g_programID, fragmentShaderID);
	glLinkProgram(g_programID);

    glGetProgramiv(g_programID, GL_LINK_STATUS, &glRet);
	glGetProgramiv(g_programID, GL_INFO_LOG_LENGTH, &errorLen);

	if(errorLen > 0)
    {
        errorMessage.resize(errorLen + 1);
		glGetProgramInfoLog
		(
            g_programID,
            errorLen,
            NULL,
            &(errorMessage.front())
        );
		printf("%s\n", errorMessage.c_str());
	}

	assert(errorLen == 0);
	//Linking error

	glDetachShader(g_programID, vertexShaderID);
	glDetachShader(g_programID, fragmentShaderID);

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Window size handling.
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void changeAspectRatio()
{
    g_projectionMat = glm::perspective(g_FOW,
                                       float(g_screenW) / float(g_screenH),
                                       g_minRenderDistance, g_maxRenderDistance);
    g_MVP = g_projectionMat * g_viewMat * g_modelMat;
}

static bool windowSizeChanged()
{
    int w, h;
    bool ret;
    g_sys->getScreenDimensions(w, h);
    ret = g_screenW != w || g_screenH != h;
    g_screenW = w;
    g_screenH = h;
    return ret;
}

static void handleWindowSizeChange()
{
    glViewport(0, 0, g_screenW, g_screenH);
    changeAspectRatio();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Buffer and array generation.
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void generateVertexArray()
{
    if(g_vertexArrayID != 0)
        glDeleteVertexArrays(1, &g_vertexArrayID);

    glGenVertexArrays(1, &g_vertexArrayID);
    glBindVertexArray(g_vertexArrayID);
}

static void generateVertexBuffer(const GLfloat *buff, int size)
{
    if(g_vertexBuffer != 0)
        glDeleteBuffers(1, &g_vertexBuffer);

    glGenBuffers(1, &g_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, g_vertexBuffer);
    glBufferData
    (
        GL_ARRAY_BUFFER,
        size,
        buff,
        GL_STATIC_DRAW
    );
}

static void generateColorBuffer(const GLfloat *buff, int size)
{
    if(g_colorBuffer != 0)
        glDeleteBuffers(1, &g_colorBuffer);

    glGenBuffers(1, &g_colorBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, g_colorBuffer);
    glBufferData
    (
        GL_ARRAY_BUFFER,
        size,
        buff,
        GL_STATIC_DRAW
    );
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Camera handling.
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void getForwardAndRigthVectors(glm::vec3 &forward, glm::vec3 &right)
{
    forward.x = cos(g_verticalAngle) * sin(g_horizontalAngle);
    forward.y = sin(g_verticalAngle);
    forward.z = cos(g_verticalAngle) * cos(g_horizontalAngle);

    right.x = sin(g_horizontalAngle - PI / 2.0);
    right.y = 0;
    right.z = cos(g_horizontalAngle - PI / 2.0);
}

static void updateCamera()
{
    glm::vec3 direction;
    glm::vec3 right;
    glm::vec3 up;

    getForwardAndRigthVectors(direction, right);
    direction.x += g_camX;
    direction.y += g_camY;
    direction.z += g_camZ;

    up = glm::cross(right, direction);

    g_viewMat = glm::lookAt
    (
        glm::vec3(g_camX, g_camY, g_camZ),
        direction,
        up
    );

    g_MVP = g_projectionMat * g_viewMat * g_modelMat;
}

static void changeCameraPosition(float x, float y, float z)
{
    g_camX = x;
    g_camY = y;
    g_camZ = z;
    updateCamera();
}

static void shiftCameraPosition(float x, float y, float z)
{
    changeCameraPosition(g_camX + x, g_camY + y, g_camZ + z);
}

static void moveCamera(int direction, double ammount)
{
    glm::vec3 forward;
    glm::vec3 right;

    double shiftX;
    double shiftY;
    double shiftZ;

    getForwardAndRigthVectors(forward, right);

    switch(direction)
    {
        case MOVE_FORWARD:
            forward *= 1.0;
            right *= 0.0;
            break;
        case MOVE_BACKWARD:
            forward *= -1.0;
            right *= 0.0;
            break;
        case MOVE_LEFT:
            forward *= 0.0;
            right *= 1.0;
            break;
        case MOVE_RIGHT:
            forward *= 0.0;
            right *= -1.0;
    }

    shiftX = (forward.x + right.x) * ammount;
    shiftY = (forward.y + right.y) * ammount;
    shiftZ = (forward.z + right.z) * ammount;

    g_camX += shiftX;
    g_camY += shiftY;
    g_camZ += shiftZ;

    updateCamera();
}

static void turnCameraTo(double horizontalAngle, double verticalAngle)
{
    g_horizontalAngle = horizontalAngle;
    g_verticalAngle = verticalAngle;

    if(fabs(g_horizontalAngle) > PI2)
        g_horizontalAngle -= floor(g_horizontalAngle / PI2) * PI2;

    if(fabs(g_verticalAngle) > PI2)
        g_verticalAngle -= floor(g_verticalAngle / PI2) * PI2;

    updateCamera();
}

static void turnCameraBy(double horizontalAngle, double verticalAngle)
{
    turnCameraTo
    (
        g_horizontalAngle + horizontalAngle,
        g_verticalAngle + verticalAngle
    );
}

static void initPerspective(int x, int y, int z)
{
    glm::mat4 translate, scale;

    windowSizeChanged();
    glGetUniformLocation(g_programID, "MVP");
    g_projectionMat = glm::perspective
    (
        g_FOW,
        // FOV
        float(g_screenW) / float(g_screenH),
        // Aspect ratio
        g_minRenderDistance, g_maxRenderDistance
        // Display range (0.1 -> 100 units)
    );
    g_viewMat = glm::lookAt
    (
        glm::vec3(x, y, z),
        // Camera is at (4,3,3), in World Space
        glm::vec3(0, 0, 0),
        // Camera looks at the origin
        glm::vec3(0, 1, 0)
        // Head is up (set to 0,-1,0 to look upside-down)
    );

    /*
    g_modelMat = glm::mat4(1.0f); // Model matrix: an identity matrix
                                  // (model will be at the origin)
    */

    translate = glm::translate(glm::mat4(1.0f), glm::vec3(g_cubeX, g_cubeY, g_cubeZ));
    scale = glm::scale(glm::mat4(1.0f), glm::vec3(g_cubeScale));

    g_modelMat = translate * scale;

    g_MVP = g_projectionMat * g_viewMat * g_modelMat; // ModelViewProjection matrix
                                                      // NOTE:
                                                      //    Multiplication effects
                                                      //    go from right to left.
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Event handling.
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void eventHeartbeat()
{
    if(g_moveDirection & MOVE_FORWARD)
        moveCamera(MOVE_FORWARD, g_timeElapsed * g_cameraSpeed);
    if(g_moveDirection & MOVE_BACKWARD)
        moveCamera(MOVE_BACKWARD, g_timeElapsed * g_cameraSpeed);
    if(g_moveDirection & MOVE_RIGHT)
        moveCamera(MOVE_RIGHT, g_timeElapsed * g_cameraSpeed);
    if(g_moveDirection & MOVE_LEFT)
        moveCamera(MOVE_LEFT, g_timeElapsed * g_cameraSpeed);
}

void testOpenGLMainKeyboardPress(int key)
{
    //printf("Time: %f\n", g_timeElapsed);
    switch(key)
    {
        case UP:
            g_moveDirection += MOVE_FORWARD;
            break;
        case DOWN:
            g_moveDirection += MOVE_BACKWARD;
            break;
        case RIGHT:
            g_moveDirection += MOVE_RIGHT;
            break;
        case LEFT:
            g_moveDirection += MOVE_LEFT;
            break;
        case W:
            g_moveDirection += MOVE_FORWARD;
            break;
        case S:
            g_moveDirection += MOVE_BACKWARD;
            break;
        case D:
            g_moveDirection += MOVE_RIGHT;
            break;
        case A:
            g_moveDirection += MOVE_LEFT;
            break;
    }
}

void testOpenGLMainKeyboardRelease(int key)
{
    //printf("Time: %f\n", g_timeElapsed);
    switch(key)
    {
        case UP:
            if(g_moveDirection & MOVE_FORWARD) g_moveDirection -= MOVE_FORWARD;
            break;
        case DOWN:
            if(g_moveDirection & MOVE_BACKWARD) g_moveDirection -= MOVE_BACKWARD;
            break;
        case RIGHT:
            if(g_moveDirection & MOVE_RIGHT) g_moveDirection -= MOVE_RIGHT;
            break;
        case LEFT:
            if(g_moveDirection & MOVE_LEFT) g_moveDirection -= MOVE_LEFT;
            break;
        case W:
            if(g_moveDirection & MOVE_FORWARD) g_moveDirection -= MOVE_FORWARD;
            break;
        case S:
            if(g_moveDirection & MOVE_BACKWARD) g_moveDirection -= MOVE_BACKWARD;
            break;
        case D:
            if(g_moveDirection & MOVE_RIGHT) g_moveDirection -= MOVE_RIGHT;
            break;
        case A:
            if(g_moveDirection & MOVE_LEFT) g_moveDirection -= MOVE_LEFT;
            break;
    }
}

void testOpenGLMainMouseMove(double x, double y)
{
    double dx = x - g_screenW / 2,
           dy = y - g_screenH / 2;

    glfwSetCursorPos
    (
        g_sys->getGLFWWindow(),
        g_screenW / 2,
        g_screenH / 2
    );

    turnCameraBy
    (
        g_mouseSensitivity * dx,
        g_mouseSensitivity * dy
    );
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Draw.
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void drawObjects(int numFloats)
{
    glUseProgram(g_programID);

    glUniformMatrix4fv(g_matrixID, 1, GL_FALSE, &(g_MVP[0][0]));

    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, g_vertexBuffer);
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

    glDrawArrays(GL_TRIANGLES, 0, numFloats);

    glDisableVertexAttribArray(0);
}

static void drawColoredObjects(int numFloats)
{
    glUseProgram(g_programID);

    glUniformMatrix4fv(g_matrixID, 1, GL_FALSE, &(g_MVP[0][0]));

    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, g_vertexBuffer);
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

    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, g_colorBuffer);
    glVertexAttribPointer
    (
        1,                  // attribute 1. No particular reason for 1,
                            // but must match the layout in the shader.
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );

    glDrawArrays(GL_TRIANGLES, 0, numFloats);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Main, init and cleanup.
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void init()
{
    acquireSystemHandle();
    // Test shaders:
    //   loadShaders("shaders/test.vertexshader", "shaders/test.fragmentshader");
    loadShaders("shaders/test/main.vertexshader", "shaders/test/main.fragmentshader");
    initPerspective(g_camX, g_camY, g_camZ);
    generateVertexBuffer(g_cube, sizeof(g_cube));
    generateColorBuffer(g_cubeColors, sizeof(g_cubeColors));
    generateVertexArray();
    g_sys->toggleMouseControl(true);
    //To hide the cursor:
    //   glfwSetInputMode(g_sys->getGLFWWindow(), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    //Since we are only using this program, we can call glUseProgram once.
    //glUseProgram(g_programID);

    g_initialized = true;
}

static void cleanup()
{
    glDeleteBuffers(1, &g_vertexBuffer);
	glDeleteVertexArrays(1, &g_vertexArrayID);
}

static void testInTest()
{

}

void testOpenGLMain(double timeElapsed)
{
    g_timeElapsed = timeElapsed;
    if(!g_initialized)
        init();
    if(windowSizeChanged())
        handleWindowSizeChange();
    drawColoredObjects(sizeof(g_cube) / sizeof(GL_FLOAT));
    eventHeartbeat();
    testInTest();
}



//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//========================================================================================================================================================================================================
//Functions and variables not currently in use.
//========================================================================================================================================================================================================
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static const GLfloat
g_triangle[] =
{
    -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    0.0f,  1.0f, 0.0f,
};

static void glideCamera()
{
    static int ticksPassed = 0, moveDirection = 1;
    changeCameraPosition(ticksPassed, 125, 110);
    if(ticksPassed == 200)
        moveDirection = -1;
    else if(ticksPassed == -200)
        moveDirection = 1;
    ticksPassed += moveDirection;
}

static void vectorTranslation()
{
    //Create translation matrix
    //1 0 0 10
    //0 1 0 0
    //0 0 1 0
    //0 0 0 1
    glm::mat4 myMatrix = glm::translate(glm::mat4(),
                                        glm::vec3(10.0f, 0.0f, 0.0f));
    glm::vec4 myVector(10.0f, 10.0f, 10.0f, 0.0f);
    glm::vec4 transformedVector = myMatrix * myVector;
    printf
    (
        "%f %f %f %f\n",
        transformedVector.x,
        transformedVector.y,
        transformedVector.z,
        transformedVector.w
    );
}

static void vectorScaling()
{
    //Create scaling matrix
    //2 0 0 0
    //0 2 0 0
    //0 0 2 0
    //0 0 0 1
    glm::mat4 myMatrix = glm::scale(2.0f, 2.0f, 2.0f);
    glm::vec4 myVector(10.0f, 10.0f, 10.0f, 0.0f);
    glm::vec4 transformedVector = myMatrix * myVector;
    printf
    (
        "%f %f %f %f\n",
        transformedVector.x,
        transformedVector.y,
        transformedVector.z,
        transformedVector.w
    );
}
