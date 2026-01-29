#include<iostream>
#include<string>


#ifndef LEXER_H
#define LEXER_H

enum class Kind : char {
    number,
    name,
    end,
    function,
    type,
    error,
    plus='+',
    minus='-',
    mul='*',
    div='/',
    equal='=',
    endOfStatement=';',
    lp='(',
    rp=')',
    lcb='{',
    rcb='}'
};

enum class Type : char {
    integer
};


// maybe we can make this a class? And use polymorphism to get the different behaviors for the different kinds?
// but idk how I would implement the function to get the value (we cannot make one volatile function to override)
// cause we couldn't override it properly (different return values type)
struct Token {
    Kind kind{};
    std::string name{};
    int value{};
    Type type{};
    unsigned int totalPos{};
    unsigned int linePos{};
    unsigned int line{};

    Token(Kind kind, int totalPos, int linePos, int line) : kind{kind}, totalPos{totalPos}, line{line} {}
    Token(Kind kind, std::string name, int totalPos, int linePos, int line) : kind{kind}, name{name}, totalPos{totalPos}, line{line} {}
    Token(Kind kind, int value, int totalPos, int linePos, int line) : kind{kind}, value{value}, totalPos{totalPos}, line{line} {}
    Token(Kind kind, Type type, int totalPos, int linePos, int line) : kind{kind}, type{type}, totalPos{totalPos}, line{line} {}

};

class Token_stream {
    public:
        Token_stream(std::istream& s) : istream{s} {}

        Token get();
        Token& current();
        Token& next();

    private:
        bool first = true;
        unsigned int oldTotalPos = 0;
        unsigned int oldLinePos = 0;
        unsigned int currTotalPos = 0;
        unsigned int currLinePos = 0;
        unsigned int line = 0;

        std::istream& istream;
        Token currentToken = {Kind::end, 0, 0, 0};
        Token nextToken = {Kind::end, 0, 0, 0};

        Token getInternal();
        void increaseLine();
        void increasePos(unsigned int n);
};

namespace Lexer {
   extern bool printPos;
}

namespace Error {
    void lexerError(const std::string& errorMsg);
}

std::ostream& operator<<(std::ostream& strm, const Token& token);
#endif