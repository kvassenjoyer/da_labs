#include <algorithm> // std::sort
#include <cctype> // std::isdigit
#include <cstdint> // std::uint32_t
#include <deque> // std::deque
#include <iostream> // std::cin, std::cout
#include <queue> // std::queue
#include <sstream> // std::istringstream
#include <string> // std::string
#include <unordered_map> // std::unordered_map
#include <vector> // std::vector

using TValue = std::uint32_t;
using TPattern = std::vector<TValue>;

struct TPosition {
    size_t lineID;
    size_t wordID;
};

struct TPatternInfo {
    size_t length;
    size_t patternID;
};

struct TAnswer {
    size_t lineID;
    size_t wordID;
    size_t patternID;

    bool operator<(const TAnswer& other) const {
        if (lineID != other.lineID) {
            return lineID < other.lineID;
        }
        if (wordID != other.wordID) {
            return wordID < other.wordID;
        }
        return patternID < other.patternID;
    }
};

std::vector<std::string> SplitStringBySpaces(const std::string& str) {
    std::vector<std::string> tokens;
    std::istringstream iss(str);
    std::string token;
    while (iss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

bool IsAllDigits(const std::string& str) {
    if (str.empty()) {
        return false;
    }

    for (char c : str) {
        if (!std::isdigit(static_cast<unsigned char>(c))) {
            return false;
        }
    }
    return true;
}

TValue ParseValue(const std::string& str) {
    TValue result = 0;
    for (char c : str) {
        result = result * 10 + static_cast<TValue>(c - '0');
    }
    return result;
}

std::vector<TPattern> GetPatterns() {
    std::vector<TPattern> patterns;
    std::string line;

    while (std::getline(std::cin, line)) {
        std::vector<std::string> splited_line = SplitStringBySpaces(line);
        if (splited_line.empty()) {
            break;
        }

        TPattern pattern;
        for (const std::string& word : splited_line) {
            if (IsAllDigits(word)) {
                pattern.push_back(ParseValue(word));
            }
        }

        if (!pattern.empty()) {
            patterns.push_back(pattern);
        }
    }

    return patterns;
}

struct Node {
    TValue value;
    std::unordered_map<TValue, Node*> children;
    Node* failLink;
    Node* exitLink;
    std::vector<TPatternInfo> patternsEndingHere;

    Node(TValue nodeValue = 0):
    value(nodeValue), children(), failLink(nullptr), exitLink(nullptr), patternsEndingHere() {}
};

class TAhoCorasick {
private:
    std::deque<Node> nodes;
    size_t maxPatternLength;

    Node* GetNextNode(const Node* currentNode, TValue value) const {
        auto it = currentNode->children.find(value);
        if (it == currentNode->children.end()) {
            return nullptr;
        }
        return it->second;
    }

    void AddAnswersFromNode(const Node* node, const std::deque<TPosition>& positions, std::vector<TAnswer>* answers) const {
        for (const TPatternInfo& pattern : node->patternsEndingHere) {
            if (pattern.length <= positions.size()) {
                const TPosition& startPosition = positions[positions.size() - pattern.length];
                answers->push_back({startPosition.lineID, startPosition.wordID, pattern.patternID});
            }
        }
    }

public:
    TAhoCorasick():
    nodes(1), maxPatternLength(0) {}

    void InsertPattern(const TPattern& pattern, size_t patternID) {
        Node* currentNode = &nodes[0];

        for (TValue value : pattern) {
            Node* nextNode = GetNextNode(currentNode, value);
            if (nextNode == nullptr) {
                nodes.push_back(Node(value));
                nextNode = &nodes.back();
                currentNode->children[value] = nextNode;
            }
            currentNode = nextNode;
        }

        TPatternInfo patternInfo;
        patternInfo.length = pattern.size();
        patternInfo.patternID = patternID;
        currentNode->patternsEndingHere.push_back(patternInfo);

        if (pattern.size() > maxPatternLength) {
            maxPatternLength = pattern.size();
        }
    }

    void BuildFailAndExitLinks() {
        std::queue<Node*> q;

        for (const auto& childItem : nodes[0].children) {
            Node* childNode = childItem.second;
            childNode->failLink = &nodes[0];
            childNode->exitLink = nullptr;
            q.push(childNode);
        }

        while (!q.empty()) {
            Node* currentNode = q.front();
            q.pop();

            for (const auto& childItem : currentNode->children) {
                TValue value = childItem.first;
                Node* childNode = childItem.second;

                Node* failureNode = currentNode->failLink;
                while (failureNode != &nodes[0] && GetNextNode(failureNode, value) == nullptr) {
                    failureNode = failureNode->failLink;
                }

                Node* nextNode = GetNextNode(failureNode, value);
                if (nextNode == nullptr) {
                    childNode->failLink = &nodes[0];
                }
                else {
                    childNode->failLink = nextNode;
                }

                Node* failLink = childNode->failLink;
                if (!failLink->patternsEndingHere.empty()) {
                    childNode->exitLink = failLink;
                }
                else {
                    childNode->exitLink = failLink->exitLink;
                }

                q.push(childNode);
            }
        }
    }

    const Node* MoveToNextNode(const Node* currentNode, TValue value) const {
        while (currentNode != &nodes[0] && GetNextNode(currentNode, value) == nullptr) {
            currentNode = currentNode->failLink;
        }

        const Node* nextNode = GetNextNode(currentNode, value);
        if (nextNode == nullptr) {
            return &nodes[0];
        }
        return nextNode;
    }

    void AddAnswers(const Node* currentNode, const std::deque<TPosition>& positions, std::vector<TAnswer>* answers) const {
        AddAnswersFromNode(currentNode, positions, answers);

        const Node* exitNode = currentNode->exitLink;
        while (exitNode != nullptr) {
            AddAnswersFromNode(exitNode, positions, answers);
            exitNode = exitNode->exitLink;
        }
    }

    const Node* GetRoot() const {
        return &nodes[0];
    }

    size_t GetMaxPatternLength() const {
        return maxPatternLength;
    }
};

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::vector<TPattern> patterns = GetPatterns();

    TAhoCorasick ahoCorasick;
    for (size_t i = 0; i < patterns.size(); ++i) {
        ahoCorasick.InsertPattern(patterns[i], i + 1);
    }
    ahoCorasick.BuildFailAndExitLinks();

    std::vector<TAnswer> answers;
    std::deque<TPosition> positions;
    std::string line;
    const Node* currentNode = ahoCorasick.GetRoot();
    size_t lineID = 1;
    size_t maxPatternLength = ahoCorasick.GetMaxPatternLength();

    while (std::getline(std::cin, line)) {
        std::vector<std::string> words = SplitStringBySpaces(line);

        for (size_t i = 0; i < words.size(); ++i) {
            if (!IsAllDigits(words[i])) {
                currentNode = ahoCorasick.GetRoot();
                positions.clear();
                continue;
            }

            TValue value = ParseValue(words[i]);
            positions.push_back({lineID, i + 1});
            if (positions.size() > maxPatternLength) {
                positions.pop_front();
            }

            currentNode = ahoCorasick.MoveToNextNode(currentNode, value);
            ahoCorasick.AddAnswers(currentNode, positions, &answers);
        }

        ++lineID;
    }

    // std::sort(answers.begin(), answers.end());

    for (const TAnswer& answer : answers) {
        // if (patterns.size() == 1) {
        //     std::cout << answer.lineID << ", " << answer.wordID << '\n';
        // } 
        // else {
            std::cout << answer.lineID << ", " << answer.wordID << ", " << answer.patternID << '\n';
        // }
    }

    return 0;
}
