//
// Created by qweer29 on 1/15/18.
//

#ifndef FLIGHTX_MOUNTS_H
#define FLIGHTX_MOUNTS_H


#include <vector>
#include "Mount.h"
#include <unordered_map>

struct KeyHash {
    std::size_t operator()(const std::pair<int, int>& k) const
    {
        return std::hash<int>()(k.first) ^
               (std::hash<int>()(k.second) << 1);
    }
};

struct KeyEqual {
    bool operator()(const std::pair<int, int>& lhs, const std::pair<int, int>& rhs) const
    {
        return lhs.first == rhs.first && lhs.second == rhs.second;
    }
};

class Mounts {
public:
    Mounts(int width, int height);

    bool isRendering = false;

    void Draw();

    float getHeight(float worldX, float worldZ);

protected:
    int width, height;
    std::vector<Mount> mounts;
    HeightGenerator *generator;

    std::pair<int, int> current_chunk;

    std::pair<int, int> getChunk();

    std::unordered_map<std::pair<int, int>, int, KeyHash, KeyEqual> chunk_map;

    void DrawChunk(std::pair<int, int> chunk, int x_offset, int z_offset);

    int cache_size = 1;
};


#endif //FLIGHTX_MOUNTS_H
