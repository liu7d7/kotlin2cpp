template<typename T>
inline void println(T& obj) {
  std::cout << toString(obj) << '\n';
}
template<typename T>
inline void println(T&& obj) {
  std::cout << toString(obj) << '\n';
}