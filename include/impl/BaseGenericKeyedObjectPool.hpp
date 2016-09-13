#ifndef _CPPOOL_BASE_GENERIC_KEYED_OBJECT_POOL_H
#define _CPPOOL_BASE_GENERIC_KEYED_OBJECT_POOL_H

#include <impl/GenericObjectPoolConfig.hpp>

namespace CPPool
{
  template <typename T>
  class BaseGenericKeyedObjectPool
  {
  public:
    BaseGenericKeyedObjectPool()
      : maxTotalPerKey_(GenericObjectPoolConfig::DEFAULT_MAX_TOTAL_PER_KEY),
        maxIdlePerKey_(GenericObjectPoolConfig::DEFAULT_MAX_IDLE_PER_KEY),
        minIdlePerKey_(GenericObjectPoolConfig::DEFAULT_MIN_IDLE_PER_KEY),
        testOnBorrow_(GenericObjectPoolConfig::DEFAULT_TEST_ON_BORROW),
        testOnCreate_(GenericObjectPoolConfig::DEFAULT_TEST_ON_CREATE)
    {}
    ~BaseGenericKeyedObjectPool() {}

    void setMaxTotalPerKey(int maxTotalPerKey) { maxTotalPerKey_ = maxTotalPerKey; }

    int getMaxTotalPerKey() const { return maxTotalPerKey_; }

    void setMaxIdlePerKey(int maxIdlePerKey) { maxIdlePerKey_ = maxIdlePerKey; }

    int getMaxIdlePerKey() const { return maxIdlePerKey_; }

    void setMinIdlePerKey(int minIdlePerKey)  { minIdlePerKey_ = minIdlePerKey; }

    int getMinIdlePerKey() const { return minIdlePerKey_; }

    void setTestOnBorrow(bool testOnBorrow) { testOnBorrow_ = testOnBorrow; }

    bool getTestOnBorrow() const { return testOnBorrow_; }

    void setTestOnCreate(bool testOnCreate) { testOnCreate_ = testOnCreate; }

    bool getTestOnCreate() const { return testOnCreate_; }

    void setConfig(GenericObjectPoolConfig *config)
    {
      maxTotalPerKey_ = config->getMaxTotalPerKey();
      maxIdlePerKey_ = config->getMaxIdlePerKey();
      minIdlePerKey_ = config->getMinIdlePerKey();
      testOnBorrow_ = config->getTestOnBorrow();
      testOnCreate_ = config->getTestOnCreate();
    }

  private:
    int maxTotalPerKey_;
    int maxIdlePerKey_;
    int minIdlePerKey_;
    bool testOnBorrow_;
    bool testOnCreate_;
  };

}

#endif
