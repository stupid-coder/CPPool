#ifndef _CPPOOL_BASE_POOLED_OBJECT_FACTORY_H
#define _CPPOOL_BASE_POOLED_OBJECT_FACTORY_H

#include <interface/PooledObjectFactory.hpp>

namespace CPPool
{
  template <typename T>
  class BasePooledObjectFactory
    : virtual public PooledObjectFactory<T>
  {
  public:
    virtual ~BasePooledObjectFactory() {}

    virtual T *create() throw(BaseException) = 0;

    virtual PooledObject<T> *wrap(T *obj) throw(BaseException) = 0;

    virtual PooledObject<T> *makeObject() throw(BaseException)
    {
      return wrap(create());
    }

    virtual void destroyObject(PooledObject<T> *object) throw(BaseException)
    {
      throw BaseException("BaseException: BasePooledObjectFactory::destroyObject");
    }

    virtual bool validateObject(PooledObject<T> *object)
    {
      return true;
    }

    virtual void activateObject(PooledObject<T> *object) throw(BaseException)
    {
    }

    virtual void passivateObject(PooledObject<T> *object) throw(BaseException)
    {
    }
  };

}

#endif
