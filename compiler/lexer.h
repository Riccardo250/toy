#include<iostream>
#include<string>

enum class Kind : char {
    number,
    name,
    end,
    plus='+',
    equal='=',
    endOfStatement=';'
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

    private:
        std::istream& istream;
        Token currentToken = {Kind::end};
        void error(const std::string& errorMsg);
};

std::ostream& operator<<(std::ostream& strm, const Token& token);