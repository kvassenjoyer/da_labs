#include <fstream>  // std::ofstream, std::ifstream, std::ios::binary
#include <iostream> // std::cin, std::cout, std::endl, std::ios::sync_with_stdio(false), std::cin.tie(nullptr)
#include <limits>   // std::numeric_limits<std::streamsize>::max()
#include <new>      // std::bad_alloc
#include <stdexcept> // std::out_of_range
#include <cstddef>  // size_t

#include <cctype>   // std::isalpha, std::isspace
#include <cstdint>  // std::uint8_t, std::uint64_t
#include <cstring>  // std::memcpy

// Type T:
// 1) should be simple type (no need to explicit destructor call)
// 2) should be default-constructible
template <typename T>
class TVector {
    private:
        T* data_;
        size_t capacity;
        size_t size_;
        mutable char* cstr_buffer_ = nullptr;

        void InvalidateCString() const {
            delete[] cstr_buffer_;
            cstr_buffer_ = nullptr;
        }
    public:
        const char* c_str() const {
            InvalidateCString();

            cstr_buffer_ = new char[size_ + 1];

            for (size_t i = 0; i < size_; ++i) {
                cstr_buffer_[i] = data_[i];
            }

            cstr_buffer_[size_] = '\0';

            return cstr_buffer_;
        }

        TVector() {
            data_ = nullptr;
            capacity = 0;
            size_ = 0;
        }

        TVector(const TVector& other) {
            data_ = nullptr;
            capacity = 0;
            size_ = 0;
            cstr_buffer_ = nullptr;

            size_ = other.size_;
            data_ = new T[size_];
            for(size_t i = 0; i < size_; ++i) {
                data_[i] = other.data_[i];
            }
            capacity = size_;
        }
        TVector(TVector&& other) noexcept {
            data_ = other.data_;
            capacity = other.capacity;
            size_ = other.size_;
            cstr_buffer_ = other.cstr_buffer_;

            other.data_ = nullptr;
            other.capacity = 0;
            other.size_ = 0;
            other.cstr_buffer_ = nullptr;
        }
        TVector(const char* s) {
            data_ = nullptr;
            capacity = 0;
            size_ = 0;
            if (s != nullptr) {
                while (*s) {
                    push_back(*s);
                    ++s;
                }
            }
        }

        TVector& operator=(const TVector& other) {
            if (this != &other) {
                T* newData = new T[other.size_];
                for(size_t i = 0; i < other.size_; ++i) {
                    newData[i] = other.data_[i];
                }

                delete[] data_;
                data_ = newData;
                size_ = other.size_;
                capacity = other.size_; 

                InvalidateCString();
            }
            return *this;
        }

        TVector& operator=(TVector&& other) noexcept {
            if (this != &other) {
                delete[] data_;
                delete[] cstr_buffer_;
                
                data_ = other.data_;
                capacity = other.capacity;
                size_ = other.size_;
                cstr_buffer_ = other.cstr_buffer_;

                other.data_ = nullptr;
                other.capacity = 0;
                other.size_ = 0;
                other.cstr_buffer_ = nullptr;
            }
            return *this;
        }

        ~TVector(){
            delete[] data_;
            delete[] cstr_buffer_;
        }

        T& operator[](size_t index) {
            if (index >= size_) {
                throw std::out_of_range("TVector index out of range");
            }
            InvalidateCString();
            return data_[index];
        }

        const T& operator[](size_t index) const {
            if (index >= size_) {
                throw std::out_of_range("TVector index out of range");
            }
            return data_[index];
        }

        void push_back(const T& obj) {
            if (size_ == capacity) {
                reserve(capacity * 2 + 1);
            }
            data_[size_] = obj;
            ++size_;
            InvalidateCString();
        }

        void reserve(size_t newCapacity) {
            if (newCapacity > capacity) {
                T* newData = new T[newCapacity];
                for (size_t i = 0; i < size_; ++i) {
                    newData[i] = data_[i];
                }
                delete[] data_;
                data_ = newData;
                capacity = newCapacity;
                InvalidateCString();
            }
        }

        void resize(size_t newSize) {
            if (newSize > capacity) {
                reserve(newSize);
            }
            size_ = newSize;
            InvalidateCString();
        }

        void assign(const T* ptr, size_t count) {
            resize(count);
            for (size_t i = 0; i < count; ++i) {
                data_[i] = ptr[i];
            }
            InvalidateCString();
        }

        void append(const T* ptr, size_t count) {
            size_t oldSize = size_;
            resize(size_ + count);
            for (size_t i = 0; i < count; ++i) {
                data_[oldSize + i] = ptr[i];
            }
            InvalidateCString();
        }

        void append(const TVector& other) {
            append(other.data_, other.size_);
        }

        bool operator==(const char* s) const;

        bool operator==(const TVector& other) const {
            if (size_ != other.size_) {
                return false;
            }
            for (size_t i = 0; i < size_; ++i) {
                if (data_[i] != other.data_[i]) {
                    return false;
                }
            }
            return true;
        }

        bool operator!=(const TVector& other) const {
            return !(*this == other);
        }

        bool operator<(const TVector& other) const {
            size_t min_size = (size_ < other.size_) ? size_ : other.size_;
            for (size_t i = 0; i < min_size; ++i) {
                if (data_[i] < other.data_[i]) {
                    return true;
                }
                if (data_[i] > other.data_[i]) {
                    return false;
                }
            }

            return size_ < other.size_;
        }

        size_t size() const {
            return size_;
        }

        bool empty() const {
            return size_ == 0;
        }

        void clear() {
            size_ = 0;
            InvalidateCString();
        }

        T* data() {
            return data_;
        }

        const T* data() const {
            return data_;
        }

        T& back() {
            if (size_ == 0) {
                throw std::out_of_range("TVector is empty");
            }
            return data_[size_ - 1];
        }

        const T& back() const {
            if (size_ == 0) {
                throw std::out_of_range("TVector is empty");
            }
            return data_[size_ - 1];
        }

        T* begin() {
            return data_;
        }

        const T* begin() const {
            return data_;
        }

        T* end() {
            return data_ + size_;
        }

        const T* end() const {
            return data_ + size_;
        }
};

template<>
bool TVector<char>::operator==(const char* s) const {
    if (s == nullptr) {
        return false;
    }

    size_t i = 0;
    while (i < size_) {
        if (s[i] == '\0') {
            return false;
        }
        if (data_[i] != s[i]) {
            return false;
        }
        ++i;
    }

    return s[i] == '\0';
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const TVector<T>& vec) {
    for (size_t i = 0; i < vec.size(); ++i) {
        os << vec[i];
    }
    return os;
}

std::istream& operator>>(std::istream& is, TVector<char>& vec) {
    vec.clear();

    int c = is.peek();
    while (c != EOF && std::isspace(static_cast<unsigned char>(c))) {
        is.get();
        c = is.peek();
    }

    bool was_read = false;
    while (c != EOF && !std::isspace(static_cast<unsigned char>(c))) {
        char symbol = static_cast<char>(is.get());
        vec.push_back(symbol);
        was_read = true;
        c = is.peek();
    }

    if (!was_read) {
        is.setstate(std::ios::failbit);
    }

    return is;
}

enum class Color : std::uint8_t {
    Red = 0,
    Black = 1
};

enum class NodeType : std::uint8_t {
    NullNode = 0,
    LeafNode = 1,
    InnerNode = 2
};

using TString = TVector<char>;

struct Node {
    Node* left;
    Node* right;
    Node* parent;
    Color color;
    TString key;
    unsigned long long value;

    Node(const TString& key, unsigned long long value):
    left(nullptr), right(nullptr), parent(nullptr), color(Color::Red), key(key), value(value) {}
};

class RedBlackTree {
private:
    Node* root;
    Node* NULL_NODE;

    void DeleteSubtree(Node* node) {
        if (node == NULL_NODE) {
            return;
        }

        DeleteSubtree(node->left);
        DeleteSubtree(node->right);
        delete node;
    }

    Node* RecursiveSearchNode(const TString& key, Node* searchRoot) {
        if ((searchRoot == NULL_NODE) || (key == searchRoot->key)) {
            return searchRoot;
        }
        else if (key < searchRoot->key) {
            return RecursiveSearchNode(key, searchRoot->left);
        }
        else {
            return RecursiveSearchNode(key, searchRoot->right);
        }

    }

    Node* GetInsertParent(Node* newNode, Node* currentNode) {
        if (newNode->key < currentNode->key) {
            if (currentNode->left == NULL_NODE) {
                return currentNode;
            }
            return GetInsertParent(newNode, currentNode->left);
        }
        
        if (currentNode->right == NULL_NODE) {
            return currentNode;
        }
        return GetInsertParent(newNode, currentNode->right);

    }

    void InsertLL(Node* parentNode) {
        parentNode->color = Color::Black;
        parentNode->parent->color = Color::Red;
        RotateRight(parentNode);
    }

    void InsertRR(Node* parentNode) {
        parentNode->color = Color::Black;
        parentNode->parent->color = Color::Red;
        RotateLeft(parentNode);
    }

    void RotateRight(Node* leftSonNode) {
        Node* rotateNode = leftSonNode->parent;
        Node* parentNode = rotateNode->parent;

        rotateNode->left = leftSonNode->right;
        if (rotateNode->left != NULL_NODE) {
            rotateNode->left->parent = rotateNode;
        }

        leftSonNode->right = rotateNode;
        rotateNode->parent = leftSonNode;

        leftSonNode->parent = parentNode;
        if (parentNode == NULL_NODE) {
            root = leftSonNode;
        } 
        else if (parentNode->left == rotateNode) {
            parentNode->left = leftSonNode;
        }
        else {
            parentNode->right = leftSonNode;
        }
    }

    void RotateLeft(Node* rightSonNode) {
        Node* rotateNode = rightSonNode->parent;
        Node* parentNode = rotateNode->parent;

        rotateNode->right = rightSonNode->left;
        if (rotateNode->right != NULL_NODE) {
            rotateNode->right->parent = rotateNode;
        }

        rightSonNode->left = rotateNode;
        rotateNode->parent = rightSonNode;

        rightSonNode->parent = parentNode; 
        if (parentNode == NULL_NODE) {
            root = rightSonNode;
        } 
        else if (parentNode->left == rotateNode) {
            parentNode->left = rightSonNode;
        }
        else {
            parentNode->right = rightSonNode;
        }
    }   

    void InsertFix(Node* newNode) {
        if (newNode->parent->color == Color::Black) {
            return;
        }
        Node* parentNode = newNode->parent;
        Node* grandpaNode = newNode->parent->parent;

        Node* uncle = NULL_NODE;
        if (grandpaNode->left != newNode->parent) {
            uncle = grandpaNode->left;
        }
        else {
            uncle = grandpaNode->right;
        }

        if (uncle->color == Color::Red) {
            parentNode->color = Color::Black;
            uncle->color = Color::Black;
            grandpaNode->color = Color::Red;
            InsertFix(grandpaNode);
        }
        else {
            if (grandpaNode->left == parentNode) {
                if (parentNode->left == newNode) {
                    InsertLL(parentNode);
                }
                else {
                    RotateLeft(newNode);
                    InsertLL(newNode);
                }
            }
            else {
                if (parentNode->left == newNode) {
                    RotateRight(newNode);
                    InsertRR(newNode);
                }
                else {
                    InsertRR(parentNode);
                }

            }
        }
        root->color = Color::Black;
    }

    void InsertHelper(Node* newNode) {
        if (root == NULL_NODE) {
            newNode->color = Color::Black;
            root = newNode;
            return;
        }

        Node* parentNode = GetInsertParent(newNode, root);
        newNode->parent = parentNode;
        if (newNode->key < parentNode->key) {
            parentNode->left = newNode;
        }
        else {
            parentNode->right = newNode;
        }

        InsertFix(newNode);
    }

    void RemoveFix(Node* removeNode) {
        while (removeNode->color == Color::Black && removeNode != root) {
            Node* parentNode = removeNode->parent;
            Node* brotherNode = parentNode->left;
            if (brotherNode == removeNode) {
                brotherNode = parentNode->right;
            } 

            // Case 1 and 2:
            if (brotherNode->color == Color::Red) {
                if (brotherNode == parentNode->left) {
                    RotateRight(brotherNode);
                }
                else {
                    RotateLeft(brotherNode);
                }
                brotherNode->color = Color::Black;
                parentNode->color = Color::Red; // parentNode != NULL_NODE так как removeNode != root
                continue;
            }

            // Case 3 and 4:
            if (brotherNode->right->color == Color::Black && brotherNode->left->color == Color::Black) {
                if (brotherNode != NULL_NODE) {
                    brotherNode->color = Color::Red;
                }
                removeNode = parentNode; // While cycle recusrsion
                continue;
            }
            else if (brotherNode == parentNode->left) {
                // Case 5:
                if (brotherNode->left->color == Color::Black && brotherNode->right->color == Color::Red) {
                    brotherNode->color = Color::Red; // brotherNode != NULL_NODE так как имеет красного сына
                    brotherNode->right->color = Color::Black;
                    RotateLeft(brotherNode->right);
                    continue;
                }
                // Case 6 and 7:
                else if (brotherNode->left->color == Color::Red) {
                    brotherNode->color = parentNode->color;
                    parentNode->color = Color::Black;
                    brotherNode->left->color = Color::Black;
                    RotateRight(brotherNode);
                    removeNode = root;
                    break;
                }

            }
            else {
                // Case 8:
                if (brotherNode->left->color == Color::Red && brotherNode->right->color == Color::Black) {
                    brotherNode->color = Color::Red; // brotherNode != NULL_NODE так как имеет красного сына
                    brotherNode->left->color = Color::Black;
                    RotateRight(brotherNode->left);
                    continue;
                }
                // Case 9 and 10:
                else if (brotherNode->right->color == Color::Red) {
                    brotherNode->color = parentNode->color;
                    parentNode->color = Color::Black;
                    brotherNode->right->color = Color::Black;
                    RotateLeft(brotherNode);
                    removeNode = root;
                    break;
                }
            }
        }
        removeNode->color = Color::Black;
    }

    void RemoveHelper(Node* removeNode) {
        Node* parentNode = removeNode->parent;
        Node* fixNode;

        if (removeNode->left == NULL_NODE && removeNode->right == NULL_NODE) { // zero child case
            if(removeNode == root) {
                root = NULL_NODE;
                delete removeNode;
                return;
            }
            if (removeNode == parentNode->left) {
                parentNode->left = NULL_NODE;
            }
            else {
                parentNode->right = NULL_NODE;
            }
            NULL_NODE->parent = removeNode->parent; // PROBLEM (*) | Ебнет? Не должно.
            fixNode = NULL_NODE;
        }
        else if (removeNode->right == NULL_NODE || removeNode->left == NULL_NODE) {  // one child case
            Node* childNode = removeNode->left;
            if (removeNode->right != NULL_NODE) {
                childNode = removeNode->right;
            }
            childNode->parent = parentNode;

            if (removeNode == root) {
                root = childNode;
            }
            else {
                if (removeNode == parentNode->left) {
                    parentNode->left = childNode;
                }
                else {
                    parentNode->right = childNode;
                }
            }
            fixNode = childNode;
        }
        else { //  two child case
            Node* lrstNode = removeNode->right; // LRST - Least in Right SubTree 
            while (lrstNode->left != NULL_NODE) {
                lrstNode = lrstNode->left;
            }
            removeNode->key = lrstNode->key;
            removeNode->value = lrstNode->value;
            // Причем цвет не передаем 
            RemoveHelper(lrstNode);
            return;
        }

        if (removeNode->color == Color::Black) {
            RemoveFix(fixNode);
        }
        NULL_NODE->parent = NULL_NODE; // PROBLEM (*) SOLVED
        delete removeNode;
    }

    template <typename T>
    bool ReadRawBytes(const TString& buffer, size_t* offset, T* value) {
        if (*offset > buffer.size()) {
            return false;
        }
        if (sizeof(T) > buffer.size() - *offset) {
            return false;
        }

        std::memcpy(value, buffer.data() + *offset, sizeof(T));
        *offset += sizeof(T);
        return true;
    }

    bool ReadStringBytes(const TString& buffer, size_t* offset, std::uint64_t size, TString* value) {
        if (*offset > buffer.size()) {
            return false;
        }
        if (size > buffer.size() - *offset) {
            return false;
        }

        // Копирование байтов ключа из бинарного буфера:
        value->resize(size);
        for (size_t i = 0; i < size; ++i) {
            (*value)[i] = buffer[*offset + i];
        }

        *offset += static_cast<size_t>(size);
        return true;
    }

    bool ReadNodeFromString(const TString& buffer, size_t* offset, Node** result) {
        *result = NULL_NODE;

        std::uint8_t raw_color;
        if (!ReadRawBytes(buffer, offset, &raw_color)) {
            return false;
        }
        if (!IsNodeColorValid(raw_color)) {
            return false;
        }
        Color color = static_cast<Color>(raw_color);

        std::uint64_t key_size;
        if (!ReadRawBytes(buffer, offset, &key_size)) {
            return false;
        }
        if (key_size == 0 || key_size > 256) {
            return false;
        }

        TString key;
        if (!ReadStringBytes(buffer, offset, key_size, &key)) {
            return false;
        }
        if (!IsKeyValid(key)) {
            return false;
        }

        unsigned long long value;
        if (!ReadRawBytes(buffer, offset, &value)) {
            return false;
        }

        Node* new_node = new Node(key, value);
        new_node->color = color;
        new_node->left = NULL_NODE;
        new_node->right = NULL_NODE;
        new_node->parent = NULL_NODE;

        *result = new_node;
        return true;
    }

    bool ReadTreeFromString(const TString& buffer, size_t* offset, Node* parentNode, Node** result) {
        *result = NULL_NODE;

        std::uint8_t raw_node_type;
        if (!ReadRawBytes(buffer, offset, &raw_node_type)) {
            return false;
        }
        if (!IsNodeTypeValid(raw_node_type)) {
            return false;
        }
        NodeType node_type = static_cast<NodeType>(raw_node_type);

        if (node_type == NodeType::NullNode) {
            return true;
        }

        if (node_type == NodeType::LeafNode) {
            Node* new_node = NULL_NODE;
            if (!ReadNodeFromString(buffer, offset, &new_node)) {
                return false;
            }

            new_node->parent = parentNode;
            *result = new_node;
            return true;
        }

        if (node_type == NodeType::InnerNode) {
            Node* left_node = NULL_NODE;
            Node* right_node = NULL_NODE;
            Node* current_node = NULL_NODE;

            if (!ReadTreeFromString(buffer, offset, NULL_NODE, &left_node)) {
                return false;
            }

            if (!ReadTreeFromString(buffer, offset, NULL_NODE, &right_node)) {
                DeleteSubtree(left_node);
                return false;
            }

            if (!ReadNodeFromString(buffer, offset, &current_node)) {
                DeleteSubtree(left_node);
                DeleteSubtree(right_node);
                return false;
            }

            current_node->parent = parentNode;
            current_node->left = left_node;
            current_node->right = right_node;

            if (left_node != NULL_NODE) {
                left_node->parent = current_node;
            }
            if (right_node != NULL_NODE) {
                right_node->parent = current_node;
            }

            *result = current_node;
            return true;
        }

        return false;
    }

    template <typename T>
    void AppendRawBytes(TString* buffer, const T& value) {
        const char* raw = reinterpret_cast<const char*>(&value);
        buffer->append(raw, sizeof(T));
    }

    void AppendNodeTypeBytes(TString* buffer, NodeType node_type) {
        buffer->push_back(static_cast<char>(node_type));
    }

    void AppendColorBytes(TString* buffer, Color color) {
        buffer->push_back(static_cast<char>(color));
    }

    void AppendStringBytes(TString* buffer, const TString& value) {
        buffer->append(value.data(), value.size());
    }

    TString GetNodeString(Node* currentNode) {
        TString node_string;

        AppendColorBytes(&node_string, currentNode->color);

        std::uint64_t key_size = static_cast<std::uint64_t>(currentNode->key.size());
        AppendRawBytes(&node_string, key_size);

        AppendStringBytes(&node_string, currentNode->key);

        AppendRawBytes(&node_string, currentNode->value);

        return node_string;
    }

    void WriteTreeToString(Node* currentNode, TString* rbt_string) {
        if (currentNode == NULL_NODE) {
            NodeType node_type = NodeType::NullNode;
            AppendNodeTypeBytes(rbt_string, node_type);
            return;
        }

        if (currentNode->left == NULL_NODE && currentNode->right == NULL_NODE) {
            NodeType node_type = NodeType::LeafNode;
            AppendNodeTypeBytes(rbt_string, node_type);
        }
        else {
            NodeType node_type = NodeType::InnerNode;
            AppendNodeTypeBytes(rbt_string, node_type);

            WriteTreeToString(currentNode->left, rbt_string);
            WriteTreeToString(currentNode->right, rbt_string);
        }

        TString current_node_string = GetNodeString(currentNode);
        rbt_string->append(current_node_string);
    }

    bool IsNodeTypeValid(std::uint8_t value) {
        return value == static_cast<std::uint8_t>(NodeType::NullNode) ||
            value == static_cast<std::uint8_t>(NodeType::LeafNode) ||
            value == static_cast<std::uint8_t>(NodeType::InnerNode);
    }

    bool IsNodeColorValid(std::uint8_t value) {
        return value == static_cast<std::uint8_t>(Color::Red) ||
            value == static_cast<std::uint8_t>(Color::Black);
    }

public:
    RedBlackTree() {
        NULL_NODE = new Node("", 0);
        NULL_NODE->color = Color::Black;
        NULL_NODE->left = NULL_NODE;
        NULL_NODE->right = NULL_NODE;
        NULL_NODE->parent = NULL_NODE;
        
        root = NULL_NODE;
    }

    ~RedBlackTree() {
        DeleteSubtree(root);
        delete NULL_NODE;
    }

    // Запрещаю копирование и перемещение для избегания double free
    RedBlackTree(const RedBlackTree&) = delete;
    RedBlackTree& operator=(const RedBlackTree&) = delete;
    RedBlackTree(RedBlackTree&&) = delete;
    RedBlackTree& operator=(RedBlackTree&&) = delete;

    void SearchNode(const TString& raw_key) {
        TString key = ToLowerCase(raw_key);
        Node* result = RecursiveSearchNode(key, root);
        if (result == NULL_NODE) {
            std::cout << "NoSuchWord" << std::endl;
        }
        else {
            std::cout << "OK: " << result->value << std::endl;
        }
    }

    void InsertNode(const TString& raw_key, unsigned long long value) {
        TString key = ToLowerCase(raw_key);
        Node* result = RecursiveSearchNode(key, root);
        if (result != NULL_NODE) {
            std::cout << "Exist" << std::endl;
        }
        else {
            Node* newNode = new Node(key, value);
            newNode->left = NULL_NODE;
            newNode->right = NULL_NODE;
            newNode->parent = NULL_NODE;

            InsertHelper(newNode);
            std::cout << "OK" << std::endl;
        }
    }
    
    void RemoveNode(const TString& raw_key) {

        TString key = ToLowerCase(raw_key);
        Node* result = RecursiveSearchNode(key, root);

        if (result == NULL_NODE) {
            std::cout << "NoSuchWord" << std::endl; 
        } 
        else {
            RemoveHelper(result);
            std::cout << "OK" << std::endl; 
        }
    }

    void SaveTree(const TString& path) {
        std::ofstream file(path.c_str(), std::ios::binary);
        if (!file) {
            std::cout << "ERROR: file opening failed" << std::endl;;
            return;
        }
        
        TString rbt_string;
        WriteTreeToString(root, &rbt_string);

        file.write(rbt_string.data(), static_cast<std::streamsize>(rbt_string.size()));
        if (!file) {
            std::cout << "ERROR: file writing failed" << std::endl;
            return;
        }

        file.close();
        std::cout << "OK" << std::endl;
    }

    void LoadTree(const TString& path) {
        std::ifstream file(path.c_str(), std::ios::binary);
        if (!file) {
            std::cout << "ERROR: file opening failed" << std::endl;
            return;
        }

        Node* loaded_root = NULL_NODE;
        size_t offset = 0;

        try {
            TString buffer;
            char symbol;
            while (file.get(symbol)) {
                buffer.push_back(symbol);
            }

            if (!file.eof() && file.fail()) {
                std::cout << "ERROR: file reading failed" << std::endl;
                return;
            }

            if (!ReadTreeFromString(buffer, &offset, NULL_NODE, &loaded_root)) {
                DeleteSubtree(loaded_root);
                std::cout << "ERROR: invalid file format" << std::endl;
                return;
            }

            if (offset != buffer.size()) {
                DeleteSubtree(loaded_root);
                std::cout << "ERROR: invalid file format" << std::endl;
                return;
            }

            // Тут можно добавить проверки считанного из файла дерева:
            // 1) Черный корень
            // 2) Отсутствие двух красных вершин подряд  
            // 3) Свойство черной высоты
        }
        catch (const std::bad_alloc&) {
            DeleteSubtree(loaded_root);
            std::cout << "ERROR: not enough memory" << std::endl;
            return;
        }

        DeleteSubtree(root);
        root = loaded_root;

        std::cout << "OK" << std::endl;
    }

    TString ToLowerCase(const TString& str) {
        TString result = str;
        for (size_t i = 0; i < result.size(); ++i) {
            if (result[i] >= 'A' && result[i] <= 'Z') {
                result[i] = static_cast<char>(result[i] + 32);
            }
        }
        return result;
    }

    bool IsAlpha(int c) {
        return c != EOF && std::isalpha(static_cast<unsigned char>(c));
    }

    bool IsKeyValid(const TString& key) {
        if (key.empty() || key.size() > 256) {
            return false;
        }

        for (char c : key) {
            if (!IsAlpha(c)) {
                return false;
            }
        }

        return true;
    }
};

bool IsCommandEndValidAndConsumeIt() {
    int next_char = std::cin.peek();
    if (next_char == EOF) {
        return true;
    }
    else if (next_char == '\n') {
        std::cin.get();
        return true;
    }
    else{
        return false;
    }
}

void HandleInvalidInput() {
    if (std::cin.fail()) {
        std::cin.clear();
    }
    std::cout << "ERROR: Invalid command" << std::endl;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}


int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    RedBlackTree rbt;
    char c;
    while (std::cin.get(c)) {

        TString key;
        unsigned long long value;
        TString file_command;
        TString path; // Note: path should be without spaces    

        if (c == '+') {
            if (!(std::cin >> key)) {
                HandleInvalidInput();
                continue;
            }

            if (!(std::cin >> value)) {
                HandleInvalidInput();
                continue;
            }
            
            bool isKeyValid = rbt.IsKeyValid(key);
            bool isCommandEndValid = IsCommandEndValidAndConsumeIt();

            if (isKeyValid && isCommandEndValid) {
                rbt.InsertNode(key, value);
            }
            else {
                HandleInvalidInput();
            }
        }
        else if (c == '-') {
            if (!(std::cin >> key)) {
                HandleInvalidInput();
                continue;
            }

            bool isKeyValid = rbt.IsKeyValid(key);
            bool isCommandEndValid = IsCommandEndValidAndConsumeIt();

            if (isKeyValid && isCommandEndValid) {
                rbt.RemoveNode(key);
            }
            else {
                HandleInvalidInput();
            }
        }
        else if (c == '!') {
            if (!(std::cin >> file_command >> path)) {
                HandleInvalidInput();
                continue;
            }

            bool isCommandEndValid = IsCommandEndValidAndConsumeIt();

            if (file_command == "Save" && isCommandEndValid) {
                rbt.SaveTree(path);
            } 
            else if (file_command == "Load" && isCommandEndValid) {
                rbt.LoadTree(path);
            }
            else {
                HandleInvalidInput();
            }
        }
        else if (rbt.IsAlpha(c)) {
            key.clear();
            key.push_back(c);
            while (std::cin && rbt.IsAlpha(std::cin.peek())) {
                key.push_back(static_cast<char>(std::cin.get()));
            }

            // bool isKeyValid = rbt.IsKeyValid(key); // True, so useless
            bool isCommandEndValid = IsCommandEndValidAndConsumeIt();

            if (isCommandEndValid) {
                rbt.SearchNode(key);
            }
            else {
                HandleInvalidInput();
            }
        }
        else {
           HandleInvalidInput();
        }
    }

    return 0;
}