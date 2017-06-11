#ifndef VECTOR_H
#define VECTOR_H

#include <cstring>

template<typename T>
class Vector {
private:
  T *data = new T[0];

  int _elementSize = sizeof(T);
  int _size = 0;
  int _allocated = 0;

  void checkAndReallocate() {
    if (_size + 1 > _allocated) {
      T *newData = new T[_size + 1];
      memmove(newData, data, _elementSize * _size);
      delete data;
      data = newData;
    }
  };

public:
  typedef T *iterator;
  typedef const T *const_iterator;

  iterator begin() {
    return &data[0];
  }

  const_iterator begin() const {
    return &data[0];
  }

  iterator end() {
    return &data[_size];
  }

  const_iterator end() const {
    return &data[_size];
  }

  Vector<T> &add(T element) {
    checkAndReallocate();
    data[_size] = element;
    _size++;
    return *this;
  };

  Vector<T> &remove(int index) {
    _size--;
    int toMove = _elementSize * (_size - index);
    if (toMove > 0) {
      memmove(data + index, data + index + 1, toMove);
    }
    return *this;
  };

  Vector<T> &remove(T element) {
    int index = indexOf(element);
    if (index != -1)
      remove(index);
    return *this;
  };

  T get(int index) {
    return data[index];
  }

  int indexOf(T element) {
    for (int i = 0; i < _size; i++) {
      T e = data[i];
      if (memcmp(&e, &element, _elementSize) == 0)
        return i;
    }

    return -1;
  };

  int size() {
    return _size;
  }
};

#endif //VECTOR_H
