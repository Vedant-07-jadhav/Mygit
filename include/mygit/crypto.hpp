#pragma once
#include <string>

namespace mygit {
std::string compute_sha1(const std::string &data);
// Converts a 40-character hex string into a 20-byte raw binary string
std::string hex_to_raw(const std::string &hex_str);

std::string raw_to_hex(const std::string &raw_str);
} // namespace mygit