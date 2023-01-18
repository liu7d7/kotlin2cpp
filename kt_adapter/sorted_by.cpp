template<typename V, typename F>
inline V sortedBy(V& arr, F selector) {
  V copy = arr;
  std::sort(copy.begin(), copy.end(), [&selector](typename std::remove_reference<decltype(arr)>::type::value_type a, typename std::remove_reference<decltype(arr)>::type::value_type b) {
    return selector(a) < selector(b);
  });
  return copy;
}

template<typename V, typename F>
inline V sortedBy(V&& arr, F selector) {
  std::sort(arr.begin(), arr.end(), [&selector](typename std::remove_reference<decltype(arr)>::type::value_type a, typename std::remove_reference<decltype(arr)>::type::value_type b) {
    return selector(a) < selector(b);
  });
  return arr;
}