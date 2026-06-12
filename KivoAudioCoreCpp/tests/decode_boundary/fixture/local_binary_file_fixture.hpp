#pragma once

#include <array>
#include <filesystem>
#include <fstream>

namespace kivo::decode_boundary::test {

class TemporaryBinaryFile {
public:
    TemporaryBinaryFile() {
        path = std::filesystem::temp_directory_path() / "kivo_byte_source_test.bin";
        std::ofstream output(path, std::ios::binary | std::ios::trunc);
        const std::array<unsigned char, 10> bytes{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        output.write(
            reinterpret_cast<const char*>(bytes.data()),
            static_cast<std::streamsize>(bytes.size()));
    }

    ~TemporaryBinaryFile() {
        std::error_code error;
        std::filesystem::remove(path, error);
    }

    std::filesystem::path path;
};

} // namespace kivo::decode_boundary::test
