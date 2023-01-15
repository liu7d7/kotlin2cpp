template<typename T>
inline void container_add(std::vector<T>& vec, const T& obj) {
  vec.push_back(obj);
}

template<typename T>
inline void container_add(std::unordered_set<T>& vec, const T& obj) {
  vec.insert(obj);
}