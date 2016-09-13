#ifndef _WRAPPER_LOCK_H
#define _WRAPPER_LOCK_H

#include <misc/Exceptions.hpp>
#include <pthread.h>

namespace CPPool
{
  class LockIf
  {
  public:
    virtual ~LockIf() {}
    virtual void lock() = 0;
    virtual bool trylock() = 0 ;
    virtual void rdlock() = 0 ;
    virtual bool tryrdlock() = 0;
    virtual void wrlock() = 0;
    virtual bool trywrlock() = 0;
    virtual void unlock() = 0;
  };

  class MutexLock : public LockIf
  {
  public:
    MutexLock()
    {
      pthread_mutex_init(&mutex_,NULL);
    }
    ~MutexLock()
    {
      pthread_mutex_destroy(&mutex_);
    }
    virtual void lock()
    {
      pthread_mutex_lock(&mutex_);
    }
    virtual bool trylock()
    {
      return pthread_mutex_trylock(&mutex_) == 0;
    }
    virtual void rdlock()
    {
      lock();
    }
    virtual bool tryrdlock()
    {
      return trylock();
    }
    virtual void wrlock()
    {
      lock();
    }
    virtual bool trywrlock()
    {
      return trylock();
    }
    virtual void unlock()
    {
      pthread_mutex_unlock(&mutex_);
    }
  private:
    pthread_mutex_t mutex_;
  };

  class RWLock : public LockIf
  {
  public:
    RWLock()
    {
      pthread_rwlock_init(&rwlock_,NULL);
    }
    ~RWLock()
    {
      pthread_rwlock_destroy(&rwlock_);
    }
    virtual void lock()
    {
      rdlock();
    }
    virtual bool trylock()
    {
      return tryrdlock();
    }
    virtual void rdlock()
    {
      pthread_rwlock_rdlock(&rwlock_);
    }
    virtual bool tryrdlock()
    {
      return pthread_rwlock_tryrdlock(&rwlock_) == 0;
    }
    virtual void wrlock()
    {
      pthread_rwlock_wrlock(&rwlock_);
    }
    virtual bool trywrlock()
    {
      return pthread_rwlock_trywrlock(&rwlock_) == 0;
    }
    virtual void unlock()
    {
      pthread_rwlock_unlock(&rwlock_);
    }
  private:
    pthread_rwlock_t rwlock_;
  };

  class GuardMutexLock
  {
  public:
    GuardMutexLock(LockIf &lock):lock_(lock)
    {
      lock_.lock();
    }

    ~GuardMutexLock()
    {
      lock_.unlock();
    }
  private:
    LockIf &lock_;
  };

  class GuardRWLockRD
  {
  public:
    GuardRWLockRD(LockIf &lock):lock_(lock)
    {
      lock_.rdlock();
    }
    ~GuardRWLockRD()
    {
      lock_.unlock();
    }
  private:
    LockIf &lock_;
  };

  class GuardRWLockWR
  {
  public:
    GuardRWLockWR(LockIf &lock):lock_(lock)
    {
      lock_.wrlock();
    }
    ~GuardRWLockWR()
    {
      lock_.unlock();
    }
  private:
    LockIf &lock_;
  };

}
#endif  // _WARPPER_LOCK_H
