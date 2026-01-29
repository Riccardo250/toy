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

    //name
    if(isalpha(firstC)) {
        std::string currString = {};
        do {
            currString += istream.get();      
        } while(isalpha(istream.peek()));

        increasePos(currString.length());

        //types
        if(currString == "int")
            return {Kind::type, Type::integer, oldTotalPos, oldLinePos, line}; 
        
        if(currString == "function")
            return {Kind::function, oldTotalPos, oldLinePos, line};

        return {Kind::name, currString, oldTotalPos, oldLinePos, line};
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
            strm << "number" << "{" << token.value << "}";
            break;
        case Kind::name:
            strm << "name" << "{" << token.name << "}";
            break;
        case Kind::end:
            strm << "end";
            break;
        case Kind::function:
            strm << "function";
            break;
        default:
            strm << static_cast<char>(token.kind);
    }

    if(Lexer::printPos) {
        strm << "[" << "l:" << token.line << ", lp;" << token.linePos << ", tp:" << token.totalPos << "]";
    }

    return strm;
}

bool Lexer::printPos = false;

void Error::lexerError(const std::string& errorMsg) {
    std::cerr << errorMsg << "\n";
}
