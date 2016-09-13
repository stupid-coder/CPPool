#ifndef _CPPOOL_BASE_OBJECT_POOL_H
#define _CPPOOL_BASE_OBJECT_POOL_H

#include <interface/ObjectPool.hpp>

namespace CPPool
{
  template <typename T>
  class BaseObjectPool
    : virtual public ObjectPool<T>
  {
  public:

    BaseObjectPool()
      : closed_(false) {}

    virtual ~BaseObjectPool() {}

    virtual T *borrowObject() throw(IllegalStateException,BaseException) = 0;

    virtual void returnObject(T *object) throw(IllegalStateException,BaseException) = 0;

    virtual void invalidateObject(T *object) throw(BaseException) = 0;

    virtual void addObject() throw(IllegalStateException,UnsupportedOperationException)
    {
      throw UnsupportedOperationException("UnsupportedOperationException: BaseObjectPool::addObject");
    }

    virtual void clear() throw(IllegalStateException,UnsupportedOperationException)
    {
      throw UnsupportedOperationException("UnsupportedOperationException: BaseObjectPool::clear");
    }

    virtual void close()
    {
      closed_ = true;
    }

    bool isClosed() const
    {
      return closed_;
    }

    void assertOpen() const throw(IllegalStateException)
    {
      if ( isClosed() ) throw IllegalStateException("IllegalStateException:  BaseObjectPool::assertOpen - failure");
    }

    virtual int getNumActive()
    {
      return -1;
    }

    virtual int getNumIdle()
    {
      return -1;
    }

  protected:
    volatile bool closed_;
  };
}

#endif
