#include <cctype> // std::isspace
#include <chrono> // std::chrono::duration, std::chrono::high_resolution_clock
#include <climits> // ULONG_MAX
#include <cmath> // std::sqrtl
#include <exception> // std::exception
#include <iostream> // std::cin, std::cout, std::istream, std::ostream
#include <random> // std::random_device, std::mt19937, std::uniform_int_distribution

template <typename T>
class TVector {
    private:
        T* data;
        size_t capacity;
        size_t size_;
    public:
        TVector() {
            data = nullptr;
            capacity = 0;
            size_ = 0;
        }

        TVector(const TVector& other) {
            size_ = other.size_;

            data = new T[size_];
            for(size_t i = 0; i < size_; ++i) {
                data[i] = other.data[i];
            }
            capacity = size_;
        }

        TVector& operator=(const TVector& other){
            if (*this != other) {
                size_ = other.size_;

                T* newData = new T[other.size_];
                for(size_t i = 0; i < size_; ++i) {
                    newData[i] = other.data[i];
                }
                delete[] data;
                data = newData;
                capacity = other.size_; 
            }
            return *this;
        }

        TVector(TVector&& other) noexcept {
            data = other.data;
            capacity = other.capacity;
            size_ = other.size_;

            other.data = nullptr;
            other.capacity = 0;
            other.size_ = 0;
        }

        TVector& operator=(TVector&& other) noexcept {
            if (*this != other) {
                delete[] data;
                
                data = other.data;
                capacity = other.capacity;
                size_ = other.size_;

                other.data = nullptr;
                other.capacity = 0;
                other.size_ = 0;
            }
            return *this;
        }

        ~TVector(){
            delete[] data;
        }

        T& operator[](size_t index) {
            if (index >= size_) {
                throw std::exception();
            }
            else {
                return data[index];
            }
        }

        const T& operator[](size_t index) const {
            if (index >= size_) {
                throw std::exception();
            }
            else {
                return data[index];
            }
        }

        void push_back(const T& obj) {
            if (size_ == capacity) {
                reserve(capacity * 2 + 1);
            }
            // data[size_] = obj;
            new (&data[size_]) T(obj); // MVP of the lab: placement new
            ++size_;
        }

        size_t size() const {
            return size_;
        }

        void reserve(size_t newCapacity) {
            if (newCapacity > capacity) {
                T* newData = new T[newCapacity];
                for (size_t i = 0; i < size_; ++i) {
                    newData[i] = data[i];
                }
                delete[] data;
                data = newData;
                capacity = newCapacity;
            }
        }

        void resize(size_t newSize) {
            if (newSize > capacity) {
                reserve(newSize);
            }
            size_ = newSize;
        }

        bool operator==(const TVector& other) const {
            if (size_ != other.size_) return false;
            for (size_t i = 0; i < size_; ++i) {
                if (data[i] != other.data[i]) return false;
            }
            return true;
        }

        bool operator!=(const TVector& other) const {
            return !(*this == other);
        }

        T* begin() {
            return data;
        }

        const T* begin() const {
            return data;
        }

        const T* end() const {
            return data + size_;
        }
};

TVector<char> MakeTVector(const char* s) {
    TVector<char> result;
    while (*s) {
        result.push_back(*s);
        ++s;
    }
    return result;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const TVector<T>& vec) {
    for (size_t i = 0; i < vec.size(); ++i) {
        os << vec[i];
    }
    return os;
}

template<typename T>
std::istream& operator>>(std::istream& is, TVector<T>& vec) {
    T value;
    if (is >> value) {
        vec.push_back(value);
    }
    return is;
}

template<>
std::istream& operator>><char>(std::istream& is, TVector<char>& vec) {
    vec.resize(0);

    while (std::isspace(is.peek())) {
        is.get();
    }

    while (is.peek() != EOF && !std::isspace(is.peek())) {
        char ch = is.get();
        vec.push_back(ch);
    }
    return is;
}

template<class T>
const T& Min(const T& a, const T& b)
{
    return (b < a) ? b : a;
}

template<class T>
const T& Max(const T& a, const T& b)
{
    return (b < a) ? a : b;
}

class TItem {
private:
    size_t key;
    TVector<char> value;
public:
    TItem() = default;

    TItem(size_t k, const char* s) {
        key = k;
        while (*s) {
            value.push_back(*s);
            ++s;
        }
    }

    TItem(size_t k, const TVector<char>& v) {
        key = k;
        value = v;
    }

    size_t getKey() const { 
        return key; 
    }

    const TVector<char>& getValue() const { 
        return value; 
    }

    bool operator==(const TItem& other) const {
        return key == other.key && value == other.value;
    }

    bool operator!=(const TItem& other) const {
        return !(*this == other);
    }
};

std::ostream& operator<<(std::ostream& os, TItem& item) {
    os << item.getKey() << '\t' << item.getValue();
    return os; 
}

void GenerateRandomInput(TVector<TItem>& data, size_t amount) {
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<unsigned long long> distribution(0, ULONG_MAX);

    for(size_t i = 0; i < amount; ++i) {
        size_t random_key = distribution(generator);
        TItem item(random_key, "stroka");
        data.push_back(item);
    }
}

using SortFunction = TVector<TItem>(*)(const TVector<TItem>&);

template <typename SortFunc>
void RunBenchmark(SortFunc sortFunc, TVector<char> sortFuncName) {
    const size_t data_sizes[] = {1000, 5000, 10000, 50000, 100000, 500000, 1000000};
    const int repeats = 3;

    std::cout << "\nBenchmarking "<< sortFuncName << ":\n";
    std::cout << "Size\tTime (ms)\n";

    for (size_t data_size : data_sizes) {
        double total_time = 0.0;
        for (int r = 0; r < repeats; ++r) {
            TVector<TItem> data;
            GenerateRandomInput(data, data_size);

            auto start = std::chrono::high_resolution_clock::now();
            TVector<TItem> sorted = sortFunc(data);
            auto end = std::chrono::high_resolution_clock::now();

            std::chrono::duration<double, std::milli> elapsed = end - start;
            total_time += elapsed.count();
        }
        double avg_time = total_time / repeats;
        std::cout << data_size << "\t" << avg_time << std::endl;
    }
}

void Merge(TVector<TItem>& data, size_t left, size_t mid, size_t right) {
    size_t it1 = 0;
    size_t it2 = 0;
    TVector<TItem> result;
    result.resize(right - left);

    while ((left + it1 < mid) && (mid + it2 < right)) {
        if (data[left + it1].getKey() <= data[mid + it2].getKey()) {
            result[it1 + it2] = data[left + it1];
            it1++;
        }
        else {
            result[it1 + it2] = data[mid + it2];
            it2++;
        }
    }

    while (left + it1 < mid) {
        result[it1 + it2] = data[left + it1];
        it1++;
    }

    while (mid + it2 < right) {
        result[it1 + it2] = data[mid + it2];
        it2++;
    }

    for(size_t i=0; i < it1 + it2; ++i) {
        data[left + i] = result[i];
    }
    

}

TVector<TItem> MergeSort(const TVector<TItem>& data) {
    TVector<TItem> result;
    result.reserve(data.size());
    for (TItem item: data) {
        result.push_back(item);
    }
    size_t n = data.size();
    for(size_t i = 1; i < n; i *= 2) {
        for(size_t left = 0; left < n - i; left += 2 * i) {
            size_t mid = left + i;
            size_t right = Min(left + 2 * i, n);
            Merge(result, left, mid, right);
        }
    }

    return result;
}


TVector<TItem> BubbleSort(const TVector<TItem>& input) {
    TVector<TItem> result = input;
    size_t n = result.size();
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n - i - 1; ++j) {
            if (result[j].getKey() > result[j+1].getKey()) {
                TItem tmp = std::move(result[j]);
                result[j] = std::move(result[j+1]);
                result[j+1] = std::move(tmp);
            }
        }
    }
    return result;
}

TVector<TItem> BucketSort(const TVector<TItem>& data) {
    size_t bucketsAmount = Max(size_t(1), static_cast<size_t>(std::sqrtl(data.size())));
    // size_t bucketsAmount = Max(size_t(1), data.size() / 2);
    TVector<TVector<TItem>> buckets;
    buckets.resize(bucketsAmount);

    size_t minElement = 0;
    size_t maxElement = ULONG_MAX;

    for(size_t i = 0; i < data.size(); ++i) {
        minElement = Min(minElement, data[i].getKey());
        maxElement = Max(maxElement, data[i].getKey());
    }

    size_t value_range = maxElement - minElement;
    for(TItem item: data) {
        size_t bucketIndex = size_t((item.getKey() - minElement) / (1 + value_range / static_cast<long double>(bucketsAmount)));
        buckets[bucketIndex].push_back(item);
    }

    TVector<TItem> result;
    result.reserve(data.size());
    for(TVector<TItem> bucket: buckets) {
        // bucket = BubbleSort(bucket);
        bucket = MergeSort(bucket);
        for(TItem item: bucket) {
            result.push_back(item);
        }
    }

    return result;
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    TVector<TItem> data;
    
    // Manual input
    size_t key;
    TVector<char> value;
    while (std::cin >> key >> value) {
        TItem item(key, value);
        data.push_back(item);
    }

    // Auto input
    // GenerateRandomInput(data, 100);

    data = BucketSort(data);

    for (size_t i = 0; i < data.size(); ++i) {
        std::cout << data[i] << '\n';
    }

    // RunBenchmark(BucketSort, MakeTVector("BucketSort"));
    // RunBenchmark(BubbleSort, MakeTVector("BubbleSort"));

    return 0;
}