#include "parser.h"
#include <unordered_set>
#define entry_cell_size 16;

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

int AdjacencyVector::getStateIndex(const State& state) {
    for(int i = 0; i < adjacencyVector.size(); i++) {
        if(state == adjacencyVector[i].first) return i;
    }

    return -1;
}

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

void ParseGraph::constructParseGraph() {
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

ParseGraph::ParseGraph(Grammar grammar) : grammar(grammar) {
    //initializes adjacencyVector and reduceActions
    constructParseGraph();
}

std::vector<std::map<Symbol, ParseTableAction>> ParseGraph::constructParseTable() {
    std::vector<std::pair<State, std::vector<Edge>>> adjVector = adjacencyVector.getRawVector();
    std::vector<std::map<Symbol, ParseTableAction>> parseTable(adjVector.size());

    //initialize parseTable
    for(int i = 0; i < parseTable.size(); i++) {
        for(const auto& s : grammar.getAlphabet()) {
            parseTable[i][s] = ParseTableAction{ACTION_NONE, -1};
        }
    }

    for(int i = 0; i < adjVector.size(); i++) {
        State currState = adjVector[i].first;
        std::vector<Edge> edges = adjVector[i].second;
        for(const auto& e : edges) {
            ParseTableActionType currType = e.s.isTerminal() ? ACTION_SHIFT : ACTION_GOTO;
            int currIndex = adjacencyVector.getStateIndex(e.to);
            ParseTableAction tempAction{currType, currIndex};
            parseTable[i][e.s] = tempAction;
        }
        Production acceptingProduction{{"S'", false}, { {grammar.getStatingSymbol(), {"$", true} }}};
        Item acceptingItem{acceptingProduction, acceptingProduction.getRight().size()};

        if(currState.find(acceptingItem) != currState.end()) {
            parseTable[i][{"S'", false}] = {ACTION_ACCEPT, -1};
        }
    }

    for(const auto& r : reduceActions) {
        for(const auto& s : grammar.getAlphabet()) {
            parseTable[adjacencyVector.getStateIndex(r.state)][s] = {ACTION_REDUCE, grammar.getProductionIndex(r.production)};
        }
    }

    return parseTable;
}

void Parser::printParseTable() {
    int ecs = entry_cell_size;
    
    std::string emptyCell(ecs, ' ');
    std::cout << emptyCell;

    for(const auto& s : grammar.getAlphabet()) {
        std::cout << constructEntryFittingString(s.getLabel());
    }

    for(int i = 0; i < )
}

Parser::Parser(Grammar grammar) : parseGraph{grammar} {
    parseGraph = ParseGraph{grammar};
    parseTable = parseGraph.constructParseTable();
}

std::ostream& operator<<(std::ostream& strm, const ParseTableAction& parseTableAction) {
    switch(parseTableAction.type) {
        case ACTION_SHIFT:
            return strm << "s" << parseTableAction.state;
        case ACTION_REDUCE:
            return strm << "r" << parseTableAction.state;
        case ACTION_GOTO:
            return strm << "g" << parseTableAction.state;
        case ACTION_ACCEPT:
            return strm << "a" << 
        case ACTION_NONE:
            return strm << "e";
    }
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

std::string constructEntryFittingString(const std::string& s) {
    std::string toRet{};
    
    int whiteSpacesNumber = entry_cell_size - s.size();
    if(whiteSpacesNumber < 0) return s;

    std::string whiteSpaces(whiteSpacesNumber, ' ');
    return s + whiteSpaces;
}

