#ifndef _CP_DEFAULT_POOLED_OBJECT_H
#define _CP_DEFAULT_POOLED_OBJECT_H

#include "interface/CPPooledObject.hpp"

template <typename T>
class CPDefaultPooledObject : public CPPooledObject<T>
{
public:

  CPDefaultPooledObject(T *object)
    : object_(object), state_(IDLE)
  {}

  T *getObject()
  {
    return object_;
  }

    enum CPPooledObjectState getState()
  {
    return state_;
  }

  void markIdle()
  {
    state_ = IDLE;
  }

  void markAllocated()
  {
    state_ = ALLOCATED;
  }

private:
  T *object_;
  enum CPPooledObjectState state_;
};

#endif  // _CP_DEFAULT_POOLED_OBJECT_H
