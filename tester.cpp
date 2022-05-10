#include <iostream>
#include <fstream>
#include <cctype>
#include <map>
#include <string>

#include "lex.h"
#include "lex.cpp"

using namespace std;


int main(int argc, char* argv[]) {

	ifstream fin;

    fin.open("Test/ints");

    char a;

    while(!fin.eof()) {
        a = fin.get();

        if(a == '\n') {
            cout << "\\n" << endl;
        } else {
            cout << a;
        }
    }

    return 0;
}