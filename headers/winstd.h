#ifndef WINSTD_H
#define WINSTD_H

#include <chrono>
#include <thread>

// Sleep thread in windows environment
void usleep (unsigned int ms){
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

#endif