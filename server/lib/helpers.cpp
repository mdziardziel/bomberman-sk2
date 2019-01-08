#include "Helpers.hpp"
char * toChar(int x) {
    int len =  snprintf(nullptr, 0, "%d", x);
    char * str = new char[len];
    sprintf(str, "%d", x);
	return str;
}

int toInt(char * number) {
    int ret;
    std::stringstream s(number);
    s >> ret;
    // int ret = (int)number - 48;
    return ret;
}