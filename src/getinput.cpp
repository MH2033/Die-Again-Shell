#include <iostream>
#include <unistd.h>
#include <sys/types.h> 
#include <string.h>
#include <vector>
#include <wordexp.h>
#include "termcolors.hpp"

using namespace std;
wordexp_t get_input(){
    wordexp_t p;
    char c;
    bool containsSlash = false;
    string inp;
    while(true){
        c = getchar();
        if(c != '\n')
            inp.push_back(c);

        if(c == '\\')
            containsSlash = true;

        if(c == '\n' && !containsSlash)
            break;

        else if(c == '\n' && containsSlash){
            cout << MAGENTA << "~> " << RESET;
            containsSlash = false;
            inp.push_back(c);
        }
    }
    wordexp(inp.c_str(), &p, 0);
    return p;
}