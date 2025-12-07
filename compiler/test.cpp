#include "parser.h"
#include <iostream>
#include <fstream>


int main(int argc, char ** argv) {
    if(argc != 2) {
        std::cerr << "ERROR: exactly 1 argument expected\n";
        return -1;
    }

    std::ifstream fileStream{argv[1]};
    if(fileStream.fail()) {
        std::cerr << "ERROR: failed to open the file\n";
        return -2;
    }

    Token_stream ts = {fileStream};

    do {
        std::cout << ts.get() << " ";
    } while(ts.current().kind != Kind::end);
    
    std::cout << "\n";

    std::ifstream fileStreamSecond{argv[1]};
    if(fileStreamSecond.fail()) {
        std::cerr << "ERROR: failed to open the file\n";
        return -2;
    }

    Parser parser{fileStreamSecond};
    AbstractSyntaxtTree ast = parser.parse();
    std::cout << "parsed\n";

    std::ofstream outputStream{"ast.json", std::ios_base::out};
    if(outputStream.fail()) {
        std::cerr << "ERROR: failed to open the file\n";
        return -2;
    }

    outputStream << ast.toJSONString();

    return 0;
}

