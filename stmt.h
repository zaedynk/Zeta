#pragma once
#include <vector>
#include "ast.h"
#include "environment.h"

class Stmt {
public:
	Stmt() = default;

	virtual ~Stmt() {}

	virtual void execute(Environment& environment) const = 0;
};

class PrintStmt : public Stmt {
public:
	std::unique_ptr<Expr> expr;

	explicit PrintStmt(std::unique_ptr<Expr> expression);

	void execute(Environment& environment) const override;
};

class ExpressionStmt : public Stmt {
public:
	std::unique_ptr<Expr> expr;

	explicit ExpressionStmt(std::unique_ptr<Expr> expression);

	void execute(Environment& environment) const override;
};

class LetStmt : public Stmt {
public:
	Token name_token;
	std::unique_ptr<Expr> expr;

	explicit LetStmt(Token token, std::unique_ptr<Expr> expression);

	void execute(Environment& environment) const override;
};

class BlockStmt : public Stmt {
public:
	std::vector<std::unique_ptr<Stmt>> statements;

	explicit BlockStmt(std::vector<std::unique_ptr<Stmt>> statements);

	void execute(Environment& environment) const override;
};

class IfStmt : public Stmt {
public:
	std::unique_ptr<Expr> condition;
	std::unique_ptr<Stmt> then_branch;
	std::unique_ptr<Stmt> else_branch;

	explicit IfStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> then_branch, std::unique_ptr<Stmt> else_branch);

	void execute(Environment& environment) const override;
};

class WhileStmt : public Stmt {
public:
	std::unique_ptr<Expr> condition;
	std::unique_ptr<Stmt> body;

	explicit WhileStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> body);

	void execute(Environment& environment) const override;
};

class FnStmt : public Stmt {
public:
	Token name_token;
	std::vector<Token> params;
	std::vector<std::unique_ptr<Stmt>> body;

	explicit FnStmt(Token name_token, std::vector<Token> params, std::vector<std::unique_ptr<Stmt>> body);

	void execute(Environment& environment) const override;
};

struct Function {
	const FnStmt* declaration;
	std::shared_ptr<Environment> closure;
};

struct ReturnValue { Value value; };

class ReturnStmt : public Stmt {
public:
	std::unique_ptr<Expr> value;

	explicit ReturnStmt(std::unique_ptr<Expr> expr);

	void execute(Environment& environment) const override;
};
