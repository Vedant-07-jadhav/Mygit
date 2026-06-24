#include "mygit/crypto.hpp"
#include "mygit/database.hpp"
#include "mygit/zlib_utils.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

namespace fs = std::filesystem;

namespace mygit {
std::string write_object(const std::string &content, const std::string &type) {
  // construct the git header "<type> <size>\0<content>"
  std::string git_object =
      type + " " + std::to_string(content.size()) + '\0' + content;

  // hash the exact string;
  std::string sha1 = compute_sha1(git_object);

  // compress the string using Zlib
  std::string compressed_data = compress_data(git_object);

  // determine the fan out path
  //  substring (start_index, length)
  std::string dir_name = sha1.substr(0, 2);
  std::string file_name = sha1.substr(2, 38);

  fs::path object_dir = fs::path(".mygit") / "objects" / dir_name;
  fs::path object_path = object_dir / file_name;

  // create the direcctory if it doesnt exist
  if (!fs::exists(object_dir)) {
    fs::create_directories(object_dir);
  }

  // write the compresed data to diskk
  //  we use std::ios::binary so c++ doesnt currupt the Zlib bytes
  //  by trying to format newlines
  std::ofstream out_file(object_path, std::ios::binary);
  if (!out_file.is_open()) {
    throw std::runtime_error("Failed to open file for writing: " + object_path.string());
  }

  out_file.write(compressed_data.data(), compressed_data.size());
  out_file.close();
  return sha1;
}


std::string read_object(const std::string& sha1) {
    if (sha1.length() != 40) {
        throw std::invalid_argument("Invalid SHA-1 hash length");
    }

    // 1. Reconstruct the fan-out path
    std::string dir_name = sha1.substr(0, 2); 
    std::string file_name = sha1.substr(2, 38);
    fs::path object_path = fs::path(".mygit") / "objects" / dir_name / file_name;

    if (!fs::exists(object_path)) {
        throw std::runtime_error("Fatal: Not a valid object name " + sha1);
    }

    // 2. Read the compressed binary file from disk
    std::ifstream in_file(object_path, std::ios::binary);
    std::stringstream buffer;
    buffer << in_file.rdbuf();
    std::string compressed_data = buffer.str();

    // 3. Decompress the data
    std::string full_data = decompress_data(compressed_data);

    // 4. Strip the Git header (find the null byte '\0' and return everything after it)
    size_t null_pos = full_data.find('\0');
    if (null_pos == std::string::npos) {
        throw std::runtime_error("Corrupted object: missing null byte header");
    }

    return full_data.substr(null_pos + 1);
}
} // namespace mygit