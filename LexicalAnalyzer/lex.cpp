
#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
#include <map>

#include "lex.h"


using namespace std;

enum State {start, inint, instr, inrl, inident, inassop, incomment};

LexItem id_or_kw(const string& lexeme, int linenum) {

    string lex;

    for(int i = 0; i < lexeme.length(); i++) {
        lex += toupper(lexeme[i]);
    }
    map<string, Token> d;
	d["PROGRAM"] = PROGRAM;
	d["END"] = END;
	d["BEGIN"] = BEGIN;
	d["WRITELN"] = 	WRITELN;
	d["IF"] = IF;
	d["INTEGER"] = INTEGER;
	d["REAL"] = REAL;
	d["STRING"] = STRING;
	d["FOR"] = FOR;
    d["THEN"] = THEN;
    d["DO"] = DO;
	d["TO"] = TO;
	d["DOWNTO"] = DOWNTO;
	d["ELSE"] = ELSE;
	d["VAR"] = VAR;
	
	if(d.count(lex) > 0) {
		return LexItem(d[lex], lex, linenum);
	} else {
		return LexItem(IDENT, lex, linenum);
	}



}

LexItem getNextToken(istream& in, int& linenum) {

    char a;
    string curr;
    bool inrl1;
    Token t;

    State s = start;
    //State d = ;

    map<char, Token> ops;
    ops['+'] = PLUS;
    ops['-'] = MINUS;
    ops['*'] = MULT;
    ops['/'] = DIV;
    ops['('] = LPAREN;
    ops[')'] = RPAREN;
    ops['='] = EQUAL;
    ops['>'] = GTHAN;
    ops['<'] = LTHAN;
    ops[':'] = COLON;
    ops[';'] = SEMICOL;
    ops[','] = COMMA;

    while(true) {

        a = in.get();

        switch(s){
            case start:
                
                if (a == '\n') {
                    linenum += 1;
                    continue;
                }

                if(isspace(a)) {
                    continue;
                } 
                
                if(a == EOF) {
                    //linenum +=1;
                    curr += a;

                    return LexItem(DONE, curr, linenum);
                }

                if(a == '(') {
                    char b = in.get();
                    if(b == '*') {
                        s = incomment;
                        continue;
                    } else {
                        in.putback(b);
                        return LexItem(ops[a], "" + a, linenum);
                    }
                }

                if(isalpha(a) || a == '_') {
                    s = inident;
                } 

                if(isdigit(a)) {
                    s = inint;
                }

                if(a == '.') {
                    s = inrl;
                    inrl1 = true;
                }

                if(a == '\'') {
                    s = instr;
                }
                if(ops.count(a) > 0) {
                    if(a == ':') {
                        s = inassop;
                    } else {
                        return LexItem(ops[a], ""+a, linenum);
                    }
                }

                if(a == '(') {
                    char b = in.get();
                    if(b == '*') {
                        s = incomment;
                    } else {
                        in.putback(b);
                        return LexItem(ops[a], "" + a, linenum);
                    }
                }
                
                if(s == start) {
                    linenum += 1;
                    return LexItem(ERR, string(1,a), linenum);
                    
                }
                
                break;
            case inident:
                if(!isalnum(a) && a != '_') {
                    in.putback(a);
                    return id_or_kw(curr, linenum);
                }
                break;
            case inint:
                if(a == '.') {
                    s = inrl;
                    inrl1 = true;
                } else if(!isdigit(a)) {
                    in.putback(a);
                    return LexItem(ICONST, curr, linenum);
                }
                break;
            case inrl:
                if(inrl1) {
                    if(!isdigit(a)) {
                        curr+=a;
                        linenum += 1;
                        return LexItem(ERR, curr, linenum);
                    } else {
                        inrl1 = false;
                    }
                } else {
                    if(a == '.') {
                        curr += a;
                        linenum += 1;
                        return LexItem(ERR, curr, linenum);
                    } else if(!isdigit(a)) {
                        in.putback(a);
                        return LexItem(RCONST, curr, linenum);
                    }
                }
                break;
            case instr:
                if(a == '\''){
                    curr += a;
                    return LexItem(SCONST, curr, linenum);
                }

                if(a == '\n' || a == EOF) {
                    in.putback(a);
                    linenum += 1;

                    return LexItem(ERR, curr, linenum);
                }
                break;
            case inassop:
                if(a == '=') {
                    curr += a;
                    return LexItem(ASSOP, curr, linenum);
                } else {
                    in.putback(a);
                    return LexItem(COLON, curr, linenum);
                }               
                break;
            case incomment:
                if(a == '*') {
                    char b = in.get();
                    if(b == ')') {
                        s = start;
                    } else {
                        in.putback(b);
                    }
                }

                if(a == '\n') {
                    linenum += 1;
                }

                if(a == EOF) {
                    //linenum += 1;
                    return LexItem(ERR, "Comment", linenum);
                    
                }

                continue;

                break;
        }

        curr += a;

    }

    if(s == inint) {
        t = ICONST;
    } else if(s == inrl) {
        t = RCONST;
    } else if(s == instr) {
        t = SCONST;
    } else if (s == inident) {
        return id_or_kw(curr, linenum);
    } else {
        t = ERR;
    }


    return LexItem(t, curr, linenum);

}

ostream& operator<<(ostream& out, const LexItem& tok) {

	map<Token, string> d;
	d[PROGRAM] = "PROGRAM";
	d[WRITELN] = "WRITELN";
	d[INTEGER] = "INTEGER";
	d[BEGIN] = "BEGIN";
	d[END] = "END";	
	d[IF] = "IF"; 
	d[REAL] = "REAL";
	d[STRING] = "STRING";
	d[VAR] = "VAR";
	d[ELSE] = "ELSE";
	d[FOR] = "FOR";
	d[THEN] = "THEN";
	d[DO] = "DO";
	d[TO] = "TO";
	d[DOWNTO] = "DOWNTO";
	d[IDENT] = "IDENT";
	d[ICONST] = "ICONST";
	d[RCONST] = "RCONST";
	d[SCONST] = "SCONST";
	d[PLUS] = "PLUS";
	d[MINUS] = "MINUS";
	d[MULT] = "MULT";
	d[DIV] = "DIV";
	d[ASSOP] = "ASSOP";
	d[LPAREN] = "LPAREN";
	d[RPAREN] = "RPAREN";
	d[COMMA] = "COMMA";
	d[EQUAL] = "EQUAL";
	d[GTHAN] = "GTHAN";
	d[LTHAN] = "LTHAN";
	d[SEMICOL] = "SEMICOL";
	d[COLON] = "COLON";
	d[ERR] = "ERR";
	d[DONE] = "DONE";

    if(tok == RCONST || tok == ICONST || tok == IDENT) {
        out << d[tok.GetToken()] << "(" << tok.GetLexeme() << ")" << endl;
    } else if(tok == SCONST) {
        out << d[tok.GetToken()] << "(" << tok.GetLexeme().substr(1,tok.GetLexeme().length()-2) << ")" << endl;
    } else if(tok == ERR) {
        if(tok.GetLexeme().substr(0,7) == "Comment") {
            out << "\nMissing a comment end delimiters '*)' at line " << tok.GetLinenum()<< endl;
        } else {
            out << "Error in line " << tok.GetLinenum() << " (" << tok.GetLexeme() << ")" << endl; 
        }  
    }else {
        out << d[tok.GetToken()] << endl;
    }

	return out;

}
