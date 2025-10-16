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

// bool grammar::allOfNullable(std::vector<symbol> vector, int startIndex, int endIndex, std::map<symbol, bool>& localNullable) {

//     for(int i = startIndex; i <= endIndex; i++) {
//         if(localNullable[vector.at(i)] == false)
//             return false;
//     }

//     return true;
// }

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
            
            std::cout << "###################################\n";
            std::cout << "before:\n";
            std::cout << "localFirst:" << localFirst << "\n";
            std::cout << "localFollow:" << localFollow << "\n";
            std::cout << "localNullable:" << localNullable << "\n";
           

            const symbol& currLeft = p.getLeft();
            const std::vector<symbol>& currRight = p.getRight();
            int k = currRight.size();

            if(k == 0) continue;

            //first
            bool allNullable = true;
            for(int x = 0; x < k; x++) {
                if(localNullable[currRight.at(x)] == false) {
                    allNullable = false;
                }
            }
            if(allNullable) 
                localNullable[currLeft] = true;

            localFirst[currLeft].merge(localFirst[currRight.at(0)]);
            localFollow[currRight.at(k - 1)].merge(localFollow[currLeft]);

            //debug
            for(int i = 0; i < k; i++) {

                //first
                allNullable = true;
                for(int x = 0; x < i; x++) {
                    if(localNullable[currRight.at(x)] == false) {
                        allNullable = false;
                    }
                }

                if(allNullable) 
                    localFirst[currLeft].merge(localFirst[currRight.at(i)]);
                //end first

                //second
                allNullable = true;
                for(int x = i + 1; x < k; x++) {
                    if(localNullable[currRight.at(x)] == false) {
                        allNullable = false;
                    }
                }

                if(allNullable) 
                    localFollow[currRight.at(i)].merge(localFollow[currLeft]);

                //end second

                //third
                for(int j = i + 1; j < k; j++) {
                    allNullable = true;
                    for(int x = i + 1; x < j; x++) {
                        if(localNullable[currRight.at(x)] == false) {
                            allNullable = false;
                        }
                    }

                    if(allNullable) 
                        localFollow[currRight.at(i)].merge(localFirst[currRight.at(j)]);
                }
                //end third
            }

            std::cout << "after:\n";
            std::cout << "localFirst:" << localFirst << "\n";
            std::cout << "localFollow:" << localFollow << "\n";
            std::cout << "localNullable:" << localNullable << "\n";
            std::cout << "###################################\n";
        }

        if(localFirst != savedLocalFirst || localFollow != savedLocalFollow || localNullable != savedLocalNullable) changed = true;
        
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