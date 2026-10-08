#include "parser.h"
#include <iostream>

Parser::Parser(std::vector<Token> token_vector) {
	tokens = std::move(token_vector);
}

Token Parser::read_token() const {
	return tokens[current_index];
}

Token Parser::read_previous_token() const {
	return tokens[current_index-1];
}

// Check to see if the token is type EOF_TOKEN
bool Parser::is_at_end() const {
	
	Token token = read_token();
	
	return token.token_type == TokenType::EOF_TOKEN;
}

Token Parser::advance() {
	// don't step past EOF_TOKEN
	if (!is_at_end()) {
		current_index += 1;
		return read_previous_token();
	}
	else {
		return read_token();
	}
}

// Check to see if it matches
bool Parser::check_type(TokenType token_type) const {
	return token_type == tokens[current_index].token_type;
}

// Check to see if it matches, and consume if true
bool Parser::match_type(TokenType token_type) {
	if (check_type(token_type)) {
		advance();
		return true;
	}
	else {
		return false;
	}
}

// Hand back the token if it's the right type
Token Parser::expect_type(TokenType token_type) {
	if (check_type(token_type)) {
		return advance();
	}
	// error if it's missing
	else {
		parser_failure = true;
		std::cerr << "Error on line " << read_token().line << ": expected " << token_type_to_string(token_type) << " but got " << read_token().lexeme << "\n";
		return tokens[current_index];
	}
}

std::unique_ptr<Expr> Parser::parse_prefix() {
	Token token = advance();

	if (token.token_type == TokenType::NUMBER ||
		token.token_type == TokenType::STRING ||
		token.token_type == TokenType::TRUE ||
		token.token_type == TokenType::FALSE ||
		token.token_type == TokenType::NIL)
	{
		// build a literal expr
		return std::make_unique<LiteralExpr>(token);
	} 
	else if (token.token_type == TokenType::IDENTIFIER)
	{
		// build a variable expr
		return std::make_unique<VariableExpr>(token);
	}
	else if (token.token_type == TokenType::MINUS
		|| token.token_type == TokenType::NOT)
	{
		// get left child
		std::unique_ptr<Expr> left_child = parse_expression(5);

		// build unary expr
		return std::make_unique<UnaryExpr>(token, std::move(left_child));
	}
	else if (token.token_type == TokenType::LEFT_PAREN)
	{
		// get child
		std::unique_ptr<Expr> child = parse_expression(0);
			
		expect_type(TokenType::RIGHT_PAREN);

		// build a grouping expr
		return std::make_unique<GroupingExpr>(std::move(child));
	}
	else {
		parser_failure = true;
		std::cerr << "Error on line " << token.line << ": expected expression" << " but got " << token.lexeme << "\n";
		return nullptr;
	}
}

bool Parser::had_error() const {
	return parser_failure;
}

int Parser::binding_power(TokenType token_type) const {
	switch (token_type) {

	case TokenType::EQUAL_EQUAL:
	case TokenType::NOT_EQUAL:
		return 1;

	case TokenType::LESS_THAN:
	case TokenType::LESS_EQUAL:
	case TokenType::GREATER_THAN:
	case TokenType::GREATER_EQUAL:
		return 2;

	case TokenType::PLUS:
	case TokenType::MINUS:
		return 3;

	case TokenType::STAR:
	case TokenType::SLASH:
		return 4;

	default:
		return 0;

	}
}

std::unique_ptr<Expr> Parser::parse_expression(int min_power) {

	// Prefix 
	std::unique_ptr<Expr> left_child = parse_prefix();

	while (true) {

		if (check_type(TokenType::LEFT_PAREN)) {
			Token paren = advance();
			std::vector<std::unique_ptr<Expr>> arguments;
			while (!check_type(TokenType::RIGHT_PAREN) && !is_at_end() && !parser_failure) {
				arguments.push_back(parse_assignment());
				match_type(TokenType::COMMA);
			}
			expect_type(TokenType::RIGHT_PAREN);
			left_child = std::make_unique<CallExpr>(std::move(left_child), paren, std::move(arguments));
			continue;
		}

		int operating_power = binding_power(read_token().token_type);
		if (operating_power > min_power) {
			Token token = advance();
			std::unique_ptr<Expr> right_child = parse_expression(operating_power);
			left_child = std::make_unique<BinaryExpr>(token, std::move(left_child), std::move(right_child));
		}
		else {
			break;
		}
	}

	return left_child;
}

std::unique_ptr<Stmt> Parser::parse_statement() {

	if (match_type(TokenType::FN)) {
		Token name = expect_type(TokenType::IDENTIFIER);
		expect_type(TokenType::LEFT_PAREN);
		std::vector<Token> params;
		while (!check_type(TokenType::RIGHT_PAREN) && !is_at_end() && !parser_failure) {
			params.push_back(expect_type(TokenType::IDENTIFIER));
			match_type(TokenType::COMMA);
		}
		expect_type(TokenType::RIGHT_PAREN);
		expect_type(TokenType::LEFT_BRACE);
		std::vector<std::unique_ptr<Stmt>> statements;
		while (!check_type(TokenType::RIGHT_BRACE) && !is_at_end()) {
			statements.push_back(parse_statement());
		}
		expect_type(TokenType::RIGHT_BRACE);

		return std::make_unique<FnStmt>(name, std::move(params), std::move(statements));
	}

	if (match_type(TokenType::IF)) {
		expect_type(TokenType::LEFT_PAREN);
		std::unique_ptr<Expr> expr = parse_assignment();
		expect_type(TokenType::RIGHT_PAREN);
		std::unique_ptr<Stmt> then_branch = parse_statement();
		std::unique_ptr<Stmt> else_branch = nullptr;
		if (match_type(TokenType::ELSE)) {
			else_branch = parse_statement();
		}

		return std::make_unique<IfStmt>(std::move(expr), std::move(then_branch), std::move(else_branch));
	}

	if (match_type(TokenType::WHILE)) {
		expect_type(TokenType::LEFT_PAREN);
		std::unique_ptr<Expr> expr = parse_assignment();
		expect_type(TokenType::RIGHT_PAREN);
		std::unique_ptr<Stmt> body = parse_statement();

		return std::make_unique<WhileStmt>(std::move(expr), std::move(body));
	}

	if (match_type(TokenType::FOR)) {
		expect_type(TokenType::LEFT_PAREN);

		std::unique_ptr<Stmt> initializer = parse_statement();
		std::unique_ptr<Expr> condition = parse_assignment();

		expect_type(TokenType::SEMICOLON);

		std::unique_ptr<Expr> increment = parse_assignment();

		expect_type(TokenType::RIGHT_PAREN);

		std::unique_ptr<Stmt>body = parse_statement();

		std::vector<std::unique_ptr<Stmt>> statements;
		statements.push_back(std::move(body));
		statements.push_back(std::make_unique<ExpressionStmt>(std::move(increment)));

		std::unique_ptr<Stmt> block_statement = std::make_unique<BlockStmt>(std::move(statements));
		std::unique_ptr<WhileStmt> while_statement = std::make_unique<WhileStmt>(std::move(condition), std::move(block_statement));

		std::vector<std::unique_ptr<Stmt>> inner_vector;
		inner_vector.push_back(std::move(initializer));
		inner_vector.push_back(std::move(while_statement));

		return std::make_unique<BlockStmt>(std::move(inner_vector));
	}

	if (match_type(TokenType::LEFT_BRACE)) {
		std::vector<std::unique_ptr<Stmt>> statements;
		while (!check_type(TokenType::RIGHT_BRACE) && !is_at_end()) {
			statements.push_back(parse_statement());
		}
		expect_type(TokenType::RIGHT_BRACE);
		return std::make_unique<BlockStmt>(std::move(statements));
	}

	if (match_type(TokenType::LET)) {
		Token name = expect_type(TokenType::IDENTIFIER);
		expect_type(TokenType::EQUAL);
		std::unique_ptr<Expr> expr = parse_assignment();
		expect_type(TokenType::SEMICOLON);
		return std::make_unique<LetStmt>(name, std::move(expr));
	}

	if (match_type(TokenType::PRINT)) {
		std::unique_ptr<Expr> expr = parse_assignment();
		expect_type(TokenType::SEMICOLON);
		return std::make_unique<PrintStmt>(std::move(expr));
	}

	if (match_type(TokenType::RETURN)) {
		std::unique_ptr<Expr> value = nullptr;
		if (!check_type(TokenType::SEMICOLON)) {
			value = parse_assignment();
		}
		expect_type(TokenType::SEMICOLON);
		return std::make_unique<ReturnStmt>(std::move(value));
	}

	std::unique_ptr<Expr> expr = parse_assignment();
	expect_type(TokenType::SEMICOLON);
	return std::make_unique<ExpressionStmt>(std::move(expr));
}

std::vector<std::unique_ptr<Stmt>> Parser::parse_program() {
	std::vector<std::unique_ptr<Stmt>> statements;

	while (!is_at_end()) {
		statements.push_back(parse_statement());
	}

	return statements;
}

std::unique_ptr<Expr> Parser::parse_assignment() {
	std::unique_ptr<Expr> child_left = parse_expression(0);

	if (match_type(TokenType::EQUAL)) {
		std::unique_ptr<Expr> child_right = parse_assignment();
		if (dynamic_cast<VariableExpr*>(child_left.get())) {
			VariableExpr* variable = dynamic_cast<VariableExpr*>(child_left.get());
			return std::make_unique<AssignExpr>(variable->name_token, std::move(child_right));
		}
		else {
			parser_failure = true;
			std::cerr << "Error on line " << read_previous_token().line << ": invalid assignment target" << "\n";
		}
	}
	return child_left;
}