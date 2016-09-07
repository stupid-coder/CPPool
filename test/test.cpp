#include <iostream>
#include "cp.hpp"
#include "TestPooledObjectFactory.hpp"
#include <pthread.h>
#include <sys/time.h>

void *handler(void *arg)
{
  std::cout << "handler start..." << std::endl;
  CPDefaultPool<object> *cp = (CPDefaultPool<object>*)arg;
  for ( int i = 0; i < 100000; ++ i )
    {
      object * o = cp->borrowObject();
      o->increament();
      cp->returnObject(o);
    }
  pthread_exit(NULL);
}

class Timer
{
public:
  Timer()
  {
    gettimeofday(&begin,NULL);
  }

  ~Timer()
  {
    timeval end;
    gettimeofday(&end,NULL);
    std::cout << "Timer: " << (end.tv_sec-begin.tv_sec + (end.tv_usec-begin.tv_usec)/1000000.0) << "s" << std::endl;
  }
private:
  timeval begin;
};

int main(int argc, char *argv[])
{
  setlinebuf(stdout);
  int size = 20;
  CPDefaultPool<object> cp(new TestPooledObjectFactory(),size);
  pthread_t thread[size];

  Timer timer;
  for ( int i = 0; i < size; ++i )
    {
      pthread_create(&thread[i],NULL,handler,&cp);
    }

  for ( int i = 0; i < size; ++i )
    {
      pthread_join(thread[i],NULL);
    }
  return 0;
}
