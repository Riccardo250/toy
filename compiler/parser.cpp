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
        case Kind::function:
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
        case Kind::function:
        case Kind::ifKind:
        case Kind::whileKind:
            statementList.push_back(stat());
            statList(statementList);
            return;  
        case Kind::end:
        case Kind::rcb:
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
        case Kind::function:
        {
            std::unique_ptr<FunDefStatement> funDef{new FunDefStatement()};
            eat(Kind::function);

            if(tokenStream.current().kind == Kind::name) {
                funDef->name = tokenStream.current().name;
                eat(Kind::name);
            } else {
                Error::parseError("var_decl_list: expected name", tokenStream.current());
                return nullptr;
            }

            eat(Kind::lp);
            funDef->arguments = funArgumentList();
            eat(Kind::rp);

            eat(Kind::column);

            if(tokenStream.current().kind == Kind::type) {
                funDef->returnType = tokenStream.current().type;
                eat(Kind::type);
            } else {
                Error::parseError("var_decl_list: expected type", tokenStream.current());
                return nullptr;
            }

            eat(Kind::lcb);
            statList(funDef->body);
            eat(Kind::rcb);

            return funDef;
        }
        case Kind::ifKind:
        {
            return ifParser();
        }

        case Kind::whileKind:
        {
            std::unique_ptr<WhileStatement> whileStatement{new WhileStatement()};

            eat(Kind::whileKind);
            eat(Kind::lp);
            whileStatement->condition = expr();
            eat(Kind::rp);

            eat(Kind::lcb);
            statList(whileStatement->body);
            eat(Kind::rcb);

            return whileStatement;
        }
        default:
            Error::parseError("no valid production for stat", tokenStream.current());
            return nullptr;
    }
}

std::unique_ptr<VarDeclListStatement> Parser::varDeclListWrapper() {
    eat(Kind::var);
    std::unique_ptr<VarDeclListStatement> variableDeclList{new VarDeclListStatement{}};
    varDeclList(variableDeclList->declList);
    return variableDeclList;
}

void Parser::varDeclList(std::vector<VarDecl>& declList) {
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
    
    declList.push_back(VarDecl{name, type});

    varDeclListR(declList);
}

void Parser::varDeclListR(std::vector<VarDecl>& declList){
    if(tokenStream.current().kind == Kind::comma) {
        eat(Kind::comma);
        varDeclList(declList);
        return;
    } else if(tokenStream.current().kind == Kind::endOfStatement || tokenStream.current().kind == Kind::rp) {
        return;
    } else {
        Error::parseError("no valid production for var_decl_list_r, expected a , a ; or a )", tokenStream.current());
        return;
    }
}

std::vector<VarDecl> Parser::funArgumentList() {
    std::vector<VarDecl> arguments{};

    if(tokenStream.current().kind == Kind::name) {
        varDeclList(arguments);
        return arguments;
    } else if (tokenStream.current().kind == Kind::rp) {
        return arguments;
    } else {
        Error::parseError("no valid production for fun_argument_list, expected name or a )", tokenStream.current());
        return arguments;
    }
}

std::unique_ptr<IfStatement> Parser::ifParser() {
    std::unique_ptr<IfStatement> ifStatement{new IfStatement()};

    eat(Kind::ifKind);
    eat(Kind::lp);
    ifStatement->condition = expr();
    eat(Kind::rp);

    eat(Kind::lcb);
    statList(ifStatement->thenBody);
    eat(Kind::rcb);

    ifParserR(ifStatement->elseBody);

    return ifStatement;
}

void Parser::ifParserR(std::vector<std::unique_ptr<Statement>>& elseBody) {
    if(tokenStream.current().kind == Kind::elseKind && tokenStream.next().kind == Kind::ifKind) {
        eat(Kind::elseKind);
        elseBody.push_back(ifParser());
        return;
    } else if(tokenStream.current().kind == Kind::elseKind) {
        eat(Kind::elseKind);
        eat(Kind::lcb);
        statList(elseBody);
        eat(Kind::rcb);
        return;
    } else if(tokenStream.current().kind == Kind::lp
            || tokenStream.current().kind == Kind::end
            || tokenStream.current().kind == Kind::function
            || tokenStream.current().kind == Kind::ifKind
            || tokenStream.current().kind == Kind::name
            || tokenStream.current().kind == Kind::number
            || tokenStream.current().kind == Kind::var
            || tokenStream.current().kind == Kind::rcb
            || tokenStream.current().kind == Kind::whileKind) {
        return;
    } else {
        Error::parseError("no valid production for if_stat_r, expected else, else if, ( end function if name number var while }", tokenStream.current());
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

void Parser::eat(Kind kind) {
    std::cout << "Eating: " + kind + "\n";
    if(tokenStream.current().kind != kind) Error::parseError("eat error, expected: " + kind, tokenStream.current());
    tokenStream.get();
}

void Error::parseError(const std::string& errorMsg, const Token& token) {
    std::cerr << "[*] parse error\n";
    Lexer::printPos = true;
    std::cerr << "Token: " << token << "\n";
    std::cerr << errorMsg << "\n";
    std::exit(-1);
}
