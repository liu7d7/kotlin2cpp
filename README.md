# kotlin2cpp

<p>
a kotlin to c++ transpiler.
</p>
<p>
a good amount of the code is heavily based off of [at](https://www.github.com/diatmi/at), a language I worked on with a friend
</p>

# milestones

<h3>
1/14/2023:
</h3>

- basic syntax translation
  - classes
  - data classes
  - functions
  - variables
  - if statements
  - for loops
  - while loops
  - do while loops

at the moment, it's able to translate this basic (but nonsense) program

<details>

<summary>
(click to expand)
</summary>

```kotlin
package e.f.g.h

class a {

}

class b {
fun a() {
println("b")
}
}

data class lol<T>(a: T, b: lol)

fun <T> add(a: T, b: T): list<T> {
val result: T = a + b
return listOf(result)
}

fun yay() {
if (true) {
println("yay")
} else if (true) {
println("maybe")
} else {
println("nay")
}

do {
println("yo")
val res = add(1, 2)
println(res)
} while (true)
}

fun add(a: String, b: String): String {
val result: String = a + b
if (result == "yay") {
result.yo = "nay"
}
return result
}

fun main() {
add(1, 2)
add("test", "ing")
}
```

</details>

into valid cpp syntax (but it won't compile because identifiers are not yet translated)

<details>

<summary>(click to expand)</summary>

```c++
namespace e_f_g_h {
  struct a {

  };

  struct b {
    void a() {
      println("b");
    }
  };

  template <typename T>
  struct lol {
    T a;
    lol b;

    lol(T a, lol b) : a(a), b(b) {}
  };

  template <typename T>
  list<T> add(T a, T b) {
    T result = (a + b);
    return listOf(result);
  }

  void yay() {
    if (true) {
      println("yay");
    } else if (true) {
      println("maybe");
    } else {
      println("nay");
    }

    do {
      println("yo");
      auto res = add(1, 2);
      println(res);
    } while (true);
  }

  String add(String a, String b) {
    String result = (a + b);
    if ((result == "yay")) {
      result.yo = "nay";
    }

    return result;
  }

  void main() {
    add(1, 2);
    add("test", "ing");
  }
}
```
</details>

<br/>

<h3>
1/15/2023:
</h3>

+ some more translation
  + lambdas
  + references in function arguments
+ basic programs run
  + identifier translation

I've gone the route of not knowing what types things are. this is a pain, but it'll work with some scuffed codegen. I've also added a bunch of stuff to the grammar, and I'm working on translating identifiers.
wow, copilot wrote that last sentence.

kt2cpp is now able to translate this program

<details>

<summary>
(click to expand)
</summary>

```kotlin
typealias list<T> = ArrayList<T>

fun transformList(a: list<Int>): list<Int> {
  val l = list<Int>()
  for (i in a) {
    l.add(i + 1)
  }
  return l
}

fun printList(a: list<Int>) {
  for (i in a) {
    print(i)
    print(" ")
  }
}

fun main() {
  val l = list<Int>()
  l.add(3)
  l.add(4)
  l.add(2)
  l.add(5)
  transformList(l)
  list<Int>().size
  printList(list<Int>().also { it.add(3) })
  print("\n")
  printList(l.also { x: list<Int> -> x.add(3) })
}
```

</details>

into this one!

<details>

<summary>(click to expand)</summary>

```c++
#include <iostream>
#include <unordered_set>
#include <vector>

// kotlin support
namespace __kt__{template<typename T,typename F>inline T&also(T&obj,F func){func(obj);return obj;}template<typename T,typename F>inline T&also(T&&obj,F func){func(obj);return obj;}template<typename T>inline void print(const T&obj){std::cout<<obj;}template<typename T>inline void container_add(std::vector<T>&vec,const T&obj){vec.push_back(obj);}template<typename T>inline void container_add(std::unordered_set<T>&vec,const T&obj){vec.insert(obj);}}

template<typename T>
using list = std::vector<T>;

list<int> transformList(list<int>& a) {
  auto l = list<int>();
  for (auto i : a) {
    __kt__::container_add(l, (i + 1));
  }

  return l;
}

void printList(list<int>& a) {
  for (auto i : a) {
    __kt__::print(i);
    __kt__::print(" ");
  }
}

int main() {
  auto l = list<int>();
  __kt__::container_add(l, 3);
  __kt__::container_add(l, 4);
  __kt__::container_add(l, 2);
  __kt__::container_add(l, 5);
  transformList(l);
  list<int>().size();
  printList(__kt__::also(list<int>(), [&](auto& it) {
    return __kt__::container_add(it, 3);
  }));
  __kt__::print("\n");
  printList(__kt__::also(l, [&](list<int>& x) {
    return __kt__::container_add(x, 3);
  }));
}
```
<p>
both the kotlin and c++ code run and produce

```
3
3 4 2 5 3
```
as output.
</p>
<p>
here you can see the kotlin support functions, minified into the `__kt__` namespace, which are called 
after translating, for example, `list.add(item)` gets translated into `__kt__::container_add(list, item)`.
</p>
<p>
tree shaking is also performed on the support functions so that the `__kt__` namespace doesn't get too crowded for basic programs.
</p>
</details>

<br/>

<h3>
1/16/2023:
</h3>

- interpolated strings
- improved ktUtils (more modular & more functions such as `.map`)
- dataclasses now get a `toString()` function generated

here is yet another nonsensical program that shows off today's progress. the interpolated string is translated
using `string::operator +` (doesn't copy, so it's efficient) and the `toString()` function is generated for the dataclasses.

<details>
<summary>
(click to expand)
</summary>

```kotlin
typealias list<T> = ArrayList<T>

data class ThingOne(val a: Int)
data class ThingTwo(val a: ThingOne)

fun main() {
  val l1 = list<ThingOne>()
  for (i in 0..10) {
    l1.add(ThingOne(i))
  }
  val l2 = l1.map { ThingTwo(it) }
  println("yo!! $l2 asdf ${l1.also { println(it) }} man")
}
```
</details>

the generated c++ code looks like this:

<details>
<summary>
(click to expand)
</summary>

```c++
#include <iostream>
#include <unordered_map>
#include <functional>
#include <unordered_set>
#include <string>
#include <vector>

// kotlin support
namespace __kt__ {template<typename T>inline void container_add(std::vector<T>& vec, const T& obj) {vec.push_back(obj);}template<typename T>inline void container_add(std::unordered_set<T>& vec, const T& obj) {vec.insert(obj);}template<typename T>struct Array {T* data;int _size;typedef T* iterator;typedef const T* const_iterator;template<typename E>Array(int size, E f) : data(new T[size]), _size(size) {for (int i = 0; i < size; i++) {data[i] = f(i);}}inline int size() {return _size;}inline T& operator[](int index) {return data[index];}iterator begin() { return &data[0]; }const_iterator begin() const { return &data[0]; }iterator end() { return &data[_size]; }const_iterator end() const { return &data[_size]; }};template<typename E>Array<std::result_of_t<E(int&)>> Array_ctor(int size, E selector) {return Array<std::result_of_t<E(int&)>>(size, selector);}template<typename T, typename E>inline std::vector<std::result_of_t<E(T&)>> map(std::vector<T>& arr, E selector) {std::vector<std::result_of_t<E(T&)>> result;for (auto& i : arr) { result.push_back(selector(i)); }return result;}template<typename T, typename E>inline std::vector<std::result_of_t<E(T&, int&)>> map(std::vector<T>& arr, E selector) {std::vector<std::result_of_t<E(T&, int&)>> result;for (int i = 0; i < arr.size(); i++) { result.push_back(selector(arr[i], i)); }return result;}template<typename T, typename E>inline std::unordered_set<std::result_of_t<E(T&)>> map(std::unordered_set<T>& arr, E selector) {std::unordered_set<std::result_of_t<E(T&)>> result;for (auto& i : arr) { result.insert(selector(i)); }return result;}template<typename T, typename E>inline std::unordered_set<std::result_of_t<E(T&, int&)>> map(std::unordered_set<T>& arr, E selector) {std::unordered_set<std::result_of_t<E(T&, int&)>> result;int i = 0;for (auto& j : arr) {result.insert(selector(j, i));i++;}return result;}template<typename T, typename E>Array<std::result_of_t<E(T&)>> map(Array<T>& arr, E selector) {Array<std::result_of_t<E(T&)>> result(arr.size(), [&](auto& it) {return selector(arr[it]);});return result;}template<typename T, typename E>Array<std::result_of_t<E(T&, int&)>> map(Array<T>& arr, E selector) {Array<std::result_of_t<E(T&, int&)>> result(arr.size(), [&](auto& it) { return selector(arr[it], it); });return result;}inline std::string toString(int val) {return std::to_string(val);}inline std::string toString(long long val) {return std::to_string(val);}inline std::string toString(char val) {return std::string(1, val);}inline std::string toString(std::string& val) {return val;}inline std::string toString(const char* val) {return std::string(val);}inline std::string toString(std::string&& val) {return val;}template<typename T>inline std::string toString(T& val) {return val.toString();}template<typename T>inline std::string toString(T&& val) {return val.toString();}template<typename T>inline std::string toString(std::vector<T>& vec) {std::string s = "[";for (int i = 0; i < vec.size(); i++) {s += toString(vec[i]);if (i < vec.size() - 1) {s += ", ";}}s += "]";return s;}template<typename T>inline std::string toString(std::vector<T>&& vec) {std::string s = "[";for (int i = 0; i < vec.size(); i++) {s += toString(vec[i]);if (i < vec.size() - 1) {s += ", ";}}s += "]";return s;}template<typename T>inline std::string toString(Array<T>& vec) {std::string s = "[";for (int i = 0; i < vec.size(); i++) {s += toString(vec[i]);if (i < vec.size() - 1) {s += ", ";}}s += "]";return s;}template<typename T>inline std::string toString(Array<T>&& vec) {std::string s = "[";for (int i = 0; i < vec.size(); i++) {s += toString(vec[i]);if (i < vec.size() - 1) {s += ", ";}}s += "]";return s;}template<typename T>inline std::string toString(std::unordered_set<T>& vec) {std::string s = "[";int i = 0;for (auto& item : vec) {s += toString(item);if (i < vec.size() - 1) {s += ", ";}i++;}s += "]";return s;}template<typename T>inline std::string toString(std::unordered_set<T>&& vec) {std::string s = "[";int i = 0;for (auto& item : vec) {s += toString(item);if (i < vec.size() - 1) {s += ", ";}i++;}s += "]";return s;}template<typename K, typename V>inline std::string toString(std::unordered_map<K, V>& map) {std::string s = "{";int i = 0;for (auto& [key, value] : map) {s += toString(key) + ": " + toString(value);if (i < map.size() - 1) {s += ", ";}i++;}s += "}";return s;}template<typename K, typename V>inline std::string toString(std::unordered_map<K, V>&& map) {std::string s = "{";int i = 0;for (auto& [key, value] : map) {s += toString(key) + ": " + toString(value);if (i < map.size() - 1) {s += ", ";}i++;}s += "}";return s;}template<typename T>inline void println(T& obj) {std::cout << toString(obj) << '\n';}template<typename T>inline void println(T&& obj) {std::cout << toString(obj) << '\n';}template<typename T, typename F>inline T& also(T& obj, F&& func) {func(obj);return obj;}template<typename T, typename F>inline T& also(T&& obj, F&& func) {func(obj);return obj;}}

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
  for (auto i = 0; i <= 10; i += (1)) {
    __kt__::container_add(l1, ThingOne(i));
  }

  auto l2 = __kt__::map(l1, [&](auto& it) {
    return ThingTwo(it);
  });
  __kt__::println((std::string("yo!! ") + __kt__::toString(l2) + " asdf " + __kt__::toString(    __kt__::also(l1, [&](auto& it) {
    return __kt__::println(it);
  })) + " man"));
}
```
</details>

it's quite a bit longer than the kotlin version, isn't it?