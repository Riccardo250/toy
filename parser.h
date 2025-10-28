#include "grammar.h"

class item {
    production production;
    int dotPosition;
};

class state {
    std::vector<item> items;
};

class edge {
    state& to;
    symbol s;
};

class parseGraph {
    private:
        std::vector<std::pair<state, std::vector<edge>>> adjVector;
        state closureAction(state& state);
        state gotoAction(state& state, symbol& symbol);
};


