#include "grammar.h"

struct Item {
    const Production production;
    const int dotPosition;
};

using State = std::set<Item>;

struct Edge {
    const State& to;
    const Symbol s;
};

struct FreeEdge {
    const State& from;
    const State& to;
    const Symbol s;
};

struct ReduceAction {
    const State state;
    const Production production;
};


class AdjacencyVector {
    private:
        std::vector<std::pair<State, std::vector<Edge>>> adjacencyVector;
        friend bool operator==(const AdjacencyVector& a, const AdjacencyVector& b);

    public:
        void addState(const State& state);
        void addEdge(const State& from, const State& to, const Symbol s);
        std::vector<std::pair<State, std::vector<Edge>>> getVector();
};

class ParseGraph {
    private:
        Grammar grammar;
        AdjacencyVector adjacencyVector;
        std::set<ReduceAction> reduceActions;
        State closureAction(const State& state);
        State gotoAction(const State& state, const Symbol& symbol);
        void constructParser();
        std::vector<Production> getProductionsWithLeftSymbol(std::vector<Production> productions, Symbol symbol);

};

bool operator==(const Item& a, const Item& b);
bool operator<(const Item& a, const Item& b);
bool operator==(const Edge& a, const Edge& b);
bool operator!=(const Edge& a, const Edge& b);
bool operator==(const FreeEdge& a, const FreeEdge& b);
bool operator!=(const FreeEdge& a, const FreeEdge& b);
bool operator<(const FreeEdge& a, const FreeEdge& b);
bool operator==(const ReduceAction& a, const ReduceAction& b);
bool operator!=(const ReduceAction& a, const ReduceAction& b);
bool operator<(const ReduceAction& a, const ReduceAction& b);
bool operator==(const AdjacencyVector& a, const AdjacencyVector& b);
