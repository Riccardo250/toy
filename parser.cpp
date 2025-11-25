#include "parser.h"
#include <unordered_set>

//parserGraph
State ParseGraph::closureAction(const State& state) {
    State oldState = state;
    State currState = state;

    do {
        std::swap(oldState, currState);
        for(const auto& item : currState) {
            if(item.dotPosition == item.production.getRight().size()) continue;
            if(item.production.getRight()[item.dotPosition].isTerminal()) continue;
            for(const auto& production : getProductionsWithLeftSymbol(grammar.getProductions(), item.production.getLeft())) {
                currState.insert(Item{production, 0});
            }
        }
    } while (currState != oldState);

    return currState;
}

State ParseGraph::gotoAction(const State& state, const Symbol& symbol) {
    State j{};

    for(const auto& item : state) {
        if(item.dotPosition == item.production.getRight().size()) continue;
        if(item.production.getRight().at(item.dotPosition) == symbol) {
            j.insert({item.production, item.dotPosition + 1});
        }
    }

    return closureAction(j);
}

std::vector<Production> ParseGraph::getProductionsWithLeftSymbol(std::vector<Production> productions, Symbol symbol) {
    std::vector<Production> toRet{};

    for(const auto& p : productions) {
        if(p.getLeft() == symbol) toRet.push_back(p);
    }

    return toRet;
}

void ParseGraph::constructParser() {
    //alogrithm - "Modern Compiler Implemenation in C" page 67

    Production newProduction{{"S'", false}, { {grammar.getStatingSymbol(), {"$", true} }}};
    grammar.augmentProductions(newProduction);

    std::set<State> oldT{closureAction({{newProduction, 0}})};
    std::set<FreeEdge> oldE{};
    std::set<State> currT = oldT;
    std::set<FreeEdge> currE = oldE;

    do {
        std::swap(currT, oldT);
        std::swap(currE, oldE);

        for(const auto& state : currT) {
            for(const auto& item : state) {
                if(item.dotPosition == item.production.getRight().size()) continue;
                Symbol tempSymbol = item.production.getRight()[item.dotPosition];
                State j = gotoAction(state, tempSymbol);
                
                currT.insert(j);
                currE.insert({state, j, tempSymbol});
            }
        }
    } while(currT != oldT || currE != oldE);

    for(const auto& edge : currE) {
        adjacencyVector.addState(edge.from);
        adjacencyVector.addEdge(edge.from, edge.to, edge.s);
    }

    for(const auto& state : currT) {
        for(const auto& item : state) {
            if(item.dotPosition == item.production.getRight().size()) {
                reduceActions.insert({state, item.production});
            }
        }
    }
}

void AdjacencyVector::addState(const State& state) {
    for(const auto& p : adjacencyVector) {
        if(p.first == state) return;
    }

    adjacencyVector.push_back({state, {}});
}

void AdjacencyVector::addEdge(const State& from, const State& to, const Symbol s) {
    Edge toInsert{to, s};

    for(auto& p: adjacencyVector) {
        if(p.first == from){
            for(const auto& e : p.second) {
                if(e == toInsert) return;
            }

            p.second.push_back(toInsert);
        }
    }
}

bool operator==(const Item& a, const Item& b) {
    return a.production == b.production && a.dotPosition == b.dotPosition;
}

bool operator<(const Item& a, const Item& b) {
    if(a.production != b.production) {
        return a.production < b.production;
    }

    if(a.dotPosition != b.dotPosition) {
        return a.dotPosition < b.dotPosition;
    }

    return false;
}

bool operator==(const Edge& a, const Edge& b) {
    return a.s == b.s && a.to == b.to;
}

bool operator!=(const Edge&a, const Edge& b) {
    return !(a == b);
}

bool operator==(const FreeEdge& a, const FreeEdge& b) {
    return a.from == b.from && a.to == b.to && a.s == b.s;
}

bool operator!=(const FreeEdge& a, const FreeEdge& b) {
    return !(a == b);
}

bool operator<(const FreeEdge& a, const FreeEdge& b) {
    if(a.from != b.from) return a.from < b.from;
    if(a.to != b.to) return a.to < b.to;
    if(a.s != b.s) return a.s < b.s;
    return false;
}

bool operator==(const ReduceAction& a, const ReduceAction& b) {
    return a.production == b.production && a.state == b.state;
}

bool operator!=(const ReduceAction& a, const ReduceAction& b) {
    return !(a == b);
}

bool operator<(const ReduceAction& a, const ReduceAction& b) {
    if(a.production != b.production) return a.production < b.production;
    if(a.state != b.state) return a.state < b.state;
    return false;
}   

bool operator==(const AdjacencyVector& a, const AdjacencyVector& b) {
    std::vector<std::pair<State, std::vector<Edge>>> aVec = a.adjacencyVector;
    std::vector<std::pair<State, std::vector<Edge>>> bVec = b.adjacencyVector;

    if(aVec.size() != bVec.size()) return false;

    for(int i = 0; i < aVec.size(); i++) {
        if(aVec[i].first != bVec[i].first) return false;
        if(aVec[i].second.size() != bVec[i].second.size()) return false;
        for(int j = 0; j < aVec[i].second.size(); j++) {
            if(aVec[i].second[j] != bVec[i].second[j]) return false;
        }
    }

    return true;
}

