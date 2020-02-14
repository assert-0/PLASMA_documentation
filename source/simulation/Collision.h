//two-particle collision
void TwoCollision(vector<Particle>& particle, int i, int j){
	
	float velocityX1, velocityY1, velocityZ1, velocityT1, velocityX2, velocityY2, velocityZ2, velocityT2;
	float newVelocityT1, newVelocityT2;
	float velocityScale1, velocityScale2;
	
	velocityScale1=(particle[i].velocityX*distXMat[i][j]+particle[i].velocityY*distYMat[i][j]+particle[i].velocityZ*distZMat[i][j])/(particle[i].velocityT*distMat[i][j]);
	velocityScale2=(particle[j].velocityX*distXMat[i][j]+particle[j].velocityY*distYMat[i][j]+particle[j].velocityZ*distZMat[i][j])/(particle[j].velocityT*distMat[i][j]);

	velocityX1=particle[i].velocityT*velocityScale1*distXMat[i][j]/distMat[i][j];
	velocityY1=particle[i].velocityT*velocityScale1*distYMat[i][j]/distMat[i][j];
	velocityZ1=particle[i].velocityT*velocityScale1*distZMat[i][j]/distMat[i][j];
	velocityT1=sqrt(velocityX1*velocityX1+velocityY1*velocityY1+velocityZ1*velocityZ1);
	
	velocityX2=particle[j].velocityT*velocityScale1*distXMat[j][i]/distMat[j][i];
	velocityY2=particle[j].velocityT*velocityScale1*distYMat[j][i]/distMat[j][i];
	velocityZ2=particle[j].velocityT*velocityScale1*distZMat[j][i]/distMat[j][i];
	velocityT2=-sqrt(velocityX2*velocityX2+velocityY2*velocityY2+velocityZ2*velocityZ2);
	
	if (particle[i].charge==-e){
		
		
	}
	
	if (particle[i].charge);
	
	else{
		
		newVelocityT1=(particle[i].mass-particle[j].mass)*velocityT1/(particle[i].mass+particle[j].mass)+2*particle[j].mass*velocityT2/(particle[i].mass+particle[j].mass);
		newVelocityT2=(2*particle[i].mass*velocityT1)/(particle[i].mass+particle[j].mass)+(particle[j].mass-particle[i].mass)*velocityT2/(particle[i].mass+particle[j].mass);
	
	}
	
	particle[i].velocityX+=newVelocityT1*distXMat[i][j]/distMat[i][j]-velocityX1;
	particle[i].velocityY+=newVelocityT1*distYMat[i][j]/distMat[i][j]-velocityY1;
	particle[i].velocityZ+=newVelocityT1*distZMat[i][j]/distMat[i][j]-velocityZ1;
	particle[i].velocityT=sqrt(particle[i].velocityX*particle[i].velocityX+particle[i].velocityY*particle[i].velocityY+particle[i].velocityZ*particle[i].velocityZ);
	
	particle[j].velocityX+=newVelocityT2*distXMat[i][j]/distMat[i][j]-velocityX2;
	particle[j].velocityY+=newVelocityT2*distYMat[i][j]/distMat[i][j]-velocityY2;
	particle[j].velocityZ+=newVelocityT2*distZMat[i][j]/distMat[i][j]-velocityZ2;
	particle[j].velocityT=sqrt(particle[i].velocityX*particle[i].velocityX+particle[i].velocityY*particle[i].velocityY+particle[i].velocityZ*particle[i].velocityZ);
	
}

//collision with one of the sides of the box/containment field
void BoxCollision(vector<Particle>& particle, int i){
	
	if (particle[i].positionX<=0 || particle[i].positionX>=spaceScale){
		
		particle[i].velocityX=-particle[i].velocityX;
	}
	else if (particle[i].positionY<=0 || particle[i].positionY>=spaceScale){
		
		particle[i].velocityY=-particle[i].velocityY;
	}
	else if (particle[i].positionZ<=0 || particle[i].positionZ>=spaceScale){
		
		particle[i].velocityZ=-particle[i].velocityZ;
	}
}
 
