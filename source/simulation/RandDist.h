//auxiliary function; distances between particles for identifying conflicts
void CalcDistances(vector<Particle>& particle, vector<vector<float>>& distances){
	
	int numparticles=particle.size();
	
	for (int i=0; i<numparticles; ++i){
		
		for (int j=0; j<numparticles; ++j){
			
			if (j>i){
				
				distances[i][j]=sqrt((particle[i].positionX-particle[j].positionX)*(particle[i].positionX-particle[j].positionX)*(particle[i].positionY-particle[j].positionY)+(particle[i].positionZ-particle[j].positionZ)*(particle[i].positionZ-particle[j].positionZ));
				distances[j][i]=distances[i][j];	
			}
			
			else if (i==j){
				
				distances[i][j]=0.0;
			}
		}
	}
}

//random particle velocity and position generation
void SetParticlePosVel(vector<Particle>& particle){
	
	int numparticles=particle.size();
	//rng variables
	random_device rd;
	mt19937_64 rng(rd());
	uniform_real_distribution<double> dist1(0.0, 1.0);
	uniform_real_distribution<double> dist2(0,3.0);
	uniform_real_distribution<double> dist3(0,2.0);
	
	for (int i=0; i<numparticles; i++){
		//basic properties - homogenous at this point
		particle[i].particleID=particleCnt;
		particleCnt++;
		particle[i].mass=1.007825*u;
		particle[i].radius=0.55e-10;
		particle[i].charge=e;
		particle[i].inherentMagneticMoment=2.79284735089*MiN;
		particle[i].ionisationEnergy=2.178e-18;
		//random position
		particle[i].positionX=dist1(rng)*spaceScale;
		particle[i].positionY=dist1(rng)*spaceScale;
		particle[i].positionZ=dist1(rng)*spaceScale;
		//general velocity (thermodynamically deterined) and priorities
		particle[i].velocityT=sqrt((3*kB*initialTemperature)/particle[i].mass);
		int tempPriority1=int(ceil(dist2(rng)));
		int tempPriority2=int(ceil(dist3(rng)));
		Type tempMod=1, tempMult=0, signModX, signModY, signModZ;
		//determining the signs of velocities
		Type tempSignX=dist3(rng);
		Type tempSignY=dist3(rng); 
		Type tempSignZ=dist3(rng);
		
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
			particle[i].velocityX=signModX*tempMult*particle[i].velocityT;
			tempMod-=tempMult*tempMult;
		}
		else if (tempPriority1==2){
			
			tempMult=dist1(rng);
			particle[i].velocityY=signModY*tempMult*particle[i].velocityT;
			tempMod-=tempMult*tempMult;
		}
		else if (tempPriority1==3){
			
			tempMult=dist1(rng);
			particle[i].velocityZ=signModZ*tempMult*particle[i].velocityT;
			tempMod-=tempMult*tempMult;
		}
		//calculating the second and last random velocity
		if (tempPriority1==1 && tempPriority2==1){
			
			tempMult=dist1(rng)*sqrt(tempMod);
			particle[i].velocityY=tempMult*particle[i].velocityT;
			tempMod-=tempMult*tempMult;
			particle[i].velocityZ=sqrt(tempMod)*particle[i].velocityT;
		}
		else if (tempPriority1==1 && tempPriority2==2){
			
			tempMult=dist1(rng)*sqrt(tempMod);
			particle[i].velocityZ=tempMult*particle[i].velocityT;
			tempMod-=tempMult*tempMult;
			particle[i].velocityY=sqrt(tempMod)*particle[i].velocityT;
		}
		else if (tempPriority1==2 && tempPriority2==1){
			
			tempMult=dist1(rng)*sqrt(tempMod);
			particle[i].velocityX=signModX*tempMult*particle[i].velocityT;
			tempMod-=tempMult*tempMult;
			particle[i].velocityZ=signModZ*sqrt(tempMod)*particle[i].velocityT;
		}
		else if (tempPriority1==2 && tempPriority2==2){
			
			tempMult=dist1(rng)*sqrt(tempMod);
			particle[i].velocityZ=tempMult*particle[i].velocityT;
			tempMod-=tempMult*tempMult;
			particle[i].velocityX=sqrt(tempMod)*particle[i].velocityT;
		}
		else if (tempPriority1==3 && tempPriority2==1){
			
			tempMult=dist1(rng)*sqrt(tempMod);
			particle[i].velocityX=tempMult*particle[i].velocityT;
			tempMod-=tempMult*tempMult;
			particle[i].velocityY=sqrt(tempMod)*particle[i].velocityT;
		}
		else if (tempPriority1==3 && tempPriority2==2){
			
			tempMult=dist1(rng)*sqrt(tempMod);
			particle[i].velocityY=signModY*tempMult*particle[i].velocityT;
			tempMod-=tempMult*tempMult;
			particle[i].velocityX=signModX*sqrt(tempMod)*particle[i].velocityT;
		}
	}
	
	bool resolved=false;
	
	vector<vector<float>> distances;
	distances.resize(numparticles);
	
	for (int m=0; m<numparticles; ++m){
		
		distances[m].resize(numparticles);
	}
	
	while (resolved=false){
		
		resolved=true;
		
		CalcDistances(particle, distances);
		
		for (int i=0; i<numparticles; ++i){
			
			for (int j=0; j<numparticles; ++j){
				
				if (j>i){
					
					if (distances[i][j]<(particle[i].radius+particle[j].radius)){
						
						resolved=false;
						particle[i].positionX=dist1(rng)*spaceScale;
						particle[i].positionY=dist1(rng)*spaceScale;
						particle[i].positionZ=dist1(rng)*spaceScale;
					}
				}
			}
		}
	}
}
