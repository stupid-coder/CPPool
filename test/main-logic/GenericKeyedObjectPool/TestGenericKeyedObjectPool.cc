#include <iostream>
#include <impl/GenericKeyedObjectPool.hpp>


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



int main(int argc, char *argv[])
{

  CPPool::GenericKeyedObjectPool<std::string,int> pool(new TestGenericKeyedObjectPoolFactory<std::string,int>());
  return 0;
}
