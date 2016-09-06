#ifndef _WRAPPER_LOCK_H
#define _WRAPPER_LOCK_H

#include <pthread.h>

class WrapperLock
{
public:
  WrapperLock()
  {
    pthread_mutex_init(&mutex_,NULL);
  }
  ~WrapperLock()
  {
    pthread_mutex_destroy(&mutex_);
  }

  void Lock()
  {
    pthread_mutex_lock(&mutex_);
  }

  void Unlock()
  {
    pthread_mutex_unlock(&mutex_);
  }
private:
  pthread_mutex_t mutex_;
};

template<typename T>
class GuardLock
{
public:
  GuardLock(T &wrapper_lock):wrapper_lock_(wrapper_lock)
  {
    wrapper_lock_.Lock();
  }

  ~GuardLock()
  {
    wrapper_lock_.Unlock();
  }
private:
  T &wrapper_lock_;
};

#endif  // _WARPPER_LOCK_H
