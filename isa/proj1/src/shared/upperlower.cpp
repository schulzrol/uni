/*
* Autor: Roland Schulz (xschul06)
*/

#include "upperlower.hpp"

string toUpper(const string str) {
    string temp = str;
    transform(temp.begin(), temp.end(), temp.begin(), toupper);
    return temp;
}

string toLower(const string str) {
    string temp = str;
    transform(temp.begin(), temp.end(), temp.begin(), tolower);
    return temp;
}
