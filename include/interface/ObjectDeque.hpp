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
    ObjectDeque() {}

    ~ObjectDeque()
    {
      for ( std::map< long, PooledObject<T>* >::iterator iter = allObjects_.begin();
            iter != allObjects_.end();
            ++ iter )
        {
          if ( iter->second != NULL ) delete iter->second;
        }
    }

    std::deque<PooledObject<T>*> &getIdleObjects()
    {
      return idleObjects_;
    }

    std::map< long, PooledObject<T> > &getAllObjects()
    {
      return allObjects_;
    }

    bool hasIdleObject() const
    {
      return idleObjects_.empty() == false;
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
      GuardRWLockRD lock(lock_)
      std::map< long, PooledObject<T>* >::iterator find_iter = allObjects_.find(id);
      if ( find_iter != NULL ) return find_iter->second;
      else return NULL:
    }

    void putPooledObjectToAllObjects(long id, PooledObject<T> *object)
    {
      GuardRWLockWR lock(lock_);
      allObjects_.insert(std::make_pair< long,PooledObject<T>* >(id,object));
    }
  private:
    RWLock lock_;
    std::deque<PooledObject<T>*> idleObjects_;
    std::map< long, PooledObject<T>* > allObjects_;

  };

}

#endif
