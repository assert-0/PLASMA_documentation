#include <iostream>
#include <fstream>
#include <cstdio>
#include <cmath>
#include <random>
#include <vector>
#include <algorithm>
#include <set>
#include <windows.h>
#include <chrono>
#include <functional>

using namespace std;

#define Type double

//physical constants
const double Pi=3.14159265357989323;
const double Mi=1e-7;
const double K=8.987551788e9;
const double kB=1.380649e-23;
const double u=1.660539066605e-27;
const double e=1.602176634e-19;
const double me=9.10938370153e-31;
const double MiN=5.087535096e-27;

//importing classes
#include "Particle.h"
#include "Coil.h"

//External field parameters

vector<Coil> coils;

int numCoils=1;
int spaceResX=10;
int spaceResY=10;
int spaceResZ=10;

vector<vector<vector<float>>> BFieldX;
vector<vector<vector<float>>> BFieldY;
vector<vector<vector<float>>> BFieldZ;

vector<vector<vector<float>>> EFieldX;
vector<vector<vector<float>>> EFieldY;
vector<vector<vector<float>>> EFieldZ;


//constant parameters
int numParticles=2;
const Type spaceScale=2e-8;
const Type initialTemperature=1000;
const Type timeIncrement=2e-19;
int numFrames=30*15;
float timeCap=timeIncrement*numFrames;


//particle properties
long long particleCnt=0;

const float PlusSpecies=1.0;
const float MinusSpecies=0.0;
const float NeutralSpecies=0.0;

const Type PlusMass=1.007825*u;
const Type PlusCharge=1.0*e;
const Type PlusMoment=2.79284735089*MiN;
const Type PlusRadius=0.55e-10;

const Type MinusMass=u/1856;
const Type MinusCharge=-1.0*e;
const Type MinusMoment=5*MiN;
const Type MinusRadius=1.0e-14;

const Type NeutralMass=1.008*u;
const Type NeutralMoment=MiN;
const Type NeutralRadius=0.7e-10;


//main variables
vector <Particle> particle(numParticles);
vector <vector<Particle>> Log;


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

//importing functions
#include "Reset.h"
#include "RandDist.h"
#include "Collision.h"
#include "DataExtraction.h"
#include "SimCalc.h"

//file to write to
const char *dumpFilename = "calc_data.pcd";

int main(){

	tp.resize(12);

	SetParticlePosVel(particle);
	ResetParticleFields(particle);
	ResetSpaceFields(spaceResX, spaceResY, spaceResZ, EFieldX, EFieldY, EFieldZ, BFieldX, BFieldY, BFieldZ);
	SetMatrices(numParticles, distXMat, distYMat, distZMat, distMat, ExMat, EyMat, EzMat, EPotMat, BPxMat, BPyMat, BPzMat);



	particle[0].positionX = 0;
	particle[0].positionY = 0;
	particle[0].positionZ = 0.5;

	particle[1].positionX = 0;
	particle[1].positionY = 0;
	particle[1].positionZ = -0.5;



	for (double t=0; t<timeCap; t+=timeIncrement){

		/*
		CalcDistMat(particle, distXMat, distYMat, distZMat, distMat);

		CalcEPFieldMat(particle, ExMat, EyMat, EzMat, EPotMat);

		CalcBPFieldMat(particle, BPxMat, BPyMat, BPzMat);

		CalcPAcceleration(particle, ExMat, EyMat, EzMat, BPxMat, BPyMat, BPzMat);

		CalcAccelerationExternal(particle, EFieldX, EFieldY, EFieldZ, BFieldX, BFieldY, BFieldZ);

		for (int i=0; i<numParticles; ++i){

			BoxCollision(particle, i);
		}
		for (int i=0; i<numParticles; i++){

			for (int j=0; j<numParticles; j++){

				if (j>i){

					if (distMat[i][j]<=particle[i].radius+particle[j].radius){

						TwoCollision(particle, i, j);
					}
				}
			}
		}

		CalcNewPosVel(particle);
		*/

		particle[0].positionZ += 0.01;
		particle[1].positionZ += -0.01;

		printf
		(
			"%g %g %g %g %g %g\n",
			particle[0].positionX,
			particle[0].positionY,
			particle[0].positionZ, 
			particle[1].positionX,
			particle[1].positionY,
			particle[1].positionZ
		);

		Log.push_back(particle);

		printf("%g%% done...\n", t / timeCap * 100);

		//ResetParticleFields(particle);
	}



	std::ofstream ostr(dumpFilename, std::ios::binary);
	ostr.write((const char*)&numFrames, sizeof(int));
	ostr.write((const char*)&numParticles, sizeof(int));
	for(int a = 0; a < Log.size(); a++)
	{
		ostr.write((const char*) &Log[a].front(), sizeof(Particle) * Log[a].size());
	}
	

	return 0;
}
