CXXFLAGS = -std=c++17 -Wall -fPIC

JSON_INCLUDE="/N/u/dsalvat/Carbonate/tools/json/include/"

all: dummy_test

hit_digitizer.o: src/hit_digitizer.cpp include/hit_digitizer.h
	g++ -c src/hit_digitizer.cpp $(CXXFLAGS) -I./include/ -I${JSON_INCLUDE} $^ -lm
	g++ -shared -o libhit_digitizer.so $(CXXFLAGS) hit_digitizer.o
	
dummy_test: dummy_test.cpp hit_digitizer.o
	g++ -o $@ $< $(CXXFLAGS) -I./include/ hit_digitizer.o

clean:
	rm hit_digitizer.o include/*.gch dummy_test
