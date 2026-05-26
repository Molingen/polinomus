/*  с чем работает:
 *  - операторы: +, -, *
 *  - скобки: ( )
 *  - мономы: 2*x^2*y
 *  - умножение только явное: 2*x, x*y
 *
 *  что я не захотел делать:
 *  - унарный минус
 *  - неявное умножение
 *  - деление полиномов
 */

#pragma once
#include <stack>
#include <string>
#include <cctype>
#include <stdexcept>

#include "polinomus.hpp"

class poly_parser {
public:

    static Polynomus parse(const std::string& expr) {
        std::stack<Polynomus> values;
        std::stack<char> ops;

        size_t i = 0;
        while (i < expr.size()) {

            if (std::isspace(expr[i])) {
                i++;
                continue;
            }
            if (expr[i] == '(') {
                ops.push('(');
                i++;
            }
            else if (expr[i] == ')') {
                while (!ops.empty() && ops.top() != '(') {
                    applyOp(values, ops.top());
                    ops.pop();
                }

                if (ops.empty())
                    throw std::runtime_error("Mismatched parentheses");

                ops.pop();
                i++;
            }

            else if (expr[i] == '+' || expr[i] == '-' || expr[i] == '*') {
                char op = expr[i];

                while (!ops.empty() && priority(ops.top()) >= priority(op)) {
                    applyOp(values, ops.top());
                    ops.pop();
                }

                ops.push(op);
                i++;
            }

            else if (std::isalnum(expr[i])) {
                std::string token;
                while (i < expr.size() &&
                       (std::isalnum(expr[i]) || expr[i] == '^')) {
                    token += expr[i++];
                }

                values.push(makePoly(token));
            }

            else {
                throw std::runtime_error("Invalid character in expression");
            }
        }

        while (!ops.empty()) {
            if (ops.top() == '(')
                throw std::runtime_error("Mismatched parentheses");
            applyOp(values, ops.top());
            ops.pop();
        }
        if (values.size() != 1)
            throw std::runtime_error("Invalid expression");
        return values.top();
    }

private:

    static int priority(char op) {
        if (op == '+' || op == '-') return 1;
        if (op == '*') return 2;
        return 0;
    }

    static void applyOp(std::stack<Polynomus>& values, char op) {
        if (values.size() < 2)
            throw std::runtime_error("Invalid expression");

        Polynomus b = values.top(); values.pop();
        Polynomus a = values.top(); values.pop();

        if (op == '+') values.push(a + b);
        else if (op == '-') values.push(a - b);
        else if (op == '*') values.push(a * b);
    }

    static Monomus parseMonom(const std::string& token) {
        double coeff = 1.0;
        uint32_t x = 0, y = 0, z = 0;

        size_t i = 0;
        if (i < token.size() && std::isdigit(token[i])) {
            coeff = 0;
            while (i < token.size() && std::isdigit(token[i])) {
                coeff = coeff * 10 + (token[i] - '0');
                i++;
            }
        }

        while (i < token.size()) {
            char var = token[i++];
            uint32_t power = 1;

            if (i < token.size() && token[i] == '^') {
                i++;
                power = 0;
                if (i >= token.size() || !std::isdigit(token[i]))
                    throw std::runtime_error("Invalid power");

                while (i < token.size() && std::isdigit(token[i])) {
                    power = power * 10 + (token[i] - '0');
                    i++;
                }
            }

            if (var == 'x') x = power;
            else if (var == 'y') y = power;
            else if (var == 'z') z = power;
            else throw std::runtime_error("Unknown variable");
        }

        return {x, y, z, coeff};
    }

    static Polynomus makePoly(const std::string& token) {
        Polynomus p;
        p.addMonom(parseMonom(token));
        return p;
    }

};

