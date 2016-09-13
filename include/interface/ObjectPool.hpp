#ifndef _CPPOOL_OBJECT_POOL_H
#define _CPPOOL_OBJECT_POOL_H

#include <misc/Exceptions.hpp>

namespace CPPool
{
  template <typename T>
  class ObjectPool
  {
  public:

    /**
     * 虚析构函数
     */
    virtual ~ObjectPool() {}

    /**
     * 从池中获取一个实例
     * <p>
     * 获取的实例，可以为新生成的，也可以是用空闲队列中拿到的！
     * <p>
     * @return 实例
     * @throws IllegalStateException
     */
    virtual T *borrowObject() throw(IllegalStateException,BaseException) = 0;

    /**
     * 返回一个实例到池中
     *
     * @param object 实例指针
     *
     * @throws IllegalStateException
     *         表示将要返回的实例状态不正确
     */
    virtual void returnObject(T *object) throw(IllegalStateException,BaseException) = 0;

    /**
     * 销毁一个实例，该实例必须已经使用 (@link #borrowObject) 获取后，并且该实例已经出现错误
     *
     * @param object 销毁实例指针
     */
    virtual void invalidateObject(T *object) throw(BaseException) = 0;

    /**
     * 预添加实例
     * 通过调用 (@link CPPooledObjectFactory#makeObject) 预添加实例到池中
     */
    virtual void addObject() throw(IllegalStateException,UnsupportedOperationException) = 0;

    /**
     * 释放空闲池中的实例
     */
    virtual void clear() throw(IllegalStateException,UnsupportedOperationException) = 0;

    /**
     * 关闭池
     */
    virtual void close() = 0;

    /**
     * 返回当前池中已经借出多少个实例
     */
    virtual int getNumActive() = 0;

    /**
     * 返回当前池中闲置实例的数量，可以是个近似的值
     * 返回负数，表示当前数量无法获得
     */
    virtual int getNumIdle() = 0;
  };
}
#endif  // _CPPOOL_OBJECT_POOL_H
