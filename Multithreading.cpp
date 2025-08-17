\
    // Multithreading.cpp — simple thread helpers
    #include <chrono>
    #include <iostream>
    #include <string>
    #include <thread>
    #include <vector>

    void worker(std::string name, int times, int delay_ms) {
        for (int i = 1; i <= times; ++i) {
            std::cout << name << " -> " << i << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
        }
    }
