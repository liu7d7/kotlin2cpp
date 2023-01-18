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

  ~Array() {
    delete[] data;
  }

  Array(const Array& other) : Array(other._size, [&](int i) { return other[i]; }) {}

  Array(Array&& other) noexcept : data(std::exchange(other.data, nullptr)), _size(other._size) {}

  Array& operator=(const Array& other) {
    if (this != &other) {
      delete[] data;
      data = new T[other._size];
      _size = other._size;
      for (int i = 0; i < _size; i++) {
        data[i] = other[i];
      }
    }
    return *this;
  }

  Array& operator=(Array&& other) noexcept {
    if (this != &other) {
      delete[] data;
      data = std::exchange(other.data, nullptr);
      _size = other._size;
    }
    return *this;
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
