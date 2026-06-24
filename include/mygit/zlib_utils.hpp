#pragma once
#include <string>

namespace mygit {
    // Takes raw data and returns Zlib DEFLATE compressed data
    std::string compress_data(const std::string& data);
    // Takes Zlib DEFLATE compressed data and returns the original raw data
    std::string decompress_data(const std::string& compressed_data);
}