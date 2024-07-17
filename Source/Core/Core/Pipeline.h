#include <array>
#include <optional>
#include <iostream>

template<typename T, std::size_t N>
class Pipeline {
public:
    Pipeline() : head(0), tail(0) {}

    // Enqueue an element at the head
    void enqueue(const T& value) {
        data[head] = value;
    }

    // Advance the pipeline, removing the element at the tail
    void advance() {
        tail = (tail + 1) % N;
        head = (head + 1) % N;
        data[tail] = std::nullopt;

        // print();
    }

    // Insert an element at the specified index
    void insert(std::size_t index, const T& value) {
        if (index < N) {
            std::size_t actualIndex = (head + index) % N;
            data[actualIndex] = value;
        } else {
            std::cerr << "Index out of range." << std::endl;
        }
    }

    // Clear the entire pipeline
    void clear() {
        data.fill(std::nullopt);
        head = 0;
        tail = 0;
    }

    // Remove an element at the specified index
    void remove(std::size_t index) {
        if (index < N) {
            std::size_t actualIndex = (head + index) % N;
            data[actualIndex] = std::nullopt;
        } else {
            std::cerr << "Index out of range." << std::endl;
        }
    }

    // Inspect an element at the specified index
    std::optional<T> inspect(std::size_t index) const {
        if (index < N) {
            std::size_t actualIndex = (head + index) % N;
            return data[actualIndex];
        } else {
            std::cerr << "Index out of range." << std::endl;
            return std::nullopt;
        }
    }

    // Function to iterate over the pipeline and print the details
    void print() const {
        std::cout << "Pipeline contents (index relative to tail, actual index, value):\n";
        for (std::size_t i = 0; i < N; ++i) {
            std::size_t actualIndex = (head + i) % N;
            std::cout << "Logical Index: " << i
                      << ", Actual Index: " << actualIndex
                      << ", Value: " << (data[actualIndex] ? std::to_string(*data[actualIndex]) : "null")
                      << '\n';
        }
    }

private:
    std::array<std::optional<T>, N> data;
    std::size_t head;
    std::size_t tail;
};