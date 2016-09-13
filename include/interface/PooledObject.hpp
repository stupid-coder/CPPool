#ifndef _CPPOOL_POOLED_OBJECT_H
#define _CPPOOL_POOLED_OBJECT_H

#include <interface/PooledObjectState.hpp>

namespace CPPool {

  /**
   * Defines the pooled object, track the additional informations.
   *
   * @param <T> the type of object in the pool
   */
  template <typename T>
  class PooledObject
  {
  public:
    /**
     * Obtain the underlying object that is wrapped.
     */
    virtual ~PooledObject() {};

    /**
     * 获取封装的对象指针
     */
    virtual T *getObject() = 0;

    /**
     * 返回当前实例的状态
     */
    virtual enum PooledObjectState getState() = 0;

    /**
     * 设置当前实例为空闲状态
     */
    virtual void markIdle() = 0;

    /**
     * 设置当前实例为使用状态
     */
    virtual void markAllocated() = 0;

  };

}

#endif  // _CPPOOL_POOLED_OBJECT_H
