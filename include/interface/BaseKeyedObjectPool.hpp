#ifndef _CPPOOL_BASE_KEYED_OBJECT_POOL_H
#define _CPPOOL_BASE_KEYED_OBJECT_POOL_H

#include <interface/KeyedObjectPool.hpp>

namespace CPPool
{

  template <typename K, typename V>
  class BaseKeyedObjectPool
    : virtual public KeyedObjectPool<K,V>
  {
  public:

    BaseKeyedObjectPool()
      :closed_(false) {}

    virtual ~BaseKeyedObjectPool() {}

    virtual V *borrowObject(const K *key) throw(IllegalStateException,BaseException) = 0;

    virtual void returnObject(const K *key, V *object) throw(IllegalStateException,BaseException) = 0;

    virtual void invalidateObject(const K *key, V *object) throw(BaseException) = 0;

    virtual void addObject(const K *key) throw(IllegalStateException,UnsupportedOperationException)
    {
      throw UnsupportedOperationException("UnsupportedOperationException: BaseKeyedObjectPool::addObject");
    }

    virtual int getNumIdle(const K *key)
    {
      return -1;
    }

    virtual int getNumActive(const K *key)
    {
      return -1;
    }

    virtual int getNumIdle()
    {
      return -1;
    }

    virtual int getNumActive()
    {
      return -1;
    }

    virtual void clear() throw(IllegalStateStateException,UnsupportedOperationException)
    {
      throw UnsupportedOperationException("UnsupportedOperationException: BaseKeyedObjectPool::clear");
    }

    virtual void clear(const K *key) throw(UnsupportedOperationException,BaseException)
    {
      throw UnsupportedOperationException("UnsupportedOperationException: BaseKeyedObjectPool::clear");
    }

    virtual void close()
    {
      closed_ = true;
    }

    bool isClosed() const
    {
      return closed_;
    }

    void assertOpen() throw(IllegalStateStateException)
    {
      if (isClosed()) throw IllegalStateStateException("IllegalStateStateException: BaseKeyedObjectPool::assertOpen - failure");
    }
  private:
    volatile bool closed_;
  };

}

#endif
