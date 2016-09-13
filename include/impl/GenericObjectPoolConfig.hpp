#ifndef _CPPOOL_GENERIC_OBJECT_POOL_CONFIG_H
#define _CPPOOL_GENERIC_OBJECT_POOL_CONFIG_H

namespace CPPool
{
  class GenericObjectPoolConfig
  {
  public:
    static const int DEFAULT_MAX_TOTAL = 20;
    static const int DEFAULT_MAX_IDLE  = 20;
    static const int DEFAULT_MAX_IDLE_PER_KEY = 20;
    static const int DEFAULT_MIN_IDLE_PER_KEY = 20;
    static const int DEFAULT_MAX_TOTAL_PER_KEY = 20;
    static const bool DEFAULT_TEST_ON_BORROW = false;
    static const bool DEFAULT_TEST_ON_CREATE = false;

  public:
    GenericObjectPoolConfig()
      : maxTotal_(DEFAULT_MAX_TOTAL),
        maxIdle_(DEFAULT_MAX_IDLE),
        maxIdlePerKey_(DEFAULT_MAX_IDLE_PER_KEY),
        minIdlePerKey_(DEFAULT_MIN_IDLE_PER_KEY),
        maxTotalPerKey_(DEFAULT_MAX_TOTAL_PER_KEY),
        testOnBorrow_(DEFAULT_TEST_ON_BORROW),
        testOnCreate_(DEFAULT_TEST_ON_CREATE)
    {}

    ~GenericObjectPoolConfig() {}

    int getMaxTotal() const { return maxTotal_; }

    void setMaxTotal(int maxTotal) { maxTotal_ = maxTotal; }

    int getMaxIdle() const { return maxIdle_; }

    void setMaxIdle(int maxIdle) { maxIdle_ = maxIdle; }

    bool getTestOnBorrow() const { return testOnBorrow_; }

    void setTestOnBorrow(bool testOnBorrow) { testOnBorrow_ = testOnBorrow; }

    bool getTestOnCreate() const { return testOnCreate_; }

    void setTestOnCreate(bool testOnCreate) { testOnCreate_ = testOnCreate; }

    int getMaxIdlePerKey() const { return maxIdlePerKey_; }

    void setMaxIdlePerKey(int maxIdlePerKey) { maxIdlePerKey_ = maxIdlePerKey; }

    int getMinIdlePerKey() const { return minIdlePerKey_; }

    void setMinIdlePerKey(int minIdlePerKey) { minIdlePerKey_ = minIdlePerKey; }

    int getMaxTotalPerKey() const { return maxTotalPerKey_; }

    void setMaxTotalPerKey(int maxTotalPerKey) { maxTotalPerKey_ = maxTotalPerKey; }

  private:

    int maxTotal_;

    int maxIdle_;

    int maxIdlePerKey_;

    int minIdlePerKey_;

    int maxTotalPerKey_;

    bool testOnBorrow_;

    bool testOnCreate_;
  };

}

#endif
