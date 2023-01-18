typealias list<T> = ArrayList<T>

data class ThingOne(val a: Int)
data class ThingTwo(val a: ThingOne)

fun main() {
  val l1: list<ThingOne>
  list<ThingOne>().also { for (i in 0..6) it.add(ThingOne(i)) }.also { l1 = it }
  do
    println("hi")
  while (false)
  while (false)
    println("hi")
  println("this is l1: $l1")
}