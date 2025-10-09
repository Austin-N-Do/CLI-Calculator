#include <napi.h>
#include <vector>
#include <string>
#include <cmath>
#include <stdexcept>

using namespace std;

// Tokenize input string
vector<string> tokenize(const string& expr) {
    vector<string> tokens;
    string token;
    for (size_t i = 0; i < expr.length(); i++) {
        char c = expr[i];
        if (isspace(c)) continue;
        
        // Handle negative numbers
        if (c == '-' && (i == 0 || expr[i-1] == '(' || expr[i-1] == '+' || 
                         expr[i-1] == '-' || expr[i-1] == '*' || 
                         expr[i-1] == '/' || expr[i-1] == '^')) {
            token += c;
        }
        else if (isdigit(c) || c == '.') {
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

// Get operator precedence
int precedence(const string& op) {
    if (op == "+" || op == "-") return 1;
    if (op == "*" || op == "/") return 2;
    if (op == "^") return 3;
    return 0;
}

// Check if operator is right-associative
bool isRightAssociative(const string& op) {
    return op == "^";
}

// Apply operator to two operands
double applyOp(double left, double right, const string& op) {
    if (op == "+") return left + right;
    if (op == "-") return left - right;
    if (op == "*") return left * right;
    if (op == "/") {
        if (right == 0) throw runtime_error("Division by zero");
        return left / right;
    }
    if (op == "^") return pow(left, right);
    throw runtime_error("Unknown operator: " + op);
}

// Evaluate using Shunting Yard algorithm
double evaluate(vector<string> tokens) {
    vector<double> values;
    vector<string> ops;
    
    for (size_t i = 0; i < tokens.size(); i++) {
        const string& token = tokens[i];
        
        // If token is a number
        if (isdigit(token[0]) || (token[0] == '-' && token.length() > 1)) {
            values.push_back(stod(token));
        }
        // If token is an operator
        else if (token == "+" || token == "-" || token == "*" || 
                 token == "/" || token == "^") {
            
            // Process operators with higher or equal precedence
            while (!ops.empty()) {
                string topOp = ops.back();
                int topPrec = precedence(topOp);
                int currPrec = precedence(token);
                
                // For right-associative operators like ^, only pop if strictly greater
                bool shouldPop;
                if (isRightAssociative(token)) {
                    shouldPop = topPrec > currPrec;
                } else {
                    shouldPop = topPrec >= currPrec;
                }
                
                if (!shouldPop) break;
                
                // Pop and apply operator
                if (values.size() < 2) {
                    throw runtime_error("Invalid expression");
                }
                double right = values.back(); values.pop_back();
                double left = values.back(); values.pop_back();
                values.push_back(applyOp(left, right, topOp));
                ops.pop_back();
            }
            
            ops.push_back(token);
        }
        else {
            throw runtime_error("Invalid token: " + token);
        }
    }
    
    // Apply remaining operators
    while (!ops.empty()) {
        if (values.size() < 2) {
            throw runtime_error("Invalid expression");
        }
        double right = values.back(); values.pop_back();
        double left = values.back(); values.pop_back();
        values.push_back(applyOp(left, right, ops.back()));
        ops.pop_back();
    }
    
    if (values.size() != 1) {
        throw runtime_error("Invalid expression");
    }
    
    return values[0];
}

// N-API wrapper
Napi::Number EvaluateWrapped(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 1 || !info[0].IsString()) {
        Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
        return Napi::Number::New(env, 0);
    }
    
    string expr = info[0].As<Napi::String>();
    
    try {
        vector<string> tokens = tokenize(expr);
        double result = evaluate(tokens);
        return Napi::Number::New(env, result);
    } catch (const exception& e) {
        Napi::Error::New(env, e.what()).ThrowAsJavaScriptException();
        return Napi::Number::New(env, 0);
    }
}

// Module initialization
Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set("evaluate", Napi::Function::New(env, EvaluateWrapped));
    return exports;
}

NODE_API_MODULE(calculator, Init)