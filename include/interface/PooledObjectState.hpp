#ifndef _CPPOOL_POOLED_OBJECT_STATE_H
#define _CPPOOL_POOLED_OBJECT_STATE_H

namespace CPPool
{
  enum PooledObjectState
    {
      IDLE,                       // non used, in the pool
      ALLOCATED,                  // used
    };
}
#endif  // _CPPOOL_POOLED_OBJECT_STATE_H
