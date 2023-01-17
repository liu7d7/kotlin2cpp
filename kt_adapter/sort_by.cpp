template<typename T, typename V>
inline void sortBy(T& arr, std::function<V(T)> selector) {
  std::sort(arr.begin(), arr.end(), [&selector](T a, T b) {
    return selector(a) < selector(b);
  });
}