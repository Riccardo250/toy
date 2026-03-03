#include "lexer.h"
#include "error.h"

Token& Token_stream::current() {
    return currentToken;
}

Token& Token_stream::next() {
    return nextToken;
}

Token Token_stream::get() {
    if(first) {
        currentToken = getInternal();
        nextToken = getInternal();
        first = false;
        return currentToken;
    } else {
        currentToken = nextToken;
        nextToken = getInternal();
        return currentToken;
    }
}

Token Token_stream::getInternal() {
    while(isspace(istream.peek())) {
        if(istream.peek() == '\n')
            increaseLine();

        increasePos(1);
        istream.ignore();
    }

    char firstC = istream.peek();

    if(istream.eof()) {
        return {Kind::end, oldTotalPos, oldLinePos, line};
    }

    //number
    if(isdigit(firstC)) {
        int num;
        istream>>num; //in this way it is hard to count the number of read characters

        // TODO: gcount returns type std::streamsize, can it be sagfely converted to unsigned int?
        increasePos(istream.gcount());
        return {Kind::number, num, oldTotalPos, oldLinePos, line};
    }

    //string
    if(isalpha(firstC)) {
        std::string currString = {};
        do {
            currString += istream.get();      
        } while(isalpha(istream.peek()));

        increasePos(currString.length());

        //types
        if(currString == "int") {
            return {Kind::type, Type::integer, oldTotalPos, oldLinePos, line}; 
        } else if(currString == "function") {
            return {Kind::function, oldTotalPos, oldLinePos, line};
        } else if (currString == "var") {
            return {Kind::var, oldTotalPos, oldLinePos, line};
        } else if (currString == "if") {
            return {Kind::ifKind, oldTotalPos, oldLinePos, line};
        } else if (currString == "else") {
            return {Kind::elseKind, oldTotalPos, oldLinePos, line};
        } else if (currString == "while") {
            return {Kind::whileKind, oldTotalPos, oldLinePos, line};
        } else if (currString == "break") {
            return {Kind::breakKind, oldTotalPos, oldLinePos, line};
        } else if (currString == "return") {
            return {Kind::returnKind, oldTotalPos, oldLinePos, line};
        } 
        return {Kind::name, currString, oldTotalPos, oldLinePos, line};
    }

    switch(firstC) {
        case '=':
            istream.ignore();
            if(istream.peek() == '=') {
                istream.ignore();
                increasePos(2);
                return {Kind::equal, oldTotalPos, oldLinePos, line};
            } else {
                increasePos(1);
                return {Kind::assign, oldTotalPos, oldLinePos, line};
            }
        case '!':
            istream.ignore();
            if(istream.peek() == '=') {
                istream.ignore();
                increasePos(2);
                return {Kind::notEqual, oldTotalPos, oldLinePos, line};
            } else {
                increasePos(1);
                return {Kind::notKind, oldTotalPos, oldLinePos, line};
            }
        case '>':
            istream.ignore();
            if(istream.peek() == '=') {
                istream.ignore();
                increasePos(2);
                return {Kind::greaterEqual, oldTotalPos, oldLinePos, line};
            } else {
                increasePos(1);
                return {Kind::greater, oldTotalPos, oldLinePos, line};
            }
        case '<':
            istream.ignore();
            if(istream.peek() == '=') {
                istream.ignore();
                increasePos(2);
                return {Kind::lessEqual, oldTotalPos, oldLinePos, line};
            } else {
                increasePos(1);
                return {Kind::less, oldTotalPos, oldLinePos, line};
            }
        case '|':
            istream.ignore();
            if(istream.peek() == '|') {
                istream.ignore();
                increasePos(2);
                return {Kind::orKind, oldTotalPos, oldLinePos, line};
            } else {
                increasePos(1);
                return {Kind::bitwiseOr, oldTotalPos, oldLinePos, line};
            }
        case '&':
            istream.ignore();
            if(istream.peek() == '&') {
                istream.ignore();
                increasePos(2);
                return {Kind::andKind, oldTotalPos, oldLinePos, line};
            } else {
                increasePos(1);
                return {Kind::bitwiseAnd, oldTotalPos, oldLinePos, line};
            }
        case '+':
        case ',':
        case ':':
        case ';':
        case '(':
        case ')':
        case '*':
        case '/':
        case '-':
        case '{':
        case '}':
            increasePos(1);
            return {static_cast<Kind>(istream.get()), oldTotalPos, oldLinePos, line};
        default:
            Error::lexerError("Invalid token");
    }

    //default to make compiler happy
    return {Kind::error, oldTotalPos, oldLinePos, line};
}

void Token_stream::increaseLine() {
    currLinePos = 0;
    line += 1;
}

void Token_stream::increasePos(unsigned int n) {
    oldLinePos = currLinePos;
    oldTotalPos = currTotalPos;
    currLinePos += n;
    currTotalPos += n;
}

std::ostream& operator<<(std::ostream& strm, const Token& token) {
    switch(token.kind) {
        case Kind::number:
            strm << kindToString(token.kind) << "{" << token.value << "}";
            break;
        case Kind::name:
            strm << kindToString(token.kind) << "{" << token.name << "}";
            break;
        default:
            strm << kindToString(token.kind);
    }

    if(Lexer::printPos) {
        strm << "[" << "l:" << token.line << ", lp;" << token.linePos << ", tp:" << token.totalPos << "]";
    }

    return strm;
}

std::string kindToString(Kind kind) {
    switch(kind) {
        case Kind::number:
            return "number";
        case Kind::name:
            return "name";
        case Kind::end:
            return "end";
        case Kind::var:
            return "var";
        case Kind::function:
            return "function";
        case Kind::type:
            return "type";
        case Kind::ifKind:
            return "if";
        case Kind::elseKind:
            return "else";
        case Kind::whileKind:
            return "while";
        case Kind::error:
            return "error";
        case Kind::equal:
            return "==";
        case Kind::notEqual:
            return "!=";
        case Kind::greaterEqual:
            return ">=";
        case Kind::lessEqual:
            return "<=";
        case Kind::orKind:
            return "||";
        case Kind::andKind:
            return "&&";
        case Kind::notKind:
            return "!";
        case Kind::greater:
            return ">";
        case Kind::bitwiseOr:
            return "|";
        case Kind::bitwiseAnd:
            return "&";
        case Kind::less:
            return "<";
        case Kind::plus:
            return "+";
        case Kind::minus:
            return "-";
        case Kind::mul:
            return "*";
        case Kind::div:
            return "/";
        case Kind::assign:
            return "=";
        case Kind::column:
            return ":";
        case Kind::comma:
            return ",";
        case Kind::endOfStatement:
            return ";";
        case Kind::lp:
            return "(";
        case Kind::rp:
            return ")";
        case Kind::lcb:
            return "{";                                                                                                
        case Kind::rcb:
            return "}"; 
        default:
            return "unknown_kind"; 
    }
}

std::string operator+(std::string string, Kind kind) {
    return string + kindToString(kind);
}

bool Lexer::printPos = false;
std::string Lexer::typeToString(Type type) {
    switch(type) {
        case Type::integer:
            return "integer";
        default:
            return "unknown_type";
    }
}

void Error::lexerError(const std::string& errorMsg) {
    std::cerr << errorMsg << "\n";
}
