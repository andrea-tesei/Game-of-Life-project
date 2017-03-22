#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ff/farm.hpp>
using namespace ff;
using ull = unsigned long long;

// see http://en.wikipedia.org/wiki/Primality_test
static bool is_prime(ull n) {
    if (n <= 3)  return n > 1; // 1 is not prime !
    
    if (n % 2 == 0 || n % 3 == 0) return false;
    
    for (ull i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) return false;
    }
    return true;
}

struct Emitter: ff_node_t<ull> {
    ff_loadbalancer *const lb;
    ull n1, n2;
    std::vector<ull> results;
    Emitter(ff_loadbalancer *const lb, ull n1, ull n2):lb(lb),n1(n1),n2(n2) {
        results.reserve((size_t)(n2-n1)/log(n1));
    }
    ull *svc(ull *number) {
        if(number == nullptr){
            ull n = n1;
            while(n <= n2){
                ff_send_out(new ull(n));
		n++;
            }
            lb->broadcast_task(EOS);
            return GO_ON;
	}
        results.push_back(*number);
        delete number;
	return GO_ON;
    }
};

struct Worker: ff_node_t<ull> {
    ull *svc(ull *input) {
        if(is_prime(*input)) return input;
        return GO_ON;
    }
};


int main(int argc, char *argv[]) {    
    if (argc<4) {
        std::cerr << "use: " << argv[0]  << " number1 number2 [print=off|on]\n";
        return -1;
    }
    
    // ff_Farm<> farm([&]{}()) the & means that you want environment by reference
    ull n1          = std::stoll(argv[1]);
    ull n2          = std::stoll(argv[2]);
    const size_t nw = std::stol(argv[3]);
    bool print_primes = false;
    if (argc >= 5)  print_primes = (std::string(argv[4]) == "on");
    
    std::vector<std::unique_ptr<ff_node>> workers;
    ff_Farm<> farmz( [&]() {
        std::vector<std::unique_ptr<ff_node> > W;
        for(size_t i=0;i<nw;++i)
            W.push_back(make_unique<Worker>());
        return W;
    } () );
    Emitter E(farmz.getlb(),n1,n2);
    farmz.add_emitter(E);      // replacing the default emitter
    farmz.remove_collector();  // removing the default collector
    farmz.wrap_around();       // adds feedback channel between worker and emitter
    int result = farmz.run_and_wait_end();
    
    const std::vector<ull> &results = E.results; 
    const size_t n = results.size();
    std::cout << "Found " << n << " primes\n";
    if (print_primes) {
        for(size_t i=0;i<n;++i) 
            std::cout << results[i] << " ";
        std::cout << "\n\n";
    }
    return 0;
}
