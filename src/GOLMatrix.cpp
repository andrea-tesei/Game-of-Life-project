//==========================
// GOLMatrix.cpp
//
// Created on: 25 ott 2016
// Author: Andrea Tesei
//==========================
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <algorithm>
#include <fstream>
#include <cmath>
#include "MD5.hpp"
#include "GOLMatrix.hpp"

namespace GameOfLife {

const unsigned char* GOLMatrix::getReadMatrix(){
	return this->matrixRead;
}

const unsigned char* GOLMatrix::getWriteMatrix(){
	return this->matrixWrite;
}


int GOLMatrix::computeValue(const int cell, const int neighboursAlive){
    // Compute the next value of the given cell according to the rules of GameOfLife
	int output =  (neighboursAlive == 2)*(cell == 1) + (neighboursAlive == 3);
	return output;
}

void GOLMatrix::computeNextGenerationRow(const int i){
	const int cols = this->columns;
	const int rowslen = this->rows;
	// Save the three rows for computation in variable (For performance reasons)
    unsigned char *previous = matrixRead + (((i-1+cols) % rowslen) * cols);
	unsigned char *thisline = matrixRead + (i * cols);
	unsigned char *nextline = matrixRead + (((i+1) % rowslen) * cols);
    // Compute the first element for this row (Avoid several cache miss with the direct access)
	const int AlivesForZero = (int) previous[cols-1] + (int) previous[0] + (int) previous[1] +
							(int) thisline[cols-1] + (int) thisline[1] +
							(int) nextline[cols-1] + (int) nextline[0] + (int) nextline[1];
    matrixWrite[(i*cols)] = computeValue(thisline[0], AlivesForZero);
    // Compute the last element for this row (Avoid several cache miss with the direct access)
	const int AlivesForLast = (int) previous[0] + (int) previous[cols-1] + (int) previous[cols-2] +
							  (int) thisline[cols-2] + (int) thisline[0] +
							  (int) nextline[0] + (int) nextline[cols-1] + (int) nextline[cols-2];
	matrixWrite[(i*cols)+cols-1] = computeValue(thisline[cols-1], AlivesForLast);
    // Compute the rest values for this row: used "#pragma ivdep" for vectorization purposes
	#pragma ivdep
	for(auto j = 1; j < cols-1; j++){
		const int cell = thisline[j];
		const int neighboursAlive = (int) previous[j-1] + (int) previous[j] + (int) previous[j+1] +
									(int) thisline[j-1] + 					  (int) thisline[j+1] +
									(int) nextline[j-1] + (int) nextline[j] + (int) nextline[j+1];
		matrixWrite[(i * cols) + j] = computeValue(cell, neighboursAlive);
	}

}

void GOLMatrix::initializeMatrix(const int seedc, const int startRow, const int endRow){
	unsigned short seed[3] = {seedc,0,seedc};
	const int rowslen = this->rows;
	const int cols = this->columns;
	for(auto i = startRow; i <= endRow; i++){
        // Initialize the read matrix with random values: used "#pragma ivdep" for vectorization purposes
		#pragma ivdep
		for(auto j = 0; j < cols; j++){
			double rand = nrand48(&seed[0]);
			matrixRead[(i * cols) + j] = ((int)ceil(rand)) % 2;
		}
	}
}

void GOLMatrix::initializeTest1Matrix(){
	this->rows = 7;
	this->columns = 7;
	this->matrixRead = new unsigned char[this->rows*this->columns]{ 0,0,0,0,0,0,0,
																	0,0,1,0,0,0,0,
																	0,0,1,0,0,0,0,
																	0,0,1,0,0,0,0,
																	0,0,0,0,0,0,0,
																	0,0,0,0,0,0,0,
																	0,0,0,0,0,0,0 };

	this->matrixWrite = new unsigned char[this->rows * this->columns];
}

void GOLMatrix::initializeTest3Matrix(){
	this->rows = 30;
	this->columns = 30;
	this->matrixRead = new unsigned char[this->rows*this->columns]{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
																	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
																	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
																	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
																	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
																	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
																	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
																	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
																	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
																	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
																	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
																	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
																	0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,
																	0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
																	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,
																	0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,
																	0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,
																	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
																	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
																	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
																	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
																	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
																	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
																	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
																	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
																	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
																	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
																	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
																	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
																	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

	this->matrixWrite = new unsigned char[this->rows * this->columns];
}

void GOLMatrix::initializeTest2Matrix(){
	this->rows = 8;
	this->columns = 8;
	this->matrixRead = new unsigned char[this->rows*this->columns]{ 1,0,0,0,0,0,0,0,
																	0,1,1,0,0,0,0,0,
																	1,1,0,0,0,0,0,0,
																	0,0,0,0,0,0,0,0,
																	0,0,0,0,0,0,0,0,
																	0,0,0,0,0,0,0,0,
																	0,0,0,0,0,0,0,0,
																	0,0,0,0,0,0,0,0};

	this->matrixWrite = new unsigned char[this->rows * this->columns];
}


std::string GOLMatrix::getConfigurationHash(){
	return std::md5(std::string(reinterpret_cast<char*>(this->matrixRead)));
}

void GOLMatrix::printMatrix(){
	for(int i = 0; i < this->rows; i++){
		std::cout << std::endl;
		for(int j = 0; j < this->columns; j++)
			std::cout << " " << (int) matrixRead[(i * this->columns) + j] << " ";
	}
	std::cout << "\n";
	std::cout.flush();
}

void GOLMatrix::swapMatrices(){
	std::swap(this->matrixRead, this->matrixWrite);
}

} /* namespace GameOfLife */
