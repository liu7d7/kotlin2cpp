template<typename T>
struct Array {
  T* data;
  int _size;
  typedef T* iterator;
  typedef const T* const_iterator;

  template<typename E>
  Array(int size, E f) : data(new T[size]), _size(size) {
    for (int i = 0; i < size; i++) {
      data[i] = f(i);
    }
  }

  inline int size() {
    return _size;
  }

  inline T& operator[](int index) {
    return data[index];
  }

  iterator begin() { return &data[0]; }
  const_iterator begin() const { return &data[0]; }
  iterator end() { return &data[_size]; }
  const_iterator end() const { return &data[_size]; }
};

template<typename E>
Array<std::result_of_t<E(int&)>> Array_ctor(int size, E selector) {
  return Array<std::result_of_t<E(int&)>>(size, selector);
}
