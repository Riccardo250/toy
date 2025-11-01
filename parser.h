#ifndef PARSER_H

#define PARSER_H

#include "grammar.h"



struct Item {
    Production production;
    int dotPosition;
};

using State = std::set<Item>;

struct Edge {
    State& to;
    Symbol s;
};

class parseGraph {
    private:
        Grammar grammar;
        std::vector<std::pair<State, std::vector<Edge>>> adjVector;
        State closureAction(State& state);
        State gotoAction(State& state, Symbol& symbol);
        std::vector<Symbol> getProductionsWithLeftSymbol(std::vector<Production> productions, Symbol symbol);
};

bool operator==(const Item& a, const Item& b);

#endif
