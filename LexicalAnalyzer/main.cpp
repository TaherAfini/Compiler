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

	string fileName = "";
    int numflags = 0;
    
    map<string, int> ins;
    ins["-v"] = 0;
    ins["-iconst"] = 0;
    ins["-rconst"] = 0;
    ins["-sconst"] = 0;
    ins["-ident"] = 0;


    for(int i = 0; i < argc; i++) {
        
        if(ins.count(argv[i]) > 0) {
            ins[argv[i]]++;
            numflags++;
        } else if(argv[i][0] == '-') {
            cout << "UNRECOGNIZED FLAG " << argv[i] << endl;
            exit(1);
        }
    }

    if(argc-numflags > 2) {
        cout << "ONLY ONE FILE NAME ALLOWED." << endl;
        exit(1);
    }

    if(argc - numflags == 1) {
        cout << "NO SPECIFIED INPUT FILE NAME." << endl;
        exit(1);
    }

    for(int i = 1; i < argc; i++) {
        if(ins.count(argv[i]) <= 0) {
            fileName = argv[i];
            break;
        }
    }


	fin.open(fileName.c_str());
	if(!fin.is_open()) {
	
		cout << "CANNOT OPEN the File " << fileName << endl;
		exit(1);

	}	
    
    bool v = false;
    bool iconst = false;
    bool rconst = false;
    bool sconst = false;
    bool ident = false;


    if(ins["-v"] > 0) {
        v = true;
    }
    if(ins["-iconst"] > 0) {
        iconst = true;
    }
    if(ins["-rconst"] > 0) {
        rconst = true;
    }
    if(ins["-sconst"] > 0) {
        sconst = true;
    }
    if(ins["-ident"] > 0) {
        ident = true;
    }


    LexItem t = LexItem();
    int line = 0;
    int tokens = 0;

    map<int, int> ints;
    map<double, int> reals;
    map<string, int> strings;
    map<string, int> idents;


    while(t != DONE) {
        t = getNextToken(fin, line);


        if(t == ERR) {
            
            cout << t;
            
            if(t.GetLexeme().substr(0,7).compare("Comment") == 0) {
                continue;
            } else {
                exit(1);
            }
        }

        if(t == DONE) {
            break;
        }

        tokens++;

        if(v) {
            
            cout << t;
        }

        if(iconst && t == ICONST) {
            int a = stoi(t.GetLexeme());
            if(ints.count(a) <= 0) {
                ints[a] = 1;
            }
        }

        if(rconst && t == RCONST) {
            double b = stod(t.GetLexeme());
            if(reals.count(b) <= 0) {
                reals[b] = 1;
            }
        }

        if(sconst && t == SCONST) {
            string c = t.GetLexeme();
            if(strings.count(c) <= 0) {
                strings[c] = 1;
            }
        }

        if(ident && t == IDENT) {
            string d = t.GetLexeme();
            if(idents.count(d) <= 0) {
                idents[d] = 1;
            }
        }

    }
    
    line = t.GetLinenum();
    
    cout << "Lines: " << line <<endl;

    if(line == 0) {
        exit(1);
    }

    cout << "Tokens: " << tokens << endl;


    if(sconst) {
        cout << "STRINGS:" << endl;
        map<string, int>::iterator it;

        for (it = strings.begin(); it != strings.end(); it++)
        {
            cout << it->first << endl;
        }
    }
    if(iconst) {
        cout << "INTEGERS:" << endl;
        map<int, int>::iterator it;

        for (it = ints.begin(); it != ints.end(); it++)
        {
            cout << it->first << endl;
        }
    }

    if(rconst) {
        cout << "REALS:" << endl;
        map<double, int>::iterator it;

        for (it = reals.begin(); it != reals.end(); it++)
        {
            cout << it->first << endl;
        }
    }

    if(ident) {
        cout << "IDENTIFIERS:" << endl;
        map<string, int>::iterator it;

        for (it = idents.begin(); it != idents.end(); it++)
        {
            
            
            cout << it->first;
            
            if(next(it) != idents.end()){
                cout << ", ";
            }  
        }

        
        cout << endl;
    }



    
    return 0;
}
