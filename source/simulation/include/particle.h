#pragma once

#include <cmath>
#include <windows.h>
#include <constants.h>

//particles paramaters
class Particle {

	public:
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

        Particle(){ZeroMemory(this, sizeof(*this));}

		Particle (float _mass, float _charge, float _radius, float _inherentMagneticMoment, float _positionX, float _positionY, float _positionZ, float _velocityX, float _velocityY, float _velocityZ){

			mass=_mass;
			charge=_charge;
			radius=_radius;
			inherentMagneticMoment=_inherentMagneticMoment;

			positionX=_positionX;
			positionY=_positionY;
			positionZ=_positionZ;
			velocityX=_velocityX;
			velocityY=_velocityY;
			velocityZ=_velocityZ;
			calcVelocityT();
		}

		void calcVelocityT(){

			velocityT=sqrt(velocityX*velocityX+velocityY*velocityY+velocityZ*velocityZ);
		}

		float getAcc(){

			return sqrt(accX*accX+accY*accY+accZ*accZ);
		}

		float getEPField(){

			return sqrt(EPFieldX*EPFieldX+EPFieldY*EPFieldY+EPFieldZ*EPFieldZ);
		}

		float getEField(){

			return sqrt(EFieldX*EFieldX+EFieldY*EFieldY+EFieldZ*EFieldZ);
		}

		float getTotalEField(){

			return sqrt((EPFieldX+EFieldX)*(EPFieldX+EFieldX)+(EPFieldX+EFieldY)*(EPFieldY+EFieldY)+(EPFieldZ+EFieldZ)*(EPFieldZ+EFieldZ));
		}

		float getBPField(){

			return sqrt(BPFieldX*BPFieldX+BPFieldY*BPFieldY+BPFieldZ*BPFieldZ);
		}

		float getBField(){

			return sqrt(BFieldX*BFieldX+BFieldY*BFieldY+BFieldZ*BFieldZ);
		}

		float getTotalBField(){

			return sqrt((BPFieldX+BFieldX)*(BPFieldX+BFieldX)+(BPFieldX+BFieldY)*(BPFieldY+BFieldY)+(BPFieldZ+BFieldZ)*(BPFieldZ+BFieldZ));
		}

		float getTemperature(){

			return mass*velocityT*velocityT/(3*kB);
		}

		float getEnergyeV(){

			return 0.5*mass*velocityT*velocityT/e;
		}
};
