#ifndef PARSENODE_H_
#define PARSENODE_H_

#include <iostream>
#include <string>
#include <vector>
#include "polylex.h"
#include <map>

using namespace std;

extern int globalErrorCount;

// objects in the language have one of these types
enum Type {
	INTEGERVAL,
	FLOATVAL,
	STRINGVAL,
	UNKNOWNVAL,
};

// this class will be used in the future to hold results of evaluations
class Value {
	int	i;
	float f;
	string s;
	Type	t;
public:
	Value(int i) : i(i), f(0), t(INTEGERVAL) {}
	Value(float f) : i(0), f(f), t(FLOATVAL) {}
	Value(string s) : i(0), f(0), s(s), t(STRINGVAL) {}

	virtual Type GetType() { return t; }
	virtual int GetIntValue() { return i; }
	virtual float GetFloatValue() { return f; }
	virtual string GetStringValue() { return s; }


};

// every node in the parse tree is going to be a subclass of this node
class ParseNode {
	ParseNode	*left;
	ParseNode	*right;
public:
	ParseNode(ParseNode *left = 0, ParseNode *right = 0) : left(left), right(right) {}
	virtual ~ParseNode() {}

	virtual Type GetType() { return UNKNOWNVAL; }
};

// a list of statements is represented by a statement to the left, and a list of statments to the right
class StatementList : public ParseNode {
public:
	StatementList(ParseNode *l, ParseNode *r) : ParseNode(l,r) {}
};

// a SetStatement represents the idea of setting id to the value of the Expr pointed to by the left node
class SetStatement : public ParseNode {
	string id;
public:
	SetStatement(string id, ParseNode* exp) : id(id), ParseNode(exp) {}
};

// a PrintStatement represents the idea of printing the value of the Expr pointed to by the left node
class PrintStatement : public ParseNode {
public:
	PrintStatement(ParseNode* exp) : ParseNode(exp) {}
};

// represents adding, or subtracting, the two child expressions
class PlusOp : public ParseNode {
public:
	PlusOp(ParseNode *l, ParseNode *r) : ParseNode(l,r) {}
	/*Value Eval(map<string,Value>& symb) {
		Value op1 = left->Eval(symb);
		Value op2 = right->Eval(symb);

		Value sum = op1 + op2;

		if( sum.GetType() == UNKNOWNVAL ) {
			runtimeError(this, "type mismatch in add");
		}
		return sum;
	}*/
};

class MinusOp : public ParseNode {
public:
	MinusOp(ParseNode *l, ParseNode *r) : ParseNode(l,r) {}
	/*Value Eval(map<string,Value>& symb) {
		Value op1 = left->Eval(symb);
		Value op2 = right->Eval(symb);

		Value sub = op1 - op2;

		if( sub.GetType() == UNKNOWNVAL ) {
			runtimeError(this, "type mismatch in add");
		}
		return sub;
	}*/
};
// represents multiplying the two child expressions
class TimesOp : public ParseNode {
public:
	TimesOp(ParseNode *l, ParseNode *r) : ParseNode(l,r) {}
	/*Value Eval(std::map<string,Value>& symb) {
		Value op1 = left->Eval(symb);
		Value op2 = right->Eval(symb);

		Value mul = op1 * op2;

		if( mul.GetType() == UNKNOWNVAL ) {
			runtimeError(this, "type mismatch in add");
		}
		return mul;
	}*/
};

// a representation of a list of coefficients must be developed
class Coefficients : public ParseNode {
	std::vector<ParseNode *> coeff;
public:
	Coefficients(std::vector<ParseNode *> coeff) {}
};

// leaves of the parse tree
// notice that the parent constructors take no arguments
// that means this is a leaf
class Iconst : public ParseNode {
	int	iValue;
public:
	Iconst(int iValue) : iValue(iValue), ParseNode() {}
	Type GetType() { return INTEGERVAL; }
	Value Eval(map<string,Value>& symb) {
		return Value(iValue);
	}
};

class Fconst : public ParseNode {
	float	fValue;
public:
	Fconst(float fValue) : fValue(fValue), ParseNode() {}
	Type GetType() { return FLOATVAL; }
	Value Eval(map<string,Value>& symb) {
		return Value(fValue);
	}
};

class Sconst : public ParseNode {
	string	sValue;
public:
	Sconst(string sValue) : sValue(sValue), ParseNode() {}
	Type GetType() { return STRINGVAL; }
	Value Eval(map<string,Value>& symb) {
		return Value(sValue);
	}
};

class Ident : public ParseNode {
	string	id;
public:
	Ident(string id) : id(id), ParseNode() {}
	Type GetType(); // not known until run time!
};

class EvalAt : public ParseNode {
public:
};

extern ParseNode *Prog(istream& in);
extern ParseNode *Stmt(istream& in);
extern ParseNode *Expr(istream& in);
extern ParseNode *Term(istream& in);
extern ParseNode *Primary(istream& in);
extern ParseNode *Poly(istream& in);
extern ParseNode *Coeffs(istream& in);
extern ParseNode *EvalAt(istream& in);
extern ParseNode *GetOneCoeff(istream& in);

#endif /* PARSENODE_H_ */