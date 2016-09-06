#ifndef _TEST_POOLED_OBJECT_FACTORY_H
#define _TEST_POOLED_OBJECT_FACTORY_H

#include "cp.hpp"
#include <iostream>

class object
{
public:
  object():count_(0) {}
  ~object() { std::cout << "count: " << count_ << std::endl;}
  void increament() { ++count_; }
private:
  int count_;
};

class TestPooledObjectFactory : public CPPooledObjectFactory<object>
{
public:
  ~TestPooledObjectFactory() {}
  CPPooledObject<object> *makeObject()
  {
    return new CPDefaultPooledObject<object>(new object());
  }

  void destroyObject(CPPooledObject<object> *object)
  {
    delete object->getObject();
    delete object;
  }
};

#endif
