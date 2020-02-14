#include "simulation.h"

#include "gui_handler.h"
#include "tools.h"
#include "settings.h"

#include <3D_engine/3D_engine.h>

#include <CEGUI/CEGUI.h>

extern ObjectHandler *g_obj;
extern ModelHandler *g_mod;
extern GUIHandler *g_gui;
static SimulationEngine g_sim;

SimulationEngine *getSimulationEngine(){return &g_sim;}

//SimulationEngine methods

void SimulationEngine::initSimulation()
{
    g_mod->loadModel("resources/models/sphere.obj", "sphere");
    g_mod->loadModel("resources/models/cage.obj", "cage");
    cageID = g_obj->createObject("cage");
    Object *cage = g_obj->getObject(cageID);
    cage->x = 0.0;
    cage->y = 0.0;
    cage->z = 0.0;
    cage->scale = 0.1;
    cage->r = 0.1;
    cage->g = 0.1;
    cage->b = 0.1;
    cage->visible = true;
}

void SimulationEngine::loadDataFromFile(std::string path)
{
    std::vector<char> raw;
    int numFrames = 0, numParticles = 0;

    dealocateParticleModels();

    loadFromFileToBuffer(path, raw);
    numFrames = *((int*)&raw.front());
    numParticles = *((int*)&raw.front() + 1);
    frames.resize(numFrames);
    for(int a = 0; a < numFrames; a++)
    {
        frames[a].resize(numParticles);
        for(int b = 0; b < numParticles; b++)
        {
            frames[a][b] =
            *((Particle*)&raw[a * numParticles * sizeof(Particle) + b * sizeof(Particle)]);
        }
        ZeroMemory(&(frames[a].front()), sizeof(Particle) * frames[a].size());
    }
    frames[0][0].positionX = 0.5;
    frames[0][0].positionZ = 10.0;
    frames[0][1].positionX = -0.5;
    frames[0][1].positionZ = 10.0;
    currentPos = 1;

    alocateParticleModels();

    setSliderPosition(0);
}

void SimulationEngine::update(double timeElapsed)
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

void SimulationEngine::setFrameToPosition(int pos)
{
    if(frames.size() <= 0)
        return;
    currentPos = frames.size() / 100 * pos;
    changeFrame();
}

void SimulationEngine::changeFrame()
{
    if(currentPos >= frames.size())
        return;
    std::vector<Object> &vec = *g_obj->getObjects();
    //printf("%d %d %d %d\n", currentPos, frames.size(), frames[0].size(), vec.size());
    for(int a = 0; a < frames[currentPos].size(); a++)
    {
        vec[a + 1].x = frames[currentPos - 1][a].positionX < 0 ?
                       frames[currentPos - 1][a].positionX - 0.1 :
                       frames[currentPos - 1][a].positionX + 0.1;
                       /* * g_particleDistance = */
        vec[a + 1].y = frames[currentPos - 1][a].positionY/* * g_particleDistance*/;
        vec[a + 1].z = frames[currentPos - 1][a].positionZ/* * g_particleDistance*/;
        vec[a + 1].scale = /*frames[currentPos][a].radius * */ g_particleScale;
        frames[currentPos][a].positionX = vec[a + 1].x;
        frames[currentPos][a].positionY = vec[a + 1].y;
        frames[currentPos][a].positionZ = vec[a + 1].z;
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


    printf("%g %g %g\n", vec[1].x,
                         vec[1].y,
                         vec[1].z);

    /*
    printf("%g %g %g\n", frames[currentPos][0].positionX,
                         frames[currentPos][0].positionY,
                         frames[currentPos][0].positionZ);
    */
}

void SimulationEngine::setSliderPosition(int pos)
{
    CEGUI::Slider *sli = static_cast<CEGUI::Slider*>(g_gui->getWindow("sli_timeline"));
    sli->setCurrentValue((float)pos);
}

void SimulationEngine::alocateParticleModels()
{
    particleModelIDs.resize(frames[0].size());
    for(int a = 0; a < frames[a].size(); a++)
    {
        particleModelIDs[a] = g_obj->createObject("sphere");
        g_obj->getObject(particleModelIDs[a])->visible = true;
    }
}

void SimulationEngine::dealocateParticleModels()
{
    for(int a = 0; a < particleModelIDs.size(); a++)
        g_obj->destroyObject(particleModelIDs[a]);
    particleModelIDs.resize(0);
}
