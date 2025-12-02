#include "lexer.h"

Token& Token_stream::current() {
    return currentToken;
}

Token Token_stream::get() {
    while(isspace(istream.peek())) {
        istream.ignore();
    }

    char firstC = istream.peek();

    if(istream.eof()) {
        return currentToken = {Kind::end};
    }

    //number
    if(isdigit(firstC)) {
        double num;
        istream>>num;
        return currentToken = {Kind::number, {}, num};
    }

    //name
    if(isalpha(firstC)) {
        std::string currString = {};
        do {
            currString += istream.get();       
        } while(isalpha(istream.peek()));

        return currentToken = {Kind::name, currString, 0};
    }

    switch(firstC) {
        case '+':
        case '=':
        case ';':
            return currentToken = {static_cast<Kind>(istream.get())};
        default:
            error("Invalid token");
    }

    //default to make compiler happy
    return currentToken = {Kind::end};
}

void Token_stream::error(const std::string& errorMsg) {
    std::cerr << errorMsg << "\n";
    std::exit(-1);
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
