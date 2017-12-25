#ifndef _GAME_H
#define _GAME_H

#include "camera.h"
#include "cloud/cloudRender.h"
#include "skybox.h"

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
    SkyBox *skyBox;

    // Build and compile our shader program.
    void loadShaders();

    // Load textures.
    void loadTextures();
};

#endif
