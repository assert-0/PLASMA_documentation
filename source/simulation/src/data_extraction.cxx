/**

FILE CURRENTLY NOT IN USE

**/

#include <data_extraction.h>

#include <functional>
#include <algorithm>
#include <vector>
#include <set>
#include <cstdio>

#include <particle.h>

using namespace std;

//auxiliary structure

template<typename c1, typename c2>
struct index_pair
{
    c1 first;
    mutable c2 second;

    index_pair(){}

    template<typename in1, typename in2>
    index_pair(in1 &&first, in2 &&second):
        first(first),
        second(second)
        {}

    bool operator<(const index_pair &p) const
    {
        return p.first < this->first;
    }

    bool operator>(const index_pair &p) const
    {
        return p.first > this->first;
    }
};


//Data extraction functions


//Average velocity

float CalculateMeanVelocity(const vector<vector<Particle>>& particleLog, int frame){

	double Sum=0;
	int particles=particleLog[frame].size();

	for (int i=0; i<particles; ++i){

		Sum+=particleLog[frame][i].velocityT;
	}
	double Sum2=Sum/particles;

	return float (Sum2);
}


//Root mean squared velocity

float CalculateVelocityRMS(const vector<vector<Particle>>& particleLog, int frame){

	double Sum=0;
	int particles=particleLog[frame].size();

	for (int i=0; i<particles; ++i){

		Sum+=particleLog[frame][i].velocityT*particleLog[frame][i].velocityT;
	}
	double Sum2=sqrt(Sum/particles);

	return float (Sum2);
}

//Maxwell distribution

const int maxwellIncrements=40;
const float maxwellCentile=0.01;
vector <pair<float, int>> maxwellDistribution;

void CalculateMaxwell(const vector<vector<Particle>>& particleLog, int Frame, vector<pair<float, int>>& maxwellDistribution){

	int frame=Frame;

	if (frame>particleLog.size()-1){

		frame=particleLog.size()-1;
	}

	int numParticles=particleLog[frame].size();
	maxwellDistribution.clear();
	vector<float> velocityList(numParticles);
	float min=0, max=0, inc=0, stamp=0;

	for (int i=0; i<numParticles; ++i){

		velocityList[i]=particleLog[frame][i].velocityT;
	}

	sort(velocityList.begin(), velocityList.end());

	int posMin=int(numParticles*maxwellCentile);
	int posMax=numParticles-int(numParticles*maxwellCentile);

	min=velocityList[posMin];
	max=velocityList[posMax-1];
	inc=(max-min)/maxwellIncrements;

	int i=0; float ii=min+inc;
	maxwellDistribution.push_back({min, 0.0});

	for (int j=posMin; j<posMax; ++j){


		if (velocityList[j]>ii){

			maxwellDistribution.push_back({ii, 0.0});
			i++; j--;
			ii+=inc;
		}
		else{
			maxwellDistribution[i].second++;
		}
	}
}

void PrintMaxwell(const vector<pair<float, int>>& maxwellDistribution){

	for (int i=0; i<maxwellIncrements; i++){

		printf("%d\n", maxwellDistribution[i].second);
	}
}

//Boltzmann distribution

vector <float> boltzmannDistribution;

void CalculateBoltzmann (vector<vector<Particle>>& particleLog, int Frame, vector<float>& boltzmannDistribution){

	int frame=Frame;

	if (frame>particleLog.size()-1){

		frame=particleLog.size()-1;
	}

	int numParticles=particleLog[0].size();

	for (int i=0; i<numParticles; ++i){

		boltzmannDistribution.push_back((particleLog[frame][i].EPotential*particleLog[frame][i].charge)/(kB*particleLog[frame][i].getTemperature()));
	}

	sort (boltzmannDistribution.begin(), boltzmannDistribution.end(), greater<float>());
}

void PrintBoltzmann(const vector<float>& boltzmannDistribution){

	int numParticles=boltzmannDistribution.size();

	for (int i=0; i<numParticles; ++i){

		printf("%.15f\n", boltzmannDistribution[i]);
	}
}

//spatial distributions

const int XDimFrac=10;
const int YDimFrac=10;
const int ZDimFrac=10;

vector <set <index_pair<int, float>>> particleDensity;
vector <set <index_pair<int, float>>> avgVolumeSpeed;
vector <set <index_pair<int, float>>> chargeDensity;
vector <set <index_pair<int, float>>> spatialPotential;


set <index_pair<int, float>> meanParticleDensity;
set <index_pair<int, float>> meanAvgVolumeSpeed;
set <index_pair<int, float>> meanChargeDensity;
set <index_pair<int, float>> meanSpatialPotential;


void CalculateSpatial(
		int startFrame,
		int endFrame,
		TYPE spaceScale,
		const vector<vector<Particle>>& particleLog,
		vector <set <index_pair<int, float>>>& particleDensity,
		vector <set <index_pair<int, float>>>& avgVolumeSpeed,
		vector <set <index_pair<int, float>>>& chargeDensity,
		vector <set <index_pair<int, float>>>& spatialPotential){

	int nt=endFrame-startFrame;
	int nj=XDimFrac*YDimFrac*ZDimFrac;

	particleDensity.clear();
	avgVolumeSpeed.clear();
	chargeDensity.clear();
	spatialPotential.clear();

	particleDensity.resize(nt);
	avgVolumeSpeed.resize(nt);
	chargeDensity.resize(nt);
	spatialPotential.resize(nt);

	int numParticles = particleLog[0].size();

	for (int i; i<nt; ++i){

		for (int j=0; j<nj; j++){

			particleDensity[i].insert({j, 0.0});
			avgVolumeSpeed[i].insert({j, 0.0});
			chargeDensity[i].insert({j, 0.0});
			spatialPotential[i].insert({j, 0.0});
		}
	}
	for (int t=0; t<nt; ++t){

		int f=startFrame+t;

		for (int i=0; i<numParticles; ++i){

			int tempX=floor(particleLog[f][i].positionX/spaceScale*XDimFrac);
			int tempY=floor(particleLog[f][i].positionY/spaceScale*YDimFrac);
			int tempZ=floor(particleLog[f][i].positionZ/spaceScale*ZDimFrac);

			int index=tempX+tempY*XDimFrac+tempZ*XDimFrac*YDimFrac;

			auto tempPair1=particleDensity[t].find({index, 0.0});
			auto tempPair2=avgVolumeSpeed[t].find({index, 0.0});
			auto tempPair3=chargeDensity[t].find({index, 0.0});
			auto tempPair4=spatialPotential[t].find({index, 0.0});

			tempPair1->second += 1;
			tempPair2->second += particleLog[f][i].velocityT;
			tempPair3->second += particleLog[f][i].charge;
			tempPair4->second += particleLog[f][i].EPotential;
		}
	}
	for (int t=0; t<nt; ++t){

		auto temp1=particleDensity[t].begin();
		auto temp2=avgVolumeSpeed[t].begin();
		auto temp3=spatialPotential[t].begin();

		for (; temp1!=particleDensity[t].end(); temp1++, temp2++, temp3++){

			if (
					temp1->second!=0 &&
					temp1!=particleDensity[t].end() &&
					temp2!=avgVolumeSpeed[t].end() &&
					temp3!=spatialPotential[t].end()){

				temp2->second/=temp1->second;
				temp3->second/=temp1->second;
				temp1->second/=numParticles;
			}
		}
	}
}

void CalculateMeanSpatial (
		vector <set <index_pair<int, float>>>& particleDensity,
		vector <set <index_pair<int, float>>>& avgVolumeSpeed,
		vector <set <index_pair<int, float>>>& chargeDensity,
		vector <set <index_pair<int, float>>>& spatialPotential,
		set <index_pair<int, float>>& meanParticleDensity,
		set <index_pair<int, float>>& meanAvgVolumeSpeed,
		set <index_pair<int, float>>& meanChargeDensity,
		set <index_pair<int, float>>& meanSpatialPotential){

	int n=particleDensity.size();
	int l=XDimFrac*YDimFrac*ZDimFrac;

	for (int i=0; i<l; ++i){

		meanParticleDensity.insert({i, 0.0});
		meanAvgVolumeSpeed.insert({i, 0.0});
		meanChargeDensity.insert({i, 0.0});
		meanSpatialPotential.insert({i, 0.0});
	}

	for (int i=0; i<n; ++i){

		auto temp1=particleDensity[i].begin();
		auto temp2=avgVolumeSpeed[i].begin();
		auto temp3=chargeDensity[i].begin();
		auto temp4=spatialPotential[i].begin();

		auto mean1=meanParticleDensity.begin();
		auto mean2=meanAvgVolumeSpeed.begin();
		auto mean3=meanChargeDensity.begin();
		auto mean4=meanSpatialPotential.begin();

		for (; temp1!=particleDensity[i].end(); temp1++, temp2++, temp3++, temp4++, mean1++, mean2++, mean3++, mean4++){

			if (temp1!=particleDensity[i].end() &&
				temp2!=avgVolumeSpeed[i].end() &&
				temp3!=chargeDensity[i].end() &&
				temp4!=spatialPotential[i].end() &&
				mean1!=meanParticleDensity.end() &&
				mean2!=meanAvgVolumeSpeed.end() &&
				mean3!=meanChargeDensity.end() &&
				mean4!=meanSpatialPotential.end()){

				mean1->second+=temp1->second;
				mean2->second+=temp2->second;
				mean3->second+=temp3->second;
				mean4->second+=temp4->second;
			}
		}
	}
	auto temp1=meanParticleDensity.begin();
	auto temp2=meanAvgVolumeSpeed.begin();
	auto temp3=meanChargeDensity.begin();
	auto temp4=meanSpatialPotential.begin();

	for (; temp1!=meanParticleDensity.end(); temp1++, temp2++, temp3++, temp4++){

		if (
				temp1->second!=0 &&
				temp2->second!=0 &&
				temp3->second!=0 &&
				temp4->second!=0 &&
				temp1!=meanParticleDensity.end() &&
				temp2!=meanAvgVolumeSpeed.end() &&
				temp3!=meanChargeDensity.end() &&
				temp4!=meanSpatialPotential.end()){

			temp1->second/=n;
			temp2->second/=n;
			temp3->second/=n;
			temp4->second/=n;
		}
	}
}

void CalculateSpatialDistribution(
		int startFrame,
		int endFrame,
		TYPE spaceScale,
		const vector<vector<Particle>>& particleLog,
		vector <set <index_pair<int, float>>>& particleDensity,
		vector <set <index_pair<int, float>>>& avgVolumeSpeed,
		vector <set <index_pair<int, float>>>& chargeDensity,
		vector <set <index_pair<int, float>>>& spatialPotential,
		set <index_pair<int, float>>& meanParticleDensity,
		set <index_pair<int, float>>& meanAvgVolumeSpeed,
		set <index_pair<int, float>>& meanChargeDensity,
		set <index_pair<int, float>>& meanSpatialPotential){

	int maxindex=particleLog.size()-1;
	int startframe, endframe;

	if (endFrame>maxindex && startFrame>maxindex && maxindex>0){

		endframe=maxindex;
		startframe=maxindex-1;
	}
	else if (endFrame>maxindex && startFrame>maxindex && maxindex==0){

		endframe=maxindex+1;
		startframe=maxindex;
	}
	else if (endFrame>maxindex && startFrame<endFrame){

		endframe=maxindex;
		startframe=startFrame;
	}
	else if (startFrame>endFrame){

		endframe=startFrame;
		startframe=endFrame;
	}
	else if (startFrame==endFrame && startFrame!=0){

		endframe=endFrame;
		startframe=endFrame-1;
	}
	else if (startFrame==endFrame && startFrame==0){

		endframe=endFrame+1;
		startframe=endFrame;
	}
	else{

		startframe=startFrame;
		endframe=endFrame;
	}

	CalculateSpatial(
		startframe,
		endframe,
        spaceScale,
		particleLog,
		particleDensity,
		avgVolumeSpeed,
		chargeDensity,
		spatialPotential);

	CalculateMeanSpatial(
		particleDensity,
		avgVolumeSpeed,
		chargeDensity,
		spatialPotential,
		meanParticleDensity,
		meanAvgVolumeSpeed,
		meanChargeDensity,
		meanSpatialPotential);
}

void PrintSpatial(
		int frame,
		vector <set <index_pair<int, float>>>& particleDensity,
		vector <set <index_pair<int, float>>>& avgVolumeSpeed,
		vector <set <index_pair<int, float>>>& chargeDensity,
		vector <set <index_pair<int, float>>>& spatialPotential){

	int nj=XDimFrac*YDimFrac*ZDimFrac;

	auto temp1=particleDensity[frame].begin();
	auto temp2=avgVolumeSpeed[frame].begin();
	auto temp3=chargeDensity[frame].begin();
	auto temp4=spatialPotential[frame].begin();

	for (; temp1!=particleDensity[frame].end(); temp1++, temp2++, temp3++, temp4++){

		if(
                temp1!=particleDensity[frame].end() &&
				temp2!=avgVolumeSpeed[frame].end() &&
				temp3!=chargeDensity[frame].end() &&
				temp4!=spatialPotential[frame].end()){

			printf("%d %.6f %.6f %.6f %.6f\n", temp1->first, temp1->second, temp2->second, temp3->second/e, temp4->second);
		}
	}
}

void PrintMeanSpatial(
		set <index_pair<int, float>>& meanParticleDensity,
		set <index_pair<int, float>>& meanAvgVolumeSpeed,
		set <index_pair<int, float>>& meanChargeDensity,
		set <index_pair<int, float>>& meanSpatialPotential){

	auto temp1=meanParticleDensity.begin();
	auto temp2=meanAvgVolumeSpeed.begin();
	auto temp3=meanChargeDensity.begin();
	auto temp4=meanSpatialPotential.begin();

	for (; temp1!=meanParticleDensity.end(); temp1++, temp2++, temp3++, temp4++){

		if (
				temp1!=meanParticleDensity.end() &&
				temp2!=meanAvgVolumeSpeed.end() &&
				temp3!=meanChargeDensity.end() &&
				temp4!=meanSpatialPotential.end()){

			printf("%d %.6f %.6f %.6f %.6f\n", temp1->first, temp1->second, temp2->second, temp3->second/e, temp4->second);
		}
	}
}
