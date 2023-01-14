//
// Created by Aadi and Michael on 7/24/22.
//

#include <Error.h>

Error::Error(Position* posStart, Position* posEnd, string name, string message) {
  this->posStart = posStart->copy();
  this->posEnd = posEnd->copy();
  this->posEnd->advance();
  this->name = std::move(name);
  this->message = std::move(message);
}

static std::vector<std::string> splitString(const std::string& str) {
  std::vector<std::string> tokens;

  std::stringstream ss(str);
  std::string token;
  while (std::getline(ss, token, '\n')) {
    tokens.push_back(token);
  }

  return tokens;
}

string Error::toString() {
  string s = name + ": " + message + "\n";
  s += "File: " + *posStart->fName + ":" + to_string(posStart->line + 1) + ":" + to_string(posStart->col + 1) + "\n";
  s += "\n\n" + addArrows(*posStart->fText);
  return s;
}

string Error::addArrows(const string& text) const {
  // Add '^' under the string s from the start index to the end index

  vector<string> splits = splitString(text);
  stringstream ss;
  for (int i = 0; i < posStart->col; i++) {
    ss << ' ';
  }
  for (int i = posStart->col; i < posEnd->col - 1; i++) {
    ss << '^';
  }
  int line = posStart->line;
  splits.insert(splits.begin() + line + 1, ss.str());
  stringstream final;
  for (int i = line; i <= posEnd->line + 1; i++) {
    final << splits[i] << '\n';
  }
  return final.str();
}

Error::Error(Token* tok, string name, string message) {
  this->posStart = tok->posStart->copy();
  this->posEnd = tok->posEnd->copy();
  this->posEnd->advance();
  this->name = std::move(name);
  this->message = std::move(message);
}
