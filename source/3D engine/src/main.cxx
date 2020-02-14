#include <cstdio>

#define PROJECT_3D_DEBUG 0

#include "runtime.h"
#include "object.h"
#include "test_engine.h"
#include "debug_flags.h"

//No main function if the project is being built as a library

#if PROJECT_3D_MAIN
    static Runtime *g_sys;

    int main()
    {
        g_sys = getRuntime();

        g_sys->createWindow("Glorious engine", 1920 * 0.5, 1080 * 0.5);
        g_sys->run();
    }
#endif // PROJECT_3D_MAIN
