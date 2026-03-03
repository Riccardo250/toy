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
        void statList(std::vector<std::unique_ptr<Statement>>& statementList);
        void varDeclList(std::vector<VarDecl>& declList);
        void varDeclListR(std::vector<VarDecl>& declList);
        std::vector<VarDecl> funArgumentList();
        std::unique_ptr<IfStatement> ifParser();
        void ifParserR(std::vector<std::unique_ptr<Statement>>& elseBody);
        std::unique_ptr<VarDeclListStatement> varDeclListWrapper();
        std::unique_ptr<Statement> stat();
        std::unique_ptr<Expr> expr(); 
        std::unique_ptr<AssExpr> ass();
        std::unique_ptr<Expr> logicOp();
        std::unique_ptr<Expr> logicOrR(std::unique_ptr<Expr> expr);
        std::unique_ptr<Expr> logicTerm();
        std::unique_ptr<Expr> logicAndR(std::unique_ptr<Expr> expr);
        std::unique_ptr<Expr> logicFactor();
        std::unique_ptr<Expr> bitwiseOrR(std::unique_ptr<Expr> expr);
        std::unique_ptr<Expr> binaryTerm();
        std::unique_ptr<Expr> bitwiseAndR(std::unique_ptr<Expr> expr);
        std::unique_ptr<Expr> comp();
        std::unique_ptr<Expr> compOpR(std::unique_ptr<Expr> expr);
        std::unique_ptr<Expr> op();
        std::unique_ptr<Expr> addR(std::unique_ptr<Expr> expr);
        std::unique_ptr<Expr> term();
        std::unique_ptr<Expr> mulR(std::unique_ptr<Expr> expr);
        std::unique_ptr<Expr> factor();
        void eat(Kind kind);
};

namespace Error {
    void parseError(const std::string& errorMsg, const Token& token);
}