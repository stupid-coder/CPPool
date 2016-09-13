#ifndef _EXPCETIONS_H
#define _EXPCETIONS_H

#include <exception>
#include <string>

namespace CPPool {

  class BaseException : public std::exception
  {
  public:
    explicit
    BaseException(const std::string & msg)
      : msg_(msg) {}

    virtual ~BaseException() throw() {}

    virtual const char *what() const throw()
    {
      return msg_.c_str();
    }

  private:
    std::string msg_;
  };

  class IllegalStateException : public BaseException
  {
  public:
    explicit
    IllegalStateException(const std::string & msg)
      : BaseException(msg) {}

    virtual ~IllegalStateException() throw() {}
  };

  class UnsupportedOperationException : public BaseException
  {
  public:
    explicit
    UnsupportedOperationException(const std::string &msg)
      : BaseException(msg) {}

    virtual ~UnsupportedOperationException() throw() {}
  };

}

#endif
