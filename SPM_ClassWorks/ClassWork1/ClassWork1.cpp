#include <iostream>
#include <ff/farm.hpp>
#include <ff/pipeline.hpp>
using namespace ff;
typedef int resultType; 

struct firstStage: ff_node_t<resultType> {
	resultType *svc(resultType*) {
        for(int i=0;i<999999;++i) {
        	ff_send_out(new resultType(i));
        }
        return EOS; // End-Of-Stream
        }
};

struct SquareTask: ff_node_t<resultType>{
	resultType *svc(resultType *task){
		resultType &part = *task; 
		part = part*part;
		return task;
	}

}; 

struct SumOfSquare: ff_node_t<resultType>{
	resultType sum;
	resultType *svc(resultType* task){	
		sum += *task;
		delete task;
		//std::cout << "Partial sum: " << sum << "\n";
		//std::cout << "Final result is: " << *sum << "\n";
		return GO_ON;
	}
	SumOfSquare(){
		sum = 0;
	}
};


int main(){
	firstStage n1;
	SquareTask n2;
	SumOfSquare n3;
	ff_Pipe<resultType> pipe(n1,n2,n3);
	int result = pipe.run_and_wait_end();
	if (result<0) error("running pipe");
	std::cout << "The final result is: " << n3.sum << "\n";
  	return 0;
} 
