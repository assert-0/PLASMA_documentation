#pragma once

#include <vector>
#include <string>

struct Particle
{
    int particleID;
    float mass;
    float charge;
    float radius;
    float inherentMagneticMoment;
    float ionisationEnergy;

    float positionX, positionY, positionZ;
    float velocityX, velocityY, velocityZ, velocityT;
    float accX, accY, accZ;

    float EPFieldX, EPFieldY, EPFieldZ;
    float EFieldX, EFieldY, EFieldZ;
    float EPotential;

    float BPFieldX, BPFieldY, BPFieldZ;
    float BFieldX, BFieldY, BFieldZ;

    float magneticMomentX, magneticMomentY, magneticMomentZ;
};

class SimulationEngine
{
public:
    void initSimulation();

    void loadDataFromFile(std::string path);

    void start(){started = true;}

    void stop(){started = false;}

    bool isStarted(){return started;}

    void update(double timeElapsed);

    void setFrameToPosition(int pos);

private:
    void changeFrame();

    void setSliderPosition(int pos);

    void alocateParticleModels();

    void dealocateParticleModels();

    std::vector< std::vector<Particle> > frames;
    std::vector<int> particleModelIDs;
    int cageID;
    int currentPos;
    bool started;
};

SimulationEngine *getSimulationEngine();
