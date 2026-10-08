#pragma once
#include <vector>
#include "token.h"
#include "ast.h"
#include "stmt.h"

class Parser {
	std::vector<Token> tokens;
	std::size_t current_index = 0;
	bool parser_failure = false;

public:

	explicit Parser(std::vector<Token> token_vector);

	Token read_token() const;

	Token read_previous_token() const;

	bool is_at_end() const;

	Token advance();

	// Check to see if it matches
	bool check_type(TokenType token_type) const;

	// Check to see if it matches, and consume if true
	bool match_type(TokenType token_type);

	// Hand back the token if it's the right type
	Token expect_type(TokenType token_type);

	int binding_power(TokenType token_type) const;

	std::unique_ptr<Expr> parse_prefix();

	std::unique_ptr<Expr> parse_expression(int min_power);

	std::unique_ptr<Stmt> parse_statement();

	std::vector<std::unique_ptr<Stmt>> parse_program();

	std::unique_ptr<Expr> parse_assignment();

	bool had_error() const;

};