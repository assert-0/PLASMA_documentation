#pragma once

#include <cmath>

#define rad(x) (((x) / 360.0) * PI2)

//Constants

const double PI = 3.14159265358979323846264338;
const double PI2 = PI * 2.0;

//Base FOV
const float g_baseFOV = rad(45.0);
//The modifier is multiplied by the slider offset
//to control the range of values the slider can achieve.
const float g_FOVModifier = 1.0;

//Base mouse horizontal and vertical speeds along with
//their modifiers

const double g_baseMouseSpeedH = 0.000025;
const double g_baseMouseSpeedV = 0.000025;
const double g_mouseSpeedHModifier = 5.0;
const double g_mouseSpeedVModifier = 5.0;

//Base keyboard movement speed and its modifier

const double g_baseKeyboardSpeed = 10.0;
const double g_keyboardSpeedModifier = 2.0;

//Also known as Z-far. This value controls the distance
//at which objects will no longer render.
const float g_maxRenderDistance = 10000000.0;

//Distance multiplier between real particles and particle models.
//(distance scale)
const double g_particleDistance = 1000000.0;
//Radius multiplier between real particles and particle models.
//(radius/size scale)
const double g_particleScale = 0.5;
