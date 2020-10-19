test: test.cpp
	g++ -std=c++0x test.cpp -o test -lOpenCL
	./test
