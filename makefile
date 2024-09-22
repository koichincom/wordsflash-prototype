CXX = g++
CXXFLAGS = -std=c++17 -lcurl

all: flashcarder

flashcarder: main.cpp flashcard.cpp
	$(CXX) $(CXXFLAGS) -o flashcarder main.cpp flashcard.cpp

clean:
	rm -f flashcarder
