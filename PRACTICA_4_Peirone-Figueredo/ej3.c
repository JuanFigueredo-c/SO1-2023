#include <omp.h>
#include <math.h>
#include <stdio.h>

unsigned long primos[] = {
  5000549, 70640473, 1000000007
};

int is_prime(unsigned long n) {
  int prime = 1;
  unsigned long top = ((unsigned long) sqrt(n)) + 1;
  #pragma omp parallel shared(prime)
  {
    int thread_nums = omp_get_num_threads();
    int div = (top - 1) / thread_nums, mod = (top - 1) % thread_nums; // mod = 
    int id = omp_get_thread_num();
    int start = 2 + id * div;
    //printf("thread_nums: %d\n", thread_nums);
    // If this is the last thread, it has to do some extra iterations
    int end = start + div + (id + 1 == thread_nums ? mod : 0);
    for (int i = start; i < end; i += 1) {
      if (!prime)
        break;
      if (n % i == 0) {
        #pragma omp atomic write
        prime = 0;
      }
    }
  }
  return prime;
}

int main() {
  long unsigned n = primos[2];
  printf("n: %lu, is_prime(n): %d\n", n, is_prime(n));
  return 0;
}
