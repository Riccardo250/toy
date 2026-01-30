#include <string>
#include <vector>
#include <memory>
#include "lexer.h"

#ifndef AST_H
#define AST_H

class AbstractSyntaxtTree {
    public:
        std::vector<Statement*> statementList;
        std::string toJSONString();
};


class Statement {
    public:
        virtual std::string toJSONString() = 0;
};

class varDeclListStatement : public Statement {
    std::vector<std::unique_ptr<VarDeclStatement>> varDeclList;
};

class VarDeclStatement : public Statement {
    std::string name;
    Type type;
};

class ExprStatement : public Statement {
    public:
        struct Expr* expr;
        std::string toJSONString() override;
};

class Expr {
    public:
        virtual std::string toJSONString() = 0;
};

class AssExpr : public Expr {
    public:
        struct Expr* name;
        struct Expr* expr;
        std::string toJSONString() override;
};

class AddOpExpr : public Expr {
    public:
        struct Expr* a;
        struct Expr* b;
        std::string toJSONString() override;
};

class SubOpExpr : public Expr {
    public:
        struct Expr* a;
        struct Expr* b;
        std::string toJSONString() override;
};

class MulOpExpr : public Expr {
    public:
        struct Expr* a;
        struct Expr* b;
        std::string toJSONString() override;
};

class DivOpExpr : public Expr {
    public:
        struct Expr* a;
        struct Expr* b;
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
