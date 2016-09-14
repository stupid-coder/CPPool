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

      if ( objectDeque == NULL ) throw BaseException("BaseException: GenericKeyedObjectPool::borrowObject - failure to get ObjectDeque");

      do {

        object = objectDeque->getPooledObjectFromIdleObjects();

        if ( object == NULL )
          {
            object = this->create(key);
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

            if ( object == NULL ) throw BaseException("BaseException GenericKeyedObjectPool::borrowObject - failure to borrow object");
          }
      } while ( object == NULL );

      this->deregiste(key);

      object->markAllocated();

      return object->getObject();
    }

    virtual void returnObject(const K *key, V *object) throw(IllegalStateException,BaseException)
    {
      this->assertOpen();

      ObjectDeque<V> *objectDeque = NULL;

      objectDeque = this->getObjectDequeFromPoolMap(key);

      if ( objectDeque == NULL ) throw BaseException("BaseException: GenericKeyedObjectPool::returnObject - failure to return object of key cannot be finded in pool");

      PooledObject<V> *pooledObject = objectDeque->getPooledObjectFromAllObjects(this->identityWrapper(object));

      if ( pooledObject == NULL ) throw BaseException("BaseException: GenericKeyedObjectPool::returnObject - failure to return object of object is not in the pool");

      pooledObject->markIdle();

      try {
        factory_->passivateObject(key,pooledObject);
      } catch ( BaseException pe ) {
        try {
          this->destroy(key,pooledObject);
        } catch (BaseException de) {
          throw de;
        }
      }

      objectDeque->putPooledObjectToIdleObjects(pooledObject);

    }

    virtual void invalidateObject(const K *key, V *object) throw(BaseException)
    {

      GuardRWLockRD lock(lock_);

      try {
        this->destroy(key,object);
      } catch (BaseException e) {
        throw e;
      }

    }

    virtual void addObject(const K *key) throw(IllegalStateException,UnsupportedOperationException)
    {

      this->assertOpen();

      try {
        PooledObject<V> *object = this->create(key);
        this->addIdleObject(key, object);
      } catch(BaseException e) {}

      this->deregiste(key);

    }

    virtual void clear() throw(UnsupportedOperationException,BaseException)
    {

      GuardRWLockWR lock(lock_);

      for ( typename std::map<K, ObjectDeque<V>* >::iterator iter = poolMap_.begin();
            iter != poolMap_.end();
            ++ iter )
        {
          delete iter->second;
        }

      poolMap_.clear();
    }

    virtual void clear(const K *key) throw(UnsupportedOperationException,BaseException)
    {

      GuardRWLockWR lock(lock_);

      ObjectDeque<V> *objectDeque = this->getObjectDequeFromPoolMap(key);

      if ( objectDeque == NULL ) return;
      else delete objectDeque;

      poolMap_.erase(*key);
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
          object->markIdle();
          factory_->passivateObject(key,object);
          ObjectDeque<V> *objectDeque = this->registe(key);
          objectDeque->putPooledObjectToIdleObjects(object);
        }
    }

    PooledObject<V> *create(const K *key) throw(BaseException)
    {
      // TODO: maxIdle minIdle maxTotal 限制
      ObjectDeque<V> *objectDeque = this->getObjectDequeFromPoolMap(key);
      PooledObject<V> *object = NULL;

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

      ObjectDeque<V> *objectDeque = this->getObjectDequeFromPoolMap(key);

      if ( objectDeque == NULL ) throw BaseException("BaseException: GenericKeyedObjectPool::invalidateObject - the object is not in the Pool");

      PooledObject<V> *pooledObject = objectDeque->getPooledObjectFromAllObjects(this->identityWrapper(object->getObject()));

      if ( pooledObject == NULL ) throw BaseException("BaseException: GenericKeyedObjectPool::invlaidateObject - the object is not in the pool");

      objectDeque->removePooledObjectFromAllObjects(this->identityWrapper(object->getObject()));

      try {
        factory_->destroyObject(key,object);
      } catch (BaseException e) {
      }

    }

    void destroy(const K *key, V *object) throw(BaseException)
    {

      ObjectDeque<V> *objectDeque = this->getObjectDequeFromPoolMap(key);

      if ( objectDeque == NULL ) throw BaseException("BaseException: GenericKeyedObjectPool::invalidateObject - the object is not in the Pool");

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

      try {
        {
          GuardRWLockRD rdlock(lock_);
          objectDeque = this->getObjectDequeFromPoolMap(key);
        }
        if ( objectDeque == NULL )
          {
            GuardRWLockWR wrlock(lock_);
            objectDeque = this->getObjectDequeFromPoolMap(key);
            if ( objectDeque == NULL )
              {
                objectDeque = new ObjectDeque<V>();
                this->putObjectDequeToPoolMap(key,objectDeque);
              }
          }
      } catch (BaseException e) {
      }

      return objectDeque;
    }

    void deregiste(const K *key)
    {
      // TODO: ObjectDeque 记录消除
      return;
    }

    ObjectDeque<V> * getObjectDequeFromPoolMap(const K *key)
    {
      GuardRWLockRD lock(lock_);

      typename std::map< K, ObjectDeque<V>* >::iterator find_iter = poolMap_.find(*key);

      if ( find_iter == poolMap_.end() ) return NULL;
      else return find_iter->second;
    }
    void putObjectDequeToPoolMap(const K *key, ObjectDeque<V> *objectDeque)
    {
      GuardRWLockWR lock(lock_);
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
