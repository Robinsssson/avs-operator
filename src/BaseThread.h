#ifndef __BASETHREAD__H__
#define __BASETHREAD__H__
#include <thread>

class BaseThread {
    explicit BaseThread();
    virtual void operator()();
    virtual ~BaseThread();
};

#endif  //!__BASETHREAD__H__

