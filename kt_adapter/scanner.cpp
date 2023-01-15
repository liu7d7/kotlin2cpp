struct {
  void* in = nullptr;
} System;

struct Scanner {
  bool needsIgnore = false;

  Scanner() {}
  Scanner(void* in) {}

  inline float nextFloat() {
    needsIgnore = true;
    float f;
    std::cin >> f;
    return f;
  }

  inline double nextDouble() {
    needsIgnore = true;
    double d;
    std::cin >> d;
    return d;
  }

  inline int nextInt() {
    needsIgnore = true;
    int i;
    std::cin >> i;
    return i;
  }

  inline long nextLong() {
    needsIgnore = true;
    long l;
    std::cin >> l;
    return l;
  }

  inline short nextShort() {
    needsIgnore = true;
    short s;
    std::cin >> s;
    return s;
  }

  inline char nextChar() {
    needsIgnore = true;
    char c;
    std::cin >> c;
    return c;
  }

  inline std::string next() {
    needsIgnore = true;
    std::string s;
    std::cin >> s;
    return s;
  }

  inline std::string nextLine() {
    std::string s;
    if (needsIgnore) std::cin.ignore();
    std::getline(std::cin, s);
    return s;
  }
};