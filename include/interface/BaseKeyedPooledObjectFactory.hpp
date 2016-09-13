#ifndef _CPPOOL_BASE_KEYED_POOLED_OBJECT_FACTORY_H
#define _CPPOOL_BASE_KEYED_POOLED_OBJECT_FACTORY_H

#include <interface/KeyedPooledObjectFactory.hpp>

namespace CPPool
{
  template <typename K, typename V>
  class BaseKeyedPooledObjectFactory
    : virtual public KeyedPooledObjectFactory<K,V>
  {
  public:

    virtual ~BaseKeyedPooledObjectFactory() {}

    virtual V *create(const K *key) throw(BaseException) = 0;

    virtual PooledObject<V> *wrap(V *object) throw(BaseException) = 0;

    virtual PooledObject<V> *makeObject(const K *key) throw(BaseException)
    {
      return wrap(create(key));
    }

    virtual void destroyObject(const K *key, PooledObject<V> *object)
    {
      throw BaseException("BaseException: BaseKeyedPooledObjectFactory::destroyObject");
    }

    virtual bool validateObject(const K *key, PooledObject<V> *object)
    {
      return true;
    }

    virtual void activateObject(const K *key, PooledObject<V> *object) throw(BaseException)
    {
    }

    virtual void passivateObject(const K *key, PooledObject<V> *object) throw(BaseException)
    {
    }

}

#endif
