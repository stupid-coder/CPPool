#ifndef _OBJECT_DEQUE_H
#define _OBJECT_DEQUE_H

#include <interface/PooledObject.hpp>
#include <misc/WrapperLock.hpp>
#include <map>
#include <deque>

namespace CPPool
{
  template <typename T>
  class ObjectDeque
  {
  public:
    ObjectDeque()
      : numInterested_(0)
    {}

    ~ObjectDeque()
    {}

    std::map< long, PooledObject<T>* > *getAllObjects()
    {
      return allObjects_;
    }

    bool hasIdleObject() const
    {
      return idleObjects_.empty() == false;
    }

    bool nonBorrowedObject() const
    {
      return idleObjects_.size() == allObjects_.size();
    }

    bool empty() const
    {
      return allObjects_.empty();
    }

    int increamentAndGetNumInterested()
    {
      return __sync_add_and_fetch(&numInterested_,1);
    }

    int decreamentAndGetNumInterested()
    {
      return __sync_sub_and_fetch(&numInterested_,1);
    }

    int getNumInterested() const
    {
      return numInterested_;
    }

    PooledObject<T> *getPooledObjectFromIdleObjects()
    {
      GuardRWLockWR lock(lock_);
      PooledObject<T> *object = NULL;
      if ( hasIdleObject() )
        {
          object = idleObjects_.front();
          idleObjects_.pop_front();
        }
      return object;
    }

    void putPooledObjectToIdleObjects(PooledObject<T> *object)
    {
      GuardRWLockWR lock(lock_);
      idleObjects_.push_back(object);
    }

    PooledObject<T> *getPooledObjectFromAllObjects(long id)
    {
      GuardRWLockRD lock(lock_);
      typename std::map< long, PooledObject<T>* >::iterator find_iter = allObjects_.find(id);
      if ( find_iter != allObjects_.end() ) return find_iter->second;
      else return NULL;
    }

    void putPooledObjectToAllObjects(long id, PooledObject<T> *object)
    {
      GuardRWLockWR lock(lock_);
      allObjects_.insert(std::make_pair< long,PooledObject<T>* >(id,object));
    }

    void removePooledObjectFromAllObjects(long id)
    {
      GuardRWLockWR lock(lock_);
      typename std::map< long, PooledObject<T>* >::iterator find_iter = allObjects_.find(id);
      if ( find_iter != allObjects_.end() ) allObjects_.erase(find_iter);
    }

    PooledObject<T> *removePooledObjectFromAllObjectsNonlock()
    {
      if ( !allObjects_.empty() )
        {
          PooledObject<T>* object = allObjects_.begin()->second;
          allObjects_.erase(allObjects_.begin());
          return object;
        }
      else
        return NULL;
    }

  private:
    RWLock lock_;
    std::deque<PooledObject<T>*> idleObjects_;
    std::map< long, PooledObject<T>* > allObjects_;
    int numInterested_;
  };

}

#endif
