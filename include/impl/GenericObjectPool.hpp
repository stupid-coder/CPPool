#ifndef _CPPOOL_GENERIC_OBJECT_POOL_H
#define _CPPOOL_GENERIC_OBJECT_POOL_H

#include <interface/BaseObjectPool.hpp>
#include <interface/BasePooledObjectFactory.hpp>
#include <impl/GenericObjectPoolConfig.hpp>
#include <impl/BaseGenericObjectPool.hpp>
#include <impl/DefaultPooledObject.hpp>

#include <misc/WrapperLock.hpp>

#include <map>
#include <deque>

namespace CPPool
{
  template <typename T>
  class GenericObjectPool
    : public BaseObjectPool<T>,
      public BaseGenericObjectPool<T>
  {
  public:
    GenericObjectPool(PooledObjectFactory<T> *factory)
      : createCount_(0)
    {
      this->initGenericObjectPool(factory,new GenericObjectPoolConfig());
    }

    GenericObjectPool(PooledObjectFactory<T> *factory,
                      GenericObjectPoolConfig *config)
      : createCount_(0)
    {
      this->initGenericObjectPool(factory,config);
    }

    ~GenericObjectPool()
    {
      this->close();
    }

    virtual T *borrowObject() throw(BaseException,IllegalStateException)
    {

      this->assertOpen();

      //TODO: clear abandoned objects

      PooledObject<T> *object = NULL;
      bool created = false;

      do {
        GuardMutexLock lock(lock_);
        if ( idleObjects_.empty() )
          {
            // create
            object = this->create();
            if ( object != NULL )
              created = true;
          }
        else
          {
            // 从空闲池中拿到一个实例
            object = this->getPooledObjectFromIdleObjects();
          }

        if ( object != NULL )
          {
            try {
              // 激活当前实例
              factory_->activateObject(object);
            } catch ( BaseException ae ) {
              try {
                // 异常，清楚当前创建的实例
                this->destroy(object);
              } catch ( BaseException de ) {
                // 忽略
              }

              object = NULL;

              if ( created ) throw BaseException("BaseException: GenericObjectPool::borrowObject - failure");

              if ( this->getTestOnBorrow() || (created && this->getTestOnCreate()) )
                {
                  bool validate = false;
                  try {
                    validate = factory_->validateObject(object);
                  } catch ( BaseException ve ) {}

                  if ( validate == false ) {
                    try {
                      this->destroy(object);
                    } catch ( BaseException de ) {
                      throw de;
                    }
                  }
                }
            }
          }
      } while ( object == NULL );

      object->markAllocated();

      return object->getObject();
    }

    virtual void returnObject(T *object) throw(IllegalStateException,BaseException)
    {
      this->assertOpen();

      PooledObject<T> *pooledObject = this->getPooledObjectFromAllObjects(object);

      GuardMutexLock lock(lock_);
      if ( pooledObject->getState() != ALLOCATED )
        {
          throw IllegalStateException("IllegalStateException: GenericObjectPool::returnObject - pooledObject state is not borrowed from this pool");
        }
      else
        {
          pooledObject->markIdle();
        }

      try {
        factory_->passivateObject(pooledObject);
      } catch (BaseException pe) {
        try {
          this->destroy(pooledObject);
        } catch (BaseException de) {
          throw de;
        }
      }

      // TODO: 大于最大的时候，不添加
      this->putPooledObjectToIdleObjects(pooledObject);
    }

    virtual void invalidateObject(T *object) throw(BaseException)
    {

      GuardMutexLock lock(lock_);

      PooledObject<T> *pooledObject = this->getPooledObjectFromAllObjects(object);

      this->destroy(pooledObject);

    }

    virtual void addObject() throw(IllegalStateException,UnsupportedOperationException)
    {
      this->assertOpen();

      if ( factory_ != NULL ) throw IllegalStateException("IllegalStateException: GenericObjectPool::addObject - factory is empty");

      PooledObject<T> *object = create();

      this->putPooledObjectToIdleObjects(object);
    }

    virtual void clear() throw(IllegalStateException,UnsupportedOperationException)
    {

      GuardMutexLock lock(lock_);
      while ( !idleObjects_.empty() )
        {
          PooledObject<T> *object = this->getPooledObjectFromIdleObjects();
          this->destroy(object);
        }

    }

    virtual void close()
    {
      if ( this->isClosed() ) return;

      this->closed_ = true;

      clear();
    }

    virtual int getNumActive()
    {
      return -1;
    }

    virtual int getNumIdle()
    {
      return -1;
    }

  protected:
    void initGenericObjectPool(PooledObjectFactory<T> *factory,
                               GenericObjectPoolConfig *config)
    {
      if ( factory == NULL || config == NULL )
        throw BaseException("BaseException: parameter is empty in  GenericObjectPool::initGenericObjectPool");
      factory_ = factory;
      this->setConfig(config);
    }

    PooledObject<T> *create() throw(BaseException)
    {
      int newCreateCount = __sync_add_and_fetch(&createCount_,1);

      // TODO: 是否超过了池中设置的大小，进行处理
      PooledObject<T> *object = NULL;

      try {
        object = factory_->makeObject();
      } catch ( BaseException e ) {
        __sync_sub_and_fetch(&createCount_,1);
        throw e;
      }

      if ( object != NULL )
        {
          this->putPooledObjectToAllObjects(object);
        }
      return object;
    }

    void destroy(PooledObject<T> *object) throw(BaseException)
    {

      __sync_sub_and_fetch(&createCount_,1);

      allObjects_.erase(this->identityWrapper(object->getObject()));
      try {
        factory_->destroyObject(object);
      } catch ( BaseException e ) {
        throw e;
      }

    }

    void putPooledObjectToAllObjects(PooledObject<T> *object)
    {
      allObjects_.insert(std::make_pair<long, PooledObject<T>*>(this->identityWrapper(object->getObject()),object));
    }

    PooledObject<T> *getPooledObjectFromAllObjects(T *object) throw(BaseException)
    {
      typename std::map< long, PooledObject<T>* >::iterator iter = allObjects_.find(this->identityWrapper(object));
      if ( iter == allObjects_.end() ) throw BaseException("BaseException: GenericObjectPool::getPooledObjectFromALlObjects - object is not in pool");
      return iter->second;
    }

    PooledObject<T> *getPooledObjectFromIdleObjects()
    {
      if ( idleObjects_.empty() ) return NULL;
      PooledObject<T> * object = idleObjects_.front();
      return object;
    }

    void putPooledObjectToIdleObjects(PooledObject<T> *object)
    {
      idleObjects_.push_back(object);
    }
  private:
    PooledObjectFactory<T> *factory_;
    MutexLock lock_;
    int createCount_;
    std::map< long, PooledObject<T>* > allObjects_;
    std::deque< PooledObject<T>* > idleObjects_;
  };

}

#endif
