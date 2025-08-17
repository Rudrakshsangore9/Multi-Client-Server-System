\
    // Multithreading_Main.cpp — launches several worker threads
    #include <thread>
    #include <vector>
    #include <string>

    void worker(std::string name, int times, int delay_ms); // from Multithreading.cpp

    int main() {
        std::vector<std::thread> ts;
        ts.emplace_back(worker, "Alpha", 5, 200);
        ts.emplace_back(worker, "Bravo", 5, 300);
        ts.emplace_back(worker, "Charlie", 5, 400);
        for (auto& t : ts) t.join();
        return 0;
    }
