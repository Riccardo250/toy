#include "grammar.h"

enum ParseTableActionType {
    ACTION_SHIFT,
    ACTION_REDUCE,
    ACTION_GOTO,
    ACTION_ACCEPT,
    ACTION_NONE
};

struct ParseTableAction {
    ParseTableActionType type;
    int state;
};

struct Item {
    const Production production;
    const long unsigned int dotPosition;

    bool operator<(const Item& other) const {
        if(production != other.production) {
            return production < other.production;
        }

        if(dotPosition != other.dotPosition) {
            return dotPosition < other.dotPosition;
        }

        return false;
    }

    bool operator==(const Item& other) const {
        return production == other.production && dotPosition == other.dotPosition;
    }
};



class Parser {
    private:
        std::vector<std::map<Symbol, ParseTableAction>> parseTable;
        ParseGraph parseGraph;
        
    public:
        Parser(Grammar grammar);
        void printParseTable();
};

std::ostream& operator<<(std::ostream& strm, const ParseTableAction& parseTableAction);
bool operator==(const AdjacencyVector& a, const AdjacencyVector& b);

std::string constructEntryFittingString(const std::string& s);