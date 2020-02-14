#pragma once

#include "object.h"
#include "runtime.h"

//Renderer declaration

class Renderer
{
public:
    friend Runtime;

    Renderer();
    ~Renderer();

    void renderSingleObject(Object *obj);

    void renderMultipleObjects(Object *objAr, int count);

    void getCameraPosition(float *x, float *y, float *z)
    {
        *x = camX;
        *y = camY;
        *z = camZ;
    }

    void setCameraPosition(float x, float y, float z)
    {
        camX = x;
        camY = y;
        camZ = z;
        recalculateMatrices();
    }

    void getCameraAngles(double *vertical, double *horizontal)
    {
        *vertical = camVerAngle;
        *horizontal = camHorAngle;
    }

    void setCameraAngles(double vertical, double horizontal)
    {
        camVerAngle = vertical;
        camHorAngle = horizontal;
        recalculateMatrices();
    }

    void getFOV(float *FOV){FOV = &this->FOV;}

    void setFOV(float FOV){this->FOV = FOV; recalculateMatrices();}

    void getMaxrenderDistance(float *maxRenderDistance)
    {
        *maxRenderDistance = this->maxRenderDistance;
    }

    void setMaxrenderDistance(float maxRenderDistance)
    {
        this->maxRenderDistance = maxRenderDistance;
        recalculateMatrices();
    }

private:
    void recalculateMatrices();

    void initShaders();

    float camX, camY, camZ;
    double camVerAngle, camHorAngle;
    float FOV;
    float maxRenderDistance;
};

//Renderer getter
Renderer *getRenderer();
