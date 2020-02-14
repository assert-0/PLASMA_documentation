//calculating the distance vector and absolute distance - GPU workload
void CalcDistMat(
		vector<Particle> particles, 
		vector<vector<float>>& distXMat, 
		vector<vector<float>>& distYMat, 
		vector<vector<float>>& distZMat, 
		vector<vector<float>>& distMat){
	
	int numparticles=particles.size();
	
	for (int i=0; i<numparticles; ++i){
		
		for (int j=0; j<numparticles; ++j){
			
			if (j>i){
				
				distXMat[i][j]=particle[j].positionX-particle[i].positionX;
				distXMat[j][i]=-distXMat[i][j];
				
				distYMat[i][j]=particle[j].positionY-particle[i].positionY;
				distYMat[j][i]=-distYMat[i][j];
				
				distZMat[i][j]=particle[j].positionZ-particle[i].positionZ;
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

//Calculating electric fields of every particle to the other one - GPU workload
void CalcEPFieldMat(
		vector<Particle> particles, 
		vector<vector<float>>& ExMat, 
		vector<vector<float>>& EyMat, 
		vector<vector<float>>& EzMat, 
		vector<vector<float>>& EPotMat){
	
	int numparticles=particles.size();
	
	for (int i=0; i<numparticles; ++i){
		
		for (int j=0; j<numparticles; ++j){
			
			if (j>i){
				
				Type temp=K*particle[i].charge/(distMat[i][j]*distMat[i][j]);
				
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

//Calculating particle magnetic fields on other particles - GPU workload
void CalcBPFieldMat(
		vector<Particle> particles, 
		vector<vector<float>>& BPxMat, 
		vector<vector<float>>& BPyMat, 
		vector<vector<float>>& BPzMat){
	
	int numparticles=particles.size();
	
	for (int i=0; i<numparticles; ++i){
		
		for (int j=0; j<numparticles; ++j){
			
			if (j>i){
				
				BPxMat[i][j]=Mi*particle[j].charge*(particle[j].velocityY*distZMat[i][j]-particle[j].velocityZ*distYMat[i][j])/(distMat[i][j]*distMat[i][j]*distMat[i][j]);
				BPxMat[j][i]=-BPxMat[i][j];
				
				BPyMat[i][j]=Mi*particle[j].charge*(particle[j].velocityZ*distXMat[i][j]-particle[j].velocityX*distZMat[i][j])/(distMat[i][j]*distMat[i][j]*distMat[i][j]);
				BPyMat[j][i]=-BPyMat[i][j];
				
				BPzMat[i][j]=Mi*particle[j].charge*(particle[j].velocityX*distYMat[i][j]-particle[j].velocityY*distXMat[i][j])/(distMat[i][j]*distMat[i][j]*distMat[i][j]);
				BPzMat[j][i]=-BPzMat[i][j];
			}
			
			else if (i==j){
				
				BPxMat[i][j]=0.0; BPyMat[i][j]=0.0; BPzMat[i][j]=0.0;
			}
		}
	}
}

//finding acceleration and fields for particles from particles - GPU workload
void CalcPAcceleration(
		vector<Particle> particles, 
		vector<vector<float>>& ExMat, 
		vector<vector<float>>& EyMat, 
		vector<vector<float>>& EzMat, 
		vector<vector<float>>& BPxMat, 
		vector<vector<float>>& BPyMat, 
		vector<vector<float>>& BPzMat){
	
	int numparticles=particles.size();
	
	for (int i=0; i<numParticles; ++i){

		for (int j=0; j<numParticles; ++j){			
			
			particle[i].EFieldX+=ExMat[i][j];
			particle[i].EFieldY+=EyMat[i][j];
			particle[i].EFieldY+=EzMat[i][j];
			
			particle[i].EPotential+=EPotMat[i][j];
			
			particle[i].BPFieldX+=BPxMat[i][j];
			particle[i].BPFieldY+=BPyMat[i][j];
			particle[i].BPFieldZ+=BPzMat[i][j];
			
			particle[i].accX+=((ExMat[i][j]+particle[i].velocityY*(BPzMat[i][j])-particle[i].velocityZ*(BPyMat[i][j]))*particle[i].charge)/particle[i].mass;
			particle[i].accY+=((EyMat[i][j]+particle[i].velocityZ*(BPxMat[i][j])-particle[i].velocityX*(BPzMat[i][j]))*particle[i].charge)/particle[i].mass;
			particle[i].accZ+=((EzMat[i][j]+particle[i].velocityX*(BPyMat[i][j])-particle[i].velocityZ*(BPxMat[i][j]))*particle[i].charge)/particle[i].mass;
		}			
	}
}

//acceleration from external fields - CPU
void CalcAccelerationExternal(
		vector<Particle> particles, 
		vector<vector<vector<float>>>& EFieldX, 
		vector<vector<vector<float>>>& EFieldY, 
		vector<vector<vector<float>>>& EFieldZ, 
		vector<vector<vector<float>>>& BFieldX, 
		vector<vector<vector<float>>>& BFieldY, 
		vector<vector<vector<float>>>& BFieldZ){
	
	int numparticles=particles.size();
	
	for (int i=0; i<numParticles; ++i){
		
		Type Ex=EFieldX[int(floor(particle[i].positionX*spaceResX))][int(floor(particle[i].positionY*spaceResY))][int(floor(particle[i].positionZ*spaceResZ))];
		Type Ey=EFieldY[int(floor(particle[i].positionX*spaceResX))][int(floor(particle[i].positionY*spaceResY))][int(floor(particle[i].positionZ*spaceResZ))];
		Type Ez=EFieldZ[int(floor(particle[i].positionX*spaceResX))][int(floor(particle[i].positionY*spaceResY))][int(floor(particle[i].positionZ*spaceResZ))];
		
		Type Bx=BFieldX[int(floor(particle[i].positionX*spaceResX))][int(floor(particle[i].positionY*spaceResY))][int(floor(particle[i].positionZ*spaceResZ))];
		Type By=BFieldY[int(floor(particle[i].positionX*spaceResX))][int(floor(particle[i].positionY*spaceResY))][int(floor(particle[i].positionZ*spaceResZ))];
		Type Bz=BFieldZ[int(floor(particle[i].positionX*spaceResX))][int(floor(particle[i].positionY*spaceResY))][int(floor(particle[i].positionZ*spaceResZ))];
		
		particle[i].accX+=((Ex+particle[i].velocityY*Bz-particle[i].velocityZ*By)*particle[i].charge)/particle[i].mass;
		particle[i].accY+=((Ey+particle[i].velocityZ*Bx-particle[i].velocityX*Bz)*particle[i].charge)/particle[i].mass;
		particle[i].accZ+=((Ez+particle[i].velocityX*By-particle[i].velocityZ*Bx)*particle[i].charge)/particle[i].mass;
	}
}

//position and velocity change between frames
void CalcNewPosVel(vector<Particle> particles){
	
	int numparticles=particles.size();
	
	for (int i=0; i<numParticles; ++i){
		
		particle[i].positionX+=particle[i].velocityX*timeIncrement+particle[i].accX*timeIncrement*timeIncrement/2;
		particle[i].positionY+=particle[i].velocityY*timeIncrement+particle[i].accY*timeIncrement*timeIncrement/2;
		particle[i].positionZ+=particle[i].velocityZ*timeIncrement+particle[i].accZ*timeIncrement*timeIncrement/2;

		particle[i].velocityX+=particle[i].accX*timeIncrement;
		particle[i].velocityY+=particle[i].accY*timeIncrement;
		particle[i].velocityZ+=particle[i].accZ*timeIncrement;
		particle[i].calcVelocityT();
	}
}
