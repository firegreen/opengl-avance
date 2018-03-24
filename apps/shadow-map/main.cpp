#include "Application.hpp"

#ifdef __cplusplus
#define EXTERN_C extern "C"
#else
#define EXTERN_C
#endif

#ifdef _WIN32
// https://docs.nvidia.com/gameworks/content/technologies/desktop/optimus.htm
EXTERN_C __declspec(dllexport) DWORD NvOptimusEnablement = 1;
// https://gpuopen.com/amdpowerxpressrequesthighperformance/
EXTERN_C __declspec(dllexport) DWORD AmdPowerXpressRequestHighPerformance = 1;
#endif

int main(int argc, char** argv)
{
    Application app(argc, argv);
    return app.run();
}
