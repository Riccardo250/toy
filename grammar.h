#ifndef GRAMMAR_H

#define GRAMMAR_H

#include <cstdio>
#include <initializer_list>
#include <vector>
#include <string>
#include <ostream>
#include <iostream>
#include <memory>
#include <set>
#include <map>

class Symbol {
    private: 
        const std::string label;
        const bool terminal;
        friend std::ostream& operator<<(std::ostream& strm, Symbol &s);

    public:
        Symbol();
        Symbol(std::string& l, bool t);
        Symbol(std::string&& l, bool t);
        
        const std::string& getLabel() const;
        const bool isTerminal() const;
};

class Production {
    private:
        const Symbol left;
        const std::vector<Symbol> right;

        friend std::ostream& operator<<(std::ostream& strm, Production &p);

    public:
        Production(Symbol&& left, std::vector<Symbol>&& right);
        Production(Symbol& left, std::vector<Symbol>&& right);
        const Symbol& getLeft() const;
        const std::vector<Symbol>& getRight() const;
};

class Grammar {
    private:
        const std::set<Symbol> alphabet;
        const std::vector<Production> productions;
        std::map<Symbol, std::set<Symbol>> first;
        std::map<Symbol, std::set<Symbol>> follow;
        std::map<Symbol, bool> nullable;

        bool allOfNullable(std::vector<Symbol> vector, int startIndex, int len, std::map<Symbol, bool>& localNullable);
        std::vector<Production>&& checkValidProductions(std::vector<Production>& Productions);
        bool isProductionInAlphabet(const Production& p) const;

    public:
        Grammar(std::set<Symbol>&& a, std::vector<Production>&& Productions);
        const std::set<Symbol>& getAlphabet() const;
        const std::vector<Production>& getProductions() const;
        void generateSets();
        void printSets();
};




template<class T>
std::set<T> merge(const std::set<T>& a, const std::set<T>& b); 
std::ostream& operator<<(std::ostream& strm, const Symbol& s);
bool operator<(const Symbol& a, const Symbol& b);
std::ostream& operator<<(std::ostream& strm, const Production& p);
std::ostream& operator<<(std::ostream& strm, const std::set<Symbol> set);
std::ostream& operator<<(std::ostream& strm, const std::vector<Production> Productions);
std::ostream& operator<<(std::ostream& strm, const Grammar& g);
template<class A, class B>
std::ostream& operator<<(std::ostream& strm, const std::map<A, B>);
bool operator==(const Symbol& a, const Symbol& b);
bool operator==(const Production& a, const Production& b);
#endif