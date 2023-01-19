import java.lang.Integer.max
import java.util.*
import kotlin.collections.ArrayList

val v = Array(1000) { false }
val l = ArrayList<ArrayList<Int>>()
var ans = 0

fun dfs(x: Int, d: Int = 0) {
  v[x] = true
  ans = max(ans, d)
  for (i in l[x])
    if (!v[i])
      dfs(i, d + 1)
}

fun main() {
  for (i in 0..999) l.add(ArrayList<Int>())
  val sc = Scanner(System.`in`)
  val n = sc.nextInt()
  for (i in 0 until n) {
    val st = sc.next()
    for (j in 0 until n) {
      if (st[j] == '1') {
        l[i].add(j)
        l[j].add(i)
      }
    }
  }
  dfs(0)
  println(ans)
}