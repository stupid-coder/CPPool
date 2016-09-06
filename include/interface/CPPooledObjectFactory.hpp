#ifndef _CP_POOLED_OBJECT_FACTORY_H
#define _CP_POOLED_OBJECT_FACTORY_H

#include "interface/CPPooledObject.hpp"

/**
 * CPPooledObjectFactory:
 * initialized&manage&destroy the pooled object
 */
template <typename T>
class CPPooledObjectFactory
{
public:
  virtual ~CPPooledObjectFactory() {};
  virtual CPPooledObject<T> *makeObject() = 0;
  virtual void destroyObject(CPPooledObject<T> *object) = 0;
  //virtual bool validateObject(CPPooledObject<T> &object) = 0;
  //virtual void activateObject(CPPooledObject<T> &object) = 0;
  //virtual void passivateObject(CPPooledObject<T> &object) = 0;
};


#endif  // _CP_POOLED_OBJECT_FACTORY_H
