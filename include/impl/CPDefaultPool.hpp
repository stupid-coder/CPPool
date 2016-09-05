#ifndef _CP_DEFAULT_POOL_H
#define _CP_DEFAULT_POOL_H

#include "interface/CPPool.hpp"
#include "WrapperLock.hpp"

template <typename T>
class CPDefaultPool : public CPPool<T>
{
public:
  CPDefaultPool(CPPooledObjectFactory<T> *factory,
                int maxTotal)
    : factory_(factory), maxTotal_(maxTotal) {}

  ~CPDefaultPool()
  {
    close();
  }

  static long identifiyObjectId(T *object)
  {
    return static_cast<long>(object);
  }

  T *borrowObject()
  {
    CPPooledObject<T> *object = NULL;

    GuardLock<WrapperLock>(idle_lock_);
    do
      {
        if ( idleObjects_.empty() ) addObject();
        object = idleObjects_.front();
        idleObjects_.pop_front();
      }
    while ( object == NULL );

    assert(object->getObject()->getState()==IDLE);
    object->getObject()->markAllocated();

    return object->getObject();
  }

  void returnObject(T *object)
  {
    std::map<long,CPPooledObject<T>*>::iterator find_iter = allObjects_.find(identifiyObjectId(object));

    assert(("out of bound" && find_iter!= allObjects_.end()));

    CPPooledObject<T> *pooled_object = find_iter->second;

    pooled_object->markIdel();

    GuardLock<WrapperLock>(idle_lock_);

    idleObjects_.push_back(pooled_object);
  }

  void addObject()
  {
    CPPooledObject<T> *object = factory_->makeObject();
    assert(("factory makeObject is empty"&&object!=NULL));

    GuardLock<WrapperLock>(all_lock_);
    allObjects_.insert(std::make_pair<long,CPPooledObject<T>*>(identifiyObjectId(object),object));
  }


  void clear()
  {
    // TODO: clear the idle queue
  }

  void close()
  {
    for ( typename std::map< long, CPPooledObject<T>* >::iterator iter = allObjects_.begin();
          iter != allObjects_.end();
          ++ iter )
      {
        if ( iter->second != NULL )
          factory_->destoryObject(iter->second);
      }
    delete factory_;
  }

private:
  int maxTotal_;
  CPPooledObjectFactory<T> *factory_;

  WrapperLock all_lock_;
  WrapperLock idle_lock_;
  std::map< long, CPPooledObject<T>* > allObjects_;
  std::deque< CPPooledObject<T>* > idleObjects_;
};

#endif
