template<typename T>
inline void print(T& obj) {
  std::cout << toString(obj);
}
template<typename T>
inline void print(T&& obj) {
  std::cout << toString(obj);
}