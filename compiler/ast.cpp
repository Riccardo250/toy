#include "ast.h"
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

std::string AddOpExpr::toJSONString() {
    std::string str{};

    str += "{";
    str += "\"type\":\"AddOpExpr\",";
    str += "\"a\":" + a->toJSONString() + ",";
    str += "\"b\":" + b->toJSONString();
    str += "}";
    return str;
}

std::string SubOpExpr::toJSONString() {
    std::string str{};

    str += "{";
    str += "\"type\":\"SubOpExpr\",";
    str += "\"a\":" + a->toJSONString() + ",";
    str += "\"b\":" + b->toJSONString();
    str += "}";
    return str;
}

std::string MulOpExpr::toJSONString() {
    std::string str{};

    str += "{";
    str += "\"type\":\"MulOpExpr\",";
    str += "\"a\":" + a->toJSONString() + ",";
    str += "\"b\":" + b->toJSONString();
    str += "}";
    return str;
}

std::string DivOpExpr::toJSONString() {
    std::string str{};

    str += "{";
    str += "\"type\":\"DivOpExpr\"";
    str += "\"a\":" + a->toJSONString() + ",";
    str += "\"b\":" + b->toJSONString();
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
