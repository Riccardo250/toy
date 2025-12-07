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
        istream.ignore();
    }

    char firstC = istream.peek();

    if(istream.eof()) {
        return {Kind::end};
    }

    //number
    if(isdigit(firstC)) {
        int num;
        istream>>num;
        return {Kind::number, {}, num};
    }

    //name
    if(isalpha(firstC)) {
        std::string currString = {};
        do {
            currString += istream.get();       
        } while(isalpha(istream.peek()));

        return {Kind::name, currString, 0};
    }

    switch(firstC) {
        case '+':
        case '=':
        case ';':
        case '(':
        case ')':
        case '*':
        case '/':
        case '-':
            return {static_cast<Kind>(istream.get())};
        default:
            Error::lexerError("Invalid token");
    }

    //default to make compiler happy
    return {Kind::end};
}

std::ostream& operator<<(std::ostream& strm, const Token& token) {
    switch(token.kind) {
        case Kind::number:
            strm << "number" << "{" << token.value << "}";
            break;
        case Kind::name:
            strm << "name" << "{" << token.name << "}";
            break;
        case Kind::end:
            strm << "end";
            break;
        default:
            strm << static_cast<char>(token.kind);
    }

    return strm;
}

void Error::lexerError(const std::string& errorMsg) {
    std::cerr << errorMsg << "\n";
}
