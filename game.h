#ifndef _GAME_H
#define _GAME_H

#include "Terrain/Mount.h"
#include "Terrain/Mounts.h"
#include "Terrain/Asphalt.h"
#include "camera.h"
#include "cloud/cloudRender.h"
#include "flare/flareRender.h"
#include "ocean/ocean.h"
#include "Aircraft.h"
#include "Terrain/Terrain.h"
#include "Terrain/Asphalt.h"

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
    Mounts *mounts;
    Trees *tree;

    const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    GLuint depthMapFBO;
    GLuint depthMap;
	int paused;
	int crashed;
    // Build and compile our shader program.
    void loadShaders();

    // Load textures.
    void loadTextures();

    // Init depth mapping frame buffer.
    void initDepthMap();

    void getSamplePoint(std::vector<glm::vec3>& points);
};

#endif
