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