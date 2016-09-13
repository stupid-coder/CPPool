#ifndef _CPPOOL_DEFAULT_POOLED_OBJECT_H
#define _CPPOOL_DEFAULT_POOLED_OBJECT_H

#include <interface/PooledObject.hpp>


namespace CPPool
{
  template <typename T>
  class DefaultPooledObject
    : public virtual PooledObject<T>
  {
  public:
    DefaultPooledObject(T *object)
      : object_(object), state_(IDLE)
    {}

    virtual ~DefaultPooledObject()
    {
      delete object_;
    }

    virtual T *getObject()
    {
      return object_;
    }

    virtual enum PooledObjectState getState()
    {
      return state_;
    }

    virtual void markIdle()
    {
      state_ = IDLE;
    }

    virtual void markAllocated()
    {
      state_ = ALLOCATED;
    }
  private:
    T * object_;
    enum PooledObjectState state_;
  };

}

#endif
