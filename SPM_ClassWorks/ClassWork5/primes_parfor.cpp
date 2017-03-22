#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ff/parallel_for.hpp>

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

int main(int argc, char *argv[]) {
    if (argc<4) {
        std::cerr << "use: " << argv[0]  << " number1 number2 [print=off|on]\n";
        return -1;
    }
    
    ull n1          = std::stoll(argv[1]);
    ull n2          = std::stoll(argv[2]);
    ull naux = n1;
    int n = 0;
    const size_t nw = std::stol(argv[3]);
    bool print_primes = false;
    std::vector<ull> results;
    //ull results[(size_t)(n2-n1)/log(n1)];
    results.reserve((size_t)(n2-n1)/log(n1));
    if (argc >= 5)  print_primes = (std::string(argv[4]) == "on");
    // Implementation of prime-check with parallel-for
    ParallelFor pf(nw, false);
    pf.parallel_for_static((ull)0,n2-n1+1, 1,[&results,&naux](const ull i) { if(is_prime(naux)) results.push_back(naux); naux++; });
    n = results.size();
    std::cout << "Found " << n << " primes\n";
    if (print_primes) {
        for(size_t i=0;i<n;++i)
            std::cout << results[i] << " ";
        std::cout << "\n\n";
    }
    return 0;
}
