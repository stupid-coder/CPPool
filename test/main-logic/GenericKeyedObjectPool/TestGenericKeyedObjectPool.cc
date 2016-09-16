#include <iostream>
#include <impl/GenericKeyedObjectPool.hpp>
#include <sys/time.h>
#include <cstdlib>
#include <cassert>
#define LOOP_COUNT 10000

int sum;

std::string backends[] =
{
  std::string("1"),
  std::string("2"),
  std::string("3"),
  std::string("4"),
  std::string("5")
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
    std::cout << "KEY:" << key->c_str() << "\tvalue:" << *object->getObject() << std::endl;
    __sync_add_and_fetch(&sum,*object->getObject());
    delete object;
  }
};

void *test_routine(void *arg)
{

  int size_backends = sizeof(backends)/sizeof(std::string);

  CPPool::KeyedObjectPool<std::string,int> *pool = (CPPool::KeyedObjectPool<std::string,int>*)arg;

  timeval seed;
  gettimeofday(&seed,NULL);

  srandom(seed.tv_usec);

  for ( int i = 0; i < LOOP_COUNT; ++i )
    {
      std::string *backend = &backends[random()%size_backends];
      int *client = pool->borrowObject(backend);
      ++ *client;
      pool->returnObject(backend,client);
    }
  pthread_exit(NULL);
}

void *test_error_routine(void *arg)
{
  int size_backends = sizeof(backends)/sizeof(std::string);

  CPPool::KeyedObjectPool<std::string,int> *pool = (CPPool::KeyedObjectPool<std::string,int>*)arg;

  timeval seed;
  gettimeofday(&seed,NULL);
  srandom(seed.tv_usec);
  for ( int i = 0; i < LOOP_COUNT; ++i )
    {
      std::string *backend = &backends[random()%size_backends];
      int *client = pool->borrowObject(backend);
      ++ *client;
      pool->invalidateObject(backend,client);
    }
  pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
  CPPool::GenericKeyedObjectPool<std::string,int> pool(new TestGenericKeyedObjectPoolFactory<std::string,int>());

  int test_num = 20;

  if (argc>1) test_num = atoi(argv[1]);

  pthread_t pid[test_num+1];

  timeval begin,end;

  gettimeofday(&begin,NULL);

  for ( int i = 0; i < test_num; ++ i )
    pthread_create(&pid[i],NULL,test_routine,&pool);

	pthread_create(&pid[test_num],NULL,test_error_routine,&pool);

  for ( int i = 0; i <= test_num; ++ i )
    pthread_join(pid[i],NULL);

  gettimeofday(&end,NULL);

  std::cout << "cost time:" << end.tv_sec-begin.tv_sec + (end.tv_usec-begin.tv_usec)/1000000.0 << std::endl;

  pool.close();

std::cout << "sum:" << sum << "\tassert:" << LOOP_COUNT*(test_num+1) << std::endl;
  assert( sum == LOOP_COUNT*(test_num+1) );

  return 0;
}
