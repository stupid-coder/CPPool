#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <misc/WrapperLock.hpp>
#include <cassert>

struct TestArg
{
  int id;
  int *i;
  CPPool::LockIf *lock;
} arg[2];

void * test_mutex_lock(void *arg)
{
  TestArg *p = (TestArg*)arg;
  CPPool::LockIf *lock = p->lock;
  int *i = p->i;
  int id = p->id;
  while ( *i < 3 )
    {
      CPPool::GuardMutexLock guardLock(*lock);
      std::cout << id << ": get lock" << std::endl;
      ++ *i;
      sleep(3);
    }
  return NULL;
}

void *test_rwlock_lock(void *arg)
{
  TestArg *p = (TestArg*)arg;
  int * i = p->i;
  int id  = p->id;
  CPPool::LockIf *lock = p->lock;

  while ( *i < 3 )
    {
      {
        CPPool::GuardRWLockRD rdlock(*lock);

        std::cout << "read lock - id:" << id << "\ti:" << *i << std::endl;

        sleep(1);
      }

      {
        CPPool::GuardRWLockWR wrlock(*lock);

        std::cout << "write lock - id:" << id << "\ti:" << *i << std::endl;

        ++ *i;
        sleep(1);
      }
    }
  return NULL;
}


int main(int argc, char *argv[])
{
  int i = 0;
  arg[0].id = 0;
  arg[1].id = 1;
  arg[0].i = &i;
  arg[1].i = &i;
  arg[0].lock = new CPPool::MutexLock();
  arg[1].lock = arg[0].lock;

  pthread_t pthread_id[2];
  pthread_create(&pthread_id[0],NULL,test_mutex_lock,&arg[0]);
  pthread_create(&pthread_id[1],NULL,test_mutex_lock,&arg[1]);

  pthread_join(pthread_id[0],NULL);
  pthread_join(pthread_id[1],NULL);

  assert(i == 4);

  delete arg[0].lock;

  i = 0;
  arg[0].lock = new CPPool::RWLock();
  arg[1].lock = arg[0].lock;

  pthread_create(&pthread_id[0],NULL,test_rwlock_lock,&arg[0]);
  pthread_create(&pthread_id[1],NULL,test_rwlock_lock,&arg[1]);

  pthread_join(pthread_id[0],NULL);
  pthread_join(pthread_id[1],NULL);

  delete arg[0].lock;

  assert( i==4 );
  return 0;
}
