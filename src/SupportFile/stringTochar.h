#include "Arduino.h"


char*  string_char(String s) {
    const int length = s.length(); 
    char* char_array = new char[length + 1]; 
    strcpy(char_array, s.c_str()); 
    return char_array;
}