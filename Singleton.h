#ifndef SINGLETON_H
#define SINGLETON_H

#include "noncopyable.h"

template <typename T>
class Singleton : Noncopyable
{
public:
    Singleton(){};
    ~Singleton(){};

    static T &GetInstance()
    {
        if (instance_ == nullptr)
        {
            instance_ = new T();
        }

        return *instance_;
    }

    static T *instance_;
};

template <typename T>
T *Singleton<T>::instance_ = nullptr;

#endif // SINGLETON_H
