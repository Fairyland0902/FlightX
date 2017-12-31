#ifndef _GAME_H
#define _GAME_H

#include "camera.h"
#include "cloud/cloudRender.h"
#include "flare/flareRender.h"

class Game
{
public:
    Game();

    ~Game();

    Camera camera;

    void Init(int width, int height);

    void Render(int width, int height, float deltaTime);

private:
    CloudRender *cloudRender;
    FlareRender *flareRender;

    // Build and compile our shader program.
    void loadShaders();
};

#endif
