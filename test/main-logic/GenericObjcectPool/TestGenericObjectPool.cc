#include <iostream>
#include <impl/GenericObjectPool.hpp>
#include <sys/time.h>

template <typename T>
class TestPooledObjectFactory :
  public virtual CPPool::BasePooledObjectFactory<T>
{
public:
  TestPooledObjectFactory() {}
  virtual ~TestPooledObjectFactory() {}
  virtual T *create() throw(CPPool::BaseException)
  {
    return new T();
  }

  virtual CPPool::PooledObject<T> *wrap(T *object) throw(CPPool::BaseException)
  {
    return new CPPool::DefaultPooledObject<T>(object);
  }

  virtual void destroyObject(CPPool::PooledObject<T> *object) throw(CPPool::BaseException)
  {
    delete object;
  }
};


void *test_routine(void *arg)
{
  CPPool::ObjectPool<int> *pool = (CPPool::ObjectPool<int>*)arg;
  int * client = 0;
  for ( int i = 0 ; i < 10000; ++ i )
    {
      try {
        client = pool->borrowObject();
      } catch( CPPool::BaseException e) {
        std::cout << "catch BaseException: " << e.what() << std::endl;
      }
      ++ *client;
      pool->returnObject(client);
    }
  pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
  CPPool::GenericObjectPool<int> pool(new TestPooledObjectFactory<int>());

  int num = 20;
  if (  argc > 1 )
    num = atoi(argv[1]);


  pthread_t pid[num];

  timeval begin,end;

  gettimeofday(&begin,NULL);

  for ( int i = 0; i < num ; ++ i )
    pthread_create(&pid[i],NULL,test_routine,&pool);

  for ( int i = 0; i < num ; ++ i )
    pthread_join(pid[i],NULL);

  gettimeofday(&end,NULL);

  std::cout << "cost time:" << end.tv_sec-begin.tv_sec+(end.tv_usec-begin.tv_usec)/1000000.0 << std::endl;

  return 0;
}
