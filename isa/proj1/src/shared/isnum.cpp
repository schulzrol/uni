#include "isnum.hpp"

bool isNum(string str) {
    for (char c : str) {
        if (!isdigit(c)) {
            return false;
        }
    }

    return true;
}