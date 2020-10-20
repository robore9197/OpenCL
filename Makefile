test: test.cpp
	g++ -g -std=c++0x test.cpp -o test -lOpenCL
	./test
