//============================================================================
// Name        : ThreadGOL.cpp
// Author      : Andrea Tesei
// Version     :
// Copyright   :
// Description : [SPM] Game of Life (multi-threaded version)
//============================================================================


#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <chrono>
#include <ctype.h>
#include <unistd.h>
#include <signal.h>
#include <thread>
#include <pthread.h>
#include <sys/time.h>
#include "GOLMatrix.hpp"
#include "Barrier.hpp"

using namespace std;
using namespace std::chrono;

#define MAXWIDTH 10
#define MAXLENGTH 10

// GOLMatrix for computation
GameOfLife::GOLMatrix *matrix;

// Barrier for busy wait
GameOfLife::Barrier *barrier;

// Boolean value for termination
bool static endgame = false;

volatile sig_atomic_t sigalarm_flag = 0;

// Signal handler
void handler (int signal){
	sigalarm_flag = 1;
}

// Function worker: receive a bunch of rows and perform computation of the next generation
void worker(int startRow, int endRow, int width, int length, int iterations, int indexOfThread){
	int cpu = sched_getcpu();
	for(int n=1; n <= iterations; n++){
		for(int i=startRow; i <= endRow; i++){
			matrix->computeNextGenerationRow(i);
		}
        // Do busy wait: the last thread which enters the barrier will perform the swapping phase
		barrier->busyWait([&]{
			matrix->swapMatrices();
		});

	}
}

int main(int argc, char* argv[]) {
	int opt, i , j, iterations, rest, division, width, length, nworkers, seed;
	struct sigaction s;
	std::thread *threads;
	if(argc < 4){
		cerr << "Usage: " << argv[0] << " -r num_rows -c num_columns -i num_iterations -s seed -n nworkers" << "\n";
		return -1;
	}
    // Get user parameters from console
	while ((opt = getopt (argc, argv, "r:c:i:n:s:")) != -1){
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
		case 'n':
			nworkers = atoi(optarg);
			break;
		case 's':
			seed = atoi(optarg);
			break;
		case '?':
			std::cout << "Usage: " << argv[0] << " -r num_rows -c num_columns -i num_iterations -s seed -n nworkers" << "\n";
			return 1;
		default:
			std::cout << "Usage: " << argv[0] << " -r num_rows -c num_columns -i num_iterations -s seed -n nworkers" << "\n";
			return -1;
		}
	}

    // Initialize the Barrier
	barrier = new GameOfLife::Barrier(nworkers);

    // Initialize the array of threads
	threads = new std::thread[nworkers];
    
    // Retrieve the number of cores
	int numCPUs = sysconf(_SC_NPROCESSORS_ONLN);

	s.sa_handler = NULL;
    
    // Initialize the signal handler
	memset(&s, 0, sizeof(s));
	
    // Signal listener SIGINT e SIGTERM
	s.sa_handler = handler;
	sigaction(SIGINT, &s, NULL);
	sigaction(SIGTERM, &s, NULL);

	// Initial conditions: matrix filled with integer randomly choosen
	matrix = new GameOfLife::GOLMatrix(width, length);
	high_resolution_clock::time_point start = high_resolution_clock::now();
	matrix->initializeMatrix(seed, 0, width-1);
	// Start iterations
	rest = width % nworkers;
	division = floor(double(width / nworkers));
	for(int x = 0; x < nworkers; x++){
		if(rest > 0){
// Calculate core_id for cpu affinity for Many Integrated Core Architecture (MIC)
#ifdef __MIC__
         size_t core_id = (x*4 + x/(numCPUs/4) ) % numCPUs;
#else
// Calculate core_id for cpu affinity for other architectures
         size_t core_id = x % numCPUs;
#endif
            // Set cpu affinity and partition statically the matrix among the threads
         	cpu_set_t cpuset;
			CPU_ZERO(&cpuset);
			CPU_SET(core_id, &cpuset);
			threads[x] = std::thread(worker, division*x, division*x + division, width, length, iterations, x);//move(t);
			int rc = pthread_setaffinity_np(threads[x].native_handle(), sizeof(cpu_set_t), &cpuset);
			rest--;
		} else {
// Set core_id for affinity
#ifdef __MIC__
         size_t core_id = (x*4 + x/(numCPUs/4) ) % numCPUs;
#else
         size_t core_id = x % numCPUs;
#endif
			cpu_set_t cpuset;
			CPU_ZERO(&cpuset);
			CPU_SET(core_id, &cpuset);
			threads[x] = std::thread(worker, division*x, division*x + division - 1, width, length, iterations, x);//move(t);
			int rc = pthread_setaffinity_np(threads[x].native_handle(), sizeof(cpu_set_t), &cpuset);
		}
	}
	for(int x = 0; x < nworkers; x++)
        // Wait the result
		threads[x].join();
	high_resolution_clock::time_point end = high_resolution_clock::now();
	std::cout << "Hash results = " << matrix->getConfigurationHash() << std::endl;
	std::cout << std::fixed;
	std::cout << "Completion time = " << duration_cast<duration<int, std::milli>>(end - start).count() << "\n";
	return 0;
}
