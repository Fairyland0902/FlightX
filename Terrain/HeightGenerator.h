//
// Created by longmensparkle on 2018/1/14.
//

#ifndef FLIGHTX_HEIGHTGENERATOR_H
#define FLIGHTX_HEIGHTGENERATOR_H


#include <random>
#include <cmath>

class HeightGenerator {
public:

    HeightGenerator();

    float generateHeight(int x, int z);

    float getNoise(int x, int z);


private:
    float AMPLITUTDE = 40.0f;
    int seed;
    int OCTAVES = 3;
    int xOffset = 0;
    int zOffset = 0;
    float ROUGHNESS = 0.3f;

    float getSmoothNoise(int x, int z);

    float interpolate(float a, float b, float blend);

    float getInterpolatedNoise(float x, float z);
};


#endif //FLIGHTX_HEIGHTGENERATOR_H
