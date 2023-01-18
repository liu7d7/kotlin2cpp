#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <unordered_set>
#include <string>
#include <vector>

// kotlin support
namespace __kt__ {template<typename T>inline void container_add(std::vector<T>& vec, const T& obj) {vec.push_back(obj);}template<typename T>inline void container_add(std::unordered_set<T>& vec, const T& obj) {vec.insert(obj);}template<typename T>struct Array {T* data;int _size;typedef T* iterator;typedef const T* const_iterator;template<typename E>Array(int size, E f) : data(new T[size]), _size(size) {for (int i = 0; i < size; i++) {data[i] = f(i);}}~Array() {delete[] data;}Array(const Array& other) : Array(other._size, [&](int i) { return other[i]; }) {}Array(Array&& other) noexcept : data(std::exchange(other.data, nullptr)), _size(other._size) {}Array& operator=(const Array& other) {if (this != &other) {delete[] data;data = new T[other._size];_size = other._size;for (int i = 0; i < _size; i++) {data[i] = other[i];}}return *this;}Array& operator=(Array&& other) noexcept {if (this != &other) {delete[] data;data = std::exchange(other.data, nullptr);_size = other._size;}return *this;}inline int size() {return _size;}inline T& operator[](int index) {return data[index];}iterator begin() { return &data[0]; }const_iterator begin() const { return &data[0]; }iterator end() { return &data[_size]; }const_iterator end() const { return &data[_size]; }};template<typename E>Array<std::result_of_t<E(int&)>> Array_ctor(int size, E selector) {return Array<std::result_of_t<E(int&)>>(size, selector);}template<typename T, typename E>inline std::vector<std::result_of_t<E(T&)>> map(std::vector<T>& arr, E selector) {std::vector<std::result_of_t<E(T&)>> result;for (auto& i : arr) { result.push_back(selector(i)); }return result;}template<typename T, typename E>inline std::vector<std::result_of_t<E(T&, int&)>> map(std::vector<T>& arr, E selector) {std::vector<std::result_of_t<E(T&, int&)>> result;for (int i = 0; i < arr.size(); i++) { result.push_back(selector(arr[i], i)); }return result;}template<typename T, typename E>inline std::unordered_set<std::result_of_t<E(T&)>> map(std::unordered_set<T>& arr, E selector) {std::unordered_set<std::result_of_t<E(T&)>> result;for (auto& i : arr) { result.insert(selector(i)); }return result;}template<typename T, typename E>inline std::unordered_set<std::result_of_t<E(T&, int&)>> map(std::unordered_set<T>& arr, E selector) {std::unordered_set<std::result_of_t<E(T&, int&)>> result;int i = 0;for (auto& j : arr) {result.insert(selector(j, i));i++;}return result;}template<typename T, typename E>Array<std::result_of_t<E(T&)>> map(Array<T>& arr, E selector) {Array<std::result_of_t<E(T&)>> result(arr.size(), [&](auto& it) {return selector(arr[it]);});return result;}template<typename T, typename E>Array<std::result_of_t<E(T&, int&)>> map(Array<T>& arr, E selector) {Array<std::result_of_t<E(T&, int&)>> result(arr.size(), [&](auto& it) { return selector(arr[it], it); });return result;}template<typename V, typename F>inline V sortedBy(V& arr, F selector) {V copy = arr;std::sort(copy.begin(), copy.end(), [&selector](typename std::remove_reference<decltype(arr)>::type::value_type a, typename std::remove_reference<decltype(arr)>::type::value_type b) {return selector(a) < selector(b);});return copy;}template<typename V, typename F>inline V sortedBy(V&& arr, F selector) {std::sort(arr.begin(), arr.end(), [&selector](typename std::remove_reference<decltype(arr)>::type::value_type a, typename std::remove_reference<decltype(arr)>::type::value_type b) {return selector(a) < selector(b);});return arr;}inline std::string toString(int val) {return std::to_string(val);}inline std::string toString(long long val) {return std::to_string(val);}inline std::string toString(char val) {return std::string(1, val);}inline std::string toString(std::string& val) {return val;}inline std::string toString(const char* val) {return std::string(val);}inline std::string toString(std::string&& val) {return val;}template<typename T>inline std::string toString(T& val) {return val.toString();}template<typename T>inline std::string toString(T&& val) {return val.toString();}template<typename T>inline std::string toString(std::vector<T>& vec) {std::string s = "[";for (int i = 0; i < vec.size(); i++) {s += toString(vec[i]);if (i < vec.size() - 1) {s += ", ";}}s += "]";return s;}template<typename T>inline std::string toString(std::vector<T>&& vec) {std::string s = "[";for (int i = 0; i < vec.size(); i++) {s += toString(vec[i]);if (i < vec.size() - 1) {s += ", ";}}s += "]";return s;}template<typename T>inline std::string toString(Array<T>& vec) {std::string s = "[";for (int i = 0; i < vec.size(); i++) {s += toString(vec[i]);if (i < vec.size() - 1) {s += ", ";}}s += "]";return s;}template<typename T>inline std::string toString(Array<T>&& vec) {std::string s = "[";for (int i = 0; i < vec.size(); i++) {s += toString(vec[i]);if (i < vec.size() - 1) {s += ", ";}}s += "]";return s;}template<typename T>inline std::string toString(std::unordered_set<T>& vec) {std::string s = "[";int i = 0;for (auto& item : vec) {s += toString(item);if (i < vec.size() - 1) {s += ", ";}i++;}s += "]";return s;}template<typename T>inline std::string toString(std::unordered_set<T>&& vec) {std::string s = "[";int i = 0;for (auto& item : vec) {s += toString(item);if (i < vec.size() - 1) {s += ", ";}i++;}s += "]";return s;}template<typename K, typename V>inline std::string toString(std::unordered_map<K, V>& map) {std::string s = "{";int i = 0;for (auto& [key, value] : map) {s += toString(key) + ": " + toString(value);if (i < map.size() - 1) {s += ", ";}i++;}s += "}";return s;}template<typename K, typename V>inline std::string toString(std::unordered_map<K, V>&& map) {std::string s = "{";int i = 0;for (auto& [key, value] : map) {s += toString(key) + ": " + toString(value);if (i < map.size() - 1) {s += ", ";}i++;}s += "}";return s;}template<typename T>inline void println(T& obj) {std::cout << toString(obj) << '\n';}template<typename T>inline void println(T&& obj) {std::cout << toString(obj) << '\n';}}

template<typename T>
using list = std::vector<T>;

struct ThingOne {
  int a;

  ThingOne() {}
  ThingOne(int a) : a(a) {}
  [[nodiscard]] std::string toString() {
    return (std::string("ThingOne(") + "a=" + __kt__::toString(a) + ")");
  }
};

struct ThingTwo {
  ThingOne a;

  ThingTwo() {}
  ThingTwo(ThingOne a) : a(a) {}
  [[nodiscard]] std::string toString() {
    return (std::string("ThingTwo(") + "a=" + __kt__::toString(a) + ")");
  }
};

int main() {
  auto l1 = list<ThingOne>();
  for (auto i = 10; i >= 3; i -= (2)) {
    __kt__::container_add(l1, ThingOne(i));
  }

  auto l2 = __kt__::map(l1, [&](auto& it) {
    return ThingTwo(it);
  });
  auto l3 = __kt__::sortedBy(l2, [&](auto& it) {
    return it.a.a;
  });
  __kt__::println((std::string("this is l1: ") + __kt__::toString(l1)));
  __kt__::println((std::string("this is l2: ") + __kt__::toString(l2)));
  __kt__::println((std::string("this is l3: ") + __kt__::toString(l3)));
}
