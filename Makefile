CXXFLAGS = -std=c++17 -Wall

JSON_INCLUDE="/N/u/dsalvat/Carbonate/tools/json/include/"

all: dummy_test

waveformer.o: src/waveformer.cpp include/waveformer.h
	g++ -c src/waveformer.cpp $(CXXFLAGS) -I./include/ -I${JSON_INCLUDE} $^ -lm
	
dummy_test: dummy_test.cpp waveformer.o
	g++ -o $@ $< $(CXXFLAGS) -I./include/ waveformer.o

clean:
	rm waveformer.o include/*.gch dummy_test
