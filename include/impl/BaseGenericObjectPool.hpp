#ifndef _CPPOOL_BASE_GENERIC_OBJECT_POOL_H
#define _CPPOOL_BASE_GENERIC_OBJECT_POOL_H

#include <impl/GenericObjectPoolConfig.hpp>
#include <interface/PooledObject.hpp>

namespace CPPool
{

  // 实现 GenericObjectPool 配置相关逻辑
  template <typename T>
  class BaseGenericObjectPool
  {
  public:
    BaseGenericObjectPool()
      : maxTotal_(GenericObjectPoolConfig::DEFAULT_MAX_TOTAL)
      , maxIdle_(GenericObjectPoolConfig::DEFAULT_MAX_IDLE)
      , testOnBorrow_(GenericObjectPoolConfig::DEFAULT_TEST_ON_BORROW)
      , testOnCreate_(GenericObjectPoolConfig::DEFAULT_TEST_ON_CREATE)
    {}

    ~BaseGenericObjectPool() {}

    void setMaxtotal(int maxTotal) { maxTotal_ = maxTotal; }

    int getMaxTotal() const { return maxTotal_; }

    void setMaxIdle(int maxIdle) { maxIdle_ = maxIdle; }

    int getMaxIdle() const { return maxIdle_; }

    void setTestOnBorrow(bool testOnBorrow) { testOnBorrow_ = testOnBorrow; }

    bool getTestOnBorrow() const { return testOnBorrow_; }

    void setTestOnCreate(bool testOnCreate) { testOnCreate_ = testOnCreate; }

    bool getTestOnCreate() const { return testOnCreate_; }

    void setConfig(GenericObjectPoolConfig *config)
    {
      maxTotal_ = config->getMaxTotal();
      maxIdle_ = config->getMaxIdle();
      testOnBorrow_ = config->getTestOnBorrow();
      testOnCreate_ = config->getTestOnCreate();
    }

    static long identityWrapper(T *object)
    {
      return (long)object;
    }
  private:
    int maxTotal_;
    int maxIdle_;
    bool testOnBorrow_;
    bool testOnCreate_;
  };

}

#endif
