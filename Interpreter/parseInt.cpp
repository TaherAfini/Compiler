/* Implementation of PLSL Interpreter
 * parseInt.cpp
 * Programming Assignment 3
 * Spring 2022
*/

#include <vector>
#include <string>
#include "parseInt.h"
#include "val.h"
//#include "lex.cpp"
using namespace std;
map<string, bool> defVar;
map<string, Token> SymTable;
map<string, Value> TempsResults; //Container of temporary locations of Value objects for results of expressions, variables values and constants 
queue <Value> * ValQue; //declare a pointer variable to a queue of Value objects

namespace Parser {
	bool pushed_back = false;
	LexItem	pushed_token;

	static LexItem GetNextToken(istream& in, int& line) {
		if( pushed_back ) {
			pushed_back = false;
			return pushed_token;
		}
		return getNextToken(in, line);
	}

	static void PushBackToken(LexItem & t) {
		if( pushed_back ) {
			abort();
		}
		pushed_back = true;
		pushed_token = t;	
	}

}

static int error_count = 0;

int ErrCount()
{
    return error_count;
}

void ParseError(int line, string msg)
{
	++error_count;
	cout << line << ": " << msg << endl;
}

bool IdentList(istream& in, int& line);

Value Value::operator+(const Value& op) const {

    if(this->IsString() && op.IsString()) {
        return Value(GetString() + op.GetString());
    }

    if(this->IsInt() && op.IsInt()) {
        return Value(GetInt() + op.GetInt());
    }

    if(this->IsReal() && op.IsReal()) {
        return Value(GetReal() + op.GetReal());
    }

    if(this->IsReal() && op.IsInt()) {
        return Value(GetReal() + op.GetInt());
    }

    if(this->IsInt() && op.IsReal()) {
        return Value(GetInt() + op.GetReal());
    }

    return Value();

}

Value Value::operator-(const Value& op) const {

    if(this->IsInt() && op.IsInt()) {
        return Value(GetInt() - op.GetInt());
    }

    if(this->IsReal() && op.IsReal()) {
        return Value(GetReal() - op.GetReal());
    }

    if(this->IsReal() && op.IsInt()) {
        return Value(GetReal() - op.GetInt());
    }

    if(this->IsInt() && op.IsReal()) {
        return Value(GetInt() - op.GetReal());
    }

    return Value();

}

Value Value::operator*(const Value& op) const {

    if(this->IsInt() && op.IsInt()) {
        return Value(GetInt() * op.GetInt());
    }

    if(this->IsReal() && op.IsReal()) {
        return Value(GetReal() * op.GetReal());
    }

    if(this->IsReal() && op.IsInt()) {
        return Value(GetReal() * op.GetInt());
    }

    if(this->IsInt() && op.IsReal()) {
        return Value(GetInt() * op.GetReal());
    }

    return Value();
}

Value Value::operator/(const Value& op) const {

    if(this->IsInt() && op.IsInt()) {
        return Value(GetInt() / op.GetInt());
    }

    if(this->IsReal() && op.IsReal()) {
        return Value(GetReal() / op.GetReal());
    }

    if(this->IsReal() && op.IsInt()) {
        return Value(GetReal() / op.GetInt());
    }

    if(this->IsInt() && op.IsReal()) {
        return Value(GetInt() / op.GetReal());
    }

    return Value();
}

Value Value::operator==(const Value& op) const {

    if(IsString() && op.IsString()) {
        return Value(GetString() == op.GetString());
    }

    if(IsInt() && op.IsInt()) {
        return Value(GetInt() == op.GetInt());
    }

    if(IsReal() && op.IsReal()) {
        return Value(GetReal() == op.GetReal());
    }

    if(this->IsReal() && op.IsInt()) {
        return Value(GetReal() == op.GetInt());
    }

    if(this->IsInt() && op.IsReal()) {
        return Value(GetInt() == op.GetReal());
    }

    if(IsBool() && op.IsBool()) {
        return Value(GetBool() == op.GetBool());
    }
    
    return Value();
}

Value Value::operator>(const Value& op) const {

    if(IsString() && op.IsString()) {
        return Value(GetString() > op.GetString());
    }

    if(IsInt() && op.IsInt()) {
        return Value(GetInt() > op.GetInt());
    }

    if(IsReal() && op.IsReal()) {
        return Value(GetReal() > op.GetReal());
    }

    if(IsReal() && op.IsInt()) {
        return Value(GetReal() > op.GetInt());
    }

    if(IsInt() && op.IsReal()) {
        return Value(GetInt() > op.GetReal());
    }
    
    return Value();
}

Value Value::operator<(const Value& op) const {

    if(IsString() && op.IsString()) {
        return Value(GetString() < op.GetString());
    }

    if(IsInt() && op.IsInt()) {
        return Value(GetInt() < op.GetInt());
    }

    if(IsReal() && op.IsReal()) {
        return Value(GetReal() < op.GetReal());
    }

    if(this->IsReal() && op.IsInt()) {
        return Value(GetReal() < op.GetInt());
    }

    if(this->IsInt() && op.IsReal()) {
        return Value(GetInt() < op.GetReal());
    }
    
    return Value();
}

//bool IdentList(istream& in, int& line, vector<string> &IdList);

bool Prog(istream& in, int& line)
{
	bool f1, f2;
	LexItem tok = Parser::GetNextToken(in, line);
		
	if (tok.GetToken() == PROGRAM) {
		tok = Parser::GetNextToken(in, line);
		if (tok.GetToken() == IDENT) {
			
			tok = Parser::GetNextToken(in, line);
			if (tok.GetToken() == SEMICOL) {
				f1 = DeclBlock(in, line); 
			
				if(f1) {
					f2 = ProgBody(in, line);
					if(!f2)
					{
						ParseError(line, "Incorrect Program Body.");
						return false;
					}
					
					return true;//Successful Parsing is completed
				}
				else
				{
					ParseError(line, "Incorrect Declaration Section.");
					return false;
				}
			}
			else
			{
				//Parser::PushBackToken(tok);
				ParseError(line-1, "Missing Semicolon.");
				return false;
			}
		}
		else
		{
			ParseError(line, "Missing Program Name.");
			return false;
		}
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	else if(tok.GetToken() == DONE && tok.GetLinenum() <= 1){
		ParseError(line, "Empty File");
		return true;
	}
	ParseError(line, "Missing PROGRAM.");
	return false;
}

bool ProgBody(istream& in, int& line){
	bool status;
		
	LexItem tok = Parser::GetNextToken(in, line);
	
	if (tok.GetToken() == BEGIN) {
		
		status = Stmt(in, line);
		
		while(status)
		{
			int l = line;
			tok = Parser::GetNextToken(in, line);
			if(tok != SEMICOL)
			{
				ParseError(l, "Missing semicolon in Statement.");
				return false;
			}
			
			status = Stmt(in, line);
		}
			
		tok = Parser::GetNextToken(in, line);
		if(tok == END )
		{
			return true;
		}
		else 
		{
			ParseError(line, "Syntactic error in Program Body.");
			return false;
		}
	}
	else
	{
		ParseError(line, "Non-recognizable Program Body.");
		return false;
	}	
}//End of ProgBody function


bool DeclBlock(istream& in, int& line) {
	bool status;
	LexItem t = Parser::GetNextToken(in, line);

	if(t == VAR) {
		
		while(true){
			status = DeclStmt(in, line);
			t = Parser::GetNextToken(in, line);
			if(!status || t != SEMICOL) {
				ParseError(line, "Syntactic error in Declaration Block.");
				return false;
			}

			t = Parser::GetNextToken(in, line);
			if(t == BEGIN) {
				Parser::PushBackToken(t);
				return true;
			}

			Parser::PushBackToken(t);
		}

	} else {
		ParseError(line, "Non-recognizable Declaration Block.");
		return false;
	}
}
bool DeclStmt(istream& in, int& line) {
	LexItem t;
	
	while(true) {
		t = Parser::GetNextToken(in, line);
		if(t != IDENT) {
			ParseError(line, "Missing Identifier.");
			return false;
		}

		if(defVar.find(t.GetLexeme()) != defVar.end()) {
			ParseError(line, "Variable Redefinition");
			ParseError(line, "Incorrect variable in Declaration Statement.");
			return false;
		} else {
			defVar.insert(pair<string,bool>(t.GetLexeme(), true));
			SymTable.insert(pair<string,Token>(t.GetLexeme(), IDENT));
		}

		t = Parser::GetNextToken(in, line);

		if(t!= COMMA) {
			
			if(t == COLON) {
				//ParseError(line, "Missing Colon.");
				//return false;
				break;
			}
			
			ParseError(line, "Unrecognized Input Pattern\n(" + t.GetLexeme() + ")");
			ParseError(line, "Incorrect variable in Declaration Statement.");

			return false;
		}
	}

	t = Parser::GetNextToken(in, line);

	if(t != INTEGER && t != REAL && t != STRING) {
		ParseError(line, "Incorrect Declaration Type.");
		return false;
	}

	for(auto it = SymTable.begin(); it != SymTable.end(); ++it) {
		if(it->second == IDENT) {
			it->second = t.GetToken();
		}
	}

	return true;


}

//IdList:= IDENT {,IDENT}
// bool IdentList(istream& in, int& line) {
// 	bool status = false;
// 	string identstr;
	
// 	LexItem tok = Parser::GetNextToken(in, line);
// 	if(tok == IDENT)
// 	{
// 		//set IDENT lexeme to the type tok value
// 		identstr = tok.GetLexeme();
// 		if (!(defVar.find(identstr)->second))
// 		{
// 			defVar[identstr] = true;
// 		}	
// 		else
// 		{
// 			ParseError(line, "Variable Redefinition");
// 			return false;
// 		}
		
// 	}
// 	else
// 	{
// 		Parser::PushBackToken(tok);
// 		return true;
// 	}
	
// 	tok = Parser::GetNextToken(in, line);
	
// 	if (tok == COMMA) {
// 		status = IdentList(in, line);
// 	}
// 	else if(tok == COLON)
// 	{
// 		Parser::PushBackToken(tok);
// 		return true;
// 	}
// 	else {
// 		ParseError(line, "Unrecognized Input Pattern");
// 		cout << "(" << tok.GetLexeme() << ")" << endl;
// 		return false;
// 	}
// 	return status;
	
// }//End of IdentList
	

//Stmt is either a WriteLnStmt, ForepeatStmt, IfStmt, or AssigStmt
//Stmt = AssigStmt | IfStmt | WriteStmt | ForStmt 
bool Stmt(istream& in, int& line) {
	bool status;
	
	LexItem t = Parser::GetNextToken(in, line);
	
	switch( t.GetToken() ) {

	case WRITELN:
		status = WriteLnStmt(in, line);
		
		break;

	case IF:
		status = IfStmt(in, line);
		break;

	case IDENT:
		Parser::PushBackToken(t);
        status = AssignStmt(in, line);
		
		break;
	default:
		Parser::PushBackToken(t);
		return false;
	}

	return status;
}//End of Stmt

//WriteStmt:= wi, ExpreList 
bool WriteLnStmt(istream& in, int& line) {
	LexItem t;
	//cout << "in WriteStmt" << endl;
	ValQue = new queue<Value>;
	t = Parser::GetNextToken(in, line);
	if( t != LPAREN ) {
		
		ParseError(line, "Missing Left Parenthesis");
		return false;
	}
	
	bool ex = ExprList(in, line);
	
	if( !ex ) {
		ParseError(line, "Missing expression after WriteLn");
		return false;
	}
	
	t = Parser::GetNextToken(in, line);
	if(t != RPAREN ) {
		
		ParseError(line, "Missing Right Parenthesis");
		return false;
	}
	
	
	//Evaluate: print out the list of expressions' values
	while (!(*ValQue).empty())
	{
		Value nextVal = (*ValQue).front();
		cout << nextVal;
		ValQue->pop();
	}
	cout << endl;

	return ex;
}//End of WriteLnStmt
//IfStmt:= if (Expr) then Stm} [Else Stmt]
bool IfStmt(istream& in, int& line) {	
	Value val;
	
	bool ex=false, status; 
	LexItem t;

	//cout << "In If" << endl;
	
	t = Parser::GetNextToken(in, line);
	if( t != LPAREN ) {
		
		ParseError(line, "Missing Left Parenthesis");
		return false;
	}
	
	ex = LogicExpr(in, line, val);
	if( !ex ) {
		ParseError(line, "Missing if statement Logic Expression");
		return false;
	}

	//cout << "Val: " << val << endl;
	
	t = Parser::GetNextToken(in, line);
	if(t != RPAREN ) {
		
		ParseError(line, "Missing Right Parenthesis");
		return false;
	}
	
	t = Parser::GetNextToken(in, line);
	if(t != THEN)
	{
		ParseError(line, "If-Stmt Syntax Error");
		return false;
	}

	if(val.GetBool() == true) {
		status = Stmt(in, line);
		if(!status)
		{
			ParseError(line, "Missing Statement for If-Stmt Then-Part");
			return false;
		}

		//cout << "If is good" << endl;
	} else {
		while(t != ELSE && t != END) {
			t = Parser::GetNextToken(in, line);
		}
		Parser::PushBackToken(t);
	}
	t = Parser::GetNextToken(in, line);
	if( t == ELSE ) {

		if(val.GetBool() == false) {
			status = Stmt(in, line);
			if(!status) {
				ParseError(line, "Missing Statement for If-Stmt Else-Part");
				return false;
			}
		} else {
			while(t != SEMICOL && t != END) {
				t = Parser::GetNextToken(in, line);
			}
			Parser::PushBackToken(t);
		}
		

		//cout << "Else is good" << endl;
		////cout << "in IFStmt status of Stmt true" << endl;
		return true;
	}
		
	Parser::PushBackToken(t);
	return true;
}//End of IfStmt function

//AssignStmt:= Var = Expr
bool AssignStmt(istream& in, int& line) {
	
	bool varstatus = false, status = false;
	LexItem t;
	
	varstatus = Var(in, line, t);

	
	if (varstatus){

		string var = t.GetLexeme();

		t = Parser::GetNextToken(in, line);
		
		if (t == ASSOP){

			Value val;
			status = Expr(in, line, val);
			if(!status) {
				ParseError(line, "Missing Expression in Assignment Statment");
				return status;
			}

			//Evaluate: assign the value of the expression to the variable
			
			if(SymTable[var] == INTEGER && val.IsInt())
			{
				TempsResults[var] = val;
			}
			else if(SymTable[var] == INTEGER && val.IsReal())
			{
				TempsResults[var] = Value(int(val.GetReal()));
			}
			else if(SymTable[var] == REAL && val.IsReal())
			{
				TempsResults[var] = val;
			} else if(SymTable[var] == REAL && val.IsInt())
			{
				TempsResults[var] = Value(float(val.GetInt()));
			} else if(SymTable[var] == STRING && val.IsString())
			{
				TempsResults[var] = val;
			} else{	
				ParseError(line, "Illegal Assignment Operation");
				return false;
			}

			//cout << "Val: " << val << endl;

			//cout << "Ident:" << var << " Value: " << TempsResults[var] << endl;			
		}
		else if(t.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << t.GetLexeme() << ")" << endl;
			return false;
		}
		else {
			ParseError(line, "Missing Assignment Operator");
			return false;
		}
	}
	else {
		ParseError(line, "Missing Left-Hand Side Variable in Assignment statement");
		return false;
	}
	return status;	
}

//Var:= ident
bool Var(istream& in, int& line, LexItem & idtok)
{
	//called only from the AssignStmt function
	string identstr;
	
	LexItem tok = Parser::GetNextToken(in, line);
	
	if (tok == IDENT){
		identstr = tok.GetLexeme();
		
		if (!(defVar.find(identstr)->second))
		{
			ParseError(line, "Undeclared Variable");
			return false;
		}

		idtok = tok;
		//cout << "Ident: " << identstr << endl;
		return true;
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	return false;
}//End of Var


//ExprList:= Expr {,Expr}
bool ExprList(istream& in, int& line) {
	bool status = false;
	Value val;
	status = Expr(in, line, val);
	if(!status){
		ParseError(line, "Missing Expression");
		return false;
	}

	ValQue->push(val);
	
	LexItem tok = Parser::GetNextToken(in, line);
	
	if (tok == COMMA) {
		
		status = ExprList(in, line);
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	else{
		Parser::PushBackToken(tok);
		return true;
	}
	return status;
}

//LogicExpr = Expr (== | <) Expr
bool LogicExpr(istream& in, int& line, Value & retVal)
{
	Value val1, val2;
	bool t1 = Expr(in, line, val1);
	LexItem tok;
	
	if( !t1 ) {
		return false;
	}

	//cout << "Val1: " << val1 << endl;

	tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	if ( tok == GTHAN  || tok == EQUAL  || tok == LTHAN)
	{
		t1 = Expr(in, line, val2);
		if( !t1 ) 
		{
			ParseError(line, "Missing expression after relational operator");
			return false;
		}

		//cout << "Val2: " << val2 << endl;

		if(tok == GTHAN) {
			retVal = (val1 > val2);
		} else if(tok == LTHAN) {
			retVal = (val1 < val2);
		} else {
			retVal = (val1 == val2);
		}

		//cout << "RetVal" << retVal << endl;

		if(retVal.IsErr()) {
			ParseError(line, "Run-Time Error-Illegal Mixed Type Operands for a Logic Expression");
			return false;
		}
		return true;
	}
	Parser::PushBackToken(tok);
	return true;
}


//Expr:= Term {(+|-) Term}
bool Expr(istream& in, int& line, Value & retVal) {
	Value val1, val2;
	////cout << "in Expr" << endl;
	bool t1 = Term(in, line, val1);
	LexItem tok;
	
	if( !t1 ) {
		return false;
	}
	retVal = val1;
	
	tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	//Evaluate: evaluate the expression for addition or subtraction
	while ( tok == PLUS || tok == MINUS ) 
	{
		t1 = Term(in, line, val2);
		if( !t1 ) 
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}
		
		if(tok == PLUS)
		{
			retVal = retVal + val2;
			if(retVal.IsErr())
			{
				ParseError(line, "Illegal addition operation.");
				//cout << "(" << tok.GetLexeme() << ")" << endl;		
				return false;
			}
		}
		else if(tok == MINUS)
		{
			retVal = retVal - val2;
			if(retVal.IsErr())
			{
				ParseError(line, "Illegal subtraction operation.");
				return false;
			}
		}
			
		tok = Parser::GetNextToken(in, line);
		if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}		
		
		
	}
	Parser::PushBackToken(tok);
	return true;
}//end of Expr
//Term:= SFactor {(*|/) SFactor}
bool Term(istream& in, int& line, Value & retVal) {
	Value val1, val2;
	bool t1 = SFactor(in, line, val1);
	
	LexItem tok;
	
	if( !t1 ) {
		return false;
	}

	retVal = val1;
	
	tok	= Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	while ( tok == MULT || tok == DIV  )
	{
		t1 = SFactor(in, line, val2);
		
		if( !t1 ) {
			ParseError(line, "Missing operand after operator");
			return false;
		}

		if(tok == MULT)
		{
			retVal = retVal * val2;
			if(retVal.IsErr())
			{
				ParseError(line, "Illegal multiplication operation.");
				//cout << "(" << tok.GetLexeme() << ")" << endl;		
				return false;
			}
		}
		else if(tok == DIV)
		{

			if(val2.IsInt() && val2.GetInt() == 0)
			{
				ParseError(line, "Run-Time Error-Illegal Division by Zero");
				return false;
			}
			if(val2.IsReal() && val2.GetReal() == 0.0){
				ParseError(line, "Run-Time Error-Illegal Division by Zero");
				return false;
			}
			retVal = retVal / val2;
			if(retVal.IsErr())
			{
				ParseError(line, "Illegal division operation.");
				return false;
			}
		}
		
		tok	= Parser::GetNextToken(in, line);
		if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}
	}
	Parser::PushBackToken(tok);
	return true;
}

//SFactor = Sign Factor | Factor
bool SFactor(istream& in, int& line, Value & retVal)
{
	LexItem t = Parser::GetNextToken(in, line);
	bool status;
	int sign = 0;
	Value val;
	if(t == MINUS )
	{
		sign = -1;
	}
	else if(t == PLUS)
	{
		sign = 1;
	}
	else
		Parser::PushBackToken(t);
		
	status = Factor(in, line, sign, val);
	retVal = val;
	return status;
}


//Factor := ident | iconst | rconst | sconst | (Expr)
bool Factor(istream& in, int& line, int sign, Value & retVal) {
	LexItem tok = Parser::GetNextToken(in, line);
	
	if( tok == SCONST && sign != 0) {
		ParseError(line, "Illegal Operand Type for Sign Operator");
		return false;
	}

	if( tok == SCONST && sign == 0) {
		retVal = tok.GetLexeme();
		return true;
	}


	if(sign == 0) {
		sign = 1;
	}

	if( tok == IDENT ) {
		string lexeme = tok.GetLexeme();
		if (TempsResults.find(lexeme) == TempsResults.end()){
			ParseError(line, "Using Undefined Variable");
			return false;	
		}

		if(SymTable.find(lexeme)->second == STRING){
			retVal = (TempsResults.find(lexeme)->second);
			return true;	
		}

		retVal = (TempsResults.find(lexeme)->second)*sign;

		//cout << "Ident: " << retVal << endl;
		return true;
	}
	else if( tok == ICONST ) {
		retVal = Value(stoi(tok.GetLexeme()) * sign);
		return true;
	}
	else if( tok == RCONST ) {
		retVal = Value(stof(tok.GetLexeme()) * sign);
		return true;
	}
	else if( tok == LPAREN ) {
		bool ex = Expr(in, line, retVal);
		if( !ex ) {
			ParseError(line, "Missing expression after (");
			return false;
		}
		if( Parser::GetNextToken(in, line) == RPAREN )
			return ex;
		else 
		{
			Parser::PushBackToken(tok);
			ParseError(line, "Missing ) after expression");
			return false;
		}
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		//cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}

	

	return false;
}








