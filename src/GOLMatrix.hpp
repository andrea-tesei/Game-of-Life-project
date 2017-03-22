//==============================================================================
// GOLMatrix.hpp : implements the matrix for GameOfLife with associated methods.
//
// Created on: 25 ott 2016
// Author: Andrea Tesei
//==============================================================================

#ifndef GOLMATRIX_HPP_
#define GOLMATRIX_HPP_
#include <iostream>

namespace GameOfLife {

// Type of a single cell of matrix
typedef unsigned long long u64b;

class GOLMatrix {
private:
    // Function computeValue: it compute the next value for cell, given the neighbours's values
	int computeValue(const int cell, const int neighboursAlive);

public:
    // Number of rows
	int rows;
    // Number of columns
	int columns;
    // Read matrix: used to read the previous generation values
	unsigned char *matrixRead;
    // Write matrix: used to write the next generation values
	unsigned char *matrixWrite;
	//GOLMatrix(void);
    // Standard constructor given #rows and #columns
	GOLMatrix(int const rows, int const columns):rows(rows),columns(columns){
		this->matrixRead = new unsigned char[rows * columns]();
		this->matrixWrite = new unsigned char[rows * columns]();
	}
    // Copy constructor
    GOLMatrix(const GOLMatrix& other){
        this->rows = other.rows;
        this->columns = other.columns;
        this->matrixRead = new unsigned char[this->rows * this->columns];
        this->matrixWrite = new unsigned char[this->rows * this->columns];
        std::copy(other.matrixRead, other.matrixRead + (this->rows * this->columns), this->matrixRead);
        std::copy(other.matrixWrite, other.matrixWrite+ (this->rows * this->columns), this->matrixWrite);
    }
    // Copy constructor
	GOLMatrix(GOLMatrix&& other){
		this->rows = other.rows;
		this->columns = other.columns;
		this->matrixRead = new unsigned char[this->rows * this->columns];
		this->matrixWrite = new unsigned char[this->rows * this->columns];
		swap(*this, other);
	}
    // Assign operator
	GOLMatrix& operator= (const GOLMatrix& other){
		GOLMatrix tmp(other);
	    std::swap(*this, tmp);
		return *this;
	}
    // Swap method implementation (Hp: the two matrices have the same rows/columns values)
	friend void swap(GOLMatrix& first, GOLMatrix& second){
		std::swap(first.matrixRead,second.matrixRead);
		std::swap(first.matrixWrite, second.matrixWrite);
	}
    // Destructor
    virtual ~GOLMatrix(){
        delete [] this->matrixRead;
        delete [] this->matrixWrite;
    }
    // Getter for the read matrix
	const unsigned char* getReadMatrix();
    // Getter for the write matrix
	const unsigned char* getWriteMatrix();
    // Initialize Matrix with random values starding from seedc
	void initializeMatrix(const int seedc, const int startRow, const int endRow);
    // Initialize Matrix to Test1 for correctness (Blinker configuration)
	void initializeTest1Matrix();
    // Initialize Matrix to Test2 for correctness (Glider configuration)
	void initializeTest2Matrix();
    // Initialize Matrix to Test3 for correctness (One example of unbounded growth)
	void initializeTest3Matrix();
    // Function computeNextGenerationRow: computes the next generation for the given row index
	void computeNextGenerationRow(const int i);
    // Function swapMatrices: swap the two matrices on the fly
	void swapMatrices();
    // Function printMatrix: print the matrixRead
	void printMatrix();
    // Function getConfigurationHash: compute md5 value of the matrices (for correctness)
	std::string getConfigurationHash();
};

} /* namespace GOLMatrix */

#endif /* GOLMATRIX_HPP_ */
