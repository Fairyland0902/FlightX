#ifndef _GAME_H
#define _GAME_H

#include "camera.h"
#include "cloud/cloudRender.h"
#include "skybox.h"
#include "aircraft.h"

class Game
{
public:
    Game();

    ~Game();

    Camera camera;
	Aircraft aircraft;
    void Init(int width, int height);

    void Render(int width, int height, float deltaTime);

private:
    CloudRender *cloudRender;
    SkyBox *skyBox;

    // Build and compile our shader program.
    void loadShaders();

    // Load textures.
    void loadTextures();
};

#endif
