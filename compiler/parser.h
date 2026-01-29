#include "lexer.h"
#include "ast.h"
#include <vector>

class Parser {
    public:
        Parser(std::istream& s) : tokenStream{s} {}
        AbstractSyntaxtTree parse();
    private:
        Token_stream tokenStream;

        //productions
        AbstractSyntaxtTree start();
        void statList(std::vector<Statement*>& statementList);
        Statement* stat();
        Expr* expr();
        AssExpr* ass();
        Expr* op();
        Expr* addR(Expr * expr);
        Expr* term();
        Expr* mulR(Expr* expr);
        Expr* factor();
        void eat(Kind kind);
};

namespace Error {
    void parseError(const std::string& errorMsg, const Token& token);
}