# ---------------------------------------------------------------------------
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License version 2 as 
#  published by the Free Software Foundation.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
#
#  As a special exception, you may use this file as part of a free software
#  library without restriction.  Specifically, if other files instantiate
#  templates or use macros or inline functions from this file, or you compile
#  this file and link it with other files to produce an executable, this
#  file does not by itself cause the resulting executable to be covered by
#  the GNU General Public License.  This exception does not however
#  invalidate any other reasons why the executable file might be covered by
#  the GNU General Public License.
#
# ---------------------------------------------------------------------------

# 
# FF_ROOT     pointing to the FastFlow root directory (i.e.
#             the one containing the ff directory).
FF_ROOT		= /home/spm1501/public/fastflow

CXX		= icc -std=c++11 -DNO_DEFAULT_MAPPING
INCLUDES	= -I $(FF_ROOT) 
CXXFLAGS  	= -Wall

LDFLAGS 	= -pthread 
OPTFLAGS	= -O3 -finline-functions 
#MMIC		= -mmic

OPTFLAGS_DEBUG  = -O0 -g -traceback -debug all 
OPTFLAGS_PERF	= -pg -vec-report5

.PHONY: clean libs

golhost		: clean SequentialGOL.out ThreadGOL.out FastFlowGOL.out
golmic		:	MMIC	=	-mmic
golmic		: clean SequentialGOL.mic.out ThreadGOL.mic.out FastFlowGOL.mic.out
debug		:	OPTFLAGS	=	-O0 -g -traceback -debug all
debug		: clean SequentialGOL.debug.out ThreadGOL.debug.out FastFlowGOL.debug.out
perf		: clean SequentialGOL.perf.out ThreadGOL.perf.out FastFlowGOL.perf.out

SequentialGOL.out: src/SequentialGOL.cpp libs/GOLMatrix.o libs/MD5.o
	$(CXX) $^ $(CXXFLAGS) $(OPTFLAGS) -o $@ $(LDFLAGS)

SequentialGOL.mic.out: src/SequentialGOL.cpp libs/GOLMatrix.o libs/MD5.o
	$(CXX) $^ $(CXXFLAGS) $(OPTFLAGS) $(MMIC) -o $@ $(LDFLAGS)

SequentialGOL.debug.out: src/SequentialGOL.cpp libs/GOLMatrix.o libs/MD5.o
	$(CXX) $^ $(CXXFLAGS) $(OPTFLAGS) -o $@ $(LDFLAGS)
	
SequentialGOL.perf.out: src/SequentialGOL.cpp libs/GOLMatrix.o libs/MD5.o
	$(CXX) $^ $(CXXFLAGS) $(OPTFLAGS) $(OPTFLAGS_PERF) -o $@ $(LDFLAGS)

ThreadGOL.out: src/ThreadGOL.cpp libs/GOLMatrix.o libs/MD5.o libs/Barrier.o
	$(CXX) $^ $(CXXFLAGS) $(OPTFLAGS) -o $@ $(LDFLAGS)

ThreadGOL.mic.out: src/ThreadGOL.cpp libs/GOLMatrix.o libs/MD5.o libs/Barrier.o
	$(CXX) $^ $(CXXFLAGS) $(OPTFLAGS) $(MMIC) -o $@ $(LDFLAGS)

ThreadGOL.debug.out: src/ThreadGOL.cpp libs/GOLMatrix.o libs/MD5.o libs/Barrier.o
	$(CXX) $^ $(CXXFLAGS) $(OPTFLAGS) -o $@ $(LDFLAGS)
	
ThreadGOL.perf.out: src/ThreadGOL.cpp libs/GOLMatrix.o libs/MD5.o libs/Barrier.o
	$(CXX) $^ $(CXXFLAGS)  $(OPTFLAGS) $(OPTFLAGS_PERF) -o $@ $(LDFLAGS)

FastFlowGOL.out: src/FastFlowGOL.cpp libs/GOLMatrix.o libs/MD5.o
	$(CXX) $^ $(CXXFLAGS) $(INCLUDES) $(OPTFLAGS) -o $@ $(LDFLAGS)

FastFlowGOL.mic.out: src/FastFlowGOL.cpp libs/GOLMatrix.o libs/MD5.o
	$(CXX) $^ $(CXXFLAGS) $(INCLUDES) $(OPTFLAGS) $(MMIC) -o $@ $(LDFLAGS)

FastFlowGOL.debug.out: src/FastFlowGOL.cpp libs/GOLMatrix.o libs/MD5.o
	$(CXX) $^ $(CXXFLAGS) $(INCLUDES) $(OPTFLAGS) -o $@ $(LDFLAGS)

FastFlowGOL.perf.out: src/FastFlowGOL.cpp libs/GOLMatrix.o libs/MD5.o
	$(CXX) $^ $(CXXFLAGS) $(INCLUDES) $(OPTFLAGS) $(OPTFLAGS_PERF) -o $@ $(LDFLAGS)

libs/%.o:src/%.cpp src/%.hpp
	mkdir -p libs
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(OPTFLAGS) $(MMIC) -c -o $@ $< $(LDFLAGS)

clean		: 
	-rm -f libs/*
	-rm -f *.out



