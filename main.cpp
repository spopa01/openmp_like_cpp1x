#include "openmplike.hpp"

#include <iostream>
#include <condition_variable>
#include <atomic>
#include <algorithm>
#include <array>
#include <queue>

template<typename T>
class basic_concurrent_queue{
public:
  basic_concurrent_queue() {}

  void push(T const& t){
    std::lock_guard<std::mutex> lg{m_};
    q_.push(t);
    cv_.notify_one();
  }

  T pop(){
    std::unique_lock<std::mutex> lg{m_};
    cv_.wait(lg, [&]{return !q_.empty();});
    auto t = q_.front(); q_.pop();
    return t;
  }

  bool try_pop(T& t){
    std::lock_guard<std::mutex> lg{m_};
    if(q_.empty()) return false;
    t = q_.front(); q_.pop();
    return true;
  }

private:
  std::mutex              m_;
  std::condition_variable cv_;
  std::queue<T>           q_;
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
