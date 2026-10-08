#include "stmt.h"
#include <iostream>

PrintStmt::PrintStmt(std::unique_ptr<Expr> expression) {
	expr = std::move(expression);
}

void PrintStmt::execute(Environment& environment) const {
	Value value = expr->evaluate(environment);

	if (std::holds_alternative<double>(value)) {
		std::cout << std::get<double>(value);
	}
	else if (std::holds_alternative<bool>(value)) {
		std::cout << std::boolalpha << std::get<bool>(value);
	}
	else if (std::holds_alternative<std::string>(value)) {
		std::cout << std::get<std::string>(value);
	}
	else if (std::holds_alternative<std::monostate>(value)) {
		std::cout << "nil";
	}
	else if (std::holds_alternative<std::shared_ptr<Function>>(value)) {
		std::cout << "<fn " << std::get<std::shared_ptr<Function>>(value)->declaration->name_token.lexeme << ">";
	}

	std::cout << "\n";
}

ExpressionStmt::ExpressionStmt(std::unique_ptr<Expr> expression) {
	expr = std::move(expression);
}

void ExpressionStmt::execute(Environment& environment) const {
	expr->evaluate(environment);
}

LetStmt::LetStmt(Token token, std::unique_ptr<Expr> expression) {
	name_token = std::move(token);
	expr = std::move(expression);
}

void LetStmt::execute(Environment& environment) const {
	Value value = expr->evaluate(environment);
	environment.define(name_token, value);
}

BlockStmt::BlockStmt(std::vector<std::unique_ptr<Stmt>> statements) {
	this->statements = std::move(statements);
}

void BlockStmt::execute(Environment& environment) const {
	std::shared_ptr<Environment> inner = std::make_shared<Environment>(environment.shared_from_this());
	for (const auto& statement : statements) {
		statement->execute(*inner);
	}
}

IfStmt::IfStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> then_branch, std::unique_ptr<Stmt> else_branch) {
	this->condition = std::move(condition);
	this->then_branch = std::move(then_branch);
	this->else_branch = std::move(else_branch);
}

void IfStmt::execute(Environment& environment) const {
	Value value = condition->evaluate(environment);

	if (!std::holds_alternative<bool>(value)) {
		throw std::runtime_error("Condition must be a bool!");
	}

	if (std::get<bool>(value)) {
		then_branch->execute(environment);
	}
	else if (else_branch != nullptr) {
		else_branch->execute(environment);
	}
}

WhileStmt::WhileStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> body) {
	this->condition = std::move(condition);
	this->body = std::move(body);
}

void WhileStmt::execute(Environment& environment) const {
	Value value = condition->evaluate(environment);

	if (!std::holds_alternative<bool>(value)) {
		throw std::runtime_error("Condition must be a bool!");
	}

	while (std::get<bool>(value)) {
		body->execute(environment);
		value = condition->evaluate(environment);
		if (!std::holds_alternative<bool>(value)) {
			throw std::runtime_error("Condition must be a bool!");
		}
	}
}

FnStmt::FnStmt(Token name_token, std::vector<Token> params, std::vector<std::unique_ptr<Stmt>> body) {
	this->name_token = std::move(name_token);
	this->params = std::move(params);
	this->body = std::move(body);
}

void FnStmt::execute(Environment& environment) const {
	std::shared_ptr<Function> function = std::make_shared<Function>();
	function->declaration = this;
	function->closure = environment.shared_from_this();
	environment.define(name_token, function);
}

ReturnStmt::ReturnStmt(std::unique_ptr<Expr> expr) {
	value = std::move(expr);
}

void ReturnStmt::execute(Environment& environment) const {
	Value result = std::monostate{};
	if (value != nullptr) {
		result = value->evaluate(environment);
	}
	throw ReturnValue{ result };
}