#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <stdio.h>
#include <sstream>
#include <string>

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