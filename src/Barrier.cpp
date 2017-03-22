//============================
// Barrier.cpp
//
// Created on: 01 nov 2016
//    Author: Andrea Tesei
//============================

#include "Barrier.hpp"

namespace GameOfLife {

/*
 * BusyWait method for this barrier.
 * It accepts a function to execute during the busy wait.
 */
void GameOfLife::Barrier::busyWait(std::function<void()> fun){

	std::size_t lgen = gen.load();

	if ( count.fetch_add(-1) - 1 == 0 ) {
		// The last thread its here.
        // execute the input function.
		fun();
		count.store(thresh);
		gen.fetch_add(1);

	} else {
        // Busy wait logic.
		while ( lgen == gen.load() );

	}
}

}

