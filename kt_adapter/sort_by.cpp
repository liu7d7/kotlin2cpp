template<typename V, typename F>
inline void sortBy(V& arr, F selector) {
  std::sort(arr.begin(), arr.end(), [&selector](typename std::remove_reference<decltype(arr)>::type::value_type a, typename std::remove_reference<decltype(arr)>::type::value_type b) {
    return selector(a) < selector(b);
  });
}

template<typename V, typename F>
inline void sortBy(V&& arr, F selector) {
  std::sort(arr.begin(), arr.end(), [&selector](typename std::remove_reference<decltype(arr)>::type::value_type a, typename std::remove_reference<decltype(arr)>::type::value_type b) {
    return selector(a) < selector(b);
  });
}