/*
                           Zeta

A lexer, parser, interpreter written by Zaedyn Thomas-Kennedy

*/

#include <iostream>
#include <fstream>
#include <sstream>

#include "lexer.h"
#include "ast.h"
#include "parser.h"
#include "stmt.h"
#include "environment.h"


int main(int argc, char* argv[])
{
    
    // Usage failure
    if (argc > 2) {
        std::cerr << "Usage: zeta [file.zt]\n";
        return 1;
    }

    // ---------- REPL mode, with no file given  ---------- 
    if (argc == 1) { 
        std::shared_ptr<Environment> env = std::make_shared<Environment>();
        std::vector<std::unique_ptr<Lexer>> lexers; // tokens point into each line's text
        std::vector<std::unique_ptr<Stmt>> program; // functions point at their statements
        std::string line;
        while (true) {
            std::cout << "> ";
            if (!std::getline(std::cin, line)) break;
            lexers.push_back(std::make_unique<Lexer>(line));
            std::vector<Token> tokens = lexers.back()->lex();
            Parser parser(std::move(tokens));

            std::vector<std::unique_ptr<Stmt>> statements = parser.parse_program();

            if (parser.had_error()) continue;

            try {
                for (const auto& statement : statements) statement->execute(*env);
            }
            catch (const std::runtime_error& evaluation_failure) {

                std::cerr << evaluation_failure.what() << "\n";
            }

            catch (const ReturnValue&) {
                std::cerr << "Error: Return outside of a function!\n";
            }

            for (auto& statement : statements) program.push_back(std::move(statement));
            
        }

        return 0; 
    }

    //  ---------- File mode ---------- 
    std::string file_path = argv[1];
    std::ifstream file(file_path);
    if (!file.is_open()) {
        std::cerr << "The file " << file_path << " could not be opened.\n";
        return 1;
    }

    std::cout << " Zeta \n------ \n\n";

    // Read the whole file into a string
    std::filebuf* pbuf = file.rdbuf();

    std::stringstream file_stream;
    file_stream << pbuf;

    std::string file_contents = file_stream.str();

    // Lex and parse
    Lexer lexer(std::move(file_contents));

    std::vector<Token> tokens = lexer.lex();

    Parser parser(std::move(tokens));

    std::vector<std::unique_ptr<Stmt>> statements = parser.parse_program();

    // Run
    if (parser.had_error()) return 1;

    try {

        std::shared_ptr<Environment> env = std::make_shared<Environment>();

        for (const auto& statement : statements) {
            statement->execute(*env);
        }
    } 

    catch (const std::runtime_error& evaluation_failure) {
        
        std::cerr << evaluation_failure.what() << "\n";
        return 1;
    }

    catch (const ReturnValue&) {
        std::cerr << "Error: Return outside of a function!\n";
        return 1;
    }

    return 0;
}