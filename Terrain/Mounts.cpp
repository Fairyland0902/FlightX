//
// Created by qweer29 on 1/15/18.
//

#include <iostream>
#include <thread>
#include "Mounts.h"

using namespace std;

pair<int, int> getLeftChunk(pair<int, int> chunk) {
    return make_pair(chunk.first - 1, chunk.second);
}

pair<int, int> getRightChunk(pair<int, int> chunk) {
    return make_pair(chunk.first + 1, chunk.second);
}

pair<int, int> getTopChunk(pair<int, int> chunk) {
    return make_pair(chunk.first, chunk.second + 1);
}

pair<int, int> getDownChunk(pair<int, int> chunk) {
    return make_pair(chunk.first, chunk.second - 1);
}

pair<int, int> getLeftTopChunk(pair<int, int> chunk) {
    return make_pair(chunk.first - 1, chunk.second + 1);
}

pair<int, int> getLeftDownChunk(pair<int, int> chunk) {
    return make_pair(chunk.first - 1, chunk.second - 1);
}

pair<int, int> getRightDownChunk(pair<int, int> chunk) {
    return make_pair(chunk.first + 1, chunk.second - 1);
}

pair<int, int> getRightTopChunk(pair<int, int> chunk) {
    return make_pair(chunk.first + 1, chunk.second + 1);
}

void Mounts::Draw() {

    current_chunk = getChunk();
//    std::cout << current_chunk.first << " " << current_chunk.second << std::endl;
    mounts[chunk_map[current_chunk]].Draw();

    int x_offset = Mount::chunk_width - 1;
    int z_offset = Mount::chunk_height - 1;

    auto left_chunk = getLeftChunk(current_chunk);
    DrawChunk(left_chunk, -x_offset, 0);

    auto right_chunk = getRightChunk(current_chunk);
    DrawChunk(right_chunk, x_offset, 0);


    auto top_chunk = getTopChunk(current_chunk);
    DrawChunk(top_chunk, 0, z_offset);

    auto down_chunk = getDownChunk(current_chunk);
    DrawChunk(down_chunk, 0, -z_offset);

    auto left_top_chunk = getLeftTopChunk(current_chunk);
    DrawChunk(left_top_chunk, -x_offset, z_offset);

    auto left_down_chunk = getLeftDownChunk(current_chunk);
    DrawChunk(left_down_chunk, -x_offset, -z_offset);

    auto right_down_chunk = getRightDownChunk(current_chunk);
    DrawChunk(right_down_chunk, x_offset, -z_offset);

    auto right_top_chunk = getRightTopChunk(current_chunk);
    DrawChunk(right_top_chunk, x_offset, z_offset);

}


Mounts::Mounts(int width, int height) : width(width), height(height) {
    generator = new HeightGenerator();

    int x_offset = Mount::chunk_width - 1;
    int z_offset = Mount::chunk_height - 1;


    for (int i = -cache_size; i <= cache_size; i++)
        for (int j = -cache_size; j <= cache_size; j++) {
            chunk_map[make_pair(i, j)] = mounts.size();
            mounts.emplace_back(width, height, mounts.size(), i * x_offset, j * z_offset, generator);
        }


    std::vector<thread> threads;
    for (Mount &mount: mounts) {
        mount.init();
//        threads.emplace_back(&Mount::init, std::ref(mount));
//        cout << mount.id << endl;
    }

}

std::pair<int, int> Mounts::getChunk() {
    float x = currentcamera->Position.x;
    float y = currentcamera->Position.z;
    x += Mount::chunk_width / 2;
    y += Mount::chunk_height / 2;

    return std::make_pair(static_cast<int>(floor(x / Mount::chunk_width)),
                          static_cast<int>(floor(y / Mount::chunk_height)));
}

void Mounts::DrawChunk(std::pair<int, int> chunk, int x_offset, int z_offset) {
    if (chunk_map.find(chunk) != chunk_map.end()) {
        mounts[chunk_map[chunk]].Draw();
    } else {
        mounts.emplace_back(width, height, mounts.size(),
                            mounts[chunk_map[current_chunk]].x_offset + x_offset,
                            mounts[chunk_map[current_chunk]].z_offset + z_offset,
                            generator);
        chunk_map[chunk] = mounts.size() - 1;
        mounts[mounts.size() - 1].init();
        mounts[mounts.size() - 1].Draw();
    }
}

