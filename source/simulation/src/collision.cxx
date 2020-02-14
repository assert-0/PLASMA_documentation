#include <engine.h>

//two-particles collision
void SimulationEngine::twoCollision(vector<Particle>& particles, int i, int j){

	float velocityX1, velocityY1, velocityZ1, velocityT1, velocityX2, velocityY2, velocityZ2, velocityT2;
	float newVelocityT1, newVelocityT2;
	float velocityScale1, velocityScale2;

	velocityScale1=(particles[i].velocityX*distXMat[i][j]+particles[i].velocityY*distYMat[i][j]+particles[i].velocityZ*distZMat[i][j])/(particles[i].velocityT*distMat[i][j]);
	velocityScale2=(particles[j].velocityX*distXMat[i][j]+particles[j].velocityY*distYMat[i][j]+particles[j].velocityZ*distZMat[i][j])/(particles[j].velocityT*distMat[i][j]);

	velocityX1=particles[i].velocityT*velocityScale1*distXMat[i][j]/distMat[i][j];
	velocityY1=particles[i].velocityT*velocityScale1*distYMat[i][j]/distMat[i][j];
	velocityZ1=particles[i].velocityT*velocityScale1*distZMat[i][j]/distMat[i][j];
	velocityT1=sqrt(velocityX1*velocityX1+velocityY1*velocityY1+velocityZ1*velocityZ1);

	velocityX2=particles[j].velocityT*velocityScale1*distXMat[j][i]/distMat[j][i];
	velocityY2=particles[j].velocityT*velocityScale1*distYMat[j][i]/distMat[j][i];
	velocityZ2=particles[j].velocityT*velocityScale1*distZMat[j][i]/distMat[j][i];
	velocityT2=-sqrt(velocityX2*velocityX2+velocityY2*velocityY2+velocityZ2*velocityZ2);

	if (particles[i].charge==-e){


	}

	if (particles[i].charge);

	else{

		newVelocityT1=(particles[i].mass-particles[j].mass)*velocityT1/(particles[i].mass+particles[j].mass)+2*particles[j].mass*velocityT2/(particles[i].mass+particles[j].mass);
		newVelocityT2=(2*particles[i].mass*velocityT1)/(particles[i].mass+particles[j].mass)+(particles[j].mass-particles[i].mass)*velocityT2/(particles[i].mass+particles[j].mass);

	}

	particles[i].velocityX+=newVelocityT1*distXMat[i][j]/distMat[i][j]-velocityX1;
	particles[i].velocityY+=newVelocityT1*distYMat[i][j]/distMat[i][j]-velocityY1;
	particles[i].velocityZ+=newVelocityT1*distZMat[i][j]/distMat[i][j]-velocityZ1;
	particles[i].velocityT=sqrt(particles[i].velocityX*particles[i].velocityX+particles[i].velocityY*particles[i].velocityY+particles[i].velocityZ*particles[i].velocityZ);

	particles[j].velocityX+=newVelocityT2*distXMat[i][j]/distMat[i][j]-velocityX2;
	particles[j].velocityY+=newVelocityT2*distYMat[i][j]/distMat[i][j]-velocityY2;
	particles[j].velocityZ+=newVelocityT2*distZMat[i][j]/distMat[i][j]-velocityZ2;
	particles[j].velocityT=sqrt(particles[i].velocityX*particles[i].velocityX+particles[i].velocityY*particles[i].velocityY+particles[i].velocityZ*particles[i].velocityZ);

}

//collision with one of the sides of the box/containment field
void SimulationEngine::boxCollision(vector<Particle>& particles, int i){

	if (particles[i].positionX<=0 || particles[i].positionX>=spaceScale){

		particles[i].velocityX=-particles[i].velocityX;
		particles[i].positionX = particles[i].positionX <= 0 ? 0 : spaceScale;
	}
	if (particles[i].positionY<=0 || particles[i].positionY>=spaceScale){

		particles[i].velocityY=-particles[i].velocityY;
		particles[i].positionY = particles[i].positionY <= 0 ? 0 : spaceScale;
	}
	if (particles[i].positionZ<=0 || particles[i].positionZ>=spaceScale){

		particles[i].velocityZ=-particles[i].velocityZ;
		particles[i].positionZ = particles[i].positionZ <= 0 ? 0 : spaceScale;
	}
}

