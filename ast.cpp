#include "ast.h"
#include "stmt.h"

LiteralExpr::LiteralExpr(Token token) {
	literal_token = std::move(token);
}

std::string LiteralExpr::print() const {
	return std::string(literal_token.lexeme);
}

Value LiteralExpr::evaluate(Environment&) const {
	switch (literal_token.token_type) {
	case TokenType::TRUE:
		return true;
	case TokenType::FALSE:
		return false;
	case TokenType::NIL:
		return std::monostate{};
	case TokenType::NUMBER:
		return std::stod(std::string(literal_token.lexeme));
	case TokenType::STRING:
		return std::string(literal_token.lexeme.substr(1, literal_token.lexeme.size() - 2));
	default:
		return std::monostate{};

	}
}

BinaryExpr::BinaryExpr(Token token, std::unique_ptr<Expr> left, std::unique_ptr<Expr> right) {
	operator_token = std::move(token);
	child_left = std::move(left);
	child_right = std::move(right);
}

std::string BinaryExpr::print() const {
	return "(" + std::string(operator_token.lexeme) + " " + child_left->print() + " " + child_right->print() + ")";
}

void BinaryExpr::expect_numbers(Value left_child_value, Value right_child_value) const {
	if (!(std::holds_alternative<double>(left_child_value) && std::holds_alternative<double>(right_child_value))) {
		throw std::runtime_error("Error on line " + std::to_string(operator_token.line) + ": operands must be two numbers!");
	}
}

Value BinaryExpr::evaluate(Environment& environment) const {
	Value left_child_value = child_left->evaluate(environment);
	Value right_child_value = child_right->evaluate(environment);

	switch (operator_token.token_type) {
	case TokenType::PLUS:
		if (std::holds_alternative<std::string>(left_child_value) && std::holds_alternative<std::string>(right_child_value)) {
			return std::get<std::string>(left_child_value) + std::get<std::string>(right_child_value);
		}
		expect_numbers(left_child_value, right_child_value);
		return std::get<double>(left_child_value) + std::get<double>(right_child_value);
	case TokenType::MINUS:
		expect_numbers(left_child_value, right_child_value);
		return std::get<double>(left_child_value) - std::get<double>(right_child_value);
	case TokenType::STAR:
		expect_numbers(left_child_value, right_child_value);
		return std::get<double>(left_child_value) * std::get<double>(right_child_value);
	case TokenType::SLASH:
		expect_numbers(left_child_value, right_child_value);
		if (std::get<double>(right_child_value) == 0) {
			throw std::runtime_error("Error on line " + std::to_string(operator_token.line) + ": division by zero!");
		}
		return std::get<double>(left_child_value) / std::get<double>(right_child_value);
	case TokenType::LESS_THAN:
		expect_numbers(left_child_value, right_child_value);
		return std::get<double>(left_child_value) < std::get<double>(right_child_value);
	case TokenType::LESS_EQUAL:
		expect_numbers(left_child_value, right_child_value);
		return std::get<double>(left_child_value) <= std::get<double>(right_child_value);
	case TokenType::GREATER_THAN:
		expect_numbers(left_child_value, right_child_value);
		return std::get<double>(left_child_value) > std::get<double>(right_child_value);
	case TokenType::GREATER_EQUAL:
		expect_numbers(left_child_value, right_child_value);
		return std::get<double>(left_child_value) >= std::get<double>(right_child_value);
	case TokenType::EQUAL_EQUAL:
		return left_child_value == right_child_value;
	case TokenType::NOT_EQUAL:
		return left_child_value != right_child_value;

	default:
		return std::monostate{};
	}
}


UnaryExpr::UnaryExpr(Token token, std::unique_ptr<Expr> child) {
	operator_token = std::move(token);
	child_expr = std::move(child);
}

std::string UnaryExpr::print() const {
	return "(" + std::string(operator_token.lexeme) + " " + child_expr->print() + ")";
}

Value UnaryExpr::evaluate(Environment& environment) const {
	Value child_value = child_expr->evaluate(environment);

	switch (operator_token.token_type) {
	case TokenType::MINUS:
		if (!std::holds_alternative<double>(child_value)) {
			throw std::runtime_error("Error on line " + std::to_string(operator_token.line) + ": operand must be a number!");
		}
		return -std::get<double>(child_value);
	case TokenType::NOT:
		if (!std::holds_alternative<bool>(child_value)) {
			throw std::runtime_error("Error on line " + std::to_string(operator_token.line) + ": operand must be a bool!");
		}
		return !std::get<bool>(child_value);

	default:
		return std::monostate{};
	}
}

GroupingExpr::GroupingExpr(std::unique_ptr<Expr> child) {
	child_expr = std::move(child);
}

std::string GroupingExpr::print() const {
	return "(group " + child_expr->print() + ")";
}

Value GroupingExpr::evaluate(Environment& environment) const {
	return child_expr->evaluate(environment);
}

VariableExpr::VariableExpr(Token token) {
	name_token = std::move(token);
}

std::string VariableExpr::print() const {
	return std::string(name_token.lexeme);
}

Value VariableExpr::evaluate(Environment& environment) const {
	return environment.get(name_token);
}

AssignExpr::AssignExpr(Token name_token, std::unique_ptr<Expr> value) {
	this->name_token = std::move(name_token);
	child_right = std::move(value);
}

std::string AssignExpr::print() const {
	return "(= " + std::string(name_token.lexeme) + " " + child_right->print() + ")";
}

Value AssignExpr::evaluate(Environment& environment) const {
	Value value = child_right->evaluate(environment);
	environment.assign(name_token, value);
	return value;
}

CallExpr::CallExpr(std::unique_ptr<Expr> callee, Token paren, std::vector<std::unique_ptr<Expr>> arguments) {
	this->callee = std::move(callee);
	this->paren = std::move(paren);
	this->arguments = std::move(arguments);
}

std::string CallExpr::print() const {
	return "(call " + callee->print() + ")";
}

Value CallExpr::evaluate(Environment& environment) const {
	Value value = callee->evaluate(environment);
	if (!std::holds_alternative<std::shared_ptr<Function>>(value)) {
		throw std::runtime_error("Error on line " + std::to_string(paren.line) + ": can only call functions!");
	}
	std::shared_ptr<Function> function = std::get<std::shared_ptr<Function>>(value);
	if (arguments.size() != function->declaration->params.size()) {
		throw std::runtime_error("Error on line " + std::to_string(paren.line) + ": expected " + std::to_string(function->declaration->params.size()) + " arguments but got " + std::to_string(arguments.size()));
	}
	std::shared_ptr<Environment> call_environment = std::make_shared<Environment>(function->closure);
	for (size_t i = 0; i < arguments.size(); i++) {
		call_environment->define(function->declaration->params[i], arguments[i]->evaluate(environment));
	}
	try {
		for (const auto& statement : function->declaration->body) {
			statement->execute(*call_environment);
		}
	}
	catch (const ReturnValue& returned) { return returned.value; }

	return std::monostate{};
}