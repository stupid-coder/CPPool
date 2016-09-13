#ifndef _CPPOOL_KEYED_POOLED_OBJECT_FACTORY_H
#define _CPPOOL_KEYED_POOLED_OBJECT_FACTORY_H

#include <interface/PooledObject.hpp>

namespace CPPool
{
  template <typename K, typename V>
  class KeyedPooledObjectFactory
  {
  public:
    virtual ~KeyedPooledObjectFactory() {}

    virtual PooledObject<V> makeObject(const K *key) throw(BaseException) = 0;

    virtual void destroyObject(const K *key, PooledObject<V> *object) throw(BaseException) = 0;

    virtual boolean validateObject(const K *key, PooledObject<V> *object) = 0;

    virtual void activateObject(const K *key, PooledObject<V> *object) throw(BaseException) = 0;

    virtual void passiveObject(const K *key, PooledObject<V> *object) throw(BaseException) = 0;
  };
}

#endif
