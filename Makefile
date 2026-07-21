CXX = g++
CXXFLAGS = -O3 -Wall

TARGET = main

SRCS = main.cpp \
       heuristic.cpp \
       EVRP.cpp \
       stats.cpp \
       stats_evolution.cpp

OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean