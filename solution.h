#include <iostream>
#include <string>
#include <vector>
#include <memory>

class Token {
public:
    public: virtual ~Token() {}
};

class NumberToken : public Token {
public:
    const int value;

    NumberToken(int v) : value(v) {}
};

class TextToken : public Token {
public:
    const std::string text;

    TextToken(const std::string& s) : text(s) {}
};

class OpenBracketToken : public Token {};
class CloseBracketToken : public Token {};


class Tokenizer {
private:
    bool IsBracket(char c) {
        return c == '[' || c == ']';
    }
    bool IsDigit(char c) {
        return '0' <= c && c <= '9';
    }

    std::shared_ptr<Token> MakeToken(const std::string& s) {
        if (s.length() < 2) {
            if (s[0] == '[') {
                return std::make_shared<OpenBracketToken>();
            }
            if (s[0] == ']') {
                return std::make_shared<CloseBracketToken>();
            }
        }
        if (IsDigit(s[0])) {
            return std::make_shared<NumberToken>(std::stoi(s));
        }
        return std::make_shared<TextToken>(s);
    }

public:
    std::vector<std::shared_ptr<Token>> Tokenize(const std::string& s) {
        std::vector<std::shared_ptr<Token>> tokens;
        std::string brackets = "[]";

        size_t left = 0, right = 0;
        for (; right < s.length(); ++right) {
            char c = s[right];
            if (IsBracket(c)) {
                if (right - left > 0) {
                    tokens.push_back(MakeToken(s.substr(left, right - left)));
                }

                tokens.push_back(MakeToken({c}));

                left = right + 1;
                continue;
            }
            if (right - left > 0 && IsDigit(s[left]) != IsDigit(s[right])) {
                tokens.push_back(MakeToken(s.substr(left, right - left)));
                left = right;
                continue;
            }
        }
        if (right - left > 0) {
            tokens.push_back(MakeToken(s.substr(left, right - left)));
        }

        return tokens;
    }
};


class Node {
public:
    Node() {}

    public: virtual ~Node() {}
};

class LeafNode : public Node {
public:
    std::string data;

    LeafNode(std::string s) : data(s) {}
};

class InnerNode : public Node {
public:
    std::weak_ptr<InnerNode> parent;
    int multiplier = 1;
    std::vector<std::shared_ptr<Node>> children;

    InnerNode() : parent(), multiplier(1) {}
    InnerNode(std::weak_ptr<InnerNode> p, int m) : parent(p), multiplier(m) {}
};


class Parser {
public:
    std::shared_ptr<InnerNode> BuildTree(std::vector<std::shared_ptr<Token>> tokens) {
        std::shared_ptr<InnerNode> root = std::make_shared<InnerNode>();
        std::shared_ptr<InnerNode> current = root;

        for (std::shared_ptr<Token> token : tokens) {
            if (auto textToken = std::dynamic_pointer_cast<TextToken>(token); textToken != nullptr) {
                current->children.push_back(std::make_shared<LeafNode>(textToken->text));
            
            } else if (auto numberToken = std::dynamic_pointer_cast<NumberToken>(token); numberToken != nullptr) {
                current->children.push_back(std::make_shared<InnerNode>(current, numberToken->value));
                            
            } else if (auto obToken = std::dynamic_pointer_cast<OpenBracketToken>(token); obToken != nullptr) {
                current = std::dynamic_pointer_cast<InnerNode>(current->children.back());
            
            } else if (auto cbToken = std::dynamic_pointer_cast<CloseBracketToken>(token); cbToken != nullptr) {
                current = current->parent.lock();
                
            } else {
                throw "unreachable";
            }
        }

        return root;
    }
};


class Evaluator {
private:
    std::string StrMultiply(const std::string& s, int multiplier) {
        std::string result;
        result.reserve(s.length() * multiplier);

        for (int i = 0; i < multiplier; ++i) {
            result += s;
        }

        return result;
    }
public:
    std::string Evaluate(const std::shared_ptr<InnerNode> root) {
        std::string result;
        
        for (const std::shared_ptr<Node> node: root->children) {
            if (auto leafNode = std::dynamic_pointer_cast<LeafNode>(node); leafNode != nullptr) {
                result += leafNode->data;
                continue;
            }
            if (auto innerNode = std::dynamic_pointer_cast<InnerNode>(node); innerNode != nullptr) {
                result += Evaluate(innerNode);
                continue;
            }
        }

        return StrMultiply(result, root->multiplier);
    }
};


class Solution {
public:
    std::string decodeString(std::string s) {
        std::vector<std::shared_ptr<Token>> tokens = Tokenizer().Tokenize(s);
        std::shared_ptr<InnerNode> tree = Parser().BuildTree(tokens);
        std::string result = Evaluator().Evaluate(tree);

        return result;
    }
};