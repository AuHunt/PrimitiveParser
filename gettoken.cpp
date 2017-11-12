#include "polylex.h"
#include <iostream>
#include <cctype>
#include <map>

std::map<char,TokenTypes> oneCharToken =
{
		{'+',PLUS},
		{'*',STAR},
		{',',COMMA},
		{'{',LBR},
		{'}',RBR},
		{'[',LSQ},
		{']',RSQ},
		{'(',LPAREN},
		{')',RPAREN},
		{';',SC},
};


Token getToken(std::istream& in)
{
	enum State { START, INID, INSTRING, INICONST, INFCONST, INCOMMENT };

	State lexstate = START;
	std::string lexeme = "";

	for(;;) {
		char ch = in.get();

		if( in.eof() || in.bad() )
			break;

		if( ch == '\n' && lexstate == START ) currentLine++;

		switch( lexstate ) {
		case START:
			if( isspace(ch) )
				break;

			lexeme = ch;
			if( isalpha(ch) ) {
				lexstate = INID;
			}
			else if( isdigit(ch) ) {
				lexstate = INICONST;
			}
			else if( ch == '#' ) {
				lexstate = INCOMMENT;
			}
			else if( ch == '"' ) {
				lexstate = INSTRING;
			}
			else if( ch == '-' ) {
				// maybe minus? maybe leading sign on a number?
				if( isdigit(in.peek()) ) {
					lexstate = INICONST;
				}
				else return Token(MINUS);
			}
			else {
				auto chi = oneCharToken.find(ch);
				if( chi != oneCharToken.end() )
					return chi->second;
				return Token(ERR,lexeme);
			}

			break;

		case INID:
			if( !isalnum(ch) ) {
				in.putback(ch);
				if (lexeme == "set"){ return Token(SET, lexeme); }
				else if (lexeme == "print") { return Token(PRINT, lexeme); }
				return Token(ID, lexeme);
			}
			lexeme += ch;
			break;

		case INSTRING:
			if( ch == '"' ) {
				lexeme += ch;
				return Token(STRING, lexeme);
			}
			else if( ch == '\n' ) {
				return Token(ERR, lexeme);
			}
			lexeme += ch;
			break;

		case INICONST:
			if( isdigit(ch) ) {
				lexeme += ch;
			}
			else if( ch == '.' ) {
				lexeme += ch;
				if( isdigit(in.peek()) )
					lexstate = INFCONST;
				else
					return Token(ERR, lexeme);
			}
			else {
				in.putback(ch);
				return Token(ICONST, lexeme);
			}
			break;

		case INFCONST:
			if( isdigit(ch) ) {
				lexeme += ch;
			}
			else {
				in.putback(ch);
				return Token(FCONST, lexeme);
			}
			break;

		case INCOMMENT:
			if( ch == '\n' ) {
                currentLine++;
				lexstate = START;
			}
			break;
		}
	}

	// handle getting DONE or ERR when not in start state
	if( lexstate == START ) {
		if( in.eof() ) return DONE;
		else return Token(ERR, lexeme);
	}

	return ERR;
}
