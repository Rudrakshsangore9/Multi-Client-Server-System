\
    CXX ?= g++
    CXXFLAGS ?= -std=c++17 -O2 -pthread -Wall -Wextra -pedantic

    all: server client multiserver threads

    server: Server.cpp
    	$(CXX) $(CXXFLAGS) $< -o $@

    client: Client.cpp
    	$(CXX) $(CXXFLAGS) $< -o $@

    multiserver: MultiClientServer.cpp
    	$(CXX) $(CXXFLAGS) $< -o $@

    threads: Multithreading.cpp Multithreading_Main.cpp
    	$(CXX) $(CXXFLAGS) $^ -o $@

    clean:
    	rm -f server client multiserver threads
