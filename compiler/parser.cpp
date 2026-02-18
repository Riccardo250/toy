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
    std::vector<std::unique_ptr<Statement>> statementList{};

    switch(tokenStream.current().kind) {
        case Kind::lp:
        case Kind::name:
        case Kind::number:
        case Kind::var:
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

void Parser::statList(std::vector<std::unique_ptr<Statement>>& statementList) {
    switch(tokenStream.current().kind) {
        case Kind::lp:
        case Kind::name:
        case Kind::number:
        case Kind::var:
            statementList.push_back(stat());
            statList(statementList);
            return;  
        case Kind::end:
            return;      
        default:
            Error::parseError("no valid production for stat_list", tokenStream.current());
    }
}

std::unique_ptr<Statement> Parser::stat() {

    switch(tokenStream.current().kind) {
        case Kind::lp:
        case Kind::name:
        case Kind::number:
        {
            std::unique_ptr<ExprStatement> exprStatement{new ExprStatement()};
            exprStatement->expr = expr(); 
            eat(Kind::endOfStatement);
            return exprStatement;
        }
        case Kind::var:
        {
            std::unique_ptr<VarDeclListStatement> variableDeclList = varDeclListWrapper();
            eat(Kind::endOfStatement);
            return variableDeclList;
        }
        default:
            Error::parseError("no valid production for stat", tokenStream.current());
            return nullptr;
    }
}

std::unique_ptr<VarDeclListStatement> Parser::varDeclListWrapper() {
    eat(Kind::var);
    std::unique_ptr<VarDeclListStatement> variableDeclList{new VarDeclListStatement{}};
    return variableDeclList;
}

void Parser::varDeclList(std::unique_ptr<VarDeclListStatement>& variableDeclList) {
    std::string name{};
    Type type{};

    if(tokenStream.current().kind == Kind::name) {
        name = tokenStream.current().name;
        eat(Kind::name);
    } else {
        Error::parseError("var_decl_list: expected name", tokenStream.current());
        return;
    }

    eat(Kind::column);

    if(tokenStream.current().kind == Kind::type) {
        type = tokenStream.current().type;
        eat(Kind::type);
    } else {
        Error::parseError("var_decl_list: expected type", tokenStream.current());
        return;
    }
    
    variableDeclList->declList.push_back(VarDecl{name, type});

    varDeclListR(variableDeclList);
}

void Parser::varDeclListR(std::unique_ptr<VarDeclListStatement>& variableDeclList){
    if(tokenStream.current().kind == Kind::comma) {
        eat(Kind::comma);
        varDeclList(variableDeclList);
        return;
    } else if(tokenStream.current().kind == Kind::column) {
        return;
    } else {
        Error::parseError("no valid production for var_decl_list_r", tokenStream.current());
        return;
    }
}

std::unique_ptr<Expr> Parser::expr() {
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

std::unique_ptr<AssExpr> Parser::ass() {
    if(tokenStream.current().kind == Kind::name) {
        std::unique_ptr<AssExpr>  assExpr{new AssExpr()};
        assExpr->name = std::unique_ptr<NameExpr>{new NameExpr{tokenStream.current().name}};
        eat(Kind::name);
        eat(Kind::equal);
        assExpr->expr = expr();
        return assExpr;
    } else {
        Error::parseError("no valid production for ass", tokenStream.current());
        return nullptr;
    }
}

std::unique_ptr<Expr> Parser::op() {
    if(tokenStream.current().kind == Kind::lp || tokenStream.current().kind == Kind::name || tokenStream.current().kind == Kind::number) {
        std::unique_ptr<Expr> expr;
        expr = term();
        return addR(std::move(expr));
    } else {
        Error::parseError("no valid production for op", tokenStream.current());
        return nullptr;
    }
}

std::unique_ptr<Expr> Parser::addR(std::unique_ptr<Expr> expr) {
    if(tokenStream.current().kind == Kind::plus) {
        std::unique_ptr<AddOpExpr> addOpExpr{new AddOpExpr()};
        eat(Kind::plus);
        addOpExpr->a = std::move(expr);
        addOpExpr->b = term();
        return addR(std::move(addOpExpr));
    } else if(tokenStream.current().kind == Kind::minus) {
        std::unique_ptr<SubOpExpr> subOpExpr{new SubOpExpr()};
        eat(Kind::minus);
        subOpExpr->a = std::move(expr);
        subOpExpr->b = term();
        return addR(std::move(subOpExpr));
    } else if(tokenStream.current().kind == Kind::rp || tokenStream.current().kind == Kind::endOfStatement){
        return expr;
    } else {
        Error::parseError("no valid production for add_r", tokenStream.current());
        return nullptr;
    }
}

std::unique_ptr<Expr> Parser::term() {
    if(tokenStream.current().kind == Kind::lp || tokenStream.current().kind == Kind::name || tokenStream.current().kind == Kind::number) {
        std::unique_ptr<Expr> expr = factor();
        return mulR(std::move(expr));
    } else {
        Error::parseError("no valid production for term", tokenStream.current());
        return nullptr;
    }
}

std::unique_ptr<Expr> Parser::mulR(std::unique_ptr<Expr> expr) {
    if(tokenStream.current().kind == Kind::mul) {
        std::unique_ptr<MulOpExpr> mulOpExpr{new MulOpExpr()};
        eat(Kind::mul);
        mulOpExpr->a = std::move(expr);
        mulOpExpr->b = term();
        return mulR(std::move(mulOpExpr));
    } else if(tokenStream.current().kind == Kind::div) {
        std::unique_ptr<DivOpExpr> divOpExpr{new DivOpExpr()};
        eat(Kind::div);
        divOpExpr->a = std::move(expr);
        divOpExpr->b = term();
        return mulR(std::move(divOpExpr));
    } else if(tokenStream.current().kind == Kind::rp || tokenStream.current().kind == Kind::endOfStatement || tokenStream.current().kind == Kind::plus || tokenStream.current().kind == Kind::minus){
        return expr;
    } else {
        Error::parseError("no valid production for mul_r", tokenStream.current());
        return nullptr;
    }
}

std::unique_ptr<Expr> Parser::factor() {
    if(tokenStream.current().kind == Kind::number) {
        std::unique_ptr<ConstExpr> constExpr{new ConstExpr(tokenStream.current().value)};
        eat(Kind::number); 
        return std::move(constExpr);
    } else if (tokenStream.current().kind == Kind::name) {
        std::unique_ptr<NameExpr> constExpr{new NameExpr(tokenStream.current().name)};
        eat(Kind::name);
        return std::move(constExpr);
    } else if(tokenStream.current().kind == Kind::lp) {
        eat(Kind::lp); 
        std::unique_ptr<Expr> expr = op(); 
        eat(Kind::rp); 
        return expr;
    } else {
        Error::parseError("no valid production for factor", tokenStream.current());
        return nullptr;
    }
}

//TODO: add "Expected token  to de message". I would like to create a function to stringify the token and append it to 
// a string
void Parser::eat(Kind kind) {
    if(tokenStream.current().kind != kind) Error::parseError("eat error", tokenStream.current());
    tokenStream.get();
}

void Error::parseError(const std::string& errorMsg, const Token& token) {
    std::cerr << "[*] parse error\n";
    Lexer::printPos = true;
    std::cerr << "Token: " << token << "\n";
    std::cerr << errorMsg << "\n";
    std::exit(-1);
}
