# kt2cpp

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

<h3>
1/15/2023:
</h3>

+ some more translation
  + lambdas
  + references in function arguments
+ basic programs run
  + identifier translation

I've gone the route of not knowing what types things are. This is a pain, but it'll work with some scuffed codegen. I've also added a bunch of stuff to the grammar, and I'm working on translating identifiers.
Wow, GitHub Copilot wrote that last sentence.

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