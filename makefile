COMPILE_FLAGS = -c -Wall -g -std=c++11 -fPIC -o3
INSTALL_DIR = /usr/local/

all: build/svector.so

build/svector.so: build build/svector.o build/memorymap.o
	 gcc -shared -o build/svector.so build/svector.o build/memorymap.o

build:
	mkdir build

build/svector.o: build svector/svector.cpp 
	 g++ $(COMPILE_FLAGS) -o build/svector.o svector/svector.cpp

build/memorymap.o: build svector/memorymap.cpp
	 g++ $(COMPILE_FLAGS) -o build/memorymap.o svector/memorymap.cpp

build/operators.o: build svector/operators.cpp
	 g++ $(COMPILE_FLAGS) -o build/operators.o svector/operators.cpp

build/plotdata.o: build svector/plotdata.cpp
	 g++ $(COMPILE_FLAGS) -o build/plotdata.o svector/plotdata.cpp

build/ppfile.o: build svector/ppfile.cpp
	 g++ $(COMPILE_FLAGS) -o build/ppfile.o svector/ppfile.cpp

build/sdialog.o: build svector/sdialog.cpp
	 g++ $(COMPILE_FLAGS) -o build/sdialog.o svector/sdialog.cpp

build/serr.o: build svector/serr.cpp
	 g++ $(COMPILE_FLAGS) -o build/serr.o svector/serr.cpp

     
clean:
	 rm -r build
