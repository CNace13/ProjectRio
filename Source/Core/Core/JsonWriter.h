// #include <fstream>
// #include <string>
// #include <type_traits>
// #include <stack>
// #include <fmt/core.h>

/*
class JsonWriter {
public:
    JsonWriter(const std::string& filename) : ofs(filename), is_first_element(true) {
        ofs << "{" << std::endl;
        structure_stack.push('}');
    }

    ~JsonWriter() {
        endJSON();
        ofs.close();
    }

    template<typename T>
    void writeKeyValue(const std::string& key, const T& value) {
        if (!is_first_element) {
            ofs << "," << std::endl;
        }
        ofs << fmt::format("{}\"{}\": {}", std::string(structure_stack.size() * 4, ' '), key, formatValue(value));
        is_first_element = false;
    }

    void startObject(const std::string& key) {
        if (!is_first_element) {
            ofs << "," << std::endl;
        }
        ofs << fmt::format("{}\"{}\": {{\n", std::string(structure_stack.size() * 4, ' '), key);
        structure_stack.push('}');
        is_first_element = true;
    }

    void endElement() {
        if (!structure_stack.empty()) {
            char ending_char = structure_stack.top();
            structure_stack.pop();
            ofs << std::endl << fmt::format("{}", std::string(structure_stack.size() * 4, ' ')) << ending_char;
            is_first_element = false;
        }
    }

    void startArray(const std::string& key) {
        if (!is_first_element) {
            ofs << "," << std::endl;
        }
        ofs << fmt::format("{}\"{}\": [\n", std::string(structure_stack.size() * 4, ' '), key);
        structure_stack.push(']');
        is_first_element = true;
    }

    void writeArrayValue(const std::string& value) {
        if (!is_first_element) {
            ofs << "," << std::endl;
        }
        ofs << fmt::format("{}{}", std::string(structure_stack.size() * 4, ' '), formatValue(value));
        is_first_element = false;
    }

    void endJSON() {
        while (!structure_stack.empty()) {
            endElement();
        }
    }

private:
    std::ofstream ofs;
    bool is_first_element;
    std::stack<char> structure_stack;

    template<typename T>
    typename std::enable_if<std::is_arithmetic<T>::value, std::string>::type
    formatValue(const T& value) const {
        return fmt::format("{}", value);
    }

    std::string formatValue(const std::string& value) const {
        return fmt::format("\"{}\"", value);
    }
};*/

/*
#include <fstream>
#include <stack>
#include <string>
#include <type_traits>
#include <fmt/core.h>
#include <fmt/ostream.h>

class JSONWriter {
private:
    std::ofstream file;
    std::stack<char> endChars;
    bool isFirstElement = true;

    void writeIndent() {
        fmt::print(file, "{:{}}", "", endChars.size() * 2);
    }

    template<typename T>
    void writeValue(const T& value) {
        if constexpr (std::is_same_v<T, std::string>) {
            fmt::print(file, "\"{}\"", value);
        } else if constexpr (std::is_same_v<T, uint8_t>) {
            fmt::print(file, "{}", static_cast<int>(value));
        } else {
            fmt::print(file, "{}", value);
        }
    }

public:
    JSONWriter(const std::string& filename) : file(filename) {
        fmt::print(file, "{{\n");
        endChars.push('}');
    }

    ~JSONWriter() {
        endJSON();
        file.close();
    }

    template<typename T>
    void writeKeyValue(const std::string& key, const T& value) {
        if (!isFirstElement) {
            fmt::print(file, ",\n");
        }
        isFirstElement = false;

        writeIndent();
        fmt::print(file, "\"{}\":", key);
        writeValue(value);
    }

    void startObject(const std::string& key) {
        if (!isFirstElement) {
            fmt::print(file, ",\n");
        }
        isFirstElement = true;

        writeIndent();
        fmt::print(file, "\"{}\":{{\n", key);
        endChars.push('}');
    }

    void startArray(const std::string& key) {
        if (!isFirstElement) {
            fmt::print(file, ",\n");
        }
        isFirstElement = true;

        writeIndent();
        fmt::print(file, "\"{}\":[\n", key);
        endChars.push(']');
    }

    void endElement() {
        fmt::print(file, "\n");
        endChars.pop();
        writeIndent();
        fmt::print(file, "{}", endChars.top());
        isFirstElement = false;
    }

    void endJSON() {
        while (!endChars.empty()) {
            fmt::print(file, "\n");
            endChars.pop();
            if (!endChars.empty()) {
                writeIndent();
                fmt::print(file, "{}", endChars.top());
            }
        }
    }
};*/

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

    ~JSONWriter() {
        endJSON();
    }

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
};
