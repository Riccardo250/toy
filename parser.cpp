#include "parser.h"

//parserGraph
State parseGraph::closureAction(State& state) {
    State oldState = state;
    State currState = state;

    do {
        for(const auto& item : currState) {
            if(item.dotPosition == item.production.getRight().size()) continue;
            if(item.production.getRight()[item.dotPosition].isTerminal()) continue;
            for(const auto& production : grammar.getProductions()) {
                currState.insert(Item{production, 0});
            }
        }
    } while (oldState == currState);


}

std::vector<Production> getProductionsWithLeftSymbol(std::vector<Production> productions, Symbol symbol) {
    std::vector<Production> toRet{};

    for(const auto& p : productions) {
        if(p.getLeft() == symbol) toRet.insert(p);
    }
}

bool operator==(const Item& a, const Item& b) {
    return a.production == b.production && a.dotPosition == b.dotPosition;
}
