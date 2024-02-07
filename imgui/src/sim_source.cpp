#include <sim_headers.h>

#include <glm/glm.hpp>
#include <random>
#include <chrono>
#include <iostream>
#include <thread>


void framerate_control(std::chrono::high_resolution_clock::time_point& start, std::chrono::high_resolution_clock::time_point& end, float& dtime, const int& target_fps) {
    end = std::chrono::high_resolution_clock::now();
    dtime = (float) std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() / 1000.0f;
    //std::cout << dtime << std::endl;

    float expected_diff = 1.0f / target_fps;
    if (dtime < expected_diff) {
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>((expected_diff - dtime) * 1000)));
    }

    // auto real_end = std::chrono::high_resolution_clock::now();
    // float real_dtime = (float) std::chrono::duration_cast<std::chrono::milliseconds>(real_end - start).count() / 1000.0f;
    // std::cout << real_dtime << std::endl;

    start = std::chrono::high_resolution_clock::now();
}

void simulate(glm::vec2* translations, int particle_count) {
    int resolution = 1024;
    float size = 2.0;

    std::random_device rd {"/dev/urandom"};
    static std::uniform_int_distribution<int> d(0, resolution);
    
    for (int i = 0; i < particle_count; i++) {
        float x = ((float) d(rd) / (float) resolution - 0.5) * size;
        float y = ((float) d(rd) / (float) resolution - 0.5) * size;
        translations[i] = glm::vec2(x, y);
    }
}