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