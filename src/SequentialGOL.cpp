//============================================================================
// Name        : GameOfLifeSEQ.cpp
// Author      : Andrea Tesei
// Version     : 1.0
// Copyright   : 
// Description : [SPM] Game of Life (sequential version)
//============================================================================

#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <chrono>
#include <sched.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/time.h>
#include "GOLMatrix.hpp"
using namespace std;

#define MAXWIDTH 10
#define MAXLENGTH 10

GameOfLife::GOLMatrix *matrix;

int main(int argc, char* argv[]) {
	using namespace std::chrono;
	int i , j, iterations, opt, width, length, seed;
	int print = 0;
	int test = -1;
	int n = 1;
	if(argc < 4){
		cerr << "Usage: " << argv[0] << " -r num_rows -c num_columns -i num_iterations -s seed -t test_code -p print" << "\n";
		return -1;
	}
    // Get user parameters from console
	while ((opt = getopt (argc, argv, "r:c:i:s:t:p:")) != -1){
		switch (opt)
		{
		case 'r':
			width = atoi(optarg);
			break;
		case 'c':
			length = atoi(optarg);
			break;
		case 'i':
			iterations = atoi(optarg);
			break;
		case 's':
			seed = atoi(optarg);
			break;
		case 't':
			test = atoi(optarg);
			break;
		case 'p':
			print = atoi(optarg);
			break;
		case '?':
			std::cout << "Usage: " << argv[0] << " -r num_rows -c num_columns -i num_iterations -s seed -t test_code" << "\n";
			return 1;
		default:
			std::cout << "Usage: " << argv[0] << " -r num_rows -c num_columns -i num_iterations -s seed -t test_code" << "\n";
			return -1;
		}
	}
	// Initial conditions: matrix filled with integer pseudo-randomly choosen
	matrix = new GameOfLife::GOLMatrix(width, length);
    // Set cpu affinity for Many Integrated Core Architecture (MIC)
#ifndef __MIC__
	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);
	CPU_SET(0, &cpuset);
	int rc = sched_setaffinity(0, sizeof(cpu_set_t), &cpuset);
#endif
	high_resolution_clock::time_point start = high_resolution_clock::now();
    // Perform initialization test case given by the user
	if(test != -1){
		switch(test){
			case 1:
				width = 7;
				length = 7;
				matrix->initializeTest1Matrix();
				break;
			case 2:
				width = 8;
				length = 8;
				matrix->initializeTest2Matrix();
				break;
			case 3:
				width = 30;
				length = 30;
				matrix->initializeTest3Matrix();
				break;
		}
	} else
		matrix->initializeMatrix(seed, 0, width-1);
	// Start iterations
	int cpu = sched_getcpu();
	while(n <= iterations){
		for(i=0; i < width; i++){
			matrix->computeNextGenerationRow(i);
		}
		matrix->swapMatrices();
		if(print)
			matrix->printMatrix();
		n++;
	}
	high_resolution_clock::time_point end = high_resolution_clock::now();
	std::cout << "Hash results = " << matrix->getConfigurationHash() << std::endl;
	std::cout << std::fixed;
	std::cout << "Completion time = " << duration_cast<duration<int, std::milli>>(end - start).count() << "\n";
    return 0;
}
