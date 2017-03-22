//===============================================================================
// ParallelGOLff.cpp : Parallel implementation of GameOfLife (FastFlow framework)
//
// Created on: 21 ott 2016
// Author: Andrea Tesei
//===============================================================================
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <ctime>
#include <cmath>
#include <chrono>
#include <ff/farm.hpp>
#include "GOLMatrix.hpp"
using namespace ff;
using namespace std::chrono;

// Custom structure for task assigned to workers
typedef struct Task_t{
	Task_t(int startRow, int endRow):startRow(startRow),endRow(endRow){	}
	const int startRow, endRow;
} GOLTask;

// GOLMatrix for computation
GameOfLife::GOLMatrix *matrix;

// Custom Emitter for the farm
struct Emitter: ff_node_t<GOLTask> {
    // Custom load balancer for Emitter
	ff_loadbalancer *const lb;
	int rows, columns, numIterations, grain, receivedResults, rest, division, nworkers, seed;
	bool send;

    // Emitter constructor: take the custom load balancer, #rows, #columns, #iterations, #workers, the initial seed for initialization phase
	Emitter(ff_loadbalancer *const lb, int rows, int columns, int iterations, int nworkers, int seed)
	:lb(lb),rows(rows),columns(columns),numIterations(iterations), grain(grain), nworkers(nworkers),seed(seed),receivedResults(0), send(true){
		division = this->rows / this->nworkers;
		matrix = new GameOfLife::GOLMatrix(this->rows, this->columns);
		matrix->initializeMatrix(this->seed, 0, this->rows-1);
	}

    // Implementation of the svc method: actually performs the scheduling of the various partions among the farm's workers
	GOLTask *svc(GOLTask *partialResult) {
		const int nw = lb->getNWorkers();
		if(partialResult != nullptr){
            // Receive the partial result from workers
			delete partialResult;
			receivedResults++;
			if(receivedResults == this->nworkers){
                // All the rows are computed: perform the swap operation
				matrix->swapMatrices();
				numIterations--;
				receivedResults = 0;
				this->send = true;
			}
		}
		if (send) {
            // Send next tasks to the workers
			if(numIterations == 0)
                // Send End-Of-Stream to each worker
				lb->broadcast_task(EOS);
			else {
				// Produce iteration
				int rest = this->rows % nw;
				for(int x = 0; x < nw; x++){
                    // Initialize the GOLTask for an available worker
                    if(rest > 0){
                        GOLTask *n = new GOLTask(division*x, division*x + division);
						rest--;
						ff_send_out( (void*)n);
					} else {
						GOLTask *n = new GOLTask(division*x, division*x + division - 1);
						ff_send_out( (void*)n);
					}
				}
				this->send = false;
				return GO_ON;
			}
		}
		return GO_ON;
	}
};

// Custom worker logic for the farm
struct GOLWorker: ff_node_t<GOLTask>{

	int rows;
	int columns;

	GOLWorker(int rows, int columns):rows(rows), columns(columns){	}

	GOLTask *svc(GOLTask *task){
		for(int i = task->startRow; i <= task->endRow; i++){
			matrix->computeNextGenerationRow(i);
		}
		return task;
	}
};



int main(int argc, char *argv[]){
	int opt, i , j, iterations, rest, division, rows, columns, nworkers, seed;
	int n = 1;
	int test = -1;
	if(argc < 4){
		std::cerr << "Usage: " << argv[0] << " -r num_rows -c num_columns -i num_iterations -n par_degree -s seed" << "\n";
		return -1;
	}
    // Get user parameters from console
	while ((opt = getopt (argc, argv, "r:c:i:n:s:")) != -1){
		switch (opt)
		{
		case 'r':
			rows = atoi(optarg);
			break;
		case 'c':
			columns = atoi(optarg);
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
			std::cout << "Usage: " << argv[0] << " -r num_rows -c num_columns -i num_iterations -s seed" << "\n";
			return 1;
		default:
			std::cout << "Usage: " << argv[0] << " -r num_rows -c num_columns -i num_iterations -s seed" << "\n";
			return -1;
		}
	}

	// Initialize workers
	std::vector<std::unique_ptr<ff_node>> workers;

	for(int i = 0; i < nworkers; i++)
		workers.push_back(make_unique<GOLWorker>(rows, columns));

    // Initialize the FastFlow farm
	ff_Farm<GOLTask> farmz(std::move(workers));
	// Initialize custom emitter
	Emitter E(farmz.getlb(), rows, columns, iterations, nworkers, seed);
	// Add custom emitter to the Farm and remove collector
    farmz.add_emitter(E);
	farmz.remove_collector();
    // Set scheduling on demand policy
    farmz.set_scheduling_ondemand();
    // Add feedback channel from each worker through the emitter
    farmz.wrap_around();
	high_resolution_clock::time_point start = high_resolution_clock::now();
	// Wait the result
    if(farmz.run_and_wait_end()<0) {
		error("running farm\n");
		return -1;
	}
	high_resolution_clock::time_point end = high_resolution_clock::now();
	std::cout << "Hash results = " << matrix->getConfigurationHash() << std::endl;
	std::cout << std::fixed;
	std::cout << "Completion time = " << duration_cast<duration<int, std::milli>>(end - start).count() << "\n";
	matrix->~GOLMatrix();
	return 0;
}




