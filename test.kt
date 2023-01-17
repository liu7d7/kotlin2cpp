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