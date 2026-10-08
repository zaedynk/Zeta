#pragma once
#include <unordered_map>
#include <stdexcept>
#include <string>
#include <memory>

#include "value.h"
#include "token.h"


class Environment : public std::enable_shared_from_this<Environment> {
	std::unordered_map<std::string, Value> values;
	std::shared_ptr<Environment> parent = nullptr;

public:

	Environment() = default;

	explicit Environment(std::shared_ptr<Environment> parent) {
		this->parent = std::move(parent);
	}

	void define(Token name, Value value) {
		values[std::string(name.lexeme)] = value;
	}

	Value get(Token name) const {
		std::string key = std::string(name.lexeme);
		if (values.contains(key)) {
			return values.at(key);
		}
		else if (parent != nullptr) {
			return parent->get(name);
		}
		else {
			throw std::runtime_error("Error on line " + std::to_string(name.line) + ": undefined variable '" + key + "'");
		}
	}

	void assign(Token name, Value value) {
		std::string key = std::string(name.lexeme);
		if (values.contains(key)) {
			values[key] = value;
		}
		else if (parent != nullptr) {
			parent->assign(name, value);
		}
		else {
			throw std::runtime_error("Error on line " + std::to_string(name.line) + ": undefined variable '" + key + "'");
		}
	}


};