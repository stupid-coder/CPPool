#include <iostream>
#include "cp.hpp"
#include "TestPooledObjectFactory.hpp"
#include <pthread.h>

void *handler(void *arg)
{
  CPDefaultPool<object> *cp = (CPDefaultPool<object>*)arg;
  for ( int i = 0; i < 100000; ++ i )
    {
      object * o = cp->borrowObject();
      o->increament();
      cp->returnObject(o);
    }
  pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
  int size = 20;
  CPDefaultPool<object> cp(new TestPooledObjectFactory(),size);
  pthread_t thread[size];
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
