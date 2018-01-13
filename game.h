#ifndef _GAME_H
#define _GAME_H

#include "Terrain/Asphalt.h"
#include "camera.h"
#include "cloud/cloudRender.h"
#include "flare/flareRender.h"
#include "ocean/ocean.h"
#include "Aircraft.h"
#include "Terrain/Terrain.h"

extern Camera *currentcamera;

class Game
{
public:
    Game();

    ~Game();

    Camera camera;
    Aircraft aircraft;

    void Init(int width, int height);

    void Render(int width, int height, float deltaTime);

    void CameraControl();

private:
    CloudRender *cloudRender;
    FlareRender *flareRender;
    Ocean *ocean;
    Terrain *terrain;
    Asphalt *asphalt;

    // Build and compile our shader program.
    void loadShaders();

    // Load textures.
    void loadTextures();
};

#endif
