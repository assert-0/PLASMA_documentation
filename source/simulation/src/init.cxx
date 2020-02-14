#include <engine.h>

#include <random>

//auxiliary function; distances between particles for identifying conflicts
void calcDistances(vector<Particle>& particles, vector<vector<float>>& distances){

	int numParticles=particles.size();

	for (int i=0; i<numParticles; ++i){

		for (int j=0; j<numParticles; ++j){

			if (j>i){

				distances[i][j]=sqrt((particles[i].positionX-particles[j].positionX)*(particles[i].positionX-particles[j].positionX)*(particles[i].positionY-particles[j].positionY)+(particles[i].positionZ-particles[j].positionZ)*(particles[i].positionZ-particles[j].positionZ));
				distances[j][i]=distances[i][j];
			}

			else if (i==j){

				distances[i][j]=0.0;
			}
		}
	}
}

//random particle velocity and position generation
void SimulationEngine::setParticlePosVel(vector<Particle>& particles){

	int numParticles=particles.size();
	//rng variables
	random_device rd;
	mt19937_64 rng(rd());
	uniform_real_distribution<double> dist1(0.0, 1.0);
	uniform_real_distribution<double> dist2(0,3.0);
	uniform_real_distribution<double> dist3(0,2.0);

	for (int i=0; i<numParticles; i++){
		//basic properties - homogenous at this point
		particles[i].particleID=nextParticleUUID;
		nextParticleUUID++;
		particles[i].mass=particleMass;
		particles[i].radius=0.55e-10;
		particles[i].charge=particleCharge;
		particles[i].inherentMagneticMoment=2.79284735089*MiN;
		particles[i].ionisationEnergy=2.178e-18;
		//random position
		particles[i].positionX=dist1(rng)*spaceScale;
		particles[i].positionY=dist1(rng)*spaceScale;
		particles[i].positionZ=dist1(rng)*spaceScale;
		//general velocity (thermodynamically deterined) and priorities
		particles[i].velocityT=sqrt((3*kB*initialTemperature)/particles[i].mass);
		int tempPriority1=int(ceil(dist2(rng)));
		int tempPriority2=int(ceil(dist3(rng)));
		TYPE tempMod=1, tempMult=0, signModX, signModY, signModZ;
		//determining the signs of velocities
		TYPE tempSignX=dist3(rng);
		TYPE tempSignY=dist3(rng);
		TYPE tempSignZ=dist3(rng);

		if (tempSignX<=1){

			signModX=-1;
		}
		else{
			signModX=1;
		}

		if (tempSignY<=1){

			signModY=-1;
		}
		else{
			signModY=1;
		}

		if (tempSignZ<=1){

			signModZ=-1;
		}
		else{
			signModZ=1;
		}

		//calculating the first random velocity
		if (tempPriority1==1){

			tempMult=dist1(rng);
			particles[i].velocityX=signModX*tempMult*particles[i].velocityT;
			tempMod-=tempMult*tempMult;
		}
		else if (tempPriority1==2){

			tempMult=dist1(rng);
			particles[i].velocityY=signModY*tempMult*particles[i].velocityT;
			tempMod-=tempMult*tempMult;
		}
		else if (tempPriority1==3){

			tempMult=dist1(rng);
			particles[i].velocityZ=signModZ*tempMult*particles[i].velocityT;
			tempMod-=tempMult*tempMult;
		}
		//calculating the second and last random velocity
		if (tempPriority1==1 && tempPriority2==1){

			tempMult=dist1(rng)*sqrt(tempMod);
			particles[i].velocityY=tempMult*particles[i].velocityT;
			tempMod-=tempMult*tempMult;
			particles[i].velocityZ=sqrt(tempMod)*particles[i].velocityT;
		}
		else if (tempPriority1==1 && tempPriority2==2){

			tempMult=dist1(rng)*sqrt(tempMod);
			particles[i].velocityZ=tempMult*particles[i].velocityT;
			tempMod-=tempMult*tempMult;
			particles[i].velocityY=sqrt(tempMod)*particles[i].velocityT;
		}
		else if (tempPriority1==2 && tempPriority2==1){

			tempMult=dist1(rng)*sqrt(tempMod);
			particles[i].velocityX=signModX*tempMult*particles[i].velocityT;
			tempMod-=tempMult*tempMult;
			particles[i].velocityZ=signModZ*sqrt(tempMod)*particles[i].velocityT;
		}
		else if (tempPriority1==2 && tempPriority2==2){

			tempMult=dist1(rng)*sqrt(tempMod);
			particles[i].velocityZ=tempMult*particles[i].velocityT;
			tempMod-=tempMult*tempMult;
			particles[i].velocityX=sqrt(tempMod)*particles[i].velocityT;
		}
		else if (tempPriority1==3 && tempPriority2==1){

			tempMult=dist1(rng)*sqrt(tempMod);
			particles[i].velocityX=tempMult*particles[i].velocityT;
			tempMod-=tempMult*tempMult;
			particles[i].velocityY=sqrt(tempMod)*particles[i].velocityT;
		}
		else if (tempPriority1==3 && tempPriority2==2){

			tempMult=dist1(rng)*sqrt(tempMod);
			particles[i].velocityY=signModY*tempMult*particles[i].velocityT;
			tempMod-=tempMult*tempMult;
			particles[i].velocityX=signModX*sqrt(tempMod)*particles[i].velocityT;
		}
	}

	bool resolved=false;

	vector<vector<float>> distances;
	distances.resize(numParticles);

	for (int m=0; m<numParticles; ++m){

		distances[m].resize(numParticles);
	}

	while (resolved=false){

		resolved=true;

		calcDistances(particles, distances);

		for (int i=0; i<numParticles; ++i){

			for (int j=0; j<numParticles; ++j){

				if (j>i){

					if (distances[i][j]<(particles[i].radius+particles[j].radius)){

						resolved=false;
						particles[i].positionX=dist1(rng)*spaceScale;
						particles[i].positionY=dist1(rng)*spaceScale;
						particles[i].positionZ=dist1(rng)*spaceScale;
					}
				}
			}
		}
	}
}
