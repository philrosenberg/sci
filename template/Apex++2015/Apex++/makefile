PROJECT = test

all : $(PROJECT)

$(PROJECT) :
	g++ `wx-config --cxxflags --libs` -Wno-deprecated -Wno-deprecated-declarations --std=c++11 -w -lplplot -lplplotcxx -lalg -lshp -lsvector -lresample -lnetcdf -lcurl *.cpp -o $(PROJECT)
