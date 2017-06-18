#ifndef TESTPROJ_SPTR_H
#define TESTPROJ_SPTR_H

#include "Arduino.h"

//#include <cstdio>

template<typename T> class SPtr;

template<typename T> class Ref;

template<typename T>
class SPtr {
private:
  int count = 0;

public:
  Ref<T> *get() {
    Serial.println("Obtaining Ref");
//    printf("Obtaining Ref\n");
    Ref<T> *ref = new Ref<T>(this);
    return ref;
  }

  void inc() {
    count++;
  }

  void decr() {
    count--;

    if (count == 0)
      delete this;
  }


  SPtr(T *object) {
    Serial.println("Constructing SPtr");
//    printf("Constructing SPtr\n");
    this->object = object;
  }

  ~SPtr() {
    Serial.println("Destructing SPtr");
//    printf("Destructing SPtr\n");
    delete object;
  }

  T *object;
};

template<typename T>
class Ref {
private:
  SPtr<T> *ptr;

public:
  Ref(SPtr<T> *ptr) {
    this->ptr = ptr;
    ptr->inc();
  }

  Ref() {}

  ~Ref() {
    Serial.println("Destructing Ref");
//    printf("Destructing Ref\n");
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
