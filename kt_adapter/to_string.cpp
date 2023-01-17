inline std::string toString(int val) {
  return std::to_string(val);
}

inline std::string toString(long long val) {
  return std::to_string(val);
}

inline std::string toString(char val) {
  return std::string(1, val);
}

inline std::string toString(std::string& val) {
  return val;
}

inline std::string toString(const char* val) {
  return std::string(val);
}

inline std::string toString(std::string&& val) {
  return val;
}

template<typename T>
inline std::string toString(T& val) {
  return val.toString();
}

template<typename T>
inline std::string toString(T&& val) {
  return val.toString();
}

template<typename T>
inline std::string toString(std::vector<T>& vec) {
  std::string s = "[";
  for (int i = 0; i < vec.size(); i++) {
    s += toString(vec[i]);
    if (i < vec.size() - 1) {
      s += ", ";
    }
  }
  s += "]";
  return s;
}

template<typename T>
inline std::string toString(std::vector<T>&& vec) {
  std::string s = "[";
  for (int i = 0; i < vec.size(); i++) {
    s += toString(vec[i]);
    if (i < vec.size() - 1) {
      s += ", ";
    }
  }
  s += "]";
  return s;
}

template<typename T>
inline std::string toString(Array<T>& vec) {
  std::string s = "[";
  for (int i = 0; i < vec.size(); i++) {
    s += toString(vec[i]);
    if (i < vec.size() - 1) {
      s += ", ";
    }
  }
  s += "]";
  return s;
}

template<typename T>
inline std::string toString(Array<T>&& vec) {
  std::string s = "[";
  for (int i = 0; i < vec.size(); i++) {
    s += toString(vec[i]);
    if (i < vec.size() - 1) {
      s += ", ";
    }
  }
  s += "]";
  return s;
}

template<typename T>
inline std::string toString(std::unordered_set<T>& vec) {
  std::string s = "[";
  int i = 0;
  for (auto& item : vec) {
    s += toString(item);
    if (i < vec.size() - 1) {
      s += ", ";
    }
    i++;
  }
  s += "]";
  return s;
}

template<typename T>
inline std::string toString(std::unordered_set<T>&& vec) {
  std::string s = "[";
  int i = 0;
  for (auto& item : vec) {
    s += toString(item);
    if (i < vec.size() - 1) {
      s += ", ";
    }
    i++;
  }
  s += "]";
  return s;
}

template<typename K, typename V>
inline std::string toString(std::unordered_map<K, V>& map) {
  std::string s = "{";
  int i = 0;
  for (auto& [key, value] : map) {
    s += toString(key) + ": " + toString(value);
    if (i < map.size() - 1) {
      s += ", ";
    }
    i++;
  }
  s += "}";
  return s;
}

template<typename K, typename V>
inline std::string toString(std::unordered_map<K, V>&& map) {
  std::string s = "{";
  int i = 0;
  for (auto& [key, value] : map) {
    s += toString(key) + ": " + toString(value);
    if (i < map.size() - 1) {
      s += ", ";
    }
    i++;
  }
  s += "}";
  return s;
}