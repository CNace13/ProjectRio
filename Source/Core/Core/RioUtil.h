#ifndef RIOUTILS_H
#define RIOUTILS_H

#include <cstdint>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <memory>
#include <mutex>

namespace RioUtil {
    // Type definition for 32-bit unsigned integer
    using u32 = uint32_t;

    // Function to convert u32 to float
    float floatConverter(u32 in_value);

    uint64_t genRand64();

    enum class LogOutput {
        CONSOLE,
        FILE,
        BOTH
    };

    class Logger {
    public:
        Logger(const std::string& name, LogOutput output, const std::string& filename = "")
            : name(name), output(output) {
            if (output == LogOutput::FILE || output == LogOutput::BOTH) {
                fileStream = std::make_unique<std::ofstream>(filename);
                if (!fileStream->is_open()) {
                    std::cerr << "Failed to open log file: " + filename;
                }
            }
        }

        ~Logger() {
            if (fileStream && fileStream->is_open()) {
                fileStream->close();
            }
        }

        template<typename T>
        Logger& operator<<(const T& message) {
            std::ostringstream oss;
            oss << message;
            log(oss.str());
            return *this;
        }

        Logger& operator<<(std::ostream& (*os)(std::ostream&)) {
            std::ostringstream oss;
            oss << os;
            log(oss.str());
            return *this;
        }

    private:
        std::string name;
        LogOutput output;
        std::unique_ptr<std::ofstream> fileStream;
        std::mutex mtx;

        void log(const std::string& message) {
            std::lock_guard<std::mutex> lock(mtx);

            if (output == LogOutput::CONSOLE || output == LogOutput::BOTH) {
                std::cout << message;
            }
            if ((output == LogOutput::FILE || output == LogOutput::BOTH) && fileStream) {
                (*fileStream) << message;
                fileStream->flush(); // Explicitly flush the file stream
            }
        }
    };
}

#endif // UTILS_H
