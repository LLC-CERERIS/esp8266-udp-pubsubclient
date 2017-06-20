#ifndef TESTPROJ_SPTR_H
#define TESTPROJ_SPTR_H

#ifdef ESP8266
#include "Arduino.h"

#define printf(arg) Serial.print(arg)
#endif
//#include <cstdio>

template<typename T> class SPtr;

template<typename T> class Ref;

template<typename T>
class SPtr {
private:
  int count = 0;

public:
  Ref<T> *get() {
    Ref<T> *ref = new Ref<T>(this);
    return ref;
  }

  void incr() {
    count++;
  }

  void decr() {
    count--;

    if (count == 0) {
      delete this;
    }
  }


  SPtr(T *object) {
    this->object = object;
  }

  ~SPtr() {
    delete object;
  }

  T *object;
};

template<typename T>
struct Ref {
private:
  SPtr<T> *ptr;

public:
  Ref(SPtr<T> *ptr) {
    this->ptr = ptr;
    ptr->incr();
  }

  Ref() {}

  ~Ref() {
    ptr->decr();
  }

  T &operator*() {
    return *ptr->object;
  }

  T *operator->() {
    return ptr->object;
  }
};

#endif //TESTPROJ_SPTR_H
