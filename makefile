COMPILE_FLAGS = -c -Wall -g -std=c++11 -fPIC -o3
INSTALL_DIR = /usr/local/

all: build/svector.so

build/svector.so: build build/svector.o
	 gcc -shared -o build/svector.so build/svector.o

build:
	mkdir build

build/svector.o: build svector/svector.cpp 
	 g++ $(COMPILE_FLAGS) -o build/svector.o svector/svector.cpp
     
clean:
	 rm -r build
