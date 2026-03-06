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
        case Kind::breakKind:
        case Kind::returnKind:
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
        case Kind::breakKind:
        {
            eat(Kind::breakKind);
            eat(Kind::endOfStatement);
            return std::unique_ptr<BreakStatement>{new BreakStatement()};
        }
        case Kind::returnKind:
        {
            eat(Kind::returnKind);
            std::unique_ptr<ReturnStatement> returnStatement{new ReturnStatement()};
            returnStatement->expr = expr();
            eat(Kind::endOfStatement);
            return returnStatement;
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
            || tokenStream.current().kind == Kind::whileKind
            || tokenStream.current().kind == Kind::breakKind
            || tokenStream.current().kind == Kind::returnKind) {
        return;
    } else {
        Error::parseError("no valid production for if_stat_r, expected else, else if, ( end function if name number var while }", tokenStream.current());
        return;
    }
}

std::unique_ptr<Expr> Parser::expr() {
    if(tokenStream.current().kind == Kind::name 
        && tokenStream.next().kind == Kind::assign) {
        return ass();
    } else if(tokenStream.current().kind == Kind::lp 
            || tokenStream.current().kind == Kind::name 
            || tokenStream.current().kind == Kind::number) {
        return logicOp();
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
        eat(Kind::assign);
        assExpr->expr = expr();
        return assExpr;
    } else {
        Error::parseError("no valid production for ass", tokenStream.current());
        return nullptr;
    }
}

std::unique_ptr<Expr> Parser::logicOp() {
    if(tokenStream.current().kind == Kind::lp || tokenStream.current().kind == Kind::name || tokenStream.current().kind == Kind::number) {
        return logicOrR(logicTerm());
    } else {
        Error::parseError("no valid production for logic_op", tokenStream.current());
        return nullptr;
    }
}

std::unique_ptr<Expr> Parser::logicOrR(std::unique_ptr<Expr> expr) {
    if(tokenStream.current().kind == Kind::orKind) {
        std::unique_ptr<BinaryOpExpr> binaryOpExpr{new BinaryOpExpr()};
        eat(Kind::orKind);
        binaryOpExpr->a = std::move(expr);
        binaryOpExpr->b = logicTerm();
        binaryOpExpr->op = BinaryOpType::logicOr;
        return logicOrR(std::move(binaryOpExpr));
    } else if(tokenStream.current().kind == Kind::rp 
            || tokenStream.current().kind == Kind::endOfStatement){
        return expr;
    } else {
        Error::parseError("no valid production for logic_or_r", tokenStream.current());
        return nullptr;
    }
}

std::unique_ptr<Expr> Parser::logicTerm() {
    if(tokenStream.current().kind == Kind::lp || tokenStream.current().kind == Kind::name || tokenStream.current().kind == Kind::number) {
        return logicAndR(comp());
    } else {
        Error::parseError("no valid production for logic_term", tokenStream.current());
        return nullptr;
    }
}

std::unique_ptr<Expr> Parser::logicAndR(std::unique_ptr<Expr> expr) {
    if(tokenStream.current().kind == Kind::andKind) {
        std::unique_ptr<BinaryOpExpr> binaryOpExpr{new BinaryOpExpr()};
        eat(Kind::andKind);
        binaryOpExpr->a = std::move(expr);
        binaryOpExpr->b = comp();
        binaryOpExpr->op = BinaryOpType::logicAnd;
        return logicAndR(std::move(binaryOpExpr));
    } else if(tokenStream.current().kind == Kind::rp 
            || tokenStream.current().kind == Kind::endOfStatement
            || tokenStream.current().kind == Kind::orKind){
        return expr;
    } else {
        Error::parseError("no valid production for logic_and_r", tokenStream.current());
        return nullptr;
    }
}

std::unique_ptr<Expr> Parser::logicFactor() {
    if(tokenStream.current().kind == Kind::lp || tokenStream.current().kind == Kind::name || tokenStream.current().kind == Kind::number) {
        return bitwiseOrR(binaryTerm());
    } else {
        Error::parseError("no valid production for logic_factor", tokenStream.current());
        return nullptr;
    }
}

std::unique_ptr<Expr> Parser::bitwiseOrR(std::unique_ptr<Expr> expr) {
    if(tokenStream.current().kind == Kind::bitwiseOr) {
        std::unique_ptr<BinaryOpExpr> binaryOpExpr{new BinaryOpExpr()};
        eat(Kind::bitwiseOr);
        binaryOpExpr->a = std::move(expr);
        binaryOpExpr->b = binaryTerm();
        binaryOpExpr->op = BinaryOpType::bitwiseOr;
        return bitwiseOrR(std::move(binaryOpExpr));
    } else if(tokenStream.current().kind == Kind::rp 
            || tokenStream.current().kind == Kind::endOfStatement
            || tokenStream.current().kind == Kind::orKind
            || tokenStream.current().kind == Kind::andKind
            || tokenStream.current().kind == Kind::equal
            || tokenStream.current().kind == Kind::notEqual
            || tokenStream.current().kind == Kind::greaterEqual
            || tokenStream.current().kind == Kind::lessEqual
            || tokenStream.current().kind == Kind::greater
            || tokenStream.current().kind == Kind::less){
        return expr;
    } else {
        Error::parseError("no valid production for bitwise_or_r", tokenStream.current());
        return nullptr;
    }
}

std::unique_ptr<Expr> Parser::binaryTerm() {
    if(tokenStream.current().kind == Kind::lp || tokenStream.current().kind == Kind::name || tokenStream.current().kind == Kind::number) {
        return bitwiseAndR(op());
    } else {
        Error::parseError("no valid production for binary_term", tokenStream.current());
        return nullptr;
    }
}

std::unique_ptr<Expr> Parser::bitwiseAndR(std::unique_ptr<Expr> expr) {
    if(tokenStream.current().kind == Kind::bitwiseAnd) {
        std::unique_ptr<BinaryOpExpr> binaryOpExpr{new BinaryOpExpr()};
        eat(Kind::bitwiseAnd);
        binaryOpExpr->a = std::move(expr);
        binaryOpExpr->b = op();
        binaryOpExpr->op = BinaryOpType::bitwiseAnd;
        return bitwiseAndR(std::move(binaryOpExpr));
    } else if(tokenStream.current().kind == Kind::rp 
            || tokenStream.current().kind == Kind::endOfStatement
            || tokenStream.current().kind == Kind::orKind
            || tokenStream.current().kind == Kind::andKind
            || tokenStream.current().kind == Kind::bitwiseOr
            || tokenStream.current().kind == Kind::equal
            || tokenStream.current().kind == Kind::notEqual
            || tokenStream.current().kind == Kind::greaterEqual
            || tokenStream.current().kind == Kind::lessEqual
            || tokenStream.current().kind == Kind::greater
            || tokenStream.current().kind == Kind::less){
        return expr;
    } else {
        Error::parseError("no valid production for bitwise_and_r", tokenStream.current());
        return nullptr;
    }
}

std::unique_ptr<Expr> Parser::comp() {
    if(tokenStream.current().kind == Kind::lp || tokenStream.current().kind == Kind::name || tokenStream.current().kind == Kind::number) {
        return compOpR(logicFactor());
    } else {
        Error::parseError("no valid production for binary_term", tokenStream.current());
        return nullptr;
    }
}

std::unique_ptr<Expr> Parser::compOpR(std::unique_ptr<Expr> expr) {
    if(tokenStream.current().kind == Kind::equal) {
        std::unique_ptr<BinaryOpExpr> binaryOpExpr{new BinaryOpExpr()};
        eat(Kind::equal);
        binaryOpExpr->a = std::move(expr);
        binaryOpExpr->b = logicFactor();
        binaryOpExpr->op = BinaryOpType::equal;
        return compOpR(std::move(binaryOpExpr));
    } else if(tokenStream.current().kind == Kind::notEqual) {
        std::unique_ptr<BinaryOpExpr> binaryOpExpr{new BinaryOpExpr()};
        eat(Kind::notEqual);
        binaryOpExpr->a = std::move(expr);
        binaryOpExpr->b = logicFactor();
        binaryOpExpr->op = BinaryOpType::notEqual;
        return compOpR(std::move(binaryOpExpr));
    } else if(tokenStream.current().kind == Kind::greaterEqual) {
        std::unique_ptr<BinaryOpExpr> binaryOpExpr{new BinaryOpExpr()};
        eat(Kind::greaterEqual);
        binaryOpExpr->a = std::move(expr);
        binaryOpExpr->b = logicFactor();
        binaryOpExpr->op = BinaryOpType::greaterEqual;
        return compOpR(std::move(binaryOpExpr));
    } else if(tokenStream.current().kind == Kind::lessEqual) {
        std::unique_ptr<BinaryOpExpr> binaryOpExpr{new BinaryOpExpr()};
        eat(Kind::lessEqual);
        binaryOpExpr->a = std::move(expr);
        binaryOpExpr->b = logicFactor();
        binaryOpExpr->op = BinaryOpType::lessEqual;
        return compOpR(std::move(binaryOpExpr));
    } else if(tokenStream.current().kind == Kind::less) {
        std::unique_ptr<BinaryOpExpr> binaryOpExpr{new BinaryOpExpr()};
        eat(Kind::less);
        binaryOpExpr->a = std::move(expr);
        binaryOpExpr->b = logicFactor();
        binaryOpExpr->op = BinaryOpType::less;
        return compOpR(std::move(binaryOpExpr));
    } else if(tokenStream.current().kind == Kind::greater) {
        std::unique_ptr<BinaryOpExpr> binaryOpExpr{new BinaryOpExpr()};
        eat(Kind::greater);
        binaryOpExpr->a = std::move(expr);
        binaryOpExpr->b = logicFactor();
        binaryOpExpr->op = BinaryOpType::greater;
        return compOpR(std::move(binaryOpExpr));
    } else if(tokenStream.current().kind == Kind::rp 
            || tokenStream.current().kind == Kind::endOfStatement
            || tokenStream.current().kind == Kind::orKind
            || tokenStream.current().kind == Kind::andKind){
        return expr;
    } else {
        Error::parseError("no valid production for comp_op_r", tokenStream.current());
        return nullptr;
    }
}

std::unique_ptr<Expr> Parser::op() {
    if(tokenStream.current().kind == Kind::lp || tokenStream.current().kind == Kind::name || tokenStream.current().kind == Kind::number) {
        return addR(term());
    } else {
        Error::parseError("no valid production for op", tokenStream.current());
        return nullptr;
    }
}

std::unique_ptr<Expr> Parser::addR(std::unique_ptr<Expr> expr) {
    if(tokenStream.current().kind == Kind::plus) {
        std::unique_ptr<BinaryOpExpr> binaryOpExpr{new BinaryOpExpr()};
        eat(Kind::plus);
        binaryOpExpr->a = std::move(expr);
        binaryOpExpr->b = term();
        binaryOpExpr->op = BinaryOpType::add;
        return addR(std::move(binaryOpExpr));
    } else if(tokenStream.current().kind == Kind::minus) {
        std::unique_ptr<BinaryOpExpr> binaryOpExpr{new BinaryOpExpr()};
        eat(Kind::minus);
        binaryOpExpr->a = std::move(expr);
        binaryOpExpr->b = term();
        binaryOpExpr->op = BinaryOpType::sub;
        return addR(std::move(binaryOpExpr));
    } else if(tokenStream.current().kind == Kind::rp 
            || tokenStream.current().kind == Kind::endOfStatement
            || tokenStream.current().kind == Kind::orKind
            || tokenStream.current().kind == Kind::andKind
            || tokenStream.current().kind == Kind::bitwiseOr
            || tokenStream.current().kind == Kind::bitwiseAnd
            || tokenStream.current().kind == Kind::equal
            || tokenStream.current().kind == Kind::notEqual
            || tokenStream.current().kind == Kind::greaterEqual
            || tokenStream.current().kind == Kind::lessEqual
            || tokenStream.current().kind == Kind::greater
            || tokenStream.current().kind == Kind::less
        ){
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
        std::unique_ptr<BinaryOpExpr> binaryOpExpr{new BinaryOpExpr()};
        eat(Kind::mul);
        binaryOpExpr->a = std::move(expr);
        binaryOpExpr->b = factor();
        binaryOpExpr->op = BinaryOpType::mul;
        return mulR(std::move(binaryOpExpr));
    } else if(tokenStream.current().kind == Kind::div) {
        std::unique_ptr<BinaryOpExpr> binaryOpExpr{new BinaryOpExpr()};
        eat(Kind::div);
        binaryOpExpr->a = std::move(expr);
        binaryOpExpr->b = factor();
        binaryOpExpr->op = BinaryOpType::div;
        return mulR(std::move(binaryOpExpr));
    } else if(tokenStream.current().kind == Kind::rp 
            || tokenStream.current().kind == Kind::endOfStatement
            || tokenStream.current().kind == Kind::orKind
            || tokenStream.current().kind == Kind::andKind
            || tokenStream.current().kind == Kind::bitwiseOr
            || tokenStream.current().kind == Kind::bitwiseAnd
            || tokenStream.current().kind == Kind::equal
            || tokenStream.current().kind == Kind::notEqual
            || tokenStream.current().kind == Kind::greaterEqual
            || tokenStream.current().kind == Kind::lessEqual
            || tokenStream.current().kind == Kind::greater
            || tokenStream.current().kind == Kind::less
            || tokenStream.current().kind == Kind::plus
            || tokenStream.current().kind == Kind::minus){
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
