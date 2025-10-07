#include <napi.h>
#include <vector>
#include <string>
#include <cmath>

using namespace std;

// Tokenize input string
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

// Evaluate tokens
double evaluate(vector<string> tokens) {
    // ^ first
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

    // * and /
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

    // + and -
    double result = stod(tokens[0]);
    for (size_t i = 1; i < tokens.size(); i += 2) {
        string op = tokens[i];
        double val = stod(tokens[i + 1]);
        if (op == "+") result += val;
        else if (op == "-") result -= val;
    }

    return result;
}

// N-API wrapper
Napi::Number EvaluateWrapped(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    string expr = info[0].As<Napi::String>();
    vector<string> tokens = tokenize(expr);
    double result = evaluate(tokens);
    return Napi::Number::New(env, result);
}

// Module initialization
Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set("evaluate", Napi::Function::New(env, EvaluateWrapped));
    return exports;
}

NODE_API_MODULE(calculator, Init)
