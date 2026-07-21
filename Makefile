CXX = g++
CXXFLAGS = -O3 -Wall

all: main

main: main.o heuristic.o EVRP.o stats.o stats_evolution.o
$(CXX)$(CXXFLAGS) main.o heuristic.o EVRP.o stats.o stats_evolution.o -o main

main.o: main.cpp
$(CXX)$(CXXFLAGS) -c main.cpp

heuristic.o: heuristic.cpp
$(CXX)$(CXXFLAGS) -c heuristic.cpp

EVRP.o: EVRP.cpp
$(CXX)$(CXXFLAGS) -c EVRP.cpp

stats.o: stats.cpp
$(CXX)$(CXXFLAGS) -c stats.cpp

stats_evolution.o: stats_evolution.cpp
$(CXX)$(CXXFLAGS) -c stats_evolution.cpp

clean:
rm -f *.o main
