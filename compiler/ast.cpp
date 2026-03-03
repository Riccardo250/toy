#include "ast.h"
#include "lexer.h"
#include <iostream>

std::string AbstractSyntaxtTree::toJSONString() {
    std::string str{};

    str += "{";
    str += "\"type\":\"statementList\",";
    str += "\"statements\":";
    str += "[";

    if(statementList.size() > 0) {
        for(int i = 0; i < statementList.size() - 1; i++) {
            str += statementList[i]->toJSONString() + ",";
        }
        str += statementList[statementList.size() - 1]->toJSONString();

    }
    str += "]";
    str += "}";
    return str;
}

std::string VarDecl::toJSONString() {
    std::string str{};

    str += "{";
    str += "\"type\":\"VarDeclStatement\",";
    str += "\"name\":\"" + name + "\",";
    str += "\"t\":\"" + Lexer::typeToString(type) +"\"";
    str += "}";
    return str;
}

std::string VarDeclListStatement::toJSONString() {
    std::string str{};

    str += "{";
    str += "\"type\":\"VarDeclListStatement\",";
    str += "\"declarations\":";
    str += "[";

    if(declList.size() > 0) {
        for(int i = 0; i < declList.size() - 1; i++) {
            str += declList[i].toJSONString() + ",";
        }
        str += declList[declList.size() - 1].toJSONString();

    }
    str += "]";
    str += "}";
    return str;
}

std::string FunDefStatement::toJSONString() {
    std::string str{};

    str += "{";
    str += "\"type\":\"FunDefStatement\",";

    str += "\"name\":\"" + name + "\",";

    str += "\"returnType\":\"" + Lexer::typeToString(returnType) +"\",";

    str += "\"arguments\":";
    str += "[";

    if(arguments.size() > 0) {
        for(int i = 0; i < arguments.size() - 1; i++) {
            str += arguments[i].toJSONString() + ",";
        }
        str += arguments[arguments.size() - 1].toJSONString();

    }
    str += "],";

    str += "\"body\":";
    str += "[";

    if(body.size() > 0) {
        for(int i = 0; i < body.size() - 1; i++) {
            str += body[i]->toJSONString() + ",";
        }
        str += body[body.size() - 1]->toJSONString();

    }
    str += "]";

    str += "}";
    return str;

}

std::string IfStatement::toJSONString () {
    std::string str{};

    str += "{";
    str += "\"type\":\"IfStatement\",";

    str += "\"condition\":" + condition->toJSONString() + ",";

    str += "\"thenBody\":";
    str += "[";

    if(thenBody.size() > 0) {
        for(int i = 0; i < thenBody.size() - 1; i++) {
            str += thenBody[i]->toJSONString() + ",";
        }
        str += thenBody[thenBody.size() - 1]->toJSONString();

    }
    str += "],";

    str += "\"elseBody\":";
    str += "[";

    if(elseBody.size() > 0) {
        for(int i = 0; i < elseBody.size() - 1; i++) {
            str += elseBody[i]->toJSONString() + ",";
        }
        str += elseBody[elseBody.size() - 1]->toJSONString();

    }
    str += "]";

    str += "}";
    return str;
}

std::string WhileStatement::toJSONString () {
    std::string str{};

    str += "{";
    str += "\"type\":\"WhileStatement\",";

    str += "\"condition\":" + condition->toJSONString() + ",";

    str += "\"body\":";
    str += "[";

    if(body.size() > 0) {
        for(int i = 0; i < body.size() - 1; i++) {
            str += body[i]->toJSONString() + ",";
        }
        str += body[body.size() - 1]->toJSONString();

    }
    str += "]";

    str += "}";
    return str;
}

std::string BreakStatement::toJSONString() {
    std::string str{};

    str += "{";
    str += "\"type\":\"BreakStatement\"";
    str += "}";
    return str;
}

std::string ReturnStatement::toJSONString() {
    std::string str{};

    str += "{";
    str += "\"type\":\"ReturnStatement\",";
    str += "\"expr\":" + expr->toJSONString();
    str += "}";
    return str;
}

std::string ExprStatement::toJSONString() {
    std::string str{};

    str += "{";
    str += "\"type\":\"ExprStatement\",";
    str += "\"expr\":" + expr->toJSONString();
    str += "}";
    return str;
}

std::string AssExpr::toJSONString() {
    std::string str{};

    str += "{";
    str += "\"type\":\"AssExpr\",";
    str += "\"name\":" + name->toJSONString() + ",";
    str += "\"expr\":" + expr->toJSONString();
    str += "}";
    return str;
}

std::string BinaryOpExpr::toJSONString() {
    std::string str{};

    str += "{";
    str += "\"type\":\"AddOpExpr\",";
    str += "\"a\":" + a->toJSONString() + ",";
    str += "\"b\":" + b->toJSONString() + ",";
    str += "\"op\":" + binaryOpTypeToString(op);
    str += "}";
    return str;
}

std::string NameExpr::toJSONString() {
    std::string str{};

    str += "{";
    str += "\"type\":\"NameExpr\",";
    str += "\"name\":\"" + name + "\"";
    str += "}";
    return str;
}

std::string ConstExpr::toJSONString() {
    std::string str{};

    str += "{";
    str += "\"type\":\"ConstExpr\",";
    str += "\"number\":\"" + std::to_string(number) + "\"";
    str += "}";
    return str;
}

std::string binaryOpTypeToString(BinaryOpType type) {
    switch(type) {
        case BinaryOpType::add:
            return "add";
        case BinaryOpType::sub:
            return "sub";
        case BinaryOpType::mul:
            return "mul";
        case BinaryOpType::div:
            return "div";
        case BinaryOpType::equal:
            return "equal";
        case BinaryOpType::notEqual:
            return "notEqual";
        case BinaryOpType::greater:
            return "greater";
        case BinaryOpType::greaterEqual:
            return "greaterEqual";
        case BinaryOpType::less:
            return "less";
        case BinaryOpType::lessEqual:
            return "lessEqual";
        case BinaryOpType::logicOr:
            return "logicOr";
        case BinaryOpType::logicAnd:
            return "logicAnd";
        case BinaryOpType::bitwiseOr:
            return "bitwiseOr";
        case BinaryOpType::bitwiseAnd:
            return "bitwiseAnd";
        default:
            return "unknown_binaryOp";
    }
}