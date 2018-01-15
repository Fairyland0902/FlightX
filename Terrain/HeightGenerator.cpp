//
// Created by longmensparkle on 2018/1/14.
//

#include <iostream>
#include "HeightGenerator.h"

HeightGenerator::HeightGenerator() {
    std::uniform_int_distribution<int> distribution{0, static_cast<int>(1e9)};
    std::random_device rd;
    seed = distribution(rd);
}

float HeightGenerator::getNoise(int x, int z) {
    std::uniform_real_distribution<float> real_dist;
    std::mt19937 pseudo_rd{static_cast<unsigned int>(seed + x * 233 + z * 23333)};
    return real_dist(pseudo_rd) * 2.0f - 1;
}

float HeightGenerator::getSmoothNoise(int x, int z) {
    float corners = (getNoise(x - 1, z - 1) + getNoise(x + 1, z - 1) + getNoise(x - 1, z + 1)
                     + getNoise(x + 1, z + 1)) / 16.f;
    float sides = (getNoise(x - 1, z) + getNoise(x + 1, z) + getNoise(x, z - 1)
                   + getNoise(x, z + 1)) / 8.f;
    float center = getNoise(x, z) / 4.f;
    return corners + sides + center;
}

float HeightGenerator::generateHeight(int x, int z) {
    float total = 0;
    float d = (float) pow(2, OCTAVES - 1);
    for (int i = 0; i < OCTAVES; i++) {
        float freq = (float) (pow(2, i) / d);
        float amp = (float) pow(ROUGHNESS, i) * AMPLITUTDE;
        total += getInterpolatedNoise((x + xOffset) * freq, (z + zOffset) * freq) * amp;
    }
    return total;
}

float HeightGenerator::interpolate(float a, float b, float blend) {
    double theta = blend * M_PI;
    float f = (float) (1.f - cos(theta)) * 0.5f;
    return a * (1.f - f) + b * f;
}

float HeightGenerator::getInterpolatedNoise(float x, float z) {
    int intX = (int) x;
    int intZ = (int) z;
    float fracX = x - intX;
    float fracZ = z - intZ;

    float v1 = getSmoothNoise(intX, intZ);
    float v2 = getSmoothNoise(intX + 1, intZ);
    float v3 = getSmoothNoise(intX, intZ + 1);
    float v4 = getSmoothNoise(intX + 1, intZ + 1);
    float i1 = interpolate(v1, v2, fracX);
    float i2 = interpolate(v3, v4, fracX);
    return interpolate(i1, i2, fracZ);
}
