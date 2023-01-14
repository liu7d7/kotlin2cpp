//
// Created by Aadi and Michael on 7/24/22.
//

#ifndef AT_POSITION_H
#define AT_POSITION_H

#include <string>

using namespace std;

class Position {
public:
    int line, col, idx;
    string *fName, *fText;

    Position(int idx, int line, int col, string *fName, string *fText);

    Position *advance(char c = '\0');

    Position *copy();

    string toString() const;
};

#endif //AT_POSITION_H
