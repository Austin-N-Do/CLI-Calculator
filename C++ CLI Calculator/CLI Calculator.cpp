#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath> // for pow()

using namespace std;

// Helper: split input into tokens (numbers and operators)
vector<string> tokenize(const string& expr) {
    vector<string> tokens;
    string token;
    for (char c : expr) {
        if (isspace(c)) continue;
        if (isdigit(c) || c == '.') {
            token += c;
        } else {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
            tokens.push_back(string(1, c));
        }
    }
    if (!token.empty()) tokens.push_back(token);
    return tokens;
}

// Evaluate expression with precedence (^, then *, /, then +, -)
double evaluate(vector<string> tokens) {
    // Pass 1: handle ^
    for (size_t i = 0; i < tokens.size(); ) {
        if (tokens[i] == "^") {
            double left = stod(tokens[i - 1]);
            double right = stod(tokens[i + 1]);
            double result = pow(left, right);
            tokens[i - 1] = to_string(result);
            tokens.erase(tokens.begin() + i, tokens.begin() + i + 2);
        } else {
            ++i;
        }
    }

    // Pass 2: handle * and /
    for (size_t i = 0; i < tokens.size(); ) {
        if (tokens[i] == "*" || tokens[i] == "/") {
            double left = stod(tokens[i - 1]);
            double right = stod(tokens[i + 1]);
            double result = (tokens[i] == "*") ? left * right : left / right;
            tokens[i - 1] = to_string(result);
            tokens.erase(tokens.begin() + i, tokens.begin() + i + 2);
        } else {
            ++i;
        }
    }

    // Pass 3: handle + and -
    double result = stod(tokens[0]);
    for (size_t i = 1; i < tokens.size(); i += 2) {
        string op = tokens[i];
        double val = stod(tokens[i + 1]);
        if (op == "+") result += val;
        else if (op == "-") result -= val;
    }

    return result;
}

int main() {
    cout << "CLI Calculator (supports +, -, *, /, ^)\nType 'quit' to exit.\n";
    string line;

    while (true) {
        cout << "Calculate: ";
        getline(cin, line);
        if (line == "quit") break;
        if (line.empty()) continue;

        try {
            vector<string> tokens = tokenize(line);
            double result = evaluate(tokens);
            cout << "= " << result << "\n";
        } catch (...) {
            cout << "Error: invalid expression\n";
        }
    }

    return 0;
}
