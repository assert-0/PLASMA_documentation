#include "simulation.h"

#include "gui_handler.h"
#include "tools.h"
#include "settings.h"

#include <3D_engine/3D_engine.h>

#include <CEGUI/CEGUI.h>

//Calculation data header struct
struct CalcDataHeader
{
    int numFrames;
    int numParticles;
    double simulationSpaceScale;
};

//System handles

extern ObjectHandler *g_obj;
extern ModelHandler *g_mod;
extern GUIHandler *g_gui;
static SimulationRenderer g_sim;

SimulationRenderer *getSimulationRenderer(){return &g_sim;}

//SimulationRenderer methods

void SimulationRenderer::initSimulation()
{
    g_mod->loadModel("resources/models/sphere.obj", "sphere");
    g_mod->loadModel("resources/models/cage.obj", "cage");
    cageID = g_obj->createObject("cage");
    Object *cage = g_obj->getObject(cageID);
    cage->x = 0.0;
    cage->y = 0.0;
    cage->z = -50.0 * g_cageScale;
    cage->scale = g_cageScale;
    cage->r = 0.3;
    cage->g = 0.3;
    cage->b = 0.3;
    cage->visible = true;
}

void SimulationRenderer::loadDataFromFile(std::string path)
{
    CalcDataHeader header;
    std::vector<char> raw;
    int numFrames = 0, numParticles = 0;

    if(!loadFromFileToBuffer(path, raw))
        return;

    dealocateParticleModels();

    header = *((CalcDataHeader*)&raw.front());

    numFrames = header.numFrames;
    numParticles = header.numParticles;
    simulationSpaceScale = header.simulationSpaceScale;

    frames.resize(numFrames);

    for(int a = 0; a < numFrames; a++)
    {
        frames[a].resize(numParticles);
        for(int b = 0; b < numParticles; b++)
        {
            frames[a][b] =
            *((Particle*)&raw
            [
                a * numParticles * sizeof(Particle) + //Row
                b * sizeof(Particle) +                //Column
                sizeof(CalcDataHeader)                //Header offset
            ]);
        }
    }

    alocateParticleModels();

    setSliderPosition(0);
}

void SimulationRenderer::update(double timeElapsed)
{
    CEGUI::Window *txt = g_gui->getWindow("txt_timeline");

    if(started && frames.size() > 0 && 100 * currentPos / frames.size() == 100)
    {
        setSliderPosition(100);
        stop();
        g_gui->getWindow("but_begin")->setText("BEGIN SIMULATION");
    }
    else if(started && frames.size() > 0 && 100 * currentPos / frames.size() < 100)
    {
        changeFrame();
        setSliderPosition(100 * currentPos / frames.size());
        currentPos++;
    }

    if(frames.size() > 0)
        txt->setText((std::to_string(100 * currentPos / frames.size()) + "%").c_str());
}

void SimulationRenderer::setFrameToPosition(int pos)
{
    if(frames.size() <= 0)
        return;
    currentPos = frames.size() / 100 * pos;
    changeFrame();
}

void SimulationRenderer::changeFrame()
{
    if(currentPos >= frames.size())
        return;
    std::vector<Object> &vec = *g_obj->getObjects();
    for(int a = 0; a < frames[currentPos].size(); a++)
    {
        vec[a + 1].x = frames[currentPos][a].positionX;
        //Conversion to simulation space
        vec[a + 1].x *= 100 * g_cageScale / simulationSpaceScale;
        //Offset in simulation space
        vec[a + 1].x -= 50 * g_cageScale;

        vec[a + 1].y = frames[currentPos][a].positionY;
        vec[a + 1].y *= 100 * g_cageScale / simulationSpaceScale;
        vec[a + 1].y -= 50 * g_cageScale;

        vec[a + 1].z = frames[currentPos][a].positionZ;
        vec[a + 1].z *= 100 * g_cageScale / simulationSpaceScale;
        vec[a + 1].z -= 50 * g_cageScale;

        vec[a + 1].scale = frames[currentPos][a].radius * g_particleScale;

        if(frames[currentPos][a].charge >= 0)
        {
            vec[a + 1].r = 1.0;
            vec[a + 1].g = 0.0;
            vec[a + 1].b = 0.0;
        }
        else
        {
            vec[a + 1].r = 0.0;
            vec[a + 1].g = 0.0;
            vec[a + 1].b = 1.0;
        }
    }
}

void SimulationRenderer::setSliderPosition(int pos)
{
    CEGUI::Slider *sli = static_cast<CEGUI::Slider*>(g_gui->getWindow("sli_timeline"));
    sli->setCurrentValue((float)pos);
}

void SimulationRenderer::alocateParticleModels()
{
    particleModelIDs.resize(frames[0].size());
    for(int a = 0; a < frames[0].size(); a++)
    {
        particleModelIDs[a] = g_obj->createObject("sphere");
        g_obj->getObject(particleModelIDs[a])->visible = true;
    }
}

void SimulationRenderer::dealocateParticleModels()
{
    for(int a = 0; a < particleModelIDs.size(); a++)
        g_obj->destroyObject(particleModelIDs[a]);
    particleModelIDs.resize(0);
}
