#pragma once
#include <variant>
#include <string>
#include <memory>

struct Function;
using Value = std::variant<std::monostate, double, bool, std::string, std::shared_ptr<Function>>;

