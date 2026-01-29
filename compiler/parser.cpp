#include "parser.h"
#include "error.h"
#include "ast.h"
#include <vector>

AbstractSyntaxtTree Parser::parse() {
    tokenStream.get();
    return start();
}

AbstractSyntaxtTree Parser::start() {
    AbstractSyntaxtTree ast{};
    std::vector<Statement*> statementList{};

    switch(tokenStream.current().kind) {
        case Kind::lp:
        case Kind::name:
        case Kind::number:
            statList(statementList); break;
        case Kind::end:
            eat(Kind::end); break;
        default:
            Error::parseError("no valid production for start", tokenStream.current());
            return ast;
    }

    ast.statementList = std::move(statementList);
    return ast;
}

void Parser::statList(std::vector<Statement*>& statementList) {
    switch(tokenStream.current().kind) {
        case Kind::lp:
        case Kind::name:
        case Kind::number:
            statementList.push_back(stat());
            statList(statementList);
            return;  
        case Kind::end:
            return;      
        default:
            Error::parseError("no valid production for stat_list", tokenStream.current());
    }
}

Statement* Parser::stat() {

    switch(tokenStream.current().kind) {
        case Kind::lp:
        case Kind::name:
        case Kind::number: {
            ExprStatement* exprStatement = new ExprStatement();
            exprStatement->expr = expr(); 
            eat(Kind::endOfStatement);
            return exprStatement;
        }
        default:
            Error::parseError("no valid production for expr", tokenStream.current());
            return nullptr;
    }
}

Expr* Parser::expr() {
    if(tokenStream.current().kind == Kind::name 
        && tokenStream.next().kind == Kind::equal) {
        return ass();
    } else if(tokenStream.current().kind == Kind::lp 
            || tokenStream.current().kind == Kind::name 
            || tokenStream.current().kind == Kind::number) {
        return op();
    } else {
        Error::parseError("no valid production for expr", tokenStream.current());
        return nullptr;
    }
}

AssExpr* Parser::ass() {
    if(tokenStream.current().kind == Kind::name) {
        AssExpr* assExpr = new AssExpr();
        assExpr->name = new NameExpr{tokenStream.current().name};
        eat(Kind::name);
        eat(Kind::equal);
        assExpr->expr = expr();
        return assExpr;
    } else {
        Error::parseError("no valid production for ass", tokenStream.current());
        return nullptr;
    }
}

Expr* Parser::op() {
    if(tokenStream.current().kind == Kind::lp || tokenStream.current().kind == Kind::name || tokenStream.current().kind == Kind::number) {
        Expr* expr;
        expr = term();
        return addR(expr);
    } else {
        Error::parseError("no valid production for op", tokenStream.current());
        return nullptr;
    }
}

Expr* Parser::addR(Expr * expr) {
    if(tokenStream.current().kind == Kind::plus) {
        AddOpExpr* addOpExpr = new AddOpExpr();
        eat(Kind::plus);
        addOpExpr->a = expr;
        addOpExpr->b = term();
        return addR(addOpExpr);
    } else if(tokenStream.current().kind == Kind::minus) {
        SubOpExpr* subOpExpr = new SubOpExpr();
        eat(Kind::minus);
        subOpExpr->a = expr;
        subOpExpr->b = term();
        return addR(subOpExpr);
    } else if(tokenStream.current().kind == Kind::rp || tokenStream.current().kind == Kind::endOfStatement){
        return expr;
    } else {
        Error::parseError("no valid production for add_r", tokenStream.current());
        return nullptr;
    }
}

Expr* Parser::term() {
    if(tokenStream.current().kind == Kind::lp || tokenStream.current().kind == Kind::name || tokenStream.current().kind == Kind::number) {
        Expr* expr = factor();
        return mulR(expr);;
    } else {
        Error::parseError("no valid production for term", tokenStream.current());
        return nullptr;
    }
}

Expr* Parser::mulR(Expr* expr) {
    if(tokenStream.current().kind == Kind::mul) {
        MulOpExpr * mulOpExpr = new MulOpExpr();
        eat(Kind::mul);
        mulOpExpr->a = expr;
        mulOpExpr->b = term();
        return mulR(mulOpExpr);
    } else if(tokenStream.current().kind == Kind::div) {
        DivOpExpr * divOpExpr = new DivOpExpr();
        eat(Kind::div);
        divOpExpr->a = expr;
        divOpExpr->b = term();
        return mulR(divOpExpr);
    } else if(tokenStream.current().kind == Kind::rp || tokenStream.current().kind == Kind::endOfStatement || tokenStream.current().kind == Kind::plus || tokenStream.current().kind == Kind::minus){
        return expr;
    } else {
        Error::parseError("no valid production for mul_r", tokenStream.current());
        return nullptr;
    }
}

Expr* Parser::factor() {
    if(tokenStream.current().kind == Kind::number) {
        ConstExpr* constExpr =  new ConstExpr(tokenStream.current().value);
        eat(Kind::number); 
        return constExpr;
    } else if (tokenStream.current().kind == Kind::name) {
        NameExpr* constExpr =  new NameExpr(tokenStream.current().name);
        eat(Kind::name);
        return constExpr;
    } else if(tokenStream.current().kind == Kind::lp) {
        eat(Kind::lp); 
        Expr* expr = op(); 
        eat(Kind::rp); 
        return expr;
    } else {
        Error::parseError("no valid production for factor", tokenStream.current());
        return nullptr;
    }
}

void Parser::eat(Kind kind) {
    if(tokenStream.current().kind != kind) Error::parseError("eat error", tokenStream.current());
    tokenStream.get();
}

void Error::parseError(const std::string& errorMsg, const Token& token) {
    std::cerr << "Token: " << token << "\n";
    std::cerr << errorMsg << "\n";
    std::exit(-1);
}
