//
// Created by Aadi and Michael on 7/24/22.
//

#include <Position.h>

Position::Position(int idx, int line, int col, string* fName, string* fText) {
  this->line = line;
  this->col = col;
  this->idx = idx;
  this->fName = fName;
  this->fText = fText;
}

Position* Position::advance(char c) {
  if (c == '\n') {
    line++;
    col = 0;
  } else {
    col++;
  }
  idx++;
  return this;
}

Position* Position::copy() {
  return new Position(idx, line, col, fName, fText);
}

string Position::toString() const {
  return "Line " + to_string(line) + ", Column " + to_string(col);
}
