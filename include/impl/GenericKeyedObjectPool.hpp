#ifndef _CPPOOL_GENERIC_KEYED_OBJECT_POOL_H
#define _CPPOOL_GENERIC_KEYED_OBJECT_POOL_H

#include <interface/BaseKeyedObjectPool.hpp>
#include <interface/KeyedPooledObjectFactory.hpp>
#include <interface/ObjectDeque.hpp>
#include <impl/GenericObjectPoolConfig.hpp>
#include <map>


namespace CPPool
{
  template <typename K, typename V>
  class GenericKeyedObjectPool
    : virtual public BaseKeyedObjectPool<K,V>,
      virtual public BaseGenericKeyedObjectPool<V>
  {
  public:
    GenericKeyedObjectPool(KeyedObjectPoolFactory<K,V> *factory);
    {
      initGenericKeyedObjectPool(factory,new GenericObjectPoolConfig());
    }

    GenericKeyedObjectPool(KeyedObjectPoolFactory<K,V> *factory,
                           GenericObjectPoolConfig *config)
    {
      initGenericKeyedObjectPool(factory,config);
    }

    virtual ~GenericKeydObjectPool()
    {
      close();
    }

    virtual V *borrowObject(const K *key) throw(IllegalStateException,BaseException)
    {
      assertOpen();

      PooledObject<V> object = NULL;
      bool create = false;

      ObjectDeque<V> objectDeque = registe(key);

      if ( objectDeque == NULL ) throw BaseException("BaseException: GenericKeyedObjectPool::borrowObject - failure to get ObjectDeque");

      do {
        object = objectDeque->getPooledObjectFromIdleObjects();

        if ( object == NULL )
          {
            object = create(key);
            create = true;
          }

        if ( object != NULL )
          {
            try {
              factory_->activateObject(key,object);
            } catch (BaseException ae) {
              try {
                destroy(key,object);
              } catch (BaseException de) {
              }
              throw ae;
            }

            if ( object != NULL && (getTetOnBorrow()|| create && getTestOnCreate()))
              {
                bool validate = false;

                try {
                  factory_->validateObject(key,object);
                } catch ( BaseException ve ) {
                }

                if ( !validate ) {
                  try {
                    destroy(key,p);
                  } catch ( BaseException de ) {
                  }
                  object = NULL;
                }
              }

            if ( object == NULL ) throw BaseException("BaseException GenericKeyedObjectPool::borrowObject - failure to borrow object");
          }
      } while ( object == NULL );

      return object->getObject();
    }

    virtual void returnObject(const K *key, V *object) throw(IllegalStateException,BaseException)
    {
      assertOpen();

      GuardRWLockRD lock(lock_);

      ObjectDeque<V> *objectDeque = getObjectDequeFromPoolMap(key);

      if ( objectDeque == NULL ) throw BaseException("BaseException: GenericKeyedObjectPool::returnObject - failure to return object of key cannot be finded in pool");

      PooledObject<V> *object = objectDeque->findPooledObjectFromAllObjects(key);

      if ( object == NULL ) throw BaseException("BaseException: GenericKeyedObjectPool::returnObject - failure to return object of object is not in the pool");

      object->markIdle();

      try {
        factory_->passivateObject(key,object);
      } catch ( BaseException pe ) {
        try {
          destroy(key,object);
        } catch (BaseException de) {
          throw de;
        }
      }

      objectDeque->putPooledObjectToIdleObjects(object);

    }

    virtual void invalidateObject(const K *key, V *object) throw(BaseException)
    {

      GuardRWLockRD lock(lock_);

      ObjectDeque<V> *objectDeque = getObjectDequeFromPoolMap(key);

      PooledObject<V> *pooledObject = objectDeque->getPooledObjectFromAllObjects(identityWrapper(object));

      if ( pooledObject == NULL ) throw BaseException("BaseException: GenericKeyedObjectPool::invlaidateObject - the object is not in the pool");
      try {
        destroy(key,object);
      } catch (BaseException e) {
        throw e;
      }

    }

    virtual void addObject(const K *key) throw(IllegalStateException,BaseException)
    {
      assertOpen();

      registe(key);
      try {
        PooledObject<T> *object = create(key);
        addIdleObject(key,object);
      } catch(BaseException e) {}

      deregiste(key);

    }

    virtual void clear() throw(IllegalStateException,BaseException)
    {
      for ( std::map<K, ObjectDeque<V>* >::iterator iter = poolMap_.begin();
            iter != poolMap_.end();
            ++ iter )
        {
          clear(&iter->first);
        }
    }

    virtual void clear(const K *key) throw(IllegalStateException,BaseException)
    {
      ObjectDeque<V> *objectDeque = registe(&(iter->first));
      if ( objectDeque == NULL ) return;
      else delete objectDeque;
    }

    virtual void close()
    {
      if ( isClosed() ) return;

      closed_ = true;

      GuardRWLockWR lock(lock_);

      clear();
    }

  protected:
    void initGenericKeyedObjectPool(KeyedObjectPoolFactory<K,V> *factory,
                                    GenericObjectPoolConfig *config)
    {
      factory_ = factory;
      setConfig(config);
    }

    void addIdleObject(const K *key, PooledObject<V> *object) throw(BaseException)
    {
      if ( object != NULL )
        {
          factory_->passivateObject(key,object);
          PooledObject<V> *objectDeque = registe(key);
          objectDeque->putPooledObjectToIdleObjects(object);
        }
    }

    PooledObject<V> *create(const K *key) throw(BaseException)
    {
      // TODO: maxIdle minIdle maxTotal 限制
      ObjectDeque<V> *objectDeque = getObjectDequeFromPoolMap(key);

      PooledObject<V> *object = NULL;

      try {
        object = factory_->makeObject(key);
      } catch(BaseException e) {
        throw e;
      }

      if ( object != NULL )
        objectDeque->putPooledObjectToAllObjects(key,object);

      return object;
    }

    void destroy(cosnt K *key, PooledObject<V> *object)
    {

    }

    ObjectDeque<V> *registe(const K *key)
    {

      ObjectDeque<V> *objectDeque = NULL;

      try {
        {
          GuardRWLockRD rdlock(lock_);
          objectDeque = getObjectDequeFromPoolMap(key);
        }
        if ( objectDeque == NULL )
          {
            GuardRWLockWR wrlock(lock_);
            objectDeque = getObjectDequeFromPoolMap(key);
            if ( objectDeque == NULL )
              {
                objectDeque = new ObjectDeque<V>();
                putObjectDequeToPoolMap(key,objectDeque);
              }
          }
        else
          {
            GuardRWLockRD rdlock(lock_);
            objectDeque = getObjectDequeFromPoolMap(key);
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

    ObjectDeque<V> * getObjectDequeFromPoolMap(const K *key) const
    {
      std::map< K, ObjectDeque<V>* >::iterator find_iter = poolMap_.find(*key);

      if ( find_iter == poolMap_.end() ) return NULL;
      else return find_iter->second;
    }
    void putObjecTDequeToPoolMap(const K *key, ObjectDeque<V> *objectDeque)
    {
      poolMap_.put(*key, objectDeque);
    }
  private:

    KeyedPooledObjectFactory<K,V> factory_;

    RWLock lock_;

    std::map< K,ObjectDeque<V>* > poolMap_;

    int numTotal_;

  };

}


#endif
