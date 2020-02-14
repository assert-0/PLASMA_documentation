#include <engine.h>

#include <cstdio>

//System handle

static SimulationEngine g_sim;

SimulationEngine *getSimulationEngine(){return &g_sim;}

//simulation engine methods

SimulationEngine::SimulationEngine(){init();}

void SimulationEngine::setHomogenousMagneticField(TYPE n)
{
    BFieldX.resize(spaceResX);
    BFieldY.resize(spaceResX);
    BFieldZ.resize(spaceResX);
    for(int a = 0; a < spaceResX; a++)
    {
        BFieldX[a].resize(spaceResY);
        BFieldY[a].resize(spaceResY);
        BFieldZ[a].resize(spaceResY);
        for(int b = 0; b < spaceResY; b++)
        {
            BFieldX[a][b].resize(spaceResZ);
            BFieldY[a][b].resize(spaceResZ);
            BFieldZ[a][b].resize(spaceResZ);
            for(int c = 0; c < spaceResZ; c++)
            {
                BFieldX[a][b][c] = n;
                BFieldY[a][b][c] = n;
                BFieldZ[a][b][c] = n;
            }
        }
    }
}

void SimulationEngine::setHomogenousElectricField(TYPE n)
{
    EFieldX.resize(spaceResX);
    EFieldY.resize(spaceResX);
    EFieldZ.resize(spaceResX);
    for(int a = 0; a < spaceResX; a++)
    {
        EFieldX[a].resize(spaceResY);
        EFieldY[a].resize(spaceResY);
        EFieldZ[a].resize(spaceResY);
        for(int b = 0; b < spaceResY; b++)
        {
            EFieldX[a][b].resize(spaceResZ);
            EFieldY[a][b].resize(spaceResZ);
            EFieldZ[a][b].resize(spaceResZ);
            for(int c = 0; c < spaceResZ; c++)
            {
                EFieldX[a][b][c] = n;
                EFieldY[a][b][c] = n;
                EFieldZ[a][b][c] = n;
            }
        }
    }
}

void SimulationEngine::calculate()
{
    reset(numParticles);

	for (double t = 0; t < timeCap; t += timeIncrement){

		calcDistMat(particles, distXMat, distYMat, distZMat, distMat);

		calcEPFieldMat(particles, ExMat, EyMat, EzMat, EPotMat);

		calcBPFieldMat(particles, BPxMat, BPyMat, BPzMat);

        checkNAN(particles);

		calcPAcceleration(particles, ExMat, EyMat, EzMat, BPxMat, BPyMat, BPzMat);

		checkNAN(particles);

		calcAccelerationExternal(particles, EFieldX, EFieldY, EFieldZ, BFieldX, BFieldY, BFieldZ);

		checkNAN(particles);

		for (int i = 0; i < numParticles; i++){

			for (int j = 0; j < numParticles; j++){

				if (j > i){

					if (distMat[i][j] <= particles[i].radius + particles[j].radius){

						twoCollision(particles, i, j);
					}
				}
			}
		}

		checkNAN(particles);

		calcNewPosVel(particles);

		checkNAN(particles);

		for (int i = 0; i < numParticles; ++i){

			boxCollision(particles, i);
		}

		log.push_back(particles);

		printf("Calculation %g%% done...\n", t / timeCap * 100);

		clearParticleFields(particles);
	}
}

void SimulationEngine::init()
{
    spaceResX = 10;
    spaceResY = 10;
    spaceResZ = 10;
}

void SimulationEngine::reset(int newNum)
{
    if(newNum != particles.size())
    {
        numParticles = newNum;
        particles.resize(numParticles);
        resetSpaceFields(spaceResX, spaceResY, spaceResZ, EFieldX, EFieldY, EFieldZ, BFieldX, BFieldY, BFieldZ);
        resetMatrices(numParticles, distXMat, distYMat, distZMat, distMat, ExMat, EyMat, EzMat, EPotMat, BPxMat, BPyMat, BPzMat);
    }
    else
        ZeroMemory(&particles.front(), numParticles * sizeof(Particle));
    setParticlePosVel(particles);
	clearParticleFields(particles);

	timeCap = numFrames * timeIncrement;
}
