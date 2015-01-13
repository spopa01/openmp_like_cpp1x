openmp_like_cpp1x
=================

OpenMP like constructs using only C++ 11


Example Usage (see main.cpp):

```C++

#include "openmplike.hpp"

//...

template<typename T>
class basic_concurrent_queue{
public:
  basic_concurrent_queue() {}

  void push(T const& t){...}
  bool try_pop(T& t){...} // non blocking pop

...
};

int main(/*...*/){
  basic_concurrent_queue<int> queue;

  const size_t number_of_threads = 4;
  const size_t number_of_samples_per_thread = 100000;

  // N/2 because we will create a prodcer/consumer scenario
  const size_t total_number_of_samples = 
    number_of_threads * number_of_samples_per_thread / 2;

  std::array<int, total_number_of_samples> computed_values;
  std::array<int, total_number_of_samples> expected_values;

  for( auto i=0; i<total_number_of_samples; ++i )
    expected_values[ i ] = i*2;
  
  //we will collect some stats about the threads...
  std::array<int, number_of_threads> stats;

  //the following scenario works simply because we produce and consume exactly N samples...
  std::atomic<int> counter{0};

  //(A NOT SO USEFUL) OPENMP LIKE CONSTRUCT
  parallel_do( number_of_threads, &queue, &computed_values, &counter, &stats ){

    //SPLIT THE THREADS GANG... IN PRODUCES AND CONSUMERS
    split( thread_num < number_of_threads/2 ){  //-> first half of threads is producing ...
      for( int i=0; i<number_of_samples_per_thread; ++i ){
        stats[thread_num]++; //produced
        queue.push( thread_num * number_of_samples_per_thread + i );
      }
    }else{                                      //-> and the other half is consuming...
      int value;
      while( counter < total_number_of_samples ){
        if( queue.try_pop( value ) ){
          stats[thread_num]++; //consumed
          computed_values[ counter++ ] = value*2;
        }
      }
    }

  }end_wait;

  //the computed_values must be the same as in expected_values but in a different order at the moment...
  std::sort( computed_values.begin(), computed_values.end() );

  std::cout << "TEST ... " << (computed_values == expected_values ? "PASSED" : "FAILED") << std::endl;

  std::cout << "STATS:" << std::endl;
  for( auto & stat : stats )
    std::cout << "\t" << stat << std::endl;

  return 0;
}

```

Output:
```
TEST ... PASSED
STATS:
  100000
  100000
  99999
  100001

```
(Note: the STATS may vary...)
