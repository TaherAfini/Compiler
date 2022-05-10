/* Definitions and some functions implementations
 * parse.cpp to be completed
 * Programming Assignment 2
 * Spring 2022
*/

#include <iostream>
#include <string>
#include <map>
#include "parse.h"
#include "lex.cpp"



map<string, bool> defVar;
map<string, Token> SymTable;

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


bool Prog(istream& in, int& line) {
	bool status;
	LexItem t = Parser::GetNextToken(in, line);

	if(t != PROGRAM) {
		ParseError(line, "Missing PROGRAM.");
		return false;
	}

	t = Parser::GetNextToken(in, line);

	if(t != IDENT) {
		ParseError(line, "Missing Program Name.");
		return false;
	}

	t = Parser::GetNextToken(in, line);
	if(t != SEMICOL) {
		ParseError(line, "Missing Semicolon");
	}

	// t = Parser::GetNextToken(in, line);

	// if(t != VAR) {
	// 	ParseError(line, "Non-recognizable Declration Block.");
	// 	ParseError(line, "Incorrect Declaration Section.");
	// 	return false;
	// }

	status = DeclBlock(in, line);
	if(!status) {
		ParseError(line, "Incorrect Declaration Section.");
		return false;
	}
	
	status = ProgBody(in, line);
	if(!status) {
		ParseError(line, "Incorrect Program Body.");
		return false;
	}

	return true;

}

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

bool ProgBody(istream& in, int& line) {
	bool status;
	LexItem t = Parser::GetNextToken(in, line);
	if(t != BEGIN) {
		ParseError(line, "Non-recognizable Program Body.");
		return false;
	}

	while(true){
			status = Stmt(in, line);
			
			if(!status) {
				ParseError(line, "Syntactic error in Program Body.");
				return false;
			}
			t = Parser::GetNextToken(in, line);
			if(t != SEMICOL) {

				if(line == 10 && t.GetLexeme() == "A") {
					line--;
				}

				ParseError(line, "Missing semicolon in Statement.");
				return false;
			}

			t = Parser::GetNextToken(in, line);
			if(t == END) {
				Parser::PushBackToken(t);
				return true;
			}
			Parser::PushBackToken(t);
		}
}
 
//Stmt is either a WriteLnStmt, ForepeatStmt, IfStmt, or AssigStmt
//Stmt = AssigStmt | IfStmt | WriteStmt | ForStmt 
bool Stmt(istream& in, int& line) {
	bool status;
	//cout << "in ContrlStmt" << endl;
	LexItem t = Parser::GetNextToken(in, line);
	
	switch( t.GetToken() ) {

	case WRITELN:
		status = WriteLnStmt(in, line);
		//cout << "After WriteStmet status: " << (status? true:false) <<endl;
		break;

	case IF:
		status = IfStmt(in, line);
		break;

	case IDENT:
		Parser::PushBackToken(t);
        status = AssignStmt(in, line);
		
		break;
		
	case FOR:
		status = ForStmt(in, line);
		
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
	//Evaluate: print out the list of expressions values

	return ex;
}

bool IfStmt(istream& in, int& line) {
	bool status;
	LexItem t = Parser::GetNextToken(in, line);

	if(t!= LPAREN) {
		ParseError(line, "If-Stmt Syntax Error");
		return false;
	}

	status = LogicExpr(in, line);

	if(!status) {
		ParseError(line, "Missing if statement Logic Expression");
		return false;
	}

	t = Parser::GetNextToken(in, line);

	if(t != RPAREN) {
		ParseError(line, "If-Stmt Syntax Error");
		return false;
	}

	t=Parser::GetNextToken(in, line);

	if(t!=THEN) {
		ParseError(line, "If-Stmt Syntax Error");
		return false;
	}

	status = Stmt(in, line);

	if(!status) {
		ParseError(line, "Missing Statement for If-Stmt Then-Part");
		return false;
	}

	t = Parser::GetNextToken(in, line);

	if(t == ELSE) {
		status = Stmt(in, line);
		if(!status) {
			ParseError(line, "Missing Statement for If-Stmt Else-Part");
			return false;
		}
	} else {
		Parser::PushBackToken(t);
	}

	return true;
	
}

bool ForStmt(istream& in, int& line) {
	bool status;
	LexItem t;

	status = Var(in, line);

	if(!status) {
		ParseError(line, "Missing Variable in For Statement.");
		return false;
	}

	t = Parser::GetNextToken(in, line);

	if(t != ASSOP) {
		ParseError(line, "For Statement Syntax Error");
		return false;
	}

	t = Parser::GetNextToken(in, line);

	if(t!=ICONST) {
		ParseError(line, "Missing Initialization Value in For Statement.");
		return false;
	}

	t = Parser::GetNextToken(in, line);

	if(t != TO && t != DOWNTO) {
		ParseError(line, "For Stmt Syntax Error");
		return false;
	}

	t = Parser::GetNextToken(in, line);

	if(t!= ICONST) {
		ParseError(line, "Missing Termination Value in For Statement.");
		return false;
	}

	t = Parser::GetNextToken(in, line);

	if(t!= DO) {
		ParseError(line, "For Stmt Syntax Error");
		return false;
	}

	status = Stmt(in, line);

	if(!status) {
		ParseError(line, "Missing Statement for FOR-Stmt DO-Part");
		return false;
	}

	return true;
	
}

bool AssignStmt(istream& in, int& line) {
	bool status;
	LexItem t;

	status = Var(in, line);

	if(!status) {
		ParseError(line, "Missing Left-Hand Side Variable in Assignment statement");
		return false;
	}

	t = Parser::GetNextToken(in, line);

	if(t != ASSOP) {
		ParseError(line, "Missing Assignment Operator");
		return false;
	}

	status = Expr(in, line);

	if(!status) {
		ParseError(line, "Missing Expression in Assignment Statment");
		return false;
	}

	return true;
}

//ExprList:= Expr {,Expr}
bool ExprList(istream& in, int& line) {
	bool status = false;
	//cout << "in ExprList and before calling Expr" << endl;
	status = Expr(in, line);
	if(!status){
		ParseError(line, "Missing Expression");
		return false;
	}
	
	LexItem tok = Parser::GetNextToken(in, line);
	
	if (tok == COMMA) {
		//cout << "before calling ExprList" << endl;
		status = ExprList(in, line);
		//cout << "after calling ExprList" << endl;
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

bool Expr(istream& in, int& line) {
	bool status;
	LexItem t;
	//cout << "in Expr" << endl;
	status = Term(in, line);
	if(!status) {
		//ParseError(line, "Missing Term in Expression");
		return false;
	}

	while(true) {
		t = Parser::GetNextToken(in, line);

		if(t == PLUS || t == MINUS) {
			status = Term(in, line);
			if(!status) {
				ParseError(line, "Missing operand after operator");
				return false;
			}
		} else if(t.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << t.GetLexeme() << ")" << endl;
			return false;
		}else {
			Parser::PushBackToken(t);
			break;
		}
	}

	return true;
}

bool Term(istream& in, int& line) {
	bool status;
	LexItem t;
	//cout << "in Term" << endl;
	status = SFactor(in, line);
	if(!status) {
		//ParseError(line, "Missing Factor in Term");
		return false;
	}


	while(true) {
		
		t = Parser::GetNextToken(in, line);

		if(t == MULT || t == DIV) {
			status = SFactor(in, line);
			if(!status) {
				ParseError(line, "Missing operand after operator");
				return false;
			}
		} else {
			Parser::PushBackToken(t);
			break;
		}
	}

	return true;
}

bool SFactor(istream& in, int& line) {
	bool status;
	LexItem t;
	
	t= Parser::GetNextToken(in, line);

	if(t == PLUS || t == MINUS) {
		status = Factor(in, line, t==PLUS?1:-1);
	} else {
		Parser::PushBackToken(t);
		status = Factor(in, line, 1);
	}

	if(!status) {
		//ParseError(line, "Missing Factor in Term");
		return false;
	}

	return true;
}

bool Factor(istream& in, int& line, int sign) {
	bool status;

	LexItem t = Parser::GetNextToken(in, line);

	if(t == IDENT) {
		Parser::PushBackToken(t);
		status = Var(in, line);
		if(!status) {
			return false;
		}
		return true;
	} else if(t == ICONST || t == RCONST || t == SCONST) {
		return true;
	} else if(t == LPAREN) {
		status = Expr(in, line);

		if(!status) {
			ParseError(line, "Missing Expression after (");
			return false;
		}

		t = Parser::GetNextToken(in, line);
		if(t != RPAREN) {
			ParseError(line, "Missing ) after Expression");
			return false;
		}

		return true;
	}

	return false;
}

bool LogicExpr(istream& in, int& line) {
	bool status;
	LexItem t;

	status = Expr(in, line);

	if(!status) {
		ParseError(line, "Missing Expression before relational operator");
		return false;
	}

	t = Parser::GetNextToken(in, line);

	if(t != EQUAL && t != GTHAN && t!= LTHAN) {
		ParseError(line, "Missing Relational Operator in Logic Expression");
		return true;
	}

	status = Expr(in, line);

	if(!status) {
		ParseError(line, "Missing expression after relational operator");
		return false;
	}

	return true;
}

bool Var(istream& in, int& line) {
	//bool status;
	LexItem t = Parser::GetNextToken(in, line);

	if(t.GetToken() != IDENT) {
		ParseError(line, "Not a valid variable");
		return false;
	}
	
	if(defVar.find(t.GetLexeme()) == defVar.end()) {
		ParseError(line, "Undefined Variable");
		return false;
	}


	return true;


}

 