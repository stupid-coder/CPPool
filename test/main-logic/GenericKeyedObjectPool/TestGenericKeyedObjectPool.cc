#include <iostream>
#include <impl/GenericKeyedObjectPool.hpp>
#include <sys/time.h>
#include <cstdlib>

#define LOOP_COUNT 10000

std::string backends[] =
{
  "1","2","3","4","5"
};

template <typename K,typename V>
class TestGenericKeyedObjectPoolFactory
  : virtual public CPPool::BaseKeyedPooledObjectFactory<K,V>
{
public:
  TestGenericKeyedObjectPoolFactory() {}
  ~TestGenericKeyedObjectPoolFactory() {}

  virtual V *create(const K *key)  throw(CPPool::BaseException)
  {
    return new V();
  }

  virtual CPPool::PooledObject<V> *wrap(V *object) throw(CPPool::BaseException)
  {
    return new CPPool::DefaultPooledObject<V>(object);
  }

  virtual void destroyObject(const K *key, CPPool::PooledObject<V> *object) throw(CPPool::BaseException)
  {
    delete object;
  }
};

void *test_routine(void *arg)
{

  CPPool::KeyedObjectPool<std::string,int> *pool = (CPPool::KeyedObjectPool<std::string,int>*)arg;

  timeval seed;
  gettimeofday(&seed,NULL);

  srandom(seed.tv_usec);

  for ( int i = 0; i < LOOP_COUNT; ++i )
    {
      std::string *backend = &backends[random()/sizeof(backends)];
      int *client = pool->borrowObject(backend);
      ++ *client;
      pool->returnObject(backend,client);
    }
  pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
  CPPool::GenericKeyedObjectPool<std::string,int> pool(new TestGenericKeyedObjectPoolFactory<std::string,int>());

  int test_num = 20;

  if (argc>1) test_num = atoi(argv[1]);

  pthread_t pid[test_num];

  timeval begin,end;

  gettimeofday(&begin,NULL);

  for ( int i = 0; i < test_num; ++ i )
    pthread_create(&pid[i],NULL,test_routine,&pool);

  for ( int i = 0; i < test_num; ++ i )
    pthread_join(pid[i],NULL);

  gettimeofday(&end,NULL);

  std::cout << "cost time:" << end.tv_sec-begin.tv_sec + (end.tv_usec-begin.tv_usec)/1000000 << std::endl;

  pool.close();

  return 0;
}
