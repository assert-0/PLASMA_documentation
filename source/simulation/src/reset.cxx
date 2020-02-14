#include <engine.h>

//Resets matrices in case of changing particles number and at the initialization

void SimulationEngine::resetMatrices(
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
		vector<vector<float>>& BPzMat){

	distXMat.resize(0);
	distYMat.resize(0);
	distZMat.resize(0);
	distMat.resize(0);

	ExMat.resize(0);
	EyMat.resize(0);
	EzMat.resize(0);
	EPotMat.resize(0);

	BPxMat.resize(0);
	BPyMat.resize(0);
	BPzMat.resize(0);

	distXMat.resize(numParticles);
	distYMat.resize(numParticles);
	distZMat.resize(numParticles);
	distMat.resize(numParticles);

	ExMat.resize(numParticles);
	EyMat.resize(numParticles);
	EzMat.resize(numParticles);
	EPotMat.resize(numParticles);

	BPxMat.resize(numParticles);
	BPyMat.resize(numParticles);
	BPzMat.resize(numParticles);


	for (int i=0; i<numParticles; ++i){

		distXMat[i].resize(numParticles);
		distYMat[i].resize(numParticles);
		distZMat[i].resize(numParticles);
		distMat[i].resize(numParticles);

		ExMat[i].resize(numParticles);
		EyMat[i].resize(numParticles);
		EzMat[i].resize(numParticles);
		EPotMat[i].resize(numParticles);

		BPxMat[i].resize(numParticles);
		BPyMat[i].resize(numParticles);
		BPzMat[i].resize(numParticles);
	}
}

//reseting accelerations, electric fields and magnetic fields as well as acceleration
void SimulationEngine::clearParticleFields(vector<Particle>& particles){

	int length=particles.size();

	for (int i=0; i<length; ++i){

		particles[i].accX=0.0;
		particles[i].accY=0.0;
		particles[i].accZ=0.0;

		particles[i].EPFieldX=0.0;
		particles[i].EPFieldY=0.0;
		particles[i].EPFieldZ=0;

		particles[i].EFieldX=0.0;
		particles[i].EFieldY=0.0;
		particles[i].EFieldZ=0.0;
		particles[i].EPotential=0.0;

		particles[i].BPFieldX=0.0;
		particles[i].BPFieldY=0.0;
		particles[i].BPFieldZ=0.0;

		particles[i].BFieldX=0.0;
		particles[i].BFieldY=0.0;
		particles[i].BFieldZ=0.0;
	}
}

void SimulationEngine::resetSpaceFields(
		const int spaceResX,
		const int spaceResY,
		const int spaceResZ,
		vector<vector<vector<float>>>& EFieldX,
		vector<vector<vector<float>>>& EFieldY,
		vector<vector<vector<float>>>& EFieldZ,
		vector<vector<vector<float>>>& BFieldX,
		vector<vector<vector<float>>>& BFieldY,
		vector<vector<vector<float>>>& BFieldZ){

	EFieldX.resize(0);
	EFieldY.resize(0);
	EFieldZ.resize(0);

	BFieldX.resize(0);
	BFieldY.resize(0);
	BFieldZ.resize(0);

	EFieldX.resize(spaceResX);
	EFieldY.resize(spaceResX);
	EFieldZ.resize(spaceResX);

	BFieldX.resize(spaceResX);
	BFieldY.resize(spaceResX);
	BFieldZ.resize(spaceResX);

	for (int i=0; i<spaceResX; ++i){

		EFieldX[i].resize(spaceResY);
		EFieldY[i].resize(spaceResY);
		EFieldZ[i].resize(spaceResY);

		BFieldX[i].resize(spaceResY);
		BFieldY[i].resize(spaceResY);
		BFieldZ[i].resize(spaceResY);

		for (int j=0; j<spaceResY; ++j){

			EFieldX[i][j].resize(spaceResZ);
			EFieldY[i][j].resize(spaceResZ);
			EFieldZ[i][j].resize(spaceResZ);

			BFieldX[i][j].resize(spaceResZ);
			BFieldY[i][j].resize(spaceResZ);
			BFieldZ[i][j].resize(spaceResZ);
		}
	}
}
