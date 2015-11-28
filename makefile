COMPILE_FLAGS = -c -Wall -g -std=c++11 -fPIC -o3
INSTALL_DIR = /usr/local/

all: build/svector.so

build/svector.so: build build/svector.o build/memorymap.o build/operators.o build/plotdata.o build/ppfile.o build/sdialog.o build/serr.o build/sframe.o build/smath.o build/sminimiser.o build/splot.o
	 gcc -shared -o build/svector.so build/svector.o build/memorymap.o build/operators.o build/plotdata.o build/ppfile.o build/sdialog.o build/serr.o build/sframe.o build/smath.o build/sminimiser.o build/splot.o

build:
	mkdir build

build/svector.o: build svector/svector.cpp 
	 g++ $(COMPILE_FLAGS) -o build/svector.o svector/svector.cpp

build/memorymap.o: build svector/memorymap.cpp
	 g++ $(COMPILE_FLAGS) -o build/memorymap.o svector/memorymap.cpp

build/operators.o: build svector/operators.cpp
	 g++ $(COMPILE_FLAGS) -o build/operators.o svector/operators.cpp

build/plotdata.o: build svector/plotdata.cpp
	 g++ $(COMPILE_FLAGS) `wx-config --cxxflags` -Wno-deprecated -Wno-deprecated-declarations -o build/plotdata.o svector/plotdata.cpp

build/ppfile.o: build svector/ppfile.cpp
	 g++ $(COMPILE_FLAGS) -o build/ppfile.o svector/ppfile.cpp

build/sdialog.o: build svector/sdialog.cpp
	 g++ $(COMPILE_FLAGS) `wx-config --cxxflags` -Wno-deprecated -Wno-deprecated-declarations -o build/sdialog.o svector/sdialog.cpp

build/serr.o: build svector/serr.cpp
	 g++ $(COMPILE_FLAGS) -o build/serr.o svector/serr.cpp

build/sframe.o: build svector/sframe.cpp
	 g++ $(COMPILE_FLAGS) -o build/sframe.o svector/sframe.cpp

build/smath.o: build svector/smath.cpp
	 g++ $(COMPILE_FLAGS) -o build/smath.o svector/smath.cpp

build/sminimiser.o: build svector/sminimiser.cpp
	 g++ $(COMPILE_FLAGS) -o build/sminimiser.o svector/sminimiser.cpp

build/splot.o: build svector/splot.cpp
	 g++ $(COMPILE_FLAGS) `wx-config --cxxflags` -Wno-deprecated -Wno-deprecated-declarations -o build/splot.o svector/splot.cpp

build/sreadwrite.o: build svector/sreadwrite.cpp
	 g++ $(COMPILE_FLAGS) -o build/sreadwrite.o svector/sreadwrite.cpp

build/sstring.o: build svector/sstring.cpp
	 g++ $(COMPILE_FLAGS) -o build/sstring.o svector/sstring.cpp

     
clean:
	 rm -r build
