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

class symbol {
    private: 
        const std::string label;
        const bool terminal;
        friend std::ostream& operator<<(std::ostream& strm, symbol &s);

    public:
        symbol();
        symbol(std::string& l, bool t);
        symbol(std::string&& l, bool t);
        
        const std::string& getLabel() const;
        const bool isTerminal() const;
};

class production {
    private:
        const symbol left;
        const std::vector<symbol> right;

        friend std::ostream& operator<<(std::ostream& strm, production &p);

    public:
        production(symbol&& left, std::vector<symbol>&& right);
        production(symbol& left, std::vector<symbol>&& right);
        const symbol& getLeft() const;
        const std::vector<symbol>& getRight() const;
};

class grammar {
    private:
        const std::set<symbol> alphabet;
        const std::vector<production> productions;
        std::map<symbol, std::set<symbol>> first;
        std::map<symbol, std::set<symbol>> follow;
        std::map<symbol, bool> nullable;

        bool allOfNullable(std::vector<symbol> vector, int startIndex, int len, std::map<symbol, bool>& localNullable);
        std::vector<production>&& checkValidProductions(std::vector<production>& productions);
        bool isProductionInAlphabet(const production& p) const;

    public:
        grammar(std::set<symbol>&& a, std::vector<production>&& productions);
        const std::set<symbol>& getAlphabet() const;
        const std::vector<production>& getProductions() const;
        void generateSets();
        void printSets();
};



std::ostream& operator<<(std::ostream& strm, const symbol& s);
bool operator<(const symbol& a, const symbol& b);
std::ostream& operator<<(std::ostream& strm, const production& p);
std::ostream& operator<<(std::ostream& strm, const std::set<symbol> set);
std::ostream& operator<<(std::ostream& strm, const std::vector<production> productions);
std::ostream& operator<<(std::ostream& strm, const grammar& g);
std::ostream& operator<<(std::ostream& strm, const std::map<symbol, std::set<symbol>>);
std::ostream& operator<<(std::ostream& strm, const std::map<symbol, bool>);
bool operator==(const symbol& first, const symbol& second);
#endif