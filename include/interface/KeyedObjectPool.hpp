#ifndef _CPPOOL_KEYED_OBJECT_POOL_H
#define _CPPOOL_KEYED_OBJECT_POOL_H

#include <misc/Exceptions.hpp>

namespace CPPool
{
  template <typename K, typename V>
  class KeyedObjectPool
  {
  public:

    virtual ~KeyedObjectPool() {}

    virtual V *borrowObject(const K *key) throw(IllegalStateException, BaseException) = 0;

    virtual void returnObject(const K *key, V *object) throw(IllegalStateException, BaseException) = 0;

    virtual void invalidateObject(const K *key, V *object) throw(BaseException) = 0;

    virtual void addObject(const K *key) throw(IllegalStateException,UnsupportedOperationException) = 0;

    virtual int getNumIdle(const K *key) = 0;

    virtual int getNumActive(const K *key) = 0;

    virtual int getNumIdle() = 0;

    virtual int getNumActive() = 0;

    virtual void clear() throw(UnsupportedOperationException,BaseException) = 0;

    virtual void clear(const K *key) throw(UnsupportedOperationException,BaseException) = 0;

    virtual void close() = 0;
  };
}

#endif
