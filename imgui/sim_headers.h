#ifndef __header_dot_H
#define __header_dot_H

#include <glm/glm.hpp>
#include <iostream>
#include <chrono>

void simulate(glm::vec2* translations, int particle_count);

void framerate_control(std::chrono::high_resolution_clock::time_point& start, std::chrono::high_resolution_clock::time_point& end, float& dtime, const int& target_fps);

#endif