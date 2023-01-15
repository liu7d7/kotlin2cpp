template<typename T, typename F>
inline T& let(T& obj, F&& func) {
    return func(obj);
}

template<typename T, typename F>
inline T& let(T&& obj, F&& func) {
  return func(obj);
}