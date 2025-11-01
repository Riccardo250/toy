#include "grammar.h"
#include <stdexcept>
#include <algorithm>

//Symbol
Symbol::Symbol() : label(""), terminal(true) {}
Symbol::Symbol(std::string& l, const bool t) : label(l), terminal (t) {}
Symbol::Symbol(std::string&& l, const bool t) : label(l), terminal (t) {}
const std::string& Symbol::getLabel() const {return label;}
const bool Symbol::isTerminal() const {return terminal;}

//Production
Production::Production(Symbol&& left, std::vector<Symbol>&& right) : left(left), right(right) {}
Production::Production(Symbol& left, std::vector<Symbol>&& right) : left(left), right(right) {}

const Symbol& Production::getLeft() const {return left;}
const std::vector<Symbol>& Production::getRight() const {return right;}

//Grammar 
Grammar::Grammar(std::set<Symbol>&& alphabet, std::vector<Production>&& ps) : alphabet(alphabet), productions(checkValidProductions(ps)) {}

bool Grammar::allOfNullable(std::vector<Symbol> vector, int startIndex, int size, std::map<Symbol, bool>& localNullable) {

    for(int i = startIndex; i < size; i++) {
        if(localNullable[vector.at(i)] == false)
            return false;
    }

    return true;
}

std::vector<Production>&& Grammar::checkValidProductions(std::vector<Production>& productions) {
    for(const Production& p : productions) {
        if(!isProductionInAlphabet(p)) {
            throw std::invalid_argument("One Symbol of the Production is not in the alphabet");
        }
    }

    return std::move(productions);
}

bool Grammar::isProductionInAlphabet(const Production& p) const {
    if (alphabet.find(p.getLeft()) == alphabet.cend()) {
        std::cerr << "Symbol " << p.getLeft() << " not found in alphabet\n";
        return false;
    }

    for(const Symbol& s : p.getRight()) {
        if(alphabet.find(s) == alphabet.cend()) {
            std::cerr << "Symbol " << s << " not found in alphabet\n";
            return false;
        }
    }

    return true;
}

const std::set<Symbol>& Grammar::getAlphabet() const {
    return alphabet;
}
const std::vector<Production>& Grammar::getProductions() const {
    return productions;
}

void Grammar::generateSets() {
    std::map<Symbol, std::set<Symbol>> localFirst;
    std::map<Symbol, std::set<Symbol>> localFollow;
    std::map<Symbol, bool> localNullable;
    bool changed;

    //initialize empty sets
    for(const Symbol& s : alphabet) {
        localFirst[s] = std::set<Symbol> ();
        localFollow[s] = std::set<Symbol> ();
        localNullable[s] = false;
    }

    for(const Production& p : productions) {
        if(p.getRight().size() == 0) localNullable[p.getLeft()] = true;
    }

    //alogrithm - "Modern Compiler Implemenation in C" page 57
    for(const Symbol& s : alphabet) {
        if(s.isTerminal()) localFirst[s].insert(s);
    }

    do {
        changed = false;
        
        std::map<Symbol, std::set<Symbol>> savedLocalFirst = localFirst;
        std::map<Symbol, std::set<Symbol>> savedLocalFollow = localFollow;
        std::map<Symbol, bool> savedLocalNullable = localNullable;
           
        for(const Production& p : productions) {
            
            const Symbol& currLeft = p.getLeft();
            const std::vector<Symbol>& currRight = p.getRight();
            int k = currRight.size();

            if(k == 0) continue;

            if(allOfNullable(currRight, 0, k, localNullable)) 
                localNullable[currLeft] = true;

            for(int i = 0; i < k; i++) {
                if(allOfNullable(currRight, 0, i, localNullable)) 
                    localFirst[currLeft] = merge(localFirst[currRight.at(i)], localFirst[currLeft]);
  

                if(allOfNullable(currRight, i + 1, k, localNullable)) 
                    localFollow[currRight.at(i)] = merge(localFollow[currLeft] ,localFollow[currRight.at(i)]);

                for(int j = i + 1; j < k; j++) {
                    if(allOfNullable(currRight, i + 1, j, localNullable)) 
                        localFollow[currRight.at(i)] = merge(localFirst[currRight.at(j)], localFollow[currRight.at(i)]);
                }
            }
        }

        if(localFirst != savedLocalFirst || localFollow != savedLocalFollow || localNullable != savedLocalNullable) 
            changed = true;

    } while(changed);

    first = std::move(localFirst);
    follow = std::move(localFollow);
    nullable = std::move(localNullable);
}

void Grammar::printSets() {
    std::cout << "first: " << first;
    std::cout << "follow: " << follow; 
    std::cout << "nullable: " << nullable;
}


//TODO: understand why this is wrong
// template<class T> 
// std::set<T>&& merge(const std::set<T>& a, const std::set<T>& b) {
//     std::set<T> mergedSet;

//     for(const auto& el : a) {
//         mergedSet.insert(el);
//     }

//     for(const auto& el : b) {
//         mergedSet.insert(el);
//     }

//     return std::move(mergedSet);
// }

template<class T> 
std::set<T> merge(const std::set<T>& a, const std::set<T>& b) {
    std::set<T> mergedSet;

    for(const auto& el : a) {
        mergedSet.insert(el);
    }

    for(const auto& el : b) {
        mergedSet.insert(el);
    }

    return mergedSet;
}

std::ostream& operator<<(std::ostream& strm, const Symbol& s) {
    return strm << s.getLabel();
}

bool operator<(const Symbol& a, const Symbol& b) {
    return a.getLabel() < b.getLabel() || a.isTerminal() < b.isTerminal();
}

std::ostream& operator<<(std::ostream& strm, const Production& p) {
    strm << p.getLeft();
    strm << " ->";
    for(const Symbol& s : p.getRight()) {
        strm << " " << s << ",";
    }
    return strm;
}

std::ostream& operator<<(std::ostream& strm, const std::set<Symbol> set) {
    strm << "{ ";
    for(const Symbol& s : set) {
        strm << s << ", ";
    }
    strm << "}";

    return strm;
}

std::ostream& operator<<(std::ostream& strm, std::vector<Production> productions) {
    for(const Production& p : productions) {
        strm << p << "; ";
    }
    return strm;
}

std::ostream& operator<<(std::ostream& strm, const Grammar& g) {
    strm << "alphabet: " << g.getAlphabet() << " productions: " << g.getProductions();
    return strm;
}

template<class A, class B>
std::ostream& operator<<(std::ostream& strm, const std::map<A, B> m) {
    strm << "{\n";
    for(const auto& p : m) {
        strm << "\t{" << p.first << " : " << p.second << "}\n";
    }
    strm << "}\n";

    return strm;
} 

// std::ostream& operator<<(std::ostream& strm, const std::map<Symbol, bool> m) {
//     strm << "{\n";
//     for(const auto& p : m) {
//         strm << "\t{" << p.first << " : " << p.second << "}\n";
//     }
//     strm << "}\n";

//     return strm;
// }

bool operator==(const Symbol& a, const Symbol& b) {
    return a.getLabel() == b.getLabel() && a.isTerminal() == b.isTerminal();
}

bool operator==(const Production& a, const Production& b) {
    return a.getLeft() == b.getLeft() && a.getRight() == b.getRight();
}