#include <PLASMA_simulation.h>
#include <constants.h>

#if PLASMA_SIMULATION_ENGINE_MAIN


#include <data_extraction.h>

#include <fstream>

//engine instance
SimulationEngine *g_sim = getSimulationEngine();

//input data

int g_numParticles;
int g_numFrames;
TYPE g_spaceScale;
TYPE g_initialTemperature;
TYPE g_timeIncrement;

TYPE g_particleCharge;
TYPE g_particleMass;

TYPE g_BField;
TYPE g_EField;

//files to write to and read from
const char *dumpFilename = "calc_data.pcd", *inputFilename = "input.dat";

//calculation data header struct
struct CalcDataHeader
{
    int numFrames;
    int numParticles;
    double simulationSpaceScale;
} header;

//output calculation data to file
void outputData()
{
	std::ofstream ostr(dumpFilename, std::ios::binary);

	header.numFrames = g_numFrames;
	header.numParticles = g_numParticles;
	header.simulationSpaceScale = g_spaceScale;

	auto log = g_sim->getLog();

	ostr.write((const char*)&header, sizeof(CalcDataHeader));
	for(int a = 0; a < log.size(); a++)
	{
		ostr.write((const char*) &log[a].front(), sizeof(Particle) * g_numParticles);
	}
}

void inputParameters()
{
	FILE *f = fopen(inputFilename, "r");
	fscanf(f, "%*[^=]= %d\n", &g_numParticles);
	fscanf(f, "%*[^=]= %lf\n", &g_spaceScale);
	fscanf(f, "%*[^=]= %lf\n", &g_initialTemperature);
	fscanf(f, "%*[^=]= %lf\n", &g_timeIncrement);
	fscanf(f, "%*[^=]= %d\n", &g_numFrames);
	fscanf(f, "%*[^=]= %lf\n", &g_particleCharge);
	fscanf(f, "%*[^=]= %lf\n", &g_particleMass);
	fscanf(f, "%*[^=]= %lf\n", &g_BField);
	fscanf(f, "%*[^=]= %lf\n", &g_EField);

	g_particleCharge = round(g_particleCharge);
	g_particleCharge *= e;
	g_particleMass *= u;

	g_sim->setNumParticles(g_numParticles);
	g_sim->setNumFrames(g_numFrames);
	g_sim->setSpaceScale(g_spaceScale);
	g_sim->setInitialTemperature(g_initialTemperature);
	g_sim->setTimeIncrement(g_timeIncrement);
	g_sim->setParticleCharge(g_particleCharge);
	g_sim->setParticleMass(g_particleMass);
	g_sim->setHomogenousMagneticField(g_BField);
	g_sim->setHomogenousElectricField(g_EField);
}

int main(){

	inputParameters();

	g_sim->calculate();

	outputData();

	return 0;
}


#endif
