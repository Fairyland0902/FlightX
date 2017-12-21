#ifndef _GAME_H
#define _GAME_H

#include "camera.h"
#include "cloud.h"

class Game
{
public:
    Game();

    Camera camera;

    void Init();

    void Render(int width, int height, float startTime);

private:
    std::unique_ptr<CloudData> cloudData;
    std::unique_ptr<CloudSimulator> cloudSim;
    std::unique_ptr<CloudRender> cloudRender;

    // Build and compile our shader program.
    void loadShaders();
};

#endif
