/* 
 * parse.h
 * Programming Assignment 2
 * Spring 2022
*/

#ifndef PARSE_H_
#define PARSE_H_

#include <iostream>

using namespace std;

#include "lex.h"



extern bool Prog(istream& in, int& line);//done Done
extern bool DeclBlock(istream& in, int& line);//done Done 
extern bool DeclStmt(istream& in, int& line);//done Done
extern bool ProgBody(istream& in, int& line);//done Done
extern bool Stmt(istream& in, int& line);//done Done
extern bool WriteLnStmt(istream& in, int& line);//done Done
extern bool IfStmt(istream& in, int& line);//done  Done
extern bool ForStmt(istream& in, int& line);//done Done
extern bool AssignStmt(istream& in, int& line);//done Done
extern bool Var(istream& in, int& line);//done Done
extern bool ExprList(istream& in, int& line);//done Done
extern bool LogicExpr(istream& in, int& line);//done Done
extern bool Expr(istream& in, int& line);//done Done
extern bool Term(istream& in, int& line);//done Done
extern bool SFactor(istream& in, int& line);//done Done
extern bool Factor(istream& in, int& line, int sign);//done Done
extern int ErrCount();

#endif /* PARSE_H_ */
