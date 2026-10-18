#include <algorithm> // std::sort
#include <iostream> // std::cin, std::cout
#include <string> // std::string
#include <utility> // std::pair
#include <vector> // std::vector

class TSuffixArray {
private:
    std::string text;
    std::vector<int> suffixArray;

    void BuildSuffixArray() {
        std::string textWithSentinel = text;
        textWithSentinel.push_back('\0');

        int textWithSentinelLength = static_cast<int>(textWithSentinel.size());
        std::vector<int> suffixOrder(textWithSentinelLength);
        std::vector<int> equivalenceClass(textWithSentinelLength);

        const int alphabet = 256;
        std::vector<int> count(std::max(alphabet, textWithSentinelLength), 0);

        for (int position = 0; position < textWithSentinelLength; ++position) {
            ++count[static_cast<unsigned char>(textWithSentinel[position])];
        }
        for (int symbolCode = 1; symbolCode < alphabet; ++symbolCode) {
            count[symbolCode] += count[symbolCode - 1];
        }
        for (int position = 0; position < textWithSentinelLength; ++position) {
            unsigned char symbol = static_cast<unsigned char>(textWithSentinel[position]);
            --count[symbol];
            suffixOrder[count[symbol]] = position;
        }

        equivalenceClass[suffixOrder[0]] = 0;
        int classes = 1;
        for (int orderIndex = 1; orderIndex < textWithSentinelLength; ++orderIndex) {
            if (textWithSentinel[suffixOrder[orderIndex]] != textWithSentinel[suffixOrder[orderIndex - 1]]) {
                ++classes;
            }
            equivalenceClass[suffixOrder[orderIndex]] = classes - 1;
        }

        std::vector<int> shiftedSuffixOrder(textWithSentinelLength);
        std::vector<int> newEquivalenceClass(textWithSentinelLength);

        for (int substringLength = 1; substringLength < textWithSentinelLength; substringLength *= 2) {
            for (int orderIndex = 0; orderIndex < textWithSentinelLength; ++orderIndex) {
                shiftedSuffixOrder[orderIndex] = suffixOrder[orderIndex] - substringLength;
                if (shiftedSuffixOrder[orderIndex] < 0) {
                    shiftedSuffixOrder[orderIndex] += textWithSentinelLength;
                }
            }

            std::fill(count.begin(), count.begin() + classes, 0);
            for (int orderIndex = 0; orderIndex < textWithSentinelLength; ++orderIndex) {
                ++count[equivalenceClass[shiftedSuffixOrder[orderIndex]]];
            }
            for (int classNumber = 1; classNumber < classes; ++classNumber) {
                count[classNumber] += count[classNumber - 1];
            }
            for (int orderIndex = textWithSentinelLength - 1; orderIndex >= 0; --orderIndex) {
                int classNumber = equivalenceClass[shiftedSuffixOrder[orderIndex]];
                --count[classNumber];
                suffixOrder[count[classNumber]] = shiftedSuffixOrder[orderIndex];
            }

            newEquivalenceClass[suffixOrder[0]] = 0;
            classes = 1;
            for (int orderIndex = 1; orderIndex < textWithSentinelLength; ++orderIndex) {
                std::pair<int, int> currentPair = {
                    equivalenceClass[suffixOrder[orderIndex]],
                    equivalenceClass[(suffixOrder[orderIndex] + substringLength) % textWithSentinelLength]
                };
                std::pair<int, int> previousPair = {
                    equivalenceClass[suffixOrder[orderIndex - 1]],
                    equivalenceClass[(suffixOrder[orderIndex - 1] + substringLength) % textWithSentinelLength]
                };

                if (currentPair != previousPair) {
                    ++classes;
                }
                newEquivalenceClass[suffixOrder[orderIndex]] = classes - 1;
            }
            equivalenceClass.swap(newEquivalenceClass);
        }

        suffixArray.clear();
        suffixArray.reserve(text.size());
        for (int suffixPosition : suffixOrder) {
            if (suffixPosition != static_cast<int>(text.size())) {
                suffixArray.push_back(suffixPosition);
            }
        }
    }

    int CompareSuffixWithPattern(int suffixPosition, const std::string& pattern) const {
        size_t matchedLength = 0;
        while (matchedLength < pattern.size() &&
               suffixPosition + static_cast<int>(matchedLength) < static_cast<int>(text.size())) {
            unsigned char textSymbol = static_cast<unsigned char>(text[suffixPosition + static_cast<int>(matchedLength)]);
            unsigned char patternSymbol = static_cast<unsigned char>(pattern[matchedLength]);

            if (textSymbol < patternSymbol) {
                return -1;
            }
            if (textSymbol > patternSymbol) {
                return 1;
            }
            ++matchedLength;
        }

        if (matchedLength == pattern.size()) {
            return 0;
        }
        return -1;
    }

    size_t LowerBound(const std::string& pattern) const {
        size_t left = 0;
        size_t right = suffixArray.size();

        while (left < right) {
            size_t mid = (left + right) / 2;
            int cmp = CompareSuffixWithPattern(suffixArray[mid], pattern);

            if (cmp < 0) {
                left = mid + 1;
            }
            else {
                right = mid;
            }
        }

        return left;
    }

    size_t UpperBound(const std::string& pattern) const {
        size_t left = 0;
        size_t right = suffixArray.size();

        while (left < right) {
            size_t mid = (left + right) / 2;
            int cmp = CompareSuffixWithPattern(suffixArray[mid], pattern);

            if (cmp <= 0) {
                left = mid + 1;
            }
            else {
                right = mid;
            }
        }

        return left;
    }

public:
    TSuffixArray(const std::string& str):
    text(str) {
        BuildSuffixArray();
    }

    std::vector<int> SearchPattern(const std::string& pattern) const {
        std::vector<int> positions;
        if (pattern.empty()) {
            return positions;
        }

        size_t left = LowerBound(pattern);
        size_t right = UpperBound(pattern);

        for (size_t suffixIndex = left; suffixIndex < right; ++suffixIndex) {
            positions.push_back(suffixArray[suffixIndex] + 1);
        }

        std::sort(positions.begin(), positions.end());
        return positions;
    }
};

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::string text;
    if (!std::getline(std::cin, text)) {
        return 0;
    }

    TSuffixArray suffixArray(text);

    std::string pattern;
    size_t patternNumber = 1;
    while (std::getline(std::cin, pattern)) {
        std::vector<int> positions = suffixArray.SearchPattern(pattern);

        if (!positions.empty()) {
            std::cout << patternNumber << ": ";
            for (size_t i = 0; i < positions.size(); ++i) {
                if (i > 0) {
                    std::cout << ", ";
                }
                std::cout << positions[i];
            }
            std::cout << '\n';
        }

        ++patternNumber;
    }

    return 0;
}
