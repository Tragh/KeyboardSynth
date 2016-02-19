CXX=g++
CXXFLAGS=-O2 -march=native -std=gnu++11 -I/usr/include/python3.4/
LDLIBS=-lSDL2 -lFLAC -lFLAC++ -lpython3.4 -lboost_python-3.4 -lpthread
SOURCES=main.cc flacwriter.cc sounder.cc
HEADERS=flacwriter.h sounder.h
OBJECTS=$(SOURCES:%.cc=%.o)
EXECUTABLE=main

all: $(HEADERS) $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(LDLIBS) $(OBJECTS) -o $@

main.o : main.cc synths.h $(HEADERS)
	$(CXX) $(CXXFLAGS) -c main.cc -o main.o
	
sounder.o: sounder.cc sounder.h
	$(CXX) $(CXXFLAGS) -c $< -o $@
	
.cc.o: macros.h
	$(CXX) $(CXXFLAGS) -c $< -o $@
	
clean:
	rm $(OBJECTS)
