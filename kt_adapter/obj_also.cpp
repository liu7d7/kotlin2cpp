template<typename T, typename F>
inline T& also(T& obj, F&& func) {
    func(obj);
    return obj;
}

template<typename T, typename F>
inline T& also(T&& obj, F&& func) {
  func(obj);
  return obj;
}