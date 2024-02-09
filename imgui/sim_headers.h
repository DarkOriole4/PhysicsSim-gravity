#ifndef __header_dot_H
#define __header_dot_H

#include <glm/glm.hpp>
#include <iostream>
#include <chrono>


struct matter {
    glm::vec2 position = glm::vec2(0.0f);
    glm::vec2 velocity = glm::vec2(0.0f);
    int mass = 10; // 1 unit represents 10^11 kg
    
};

void create_start_condition(matter* bodies, const int& particle_count);

void simulate(glm::vec2* translations, float* velocities, matter* bodies, int particle_count, const int& fps, const float& sim_speed);

void framerate_control(std::chrono::high_resolution_clock::time_point& start, std::chrono::high_resolution_clock::time_point& end, float& dtime, const int& target_fps);

#endif