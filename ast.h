#pragma once
#include <memory>
#include <stdexcept>
#include <vector>

#include "token.h"
#include "value.h"
#include "environment.h"

class Expr {
public:
	Expr() = default;

	virtual ~Expr() {}

	virtual std::string print() const = 0;

	virtual Value evaluate(Environment& environment) const = 0;
};

class LiteralExpr : public Expr {
public:

	// 5, "hi", true, nil
	Token literal_token;

	explicit LiteralExpr(Token token);

	std::string print() const override;

	Value evaluate(Environment& environment) const override;
};

// a left child, the operator, and a right child.
class BinaryExpr : public Expr {
public:

	// a + b
	Token operator_token;
	std::unique_ptr<Expr> child_left;
	std::unique_ptr<Expr> child_right;

	explicit BinaryExpr(Token token, std::unique_ptr<Expr> left, std::unique_ptr<Expr> right);

	std::string print() const override;

	void expect_numbers(Value left_child_value, Value right_child_value) const;

	Value evaluate(Environment& environment) const override;
};

// The operator (- or !) and one child expression.
class UnaryExpr : public Expr {
public:

	// -x, !x
	Token operator_token;
	std::unique_ptr<Expr> child_expr;

	explicit UnaryExpr(Token token, std::unique_ptr<Expr> child);

	std::string print() const override;

	Value evaluate(Environment& environment) const override;
};

// one child, the expression inside the parentheses.
class GroupingExpr : public Expr {
public:
	
	// (a)
	std::unique_ptr<Expr> child_expr;

	explicit GroupingExpr(std::unique_ptr<Expr> child);

	std::string print() const override;

	Value evaluate(Environment& environment) const override;
};

class VariableExpr : public Expr {
public:
	
	// x
	Token name_token;

	explicit VariableExpr(Token token);

	std::string print() const override;

	Value evaluate(Environment& environment) const override;
};

class AssignExpr : public Expr {
public:
	
	// x = value
	Token name_token;
	std::unique_ptr<Expr> child_right;

	explicit AssignExpr(Token name_token, std::unique_ptr<Expr> value);

	std::string print() const override;

	Value evaluate(Environment& environment) const override;
};

class CallExpr : public Expr {
public:
	
	// f(a, b)
	std::unique_ptr<Expr> callee;
	Token paren;
	std::vector<std::unique_ptr<Expr>> arguments;

	explicit CallExpr(std::unique_ptr<Expr> callee, Token paren, std::vector<std::unique_ptr<Expr>> arguments);

	std::string print() const override;

	Value evaluate(Environment& environment) const override;
};
