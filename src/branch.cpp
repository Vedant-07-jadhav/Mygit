#include "mygit/branch.hpp"
#include <filesystem>
#include <fstream>
#include <stdexcept>

namespace fs = std::filesystem;

namespace mygit {
  void create_branch(const std::string& branch_name, const std::string& commit_sha) {
    // Implementation for creating a branch
    if (branch_name.empty() || commit_sha.empty()) {
      throw std::invalid_argument("Branch name and commit SHA cannot be empty.");
    }
    if (commit_sha.length() != 40) {
      throw std::invalid_argument("Commit SHA must be a 40-character string.");
    }
    fs::path branch_file_path = fs::path(".mygit") / "refs" / "heads" / branch_name;
    
    // 1. Create the parent directories FIRST
    fs::create_directories(branch_file_path.parent_path());

    // 2. NOW open the file stream
    std::ofstream branch_file(branch_file_path);
    if (!branch_file.is_open()) {
        throw std::runtime_error("Fatal: Failed to open branch file at " + branch_file_path.string());
    }

    branch_file << commit_sha;
    branch_file.close();
  }
} // namespace mygit