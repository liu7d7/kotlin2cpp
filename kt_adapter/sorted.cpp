template<typename T>
inline T sorted(T& arr) {
  T copy = arr;
  std::sort(copy.begin(), copy.end());
  return copy;
}

template<typename T>
inline T sorted(T&& arr) {
  std::sort(arr.begin(), arr.end());
  return arr;
}