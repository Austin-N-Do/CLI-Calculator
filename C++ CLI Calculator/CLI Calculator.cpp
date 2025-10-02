#include <iostream>
#include <sstream>
#include <vector>
#include <string>

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

// Evaluate expression with simple precedence (*,/ before +,-)
double evaluate(const vector<string>& tokens) {
    vector<string> temp = tokens;

    // First pass: handle * and /
    for (size_t i = 0; i < temp.size(); ) {
        if (temp[i] == "*" || temp[i] == "/") {
            double left = stod(temp[i-1]);
            double right = stod(temp[i+1]);
            double result = (temp[i] == "*") ? left * right : left / right;

            // Replace three tokens with result
            temp[i-1] = to_string(result);
            temp.erase(temp.begin() + i, temp.begin() + i + 2);
        } else {
            ++i;
        }
    }

    // Second pass: handle + and -
    double result = stod(temp[0]);
    for (size_t i = 1; i < temp.size(); i += 2) {
        string op = temp[i];
        double val = stod(temp[i+1]);
        if (op == "+") result += val;
        else if (op == "-") result -= val;
    }

    return result;
}

int main() {
    cout << "CLI Calculator (type 'quit' to exit)\n";
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
