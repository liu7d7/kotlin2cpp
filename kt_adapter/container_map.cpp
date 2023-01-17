template<typename T, typename E>
inline std::vector<std::result_of_t<E(T&)>> map(std::vector<T>& arr, E selector) {
  std::vector<std::result_of_t<E(T&)>> result;
  for (auto& i : arr) { result.push_back(selector(i)); }
  return result;
}

template<typename T, typename E>
inline std::vector<std::result_of_t<E(T&, int&)>> map(std::vector<T>& arr, E selector) {
  std::vector<std::result_of_t<E(T&, int&)>> result;
  for (int i = 0; i < arr.size(); i++) { result.push_back(selector(arr[i], i)); }
  return result;
}

template<typename T, typename E>
inline std::unordered_set<std::result_of_t<E(T&)>> map(std::unordered_set<T>& arr, E selector) {
  std::unordered_set<std::result_of_t<E(T&)>> result;
  for (auto& i : arr) { result.insert(selector(i)); }
  return result;
}

template<typename T, typename E>
inline std::unordered_set<std::result_of_t<E(T&, int&)>> map(std::unordered_set<T>& arr, E selector) {
  std::unordered_set<std::result_of_t<E(T&, int&)>> result;
  int i = 0;
  for (auto& j : arr) {
    result.insert(selector(j, i));
    i++;
  }
  return result;
}

template<typename T, typename E>
Array<std::result_of_t<E(T&)>> map(Array<T>& arr, E selector) {
  Array<std::result_of_t<E(T&)>> result(arr.size(), [&](auto& it) {
    return selector(arr[it]);
  });
  return result;
}

template<typename T, typename E>
Array<std::result_of_t<E(T&, int&)>> map(Array<T>& arr, E selector) {
  Array<std::result_of_t<E(T&, int&)>> result(arr.size(), [&](auto& it) { return selector(arr[it], it); });
  return result;
}