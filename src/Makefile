TARGET_EXEC := main

CXX = g++
CXXFLAGS = -std=c++17 -march=native -I ../lib/eigen-3.4.0


SRCS := $(shell find -name '*.cpp')
OBJS := $(SRCS:%.cpp=%.o)


$(TARGET_EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET_EXEC) $(OBJS)

main.o: main.cpp lsh_forest.cpp util.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

lsh_forest.o: lsh_forest.cpp util.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

util.o: util.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean

clean:
	rm main $(OBJS)
