#ifndef __OPENMP_LIKE_H__
#define __OPENMP_LIKE_H__

//http://www.thesaguaros.com/openmp-style-constructs-in-c11.html
//http://www.thesaguaros.com/openmp-style-constructs-in-c11-part-ii.html

#include <thread>
#include <functional>
#include <vector>

using task_t = std::function<void()>;

extern thread_local unsigned int thread_num;
extern thread_local unsigned int num_threads;

class thread_pool{
private:
  std::vector<std::thread> pool;

public:
  template<typename T>
  thread_pool( size_t n_thrds, T task ){
    for( size_t n = 0; n < n_thrds; ++n )
      pool.emplace_back([=](){ thread_num = n; num_threads = n_thrds; task(); });
  }

  void wait(){ for( auto& thrd : pool ) thrd.join(); }
  void no_wait(){ for( auto& thrd : pool ) thrd.detach(); }
};

#define parallel_do(N, ...) thread_pool (N, [__VA_ARGS__]()
#define end_wait ).wait();
#define end_no_wait ).no_wait();

#define split(cond) if(cond)
#define single(N) if(thread_num == N)
#define master if(thread_num == 0)

#endif//__OPENMP_LIKE_H__
