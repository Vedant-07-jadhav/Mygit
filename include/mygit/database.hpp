#pragma once
#include <string>

namespace mygit {
  // Takes raw file content, formats it, hashes it, compresses it, 
    // and writes it to the .mygit/objects/ fan-out directory.
    // Returns the calculated 40-character SHA-1 hash.
    std::string write_object(const std::string& content, const std::string& type = "blob");
    // Reads an object from the fan-out directory by its 40-char SHA-1 hash.
    // Decompresses it, removes the Git header, and returns the pure file content.
    std::string read_object(const std::string& sha1);
}