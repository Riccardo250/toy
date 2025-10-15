#include "grammar.h"
#include <stdexcept>
#include <algorithm>

//symbol
symbol::symbol() : label(""), terminal(true) {}
symbol::symbol(std::string& l, const bool t) : label(l), terminal (t) {}
symbol::symbol(std::string&& l, const bool t) : label(l), terminal (t) {}
const std::string& symbol::getLabel() const {return label;}
const bool symbol::isTerminal() const {return terminal;}

//production
production::production(symbol&& left, std::vector<symbol>&& right) : left(left), right(right) {}
production::production(symbol& left, std::vector<symbol>&& right) : left(left), right(right) {}

const symbol& production::getLeft() const {return left;}
const std::vector<symbol>& production::getRight() const {return right;}

//grammar 
grammar::grammar(std::set<symbol>&& alphabet, std::vector<production>&& ps) : alphabet(alphabet), productions(checkValidProductions(ps)) {}
std::vector<production>&& grammar::checkValidProductions(std::vector<production>& productions) {
    for(const production& p : productions) {
        if(!isProductionInAlphabet(p)) {
            throw std::invalid_argument("One symbol of the production is not in the alphabet");
        }
    }

    return std::move(productions);
}

bool grammar::isProductionInAlphabet(const production& p) const {
    if (alphabet.find(p.getLeft()) == alphabet.cend()) {
        std::cerr << "symbol " << p.getLeft() << " not found in alphabet\n";
        return false;
    }

    for(const symbol& s : p.getRight()) {
        if(alphabet.find(s) == alphabet.cend()) {
            std::cerr << "symbol " << s << " not found in alphabet\n";
            return false;
        }
    }

    return true;
}

const std::set<symbol>& grammar::getAlphabet() const {
    return alphabet;
}
const std::vector<production>& grammar::getProductions() const {
    return productions;
}

void grammar::generateSets() {
    std::map<symbol, std::set<symbol>> localFirst;
    std::map<symbol, std::set<symbol>> localFollow;
    std::map<symbol, bool> localNullable;
    bool changed;

    //initialize empty sets
    for(const symbol& s : alphabet) {
        localFirst[s] = std::set<symbol> ();
        localFollow[s] = std::set<symbol> ();
        localNullable[s] = false;
    }


    for(const production& p : productions) {
        if(p.getRight().size() == 0) localNullable[p.getLeft()] = true;
    }

    //alogrithm - "Modern Compiler Implemenation in C" page 57
    for(const symbol& s : alphabet) {
        if(s.isTerminal()) localFirst[s].insert(s);
    }

    do {
        changed = false;
        
        std::map<symbol, std::set<symbol>> savedLocalFirst = localFirst;
        std::map<symbol, std::set<symbol>> savedLocalFollow = localFollow;
        std::map<symbol, bool> savedLocalNullable = localNullable;

        for(const production& p : productions) {
            int k = p.getRight().size();
            for(int i = 0; i < k; i++) {

                const symbol& currLeft = p.getLeft();
                const std::vector<symbol>& currRight = p.getRight();

                for(int j = i + 1; j < k; j++) {      

                    if(std::all_of(currRight.begin(), currRight.end(), [&localNullable](const symbol& s) {return localNullable[s];})) {
                        localNullable[currLeft] = true;
                        std::cout << "first if\n";
                    }

                    if(std::all_of(currRight.begin(), currRight.begin() + (i - 1), [&localNullable](const symbol& s) {return localNullable[s];})) {
                        localFirst[currLeft].merge(localFirst[currRight.at(i)]);
                        std::cout << "second if\n";
                    }

                    if(std::all_of(currRight.begin() + (i + 1), currRight.end(), [&localNullable](const symbol& s) {return localNullable[s];})) {
                        localFollow[currRight.at(i)].merge(localFollow[currLeft]);
                        std::cout << "third if\n";

                    }

                    if(std::all_of(currRight.begin() + (i + 1), currRight.begin() + (j - 1), [&localNullable](const symbol& s) {return localNullable[s];})) {
                        localFollow[currRight.at(i)].merge(localFirst[currRight.at(j)]);
                        std::cout << "fourth if\n";
                    }
                }
            }
        }

        if(localFirst != savedLocalFirst || localFollow != savedLocalFollow || localNullable != savedLocalNullable) changed = true;
        std::cout << "iterating ... \n";
    } while(changed);

    first = std::move(localFirst);
    follow = std::move(localFollow);
    nullable = std::move(localNullable);
}

void grammar::printSets() {
    std::cout << "first: " << first;
    std::cout << "follow: " << follow; 
    std::cout << "nullable: " << nullable;
}


std::ostream& operator<<(std::ostream& strm, const symbol& s) {
    return strm << s.getLabel();
}

bool operator<(const symbol& a, const symbol& b) {
    return a.getLabel() < b.getLabel() || a.isTerminal() < b.isTerminal();
}

std::ostream& operator<<(std::ostream& strm, const production& p) {
    strm << p.getLeft();
    strm << " ->";
    for(const symbol& s : p.getRight()) {
        strm << " " << s << ",";
    }
    return strm;
}

std::ostream& operator<<(std::ostream& strm, const std::set<symbol> set) {
    strm << "{ ";
    for(const symbol& s : set) {
        strm << s << ", ";
    }
    strm << "}";

    return strm;
}

std::ostream& operator<<(std::ostream& strm, std::vector<production> productions) {
    for(const production& p : productions) {
        strm << p << "; ";
    }
    return strm;
}

std::ostream& operator<<(std::ostream& strm, const grammar& g) {
    strm << "alphabet: " << g.getAlphabet() << " productions: " << g.getProductions();
    return strm;
}

std::ostream& operator<<(std::ostream& strm, const std::map<symbol, std::set<symbol>> m) {
    strm << "{\n";
    for(const auto& p : m) {
        strm << "\t{" << p.first << " : " << p.second << "}\n";
    }
    strm << "}\n";

    return strm;
} 

std::ostream& operator<<(std::ostream& strm, const std::map<symbol, bool> m) {
    strm << "{\n";
    for(const auto& p : m) {
        strm << "\t{" << p.first << " : " << p.second << "}\n";
    }
    strm << "}\n";

    return strm;
}

bool operator==(const symbol& first, const symbol& second) {
    return first.getLabel() == second.getLabel() && first.isTerminal() == second.isTerminal();
}