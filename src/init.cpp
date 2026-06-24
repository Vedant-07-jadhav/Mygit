#include "mygit/core.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

namespace mygit {

bool init_repository() {
  fs::path repo_dir = ".mygit";
  if (fs::exists(repo_dir)) {
    std::cerr << "Error: .mygit repository already exists in this directory.\n";
    return false;
  }
  try {
    // create directory tree
    fs::create_directory(repo_dir);
    fs::create_directory(repo_dir / "objects");
    fs::create_directories(repo_dir / "refs" / "heads");

    // create HEAD file

    std::ofstream head_file(repo_dir / "HEAD");
    if (head_file.is_open()) {
      head_file << "ref: refs/heads/main\n";
      head_file.close();
    } else {
      std::cerr << "Error: Could not create HEAD file\n.";
      return false;
    }

    std::cout << "Initialized empty MyGit repository in "
              << fs::absolute(repo_dir) << "\n";
    return true;
  }

  catch (const fs::filesystem_error &e) {
    std::cerr << "Filesystem error: " << e.what() << "\n";
    return false;
  }
}
} // namespace mygit