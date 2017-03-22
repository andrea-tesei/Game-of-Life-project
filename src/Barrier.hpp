//==============================================================
// Barrier.hpp : implements a busy wait barrier for the workers.
//
// Created on: 30 ott 2016
// Author: Andrea Tesei
//==============================================================

#ifndef BARRIER_HPP_
#define BARRIER_HPP_

#include <functional>
#include <atomic>

namespace GameOfLife {

class Barrier {
public:

    // Copy constructor
    Barrier(const Barrier&);
    // Copy constructor
    Barrier& operator=(const Barrier&);
    // Constructor
    Barrier(std::size_t count):thresh(count), count(count), gen(0) { }

    void busyWait(std::function<void()> fun);

private:

    // Number of threads
    const std::size_t thresh;

    // Internal count for busyWait
    std::atomic<std::size_t> count;

    // Number of computed generations
    std::atomic<std::size_t> gen;
};

} /* namespace GameOfLife */

#endif /* BARRIER_HPP_ */
