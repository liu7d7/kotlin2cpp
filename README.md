# kt2cpp

heavily based off of [at](https://www.github.com/diatmi/at), a language I worked on with a friend

# what can it do?

at the moment, it's able to translate this basic program
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
into valid cpp syntax (but it won't compile because identifiers are not yet translated)
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