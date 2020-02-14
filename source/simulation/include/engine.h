#pragma once

#include <vector>

#include <coil.h>
#include <particle.h>
#include <constants.h>

using namespace std;

//Main class


class SimulationEngine
{
public:
    SimulationEngine();
    ~SimulationEngine(){}

    //main calculation function
    void calculate();

    //particle number control
    int  getNumParticles()     {return numParticles;}
    void setNumParticles(int n){reset(n);}

    //frame number control
    int  getNumFrames()     {return numFrames;}
    void setNumFrames(int n){numFrames = n;}

    //space scale control
    TYPE getSpaceScale()      {return spaceScale;}
    void setSpaceScale(TYPE n){spaceScale = n;}

    //initial temperature control
    TYPE getInitialTemperature()      {return initialTemperature;}
    void setInitialTemperature(TYPE n){initialTemperature = n;}

    //time increment control
    TYPE getTimeIncrement()      {return timeIncrement;}
    void setTimeIncrement(TYPE n){timeIncrement = n;}

    //particle charge control
    TYPE getParticleCharge()      {return particleCharge;}
    void setParticleCharge(TYPE n){particleCharge = n;}

    //particle mass control
    TYPE getParticleMass()      {return particleMass;}
    void setParticleMass(TYPE n){particleMass = n;}

    void setHomogenousMagneticField(TYPE n);

    void setHomogenousElectricField(TYPE n);

    vector<vector<Particle>> &getLog(){return log;}

private:
    //initialization function
    void init();
    //prepare engine data members
    void reset(int numParticles);

    //two-particle collision
    void twoCollision(vector<Particle>& particles, int i, int j);

    //collision with one of the sides of the box/containment field
    void boxCollision(vector<Particle>& particles, int i);

    //random particle velocity and position generation
    void setParticlePosVel(vector<Particle>& particles);

    //resets matrices in case of changing particles number and at the initialization
    void resetMatrices
    (
        int numParticles,
        vector<vector<float>>& distXMat,
        vector<vector<float>>& distYMat,
        vector<vector<float>>& distZMat,
        vector<vector<float>>& distMat,
        vector<vector<float>>& ExMat,
        vector<vector<float>>& EyMat,
        vector<vector<float>>& EzMat,
        vector<vector<float>>& EPotMat,
        vector<vector<float>>& BPxMat,
        vector<vector<float>>& BPyMat,
        vector<vector<float>>& BPzMat
    );

    //reseting accelerations, electric fields and magnetic fields as well as acceleration

    void clearParticleFields(vector<Particle>& particles);
    void resetSpaceFields
    (
        const int spaceResX,
		const int spaceResY,
		const int spaceResZ,
		vector<vector<vector<float>>>& EFieldX,
		vector<vector<vector<float>>>& EFieldY,
		vector<vector<vector<float>>>& EFieldZ,
		vector<vector<vector<float>>>& BFieldX,
		vector<vector<vector<float>>>& BFieldY,
		vector<vector<vector<float>>>& BFieldZ
    );


    //main calculations


    //calculating the distance vector and absolute distance
    void calcDistMat
    (
        vector<Particle>& particles,
		vector<vector<float>>& distXMat,
		vector<vector<float>>& distYMat,
		vector<vector<float>>& distZMat,
		vector<vector<float>>& distMat
    );

    //calculating electric fields of every particles to the other one
    void calcEPFieldMat
    (
		vector<Particle>& particles,
		vector<vector<float>>& ExMat,
		vector<vector<float>>& EyMat,
		vector<vector<float>>& EzMat,
		vector<vector<float>>& EPotMat
    );

    //calculating particles magnetic fields on other particles
    void calcBPFieldMat
    (
		vector<Particle>& particles,
		vector<vector<float>>& BPxMat,
		vector<vector<float>>& BPyMat,
		vector<vector<float>>& BPzMat
    );

    //finding acceleration and fields for particles from particles
    void calcPAcceleration
    (
		vector<Particle>& particles,
		vector<vector<float>>& ExMat,
		vector<vector<float>>& EyMat,
		vector<vector<float>>& EzMat,
		vector<vector<float>>& BPxMat,
		vector<vector<float>>& BPyMat,
		vector<vector<float>>& BPzMat
    );

    //acceleration from external fields
    void calcAccelerationExternal
    (
		vector<Particle>& particles,
		vector<vector<vector<float>>>& EFieldX,
		vector<vector<vector<float>>>& EFieldY,
		vector<vector<vector<float>>>& EFieldZ,
		vector<vector<vector<float>>>& BFieldX,
		vector<vector<vector<float>>>& BFieldY,
		vector<vector<vector<float>>>& BFieldZ
    );

    //position and velocity change between frames
    void calcNewPosVel(vector<Particle>& particles);

    //resets NAN floats
    void checkNAN(vector<Particle>& particles);


    //variables


    long long nextParticleUUID;

    int numParticles;
    int numFrames;
    TYPE spaceScale;
    TYPE initialTemperature;
    TYPE timeIncrement;
    TYPE timeCap;

    TYPE particleCharge;
    TYPE particleMass;

    vector<Particle> particles;
    vector<vector<Particle>> log;

    vector<vector<float>> distXMat;
    vector<vector<float>> distYMat;
    vector<vector<float>> distZMat;
    vector<vector<float>> distMat;

    vector<vector<float>> ExMat;
    vector<vector<float>> EyMat;
    vector<vector<float>> EzMat;
    vector<vector<float>> EPotMat;

    vector<vector<float>> BPxMat;
    vector<vector<float>> BPyMat;
    vector<vector<float>> BPzMat;

    //external field parameters

    //currently not used
    //vector<Coil> coils;

    int numCoils;
    int spaceResX;
    int spaceResY;
    int spaceResZ;

    vector<vector<vector<float>>> BFieldX;
    vector<vector<vector<float>>> BFieldY;
    vector<vector<vector<float>>> BFieldZ;

    vector<vector<vector<float>>> EFieldX;
    vector<vector<vector<float>>> EFieldY;
    vector<vector<vector<float>>> EFieldZ;
};

SimulationEngine *getSimulationEngine();
