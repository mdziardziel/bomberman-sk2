#include "Helpers.hpp"
char * toChar(int x) {
    int len =  snprintf(nullptr, 0, "%d", x);
    char * str = new char[len];
    sprintf(str, "%d", x);
	return str;
}

char* toChar2(int x) {
    int len =  snprintf(nullptr, 0, "%d", x);
    char* conv = new char[2];
    if(x < 0){
        conv[0] = '-';
        conv[1] = -x + '0';      
    }else if(len == 1){
        conv[0] = '0';
        conv[1] = x + '0';
    }else if(len == 2){
        int first = x/10;
        int second = x%10;
        conv[0] = first + '0';
        conv[1] = second + '0';
    }
    return conv;
}

char* toChar3(int x) {
    int len =  snprintf(nullptr, 0, "%d", x);
    char* conv = new char[3];   
    if(len == 1){
        conv[0] = '0';
        conv[1] = '0';
        conv[2] = x + '0';
    }else if(len == 2){
        int first = x/10;
        int second = x%10;
        conv[0] = '0';
        conv[1] = first + '0';
        conv[2] = second + '0';
    }else if(len == 3){
        int first = x/100;
        int second = (x/10)%10;
        int third = x%10;
        conv[0] = first + '0';
        conv[1] = second + '0';
        conv[2] = third + '0';
    }
    return conv;
}

int toInt(char * number) {
    int ret;
    std::stringstream s(number);
    s >> ret;
    // int ret = (int)number - 48;
    return ret;
}