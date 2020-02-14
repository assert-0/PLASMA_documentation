#include <engine.h>

#include <cstdio>

#if TYPE == double
    #define CHECK_NAN(x) isnan(x)
#else
    #define CHECK_NAN(x) isnanf(x)
#endif // TYPE


//calculating the distance vector and absolute distance
void SimulationEngine::calcDistMat(
		vector<Particle>& particles,
		vector<vector<float>>& distXMat,
		vector<vector<float>>& distYMat,
		vector<vector<float>>& distZMat,
		vector<vector<float>>& distMat){

	for (int i=0; i<numParticles; ++i){

		for (int j=0; j<numParticles; ++j){

			if (j>i){

				distXMat[i][j]=particles[j].positionX-particles[i].positionX;
				distXMat[j][i]=-distXMat[i][j];

				distYMat[i][j]=particles[j].positionY-particles[i].positionY;
				distYMat[j][i]=-distYMat[i][j];

				distZMat[i][j]=particles[j].positionZ-particles[i].positionZ;
				distZMat[j][i]=-distZMat[i][j];

				distMat[i][j]=sqrt(distXMat[i][j]*distXMat[i][j]+distYMat[i][j]*distYMat[i][j]+distZMat[i][j]*distZMat[i][j]);
				distMat[j][i]=distMat[i][j];
			}

			else if (i==j){

				distXMat[i][j]=0.0; distYMat[i][j]=0.0; distZMat[i][j]=0.0; distMat[i][j]=0.0;
			}
		}
	}
}

//calculating electric fields of every particles to the other one
void SimulationEngine::calcEPFieldMat(
		vector<Particle>& particles,
		vector<vector<float>>& ExMat,
		vector<vector<float>>& EyMat,
		vector<vector<float>>& EzMat,
		vector<vector<float>>& EPotMat){

	for (int i=0; i<numParticles; ++i){

		for (int j=0; j<numParticles; ++j){

			if (j>i){

				TYPE temp=K*particles[i].charge/(distMat[i][j]*distMat[i][j]);

				EPotMat[i][j]=temp*distMat[i][j];

				ExMat[i][j]=temp*distXMat[j][i]/distMat[i][j];
				ExMat[j][i]=-ExMat[i][j];

				EyMat[i][j]=temp*distYMat[j][i]/distMat[i][j];
				EyMat[j][i]=-EyMat[i][j];

				EzMat[i][j]=temp*distZMat[j][i]/distMat[i][j];
				EzMat[j][i]=-EzMat[i][j];
			}

			else if (i==j){

				ExMat[i][j]=0.0; EyMat[i][j]=0.0; EzMat[i][j]=0.0; EPotMat[i][j]=0.0;
			}
		}
	}
}

//calculating particles magnetic fields on other particles
void SimulationEngine::calcBPFieldMat(
		vector<Particle>& particles,
		vector<vector<float>>& BPxMat,
		vector<vector<float>>& BPyMat,
		vector<vector<float>>& BPzMat){

	for (int i=0; i<numParticles; ++i){

		for (int j=0; j<numParticles; ++j){

			if (j>i){

				BPxMat[i][j]=Mi*particles[j].charge*(particles[j].velocityY*distZMat[i][j]-particles[j].velocityZ*distYMat[i][j])/(distMat[i][j]*distMat[i][j]*distMat[i][j]);
				BPxMat[j][i]=-BPxMat[i][j];

				BPyMat[i][j]=Mi*particles[j].charge*(particles[j].velocityZ*distXMat[i][j]-particles[j].velocityX*distZMat[i][j])/(distMat[i][j]*distMat[i][j]*distMat[i][j]);
				BPyMat[j][i]=-BPyMat[i][j];

				BPzMat[i][j]=Mi*particles[j].charge*(particles[j].velocityX*distYMat[i][j]-particles[j].velocityY*distXMat[i][j])/(distMat[i][j]*distMat[i][j]*distMat[i][j]);
				BPzMat[j][i]=-BPzMat[i][j];
			}

			else if (i==j){

				BPxMat[i][j]=0.0; BPyMat[i][j]=0.0; BPzMat[i][j]=0.0;
			}
		}
	}
}

//finding acceleration and fields for particles from particles
void SimulationEngine::calcPAcceleration(
		vector<Particle>& particles,
		vector<vector<float>>& ExMat,
		vector<vector<float>>& EyMat,
		vector<vector<float>>& EzMat,
		vector<vector<float>>& BPxMat,
		vector<vector<float>>& BPyMat,
		vector<vector<float>>& BPzMat){

	for (int i=0; i<numParticles; ++i){

		for (int j=0; j<numParticles; ++j){

			particles[i].EFieldX+=ExMat[i][j];
			particles[i].EFieldY+=EyMat[i][j];
			particles[i].EFieldY+=EzMat[i][j];

			particles[i].EPotential+=EPotMat[i][j];

			particles[i].BPFieldX+=BPxMat[i][j];
			particles[i].BPFieldY+=BPyMat[i][j];
			particles[i].BPFieldZ+=BPzMat[i][j];

			particles[i].accX+=((ExMat[i][j]+particles[i].velocityY*(BPzMat[i][j])-particles[i].velocityZ*(BPyMat[i][j]))*particles[i].charge)/particles[i].mass;
			particles[i].accY+=((EyMat[i][j]+particles[i].velocityZ*(BPxMat[i][j])-particles[i].velocityX*(BPzMat[i][j]))*particles[i].charge)/particles[i].mass;
			particles[i].accZ+=((EzMat[i][j]+particles[i].velocityX*(BPyMat[i][j])-particles[i].velocityZ*(BPxMat[i][j]))*particles[i].charge)/particles[i].mass;
		}
	}
}

//acceleration from external fields
void SimulationEngine::calcAccelerationExternal(
		vector<Particle>& particles,
		vector<vector<vector<float>>>& EFieldX,
		vector<vector<vector<float>>>& EFieldY,
		vector<vector<vector<float>>>& EFieldZ,
		vector<vector<vector<float>>>& BFieldX,
		vector<vector<vector<float>>>& BFieldY,
		vector<vector<vector<float>>>& BFieldZ){

	int subsetX, subsetY, subsetZ;

	subsetX = int(floor(particles[0].positionX / spaceScale * (spaceResX - 1)));
	subsetY = int(floor(particles[0].positionY / spaceScale * (spaceResY - 1)));
	subsetZ = int(floor(particles[0].positionZ / spaceScale * (spaceResZ - 1)));

	for (int i=0; i<numParticles; ++i){

		subsetX = int(floor(particles[i].positionX / spaceScale * (spaceResX - 1)));
		subsetY = int(floor(particles[i].positionY / spaceScale * (spaceResY - 1)));
		subsetZ = int(floor(particles[i].positionZ / spaceScale * (spaceResZ - 1)));

		TYPE Ex=EFieldX[subsetX][subsetY][subsetZ];
		TYPE Ey=EFieldY[subsetX][subsetY][subsetZ];
		TYPE Ez=EFieldZ[subsetX][subsetY][subsetZ];
		TYPE Bx=BFieldX[subsetX][subsetY][subsetZ];
		TYPE By=BFieldY[subsetX][subsetY][subsetZ];
		TYPE Bz=BFieldZ[subsetX][subsetY][subsetZ];

		particles[i].accX+=((Ex+particles[i].velocityY*Bz-particles[i].velocityZ*By)*particles[i].charge)/particles[i].mass;
		particles[i].accY+=((Ey+particles[i].velocityZ*Bx-particles[i].velocityX*Bz)*particles[i].charge)/particles[i].mass;
		particles[i].accZ+=((Ez+particles[i].velocityX*By-particles[i].velocityZ*Bx)*particles[i].charge)/particles[i].mass;
	}
}

//position and velocity change between frames
void SimulationEngine::calcNewPosVel(vector<Particle>& particles){

	for (int i=0; i<numParticles; ++i){

		particles[i].positionX+=particles[i].velocityX*timeIncrement+particles[i].accX*timeIncrement*timeIncrement/2;
		particles[i].positionY+=particles[i].velocityY*timeIncrement+particles[i].accY*timeIncrement*timeIncrement/2;
		particles[i].positionZ+=particles[i].velocityZ*timeIncrement+particles[i].accZ*timeIncrement*timeIncrement/2;

		particles[i].velocityX+=particles[i].accX*timeIncrement;
		particles[i].velocityY+=particles[i].accY*timeIncrement;
		particles[i].velocityZ+=particles[i].accZ*timeIncrement;
		particles[i].calcVelocityT();
	}
}

void SimulationEngine::checkNAN(vector<Particle>& particles)
{
    for (int i=0; i<numParticles; ++i){

        if(CHECK_NAN(particles[i].velocityX))
            particles[i].velocityX = 0;
        if(CHECK_NAN(particles[i].velocityY))
            particles[i].velocityY = 0;
        if(CHECK_NAN(particles[i].velocityZ))
            particles[i].velocityZ = 0;
        if(CHECK_NAN(particles[i].accX))
            particles[i].accX = 0;
        if(CHECK_NAN(particles[i].accY))
            particles[i].accY = 0;
        if(CHECK_NAN(particles[i].accZ))
            particles[i].accZ = 0;
    }
}
