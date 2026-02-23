#include <string>
#include <vector>
#include <memory>
#include "lexer.h"

#ifndef AST_H
#define AST_H

class Statement {
    public:
        virtual std::string toJSONString() = 0;
};

class Expr {
    public:
        virtual std::string toJSONString() = 0;
};

class VarDecl{
    public:
        std::string name;
        Type type;
        std::string toJSONString();
};

class AbstractSyntaxtTree {
    public:
        std::vector<std::unique_ptr<Statement>> statementList;
        std::string toJSONString();
};

class VarDeclListStatement : public Statement {
    public:
        std::vector<VarDecl> declList;
        std::string toJSONString() override;
};

class FunDefStatement : public Statement {
    public:
        std::string name;
        Type returnType;
        std::vector<VarDecl> arguments;
        std::vector<std::unique_ptr<Statement>> body;
        std::string toJSONString() override;
};

class IfStatement : public Statement {
    public:
        std::unique_ptr<Expr> condition;
        std::vector<std::unique_ptr<Statement>> thenBody;
        std::vector<std::unique_ptr<Statement>> elseBody;
        std::string toJSONString() override;
};

class WhileStatement : public Statement {
    public:
        std::unique_ptr<Expr> condition;
        std::vector<std::unique_ptr<Statement>> body;
        std::string toJSONString() override;
};

class BreakStatement : public Statement {
    public:
        std::string toJSONString() override;
};

class ReturnStatement : public Statement {
    public:
        std::unique_ptr<Expr> expr;
        std::string toJSONString() override;
};

class ExprStatement : public Statement {
    public:
        std::unique_ptr<Expr> expr;
        std::string toJSONString() override;
};

class AssExpr : public Expr {
    public:
        std::unique_ptr<Expr> name;
        std::unique_ptr<Expr> expr;
        std::string toJSONString() override;
};

class AddOpExpr : public Expr {
    public:
        std::unique_ptr<Expr> a;
        std::unique_ptr<Expr> b;
        std::string toJSONString() override;
};

class SubOpExpr : public Expr {
    public:
        std::unique_ptr<Expr> a;
        std::unique_ptr<Expr> b;
        std::string toJSONString() override;
};

class MulOpExpr : public Expr {
    public:
        std::unique_ptr<Expr> a;
        std::unique_ptr<Expr> b;
        std::string toJSONString() override;
};

class DivOpExpr : public Expr {
    public:
        std::unique_ptr<Expr> a;
        std::unique_ptr<Expr> b;
        std::string toJSONString() override;
};

class NameExpr : public Expr {
    public:
        std::string name;
        NameExpr(std::string name) : name{name} {}
        std::string toJSONString() override;
};

class ConstExpr : public Expr {
    public:
        int number;
        ConstExpr(int number) : number{number} {}
        std::string toJSONString() override;
};

#endif
