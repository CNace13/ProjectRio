#include <array>
#include <optional>
#include <iostream>

template<typename T, std::size_t N>
class Pipeline {
public:
    Pipeline() : head(0), tail(0) {}

     // Advance the pipeline, removing the element at the tail
    void advance() {
        head = (head + 1) % N;
        remove(0);  // Clear the tail element
    }

    void enqueue(const T& element) {
        insert(0,element);
    }

     // Insert an element at the specified index
    void insert(std::size_t index, const T& value) {
        if (index < N) {
            std::size_t actualIndex = (head + N - index - 1) % N;
            data[actualIndex] = value;
        } else {
            std::cerr << "Index out of range." << std::endl;
        }
    }

    // Remove an element at the specified index
    void remove(std::size_t index) {
        if (index < N) {
            std::size_t actualIndex = (head + N - index - 1) % N;
            data[actualIndex] = std::nullopt;
        } else {
            std::cerr << "Index out of range." << std::endl;
        }
    }

    // Inspect an element at the specified index
    std::optional<T> inspect(std::size_t index) const {
        if (index < N) {
            std::size_t actualIndex = (head + N - index - 1) % N;
            return data[actualIndex];
        } else {
            std::cerr << "Index out of range." << std::endl;
            return std::nullopt;
        }
    }

    // Clear the entire pipeline
    void clear() {
        data.fill(std::nullopt);
        head = 0;
        tail = 0;
    }

    // Function to iterate over the pipeline and print the details
    // void print() const {
    //     std::cout << "H ";
    //     for (std::size_t i = 0; i < N; ++i) {
    //         if (inspect(i)) {
    //             std::cout << std::to_string(*inspect(i)) << " ";
    //         } else {
    //             std::cout << "null ";
    //         }
    //     }
    //     std::cout << "T" << std::endl;
    // }

private:
    std::array<std::optional<T>, N> data;
    std::size_t head;
    std::size_t tail;
};