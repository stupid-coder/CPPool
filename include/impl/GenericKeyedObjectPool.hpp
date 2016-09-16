#ifndef _CPPOOL_GENERIC_KEYED_OBJECT_POOL_H
#define _CPPOOL_GENERIC_KEYED_OBJECT_POOL_H

#include <interface/BaseKeyedObjectPool.hpp>
#include <interface/BaseKeyedPooledObjectFactory.hpp>
#include <interface/ObjectDeque.hpp>

#include <impl/BaseGenericKeyedObjectPool.hpp>
#include <impl/GenericObjectPoolConfig.hpp>
#include <impl/DefaultPooledObject.hpp>

#include <map>

namespace CPPool
{
  template <typename K, typename V>
  class GenericKeyedObjectPool
    : virtual public BaseKeyedObjectPool<K,V>,
      virtual public BaseGenericKeyedObjectPool<V>
  {
  public:
    GenericKeyedObjectPool(KeyedPooledObjectFactory<K,V> *factory)
    {
      this->initGenericKeyedObjectPool(factory,new GenericObjectPoolConfig());
    }

    GenericKeyedObjectPool(KeyedPooledObjectFactory<K,V> *factory,
                           GenericObjectPoolConfig *config)
    {
      this->initGenericKeyedObjectPool(factory,config);
    }

    virtual ~GenericKeyedObjectPool()
    {
      this->close();
    }

    virtual V *borrowObject(const K *key) throw(IllegalStateException,BaseException)
    {
      this->assertOpen();

      PooledObject<V> *object = NULL;
      bool created = false;

      ObjectDeque<V> *objectDeque = this->registe(key);

      do {

        object = objectDeque->getPooledObjectFromIdleObjects();

        if ( object == NULL )
          {
            try {
              object = this->create(key);
            } catch (BaseException e) {
              this->deregiste(key);
              throw e;
            }
            created = true;
          }

        if ( object != NULL )
          {
            try {
              factory_->activateObject(key,object);
            } catch (BaseException ae) {
              try {
                this->destroy(key,object);
              } catch (BaseException de) {
              }
              this->deregiste(key);
              throw ae;
            }

            if ( object != NULL && (this->getTestOnBorrow() || (created && this->getTestOnCreate())))
              {
                bool validate = false;

                try {
                  factory_->validateObject(key,object);
                } catch ( BaseException ve ) {
                }

                if ( !validate ) {
                  try {
                    this->destroy(key,object);
                  } catch ( BaseException de ) {
                  }
                  object = NULL;
                }
              }

            if ( object == NULL )
              {
                this->deregiste(key);
                throw BaseException("BaseException GenericKeyedObjectPool::borrowObject - failure to borrow object");
              }
          }
      } while ( object == NULL );

      this->deregiste(key);

      object->markAllocated();

      return object->getObject();
    }

    virtual void returnObject(const K *key, V *object) throw(IllegalStateException,BaseException)
    {
      this->assertOpen();

      ObjectDeque<V> *objectDeque = this->registe(key);

      PooledObject<V> *pooledObject = objectDeque->getPooledObjectFromAllObjects(this->identityWrapper(object));

      if ( pooledObject == NULL ) throw BaseException("BaseException: GenericKeyedObjectPool::returnObject - failure to return object of object is not in the pool");

      pooledObject->markIdle();

      try {
        factory_->passivateObject(key,pooledObject);
      } catch ( BaseException pe ) {
        try {
          this->destroy(key,pooledObject);
        } catch (BaseException de) {
          this->deregiste(key);
          throw de;
        }
      }

      objectDeque->putPooledObjectToIdleObjects(pooledObject);

      this->deregiste(key);
    }

    virtual void invalidateObject(const K *key, V *object) throw(BaseException)
    {

      this->registe(key);

      try {
        this->destroy(key,object);
      } catch (BaseException e) {
        this->deregiste(key);
        throw e;
      }

      this->deregiste(key);

    }

    virtual void addObject(const K *key) throw(IllegalStateException,UnsupportedOperationException)
    {

      this->assertOpen();

      this->registe(key);

      try {
        PooledObject<V> *object = this->create(key);
        this->addIdleObject(key, object);
      } catch(BaseException e) {}

      this->deregiste(key);
    }

    virtual void clear() throw(UnsupportedOperationException,BaseException)
    {

      GuardRWLockWR wrlock(lock_);

      for ( typename std::map<K, ObjectDeque<V>* >::iterator iter = poolMap_.begin();
            iter != poolMap_.end();
            ++ iter )
        {
          clear(&iter->first);
        }

      poolMap_.clear();

    }

    virtual void clear(const K *key) throw(UnsupportedOperationException,BaseException)
    {
      ObjectDeque<V> *objectDeque = this->registe(key);

      for ( PooledObject<V> *object = objectDeque->removePooledObjectFromAllObjectsNonlock();
            object != NULL;
            object = objectDeque->removePooledObjectFromAllObjectsNonlock() )
        {
          try {
            factory_->destroyObject(key,object);
          } catch( BaseException e ) {}
        }
      this->deregiste(key);
    }

    virtual void close()
    {
      if ( this->isClosed() ) return;

      this->closed_ = true;

      this->clear();
    }

  protected:
    void initGenericKeyedObjectPool(KeyedPooledObjectFactory<K,V> *factory,
                                    GenericObjectPoolConfig *config)
    {
      factory_ = factory;
      this->setConfig(config);
    }

    void addIdleObject(const K *key, PooledObject<V> *object) throw(BaseException)
    {

      if ( object != NULL )
        {
          GuardRWLockRD rdlock(lock_);
          ObjectDeque<V> *objectDeque = this->getObjectDequeFromPoolMap(key);
          object->markIdle();
          factory_->passivateObject(key,object);
          objectDeque->putPooledObjectToIdleObjects(object);
        }
    }

    PooledObject<V> *create(const K *key) throw(BaseException)
    {
      // TODO: maxIdle minIdle maxTotal 限制
      ObjectDeque<V> *objectDeque = NULL;
      PooledObject<V> *object = NULL;

      objectDeque = this->getObjectDequeFromPoolMap(key);

      try {
        object = factory_->makeObject(key);
      } catch(BaseException e) {
        throw e;
      }

      if ( object != NULL )
        objectDeque->putPooledObjectToAllObjects(this->identityWrapper(object->getObject()),object);

      return object;
    }

    void destroy(const K *key, PooledObject<V> *object) throw(BaseException)
    {

      this->destroy(key,object->getObject());
    }

    void destroy(const K *key, V *object) throw(BaseException)
    {

      ObjectDeque<V> *objectDeque = this->getObjectDequeFromPoolMap(key);

      if ( objectDeque == NULL ) throw BaseException("BaseException: GenericKeyedObjectPool::destroy - failure to fidn ObjectDeque");

      PooledObject<V> *pooledObject = objectDeque->getPooledObjectFromAllObjects(this->identityWrapper(object));

      if ( pooledObject == NULL ) throw BaseException("BaseException: GenericKeyedObjectPool::invlaidateObject - the object is not in the pool");

      objectDeque->removePooledObjectFromAllObjects(this->identityWrapper(object));

      try {
        factory_->destroyObject(key,pooledObject);
      } catch (BaseException e) {
      }

    }

    ObjectDeque<V> *registe(const K *key)
    {

      ObjectDeque<V> *objectDeque = NULL;

      {
        GuardRWLockRD rdlock(lock_);
        objectDeque = this->getObjectDequeFromPoolMapNonlock(key);
        if ( objectDeque != NULL )
          objectDeque->increamentAndGetNumInterested();
      }

      if ( objectDeque == NULL )
        {
          GuardRWLockWR wrlock(lock_);
          objectDeque = this->getObjectDequeFromPoolMapNonlock(key);
          if ( objectDeque == NULL )
            {
              objectDeque = new ObjectDeque<V>();
              this->putObjectDequeToPoolMapNonlock(key,objectDeque);
            }
          objectDeque->increamentAndGetNumInterested();
        }

      return objectDeque;

    }

    void deregiste(const K *key) throw(BaseException)
    {
      // TODO: ObjectDeque 记录消除
      ObjectDeque<V> *objectDeque = NULL;
      int numInterested = 0;

      objectDeque = this->getObjectDequeFromPoolMap(key);

      if ( objectDeque == NULL ) throw BaseException("BaseException: GenericKeyedObjectPool::deregiste - failure to find ObjectDeque");

      numInterested = objectDeque->decreamentAndGetNumInterested();

      if ( numInterested == 0 && objectDeque->nonBorrowedObject() )
        {
          GuardRWLockWR wrlock(lock_);

          objectDeque = this->getObjectDequeFromPoolMapNonlock(key);

          if ( objectDeque != NULL ) {
            numInterested = objectDeque->getNumInterested();

            if ( numInterested == 0 && objectDeque->nonBorrowedObject() )
              {
                poolMap_.erase(*key);
                delete objectDeque;
              }
          }
        }
    }

    ObjectDeque<V> * getObjectDequeFromPoolMap(const K *key)
    {
      GuardRWLockRD rdlock(lock_);
      return getObjectDequeFromPoolMapNonlock(key);
    }

    ObjectDeque<V> *getObjectDequeFromPoolMapNonlock(const K *key)
    {
      typename std::map< K, ObjectDeque<V>* >::iterator find_iter = poolMap_.find(*key);

      if ( find_iter == poolMap_.end() ) return NULL;
      else return find_iter->second;
    }

    void putObjectDequeToPoolMap(const K *key, ObjectDeque<V> *objectDeque)
    {
      GuardRWLockWR wrlock(lock_);
      putObjectDequeToPoolMapNonlock(key,objectDeque);
    }

    void putObjectDequeToPoolMapNonlock(const K *key, ObjectDeque<V> *objectDeque)
    {
      poolMap_.insert(std::make_pair< K,ObjectDeque<V>* >(*key, objectDeque));
    }


  private:

    KeyedPooledObjectFactory<K,V> *factory_;

    RWLock lock_;

    std::map< K, ObjectDeque<V>* > poolMap_;

    int numTotal_;

  };

}


#endif
