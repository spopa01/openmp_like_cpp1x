#include "openmplike.hpp"

#include <iostream>
#include <condition_variable>
#include <atomic>
#include <algorithm>
#include <array>
#include <queue>

template<typename T>
class naive_concurrent_queue{
public:
  naive_concurrent_queue() {}

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

  bool pop(T& t){
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
  naive_concurrent_queue<int> queue;

  const size_t NT = 4;          //number of threads
  const size_t NPT = 100000;    //number of samples per thread

  const size_t N = NT*NPT/2;    //total number of samples

  std::array<int, N> values_computed;
  std::array<int, N> values_expected;

  for( auto i=0; i<N; ++i )
    values_expected[ i ] = i;

  std::atomic<int> count{0};

  std::array<int, NT> stats;

  //OPENMP LIKE CONSTRUCT
  parallel_do(NT, &queue, &values_computed, &count, &stats ){

    //SPLIT THE THREADS GANG...
    //first half of threads is producing and the other half is consuming...
    split( thread_num < NT/2 ){
      for( int i=0; i<NPT; ++i ){
        stats[thread_num]++; //produced
        queue.push( thread_num * NPT + i );
      }
    }else{
      int value;
      while( count < N ){
        if( queue.pop( value ) ){
          stats[thread_num]++; //consumed
          values_computed[ count++ ] = value;
        }
      }
    }

  }end_wait;

  std::sort( values_computed.begin(), values_computed.end() );

  std::cout << "TEST ... " << (values_computed == values_expected ? "PASSED" : "FAILED") << std::endl;

  std::cout << "STATS:" << std::endl;
  for( auto & stat : stats )
    std::cout << "\t" << stat << std::endl;

  return 0;
}
