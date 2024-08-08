#include <fstream>
#include <stack>
#include <string>
#include <type_traits>
#include <fmt/core.h>
#include <fmt/ostream.h>
#include <iostream>

class JSONWriter {
public:
    enum class OutputMode {
        File,
        Cout,
        Both
    };

private:
    std::string filename;
    std::string buffer;
    std::stack<char> endChars;
    bool isFirstElement = true;
    OutputMode outputMode;

    void writeIndent() {
        buffer += std::string(endChars.size() * 2, ' ');
    }

    template<typename T>
    void writeValue(const T& value) {
        if constexpr (std::is_same_v<T, std::string>) {
            fmt::format_to(std::back_inserter(buffer), "\"{}\"", value);
        } else if constexpr (std::is_same_v<T, uint8_t>) {
            fmt::format_to(std::back_inserter(buffer), "{}", static_cast<int>(value));
        } else {
            fmt::format_to(std::back_inserter(buffer), "{}", value);
        }
    }

    void writeToFile() {
        std::ofstream file(filename);
        if (file.is_open()) {
            file << buffer;
            file.close();
        } else {
            std::cerr << "Unable to open file for writing";
        }
    }

    void writeToCout() {
        std::cout << buffer;
    }

public:
    JSONWriter(const std::string& filename, OutputMode mode = OutputMode::File)
        : filename(filename), outputMode(mode) {
        buffer = "{\n";
        endChars.push('}');
    }

    ~JSONWriter() { }

    template<typename T>
    void writeKeyValue(const std::string& key, const T& value) {
        if (!isFirstElement) {
            buffer += ",\n";
        }
        isFirstElement = false;

        writeIndent();
        fmt::format_to(std::back_inserter(buffer), "\"{}\":", key);
        writeValue(value);
    }

    void startObject(const std::string& key) {
        if (!isFirstElement) {
            buffer += ",\n";
        }
        isFirstElement = true;

        writeIndent();
        fmt::format_to(std::back_inserter(buffer), "\"{}\":{{\n", key);
        endChars.push('}');
    }

    void startArray(const std::string& key) {
        if (!isFirstElement) {
            buffer += ",\n";
        }
        isFirstElement = true;

        writeIndent();
        fmt::format_to(std::back_inserter(buffer), "\"{}\":[\n", key);
        endChars.push(']');
    }

    void endElement() {
        buffer += "\n";
        endChars.pop();
        writeIndent();
        buffer += endChars.top();
        isFirstElement = false;
    }

    void endJSON() {
        while (!endChars.empty()) {
            buffer += "\n";
            endChars.pop();
            if (!endChars.empty()) {
                writeIndent();
                buffer += endChars.top();
            }
        }

        if (outputMode == OutputMode::File || outputMode == OutputMode::Both) {
            writeToFile();
        }
        if (outputMode == OutputMode::Cout || outputMode == OutputMode::Both) {
            writeToCout();
        }
    }

    void reset() {
        while (!endChars.empty()) {
            buffer += "\n";
            endChars.pop();
            if (!endChars.empty()) {
                writeIndent();
                buffer += endChars.top();
            }
        }

        if (outputMode == OutputMode::File || outputMode == OutputMode::Both) {
            writeToFile();
        }
        if (outputMode == OutputMode::Cout || outputMode == OutputMode::Both) {
            writeToCout();
        }
    }
};
