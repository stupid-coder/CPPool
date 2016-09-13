#ifndef _CPPOOL_POOLED_OBJECT_FACTORY_H
#define _CPPOOL_POOLED_OBJECT_FACTORY_H

#include <interface/PooledObject.hpp>

namespace CPPool
{

  /**
   * CPPooledObjectFactory:
   * initialized&manage&destroy the pooled object
   */
  template <typename T>
  class PooledObjectFactory
  {
  public:
    virtual ~PooledObjectFactory() {};

    /**
     * 创建一个实例，使用 (@link CPPooledObject) 封装
     */
    virtual PooledObject<T> *makeObject() throw(BaseException) = 0;

    /**
     * 释放一个实例，释放资源
     */
    virtual void destroyObject(PooledObject<T> *object) throw(BaseException) = 0;

    /**
     * 检测实例是否正常
     */
    virtual bool validateObject(PooledObject<T> *object) = 0;

    /**
     * 创建实例后，进行激活
     */
    virtual void activateObject(PooledObject<T> *object) throw(BaseException) = 0;

    /**
     * 释放实例前，钝化实例，即断开链接，或者释放一些额外的资源
     */
    virtual void passivateObject(PooledObject<T> *object) throw(BaseException) = 0;
  };

}
#endif  // _CPPOOL_POOLED_OBJECT_FACTORY_H
