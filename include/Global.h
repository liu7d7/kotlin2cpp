//
// Created by richard may clarkson on 12/01/2023.
//

#ifndef KT2CPP_GLOBAL_H
#define KT2CPP_GLOBAL_H

#include <chrono>

inline long long currentTimeMillis() {
  return std::chrono::duration_cast<std::chrono::milliseconds>(
    std::chrono::steady_clock::now().time_since_epoch()
  ).count();
}

#endif //KT2CPP_GLOBAL_H
