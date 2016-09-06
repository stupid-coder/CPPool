#ifndef _CP_POOL_H
#define _CP_POOL_H

template <typename T>
class CPPool
{
public:
  virtual ~CPPool() {}
  virtual T *borrowObject() = 0;
  virtual void returnObject(T *object) = 0;
  virtual void addObject() = 0;
  virtual void clear() = 0;
  virtual void close() = 0;
};


#endif  // _CP_POOL_H
