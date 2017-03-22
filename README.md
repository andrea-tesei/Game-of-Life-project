# Game of Life project

For the final project of the Distributed Systems: Paradigms and Models exam i've developed three different versions of "Game of Life": one concurrent version implemented with standard C++11 threads; one parallel version implemented with an existing parallel programming framework named [FastFlow](http://calvados.di.unipi.it/)(studied during the course); and a sequential version, user for performance evaluation.
For further informations and to read the rules of the Game of Life, you can read [here](https://en.wikipedia.org/wiki/Conway’s_Game_of_Life).

# Test machines

This project has been tested on a workstation with two Intel Xeon CPU E5-2650 with 8 cores each, and a Xeon Phi coprocessor with 60 cores. All compilation flags specified in the Makefile are optimized for this two machines, but you can easily adapt the compilation commands to a commodity computer.

# How to execute the code

Once you've cloned this repository, move on the root directory. At this point you can compile the three version by running:
```c++
make golhost // compile code for Xeon Host machine
make golmic // compile code for Xeon Phi MIC machine
```
Now you have created the executable file for all three versions. You can run the sequential version with the following command:
```
SequentialGOL[.mic].out -r #rows -c #columns -i #iterations -s starting_seed
```
You can run the concurrent version with the following command:
```
ThreadGOL[.mic].out -r #rows -c #columns -i #iterations -s starting_seed
```
Or finally you can run the parallel version with the following command:
```
FastFlowGOL[.mic].out -r #rows -c #columns -i #iterations -s starting_seed
```
Note that you can execute the test cases only with the sequential version by adding <b>-t</b> option to the input of the program, specifying one value out of <b>1,2,3</b>. For debug purposes, you can also print the matrix in the sequential version adding <b>-p</b> option to the input of the program.

# Other details

For further explanation about the design choices, the skeleton choosen for the paralell version, the performance models/evaluation and other stuff, you can read my final report [here](https://github.com/andrea-tesei/Game-of-Life-project/blob/master/report_tesei.pdf).
