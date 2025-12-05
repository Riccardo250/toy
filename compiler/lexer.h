#include<iostream>
#include<string>

enum class Kind : char {
    number,
    name,
    end,
    plus='+',
    equal='=',
    endOfStatement=';',
    lp='(',
    rp=')'
};

struct Token {
    Kind kind;
    std::string name;
    double value;
};

class Token_stream {
    public:
        Token_stream(std::istream& s) : istream{s} {}

        Token get();
        Token& current();
        Token& next();

    private:
        bool first = true;
        std::istream& istream;
        Token currentToken = {Kind::end};
        Token nextToken = {Kind::end};

        Token getInternal();
};

std::ostream& operator<<(std::ostream& strm, const Token& token);