#include "Helpers.hpp"
char * toChar(int x) {
	std::stringstream str;
	str << x;
	return &str.str()[0];
}

int toInt(char * number) {
    int ret;
    std::stringstream s(number);
    s >> ret;
    // int ret = (int)number - 48;
    return ret;
}