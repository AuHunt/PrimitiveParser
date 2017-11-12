#include "ParseNode.h"
#include <map>
#include "polylex.h"

using namespace std;
// We want to use our getToken routine unchanged... BUT we want to have the ability
// to push back a token if we read one too many; this is our "lookahead"
// so we implement a "wrapper" around getToken

static bool pushedBack = false;
static Token pushedToken;
map <string, bool> idseen;
map <string, ParseNode*> idval;

Token GetToken(istream& in) {cout << "1 = get token" << endl;
	if( pushedBack ) {
		pushedBack = false;
		return pushedToken;
	}
	return getToken(in);
}

void PutBackToken(Token& t) {cout << "2 = putback" << endl;
	if( pushedBack ) {
		cout << "You can only push back one token!" << endl;
		exit(0);
	}
	pushedBack = true;
	pushedToken = t;
}

// handy function to print out errors
void error(string s) {cout << "3 = error" << endl;
	cout << "PARSE ERROR: " << currentLine << " " << s << endl;
	++globalErrorCount;
}

// Prog := Stmt | Stmt Prog
ParseNode *Prog(istream& in) {cout << "4 = ini prog" << endl;
	ParseNode *stmt = Stmt(in);
	if( stmt != 0 )
		return new StatementList(stmt, Prog(in));
	return 0;
}

// Stmt := Set ID Expr SC | PRINT Expr SC
ParseNode *Stmt(istream& in) {cout << "5 = ini parse" << endl;
	Token cmd = GetToken(in);
    cout << cmd.getLexeme() << " - " << cmd.getType() << endl;
	if( cmd == SET ) {cout << "51 = set" << endl;
		Token idTok = GetToken(in);
        cout << idTok.getLexeme() << " - " << idTok.getType() << endl;
		if( idTok != ID ) {
            error("Identifier required after set");
            return 0;
        }
        idseen[ idTok.getLexeme() ] = true;
		ParseNode *exp = Expr(in);
		if( exp == 0 ) {cout << "52 = expr is 0" << endl;
			error("expression required after id in set");
			return 0;
		}
        Token test = GetToken(in);
		if( test != SC ) {cout << 53 << endl;
			error("semicolon required");
			return 0;
		}
		cout << "congratz set" << endl;
        idval[ idTok.getLexeme() ] = exp;
		return new SetStatement(idTok.getLexeme(), exp);
	}
	else if( cmd == PRINT ) {cout << "54 = print" << endl;
		ParseNode *exp = Expr(in);
		if( exp == 0 ) { cout << "55 = print is 0" << endl;
			error("expression required after id in print");
			return 0;
		}
        cout << exp->GetType() << endl;
        Token t = GetToken(in);
		if( t != SC ) { cout << 56 << endl;
            if( t == LSQ) {
                while (GetToken(in) != RSQ){
                    //
                }
                if ( GetToken(in) != SC ) {
                    error("semicolon required");
                    return 0;
                }
                cout << "congratz print" << endl;
                return new PrintStatement(exp);
            }
			error("semicolon required");
			return 0;
		}
		cout << "congratz print" << endl;
		return new PrintStatement(exp);
	}
    else if (cmd == DONE || cmd == ERR) { cout << 57 << endl; return 0; }
	else { cout << 58 << endl; error("Unrecognized first statement"); PutBackToken(cmd); }
	return 0;
}

ParseNode *Expr(istream& in) {cout << "6 = expr" << endl;
    ParseNode *t1 = Term(in);
    if( t1 == 0 ){ return 0; }
    for(;;) {
		Token op = GetToken(in);
        cout << op.getLexeme() << " - " << op.getType() << endl;
		if (op != PLUS && op != MINUS && op != STAR) {
			PutBackToken(op);
			return t1;
		}
		ParseNode *t2 = Expr(in);
		if (t2 == 0) {
			error("expression required after +, -, or * operator");
			exit(0);
		}
		// combine t1 and t2 together
		if (op == PLUS){
			t1 = new PlusOp(t1, t2);
			return t1;
		}
        else if (op == MINUS){
            t1 = new MinusOp(t1, t2);
            return t1;
        }
		else if (op == STAR){
			t1 = new TimesOp(t1, t2);
			return t1;
		}
    }
     // should never get here...
    return 0;
}

ParseNode *Term(istream& in) {cout << "7 = term" << endl;
    Token tokcheck = GetToken(in);
    cout << tokcheck.getLexeme() << " - " << tokcheck.getType() << endl;
    if (tokcheck == ID){
        if (idseen.find(tokcheck.getLexeme()) == idseen.end()){
            error("Identifier not set before printing");
            exit(0);
        }
        else { return idval[ tokcheck.getLexeme() ]; }
    }
    PutBackToken(tokcheck);
	ParseNode *p = Primary(in);
	if ( p == 0 ) { return 0; }
	return p;
}

// Primary :=  ICONST | FCONST | STRING | ( Expr ) | Poly
ParseNode *Primary(istream& in) {cout << "8 = primary" << endl;
	// check tokens... or call Poly
	Token term = GetToken(in);
    cout << term.getLexeme() << " - " << term.getType() << endl;
	if (term == ICONST || term == FCONST) {
		PutBackToken(term);
		ParseNode *cof = GetOneCoeff(in);
		if ( cof == 0 ) { return 0; }
		return cof;
	}
    else if (term == LPAREN) {
        ParseNode *exp = Expr(in);
        if (exp == 0) { return 0; }
        Token par = GetToken(in);
        if (par != RPAREN) {
            error("Missing parenthesis");
            exit(0);
        }
        return exp;
    }
    else if (term == LBR){
        ParseNode *pol = Poly(in);
        Token cbr = GetToken(in);
        if (cbr != RBR) {
            error("Missing curly bracket");
            exit(0);
        }
        return pol;
    }
	return 0;
}

// Poly := LCURLY Coeffs RCURLY { EvalAt } | ID { EvalAt }
ParseNode *Poly(istream& in) {cout << "9 = poly" << endl;
	// note EvalAt is optional
    ParseNode *coe = Coeffs(in);
    if (coe == 0) { return 0; }
    return coe;
}

// notice we don't need a separate rule for ICONST | FCONST
// this rule checks for a list of length at least one
ParseNode *Coeffs(istream& in) {cout << "10 = coeffs" << endl;
	vector <ParseNode *> coeffs;
    ParseNode *p = GetOneCoeff(in);
    if( p == 0 )
        return 0;
    coeffs.push_back(p);
    while( true ) {
        Token t = GetToken(in);
        cout << t.getLexeme() << " - " << t.getType() << endl;
        if( t != COMMA ) {
            PutBackToken(t);
            cout << "boop" << endl;
            break;
        }
        p = GetOneCoeff(in);
        if( p == 0 ) {
            error("Missing coefficient after comma");
            return 0;
        }
        coeffs.push_back(p);
    }
    return new Coefficients(coeffs); //return 0;
}

// EvalAt := LSQ Expr RSQ
ParseNode *EvalAt(istream& in) {cout << "11 = evalat" << endl;
	return 0;
}

ParseNode* GetOneCoeff(istream& in) { cout << "12 = getonecoeff" << endl;
    Token t = GetToken(in);
    cout << t.getLexeme() << " - " << t.getType() << endl;
    if( t == ICONST ) {
        cout << "iconst" << endl;
        return new Iconst(std::stoi(t.getLexeme()));
    }
    else if( t == FCONST ) {
        cout << "fconst" << endl;
        return new Fconst(std::stof(t.getLexeme()));
    }
    else { cout << "rip getonecoeff" << endl; return 0; }
}