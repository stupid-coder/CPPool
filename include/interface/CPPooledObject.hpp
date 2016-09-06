#ifndef _CP_POOLED_OBJECT_H
#define _CP_POOLED_OBJECT_H

#include "interface/CPPooledObjectState.hpp"

/**
 * Defines the pooled object, track the additional informations.
 *
 * @param <T> the type of object in the pool
 */
template <typename T>
class CPPooledObject
{
public:
  /**
   * Obtain the underlying object that is wrapped.
   */
  virtual ~CPPooledObject() {};

  virtual T *getObject() = 0;

  virtual enum CPPooledObjectState getState() = 0;

  virtual void markIdle() = 0;

  virtual void markAllocated() = 0;

};

#endif  // _CP_POOLED_OBJECT_H
