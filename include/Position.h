//
// Created by Aadi and Michael on 7/24/22.
//

#ifndef KT2CPP_POSITION_H
#define KT2CPP_POSITION_H

#include <string>

class Position {
public:
    int line, col, idx;
    std::string *fName, *fText;

    Position(int idx, int line, int col, std::string *fName, std::string *fText);

    Position *advance(char c = '\0');

    Position *copy();

    [[nodiscard]] std::string toString() const;
};

#endif //KT2CPP_POSITION_H
