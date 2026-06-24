#include "mygit/branch.hpp"
#include "mygit/checkout.hpp"
#include "mygit/commit.hpp"
#include "mygit/core.hpp"
#include "mygit/database.hpp"
#include "mygit/index.hpp"
#include "mygit/tree.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: mygit <command>\n";
    return 1;
  }

  std::string command = argv[1];

  if (command == "init") {
    return mygit::init_repository() ? 0 : 1;
  }
  // NEW COMMAND: mygit hash-object -w <file_path>
  else if (command == "hash-object") {
    if (argc < 4 || std::string(argv[2]) != "-w") {
      std::cerr << "Usage: mygit hash-object -w <file>\n";
      return 1;
    }

    std::string file_path = argv[3];

    // Read the target file from the filesystem into a string
    std::ifstream file(file_path, std::ios::binary);
    if (!file.is_open()) {
      std::cerr << "Fatal: Cannot open file '" << file_path << "'\n";
      return 1;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    // Write to database and print the hash!
    try {
      std::string sha1 = mygit::write_object(content);
      std::cout << sha1 << "\n";
      return 0;
    } catch (const std::exception &e) {
      std::cerr << "Error: " << e.what() << "\n";
      return 1;
    }
  }
  // NEW COMMAND: mygit cat-file -p <hash>
  else if (command == "cat-file") {
    if (argc < 4 || std::string(argv[2]) != "-p") {
      std::cerr << "Usage: mygit cat-file -p <object_hash>\n";
      return 1;
    }

    std::string sha1 = argv[3];
    try {
      std::string content = mygit::read_object(sha1);
      std::cout << content; // Print exactly what was stored
      return 0;
    } catch (const std::exception &e) {
      std::cerr << e.what() << "\n";
      return 1;
    }
  }

  else if (command == "add") {
    if (argc < 3) {
      std::cerr << "Usage: mygit add <path>\n";
      return 1;
    }

    std::string target_path = argv[2];

    // Load the index once before the loop
    mygit::Index index;
    index.read_from_disk();

    try {
      if (std::filesystem::is_directory(target_path)) {
        // The Directory Crawler
        for (const auto &entry :
             std::filesystem::recursive_directory_iterator(target_path)) {
          // Skip directories (we only hash files)
          if (entry.is_directory())
            continue;

          std::string filepath = entry.path().string();

          // CRITICAL: Do not hash the .mygit database itself!
          if (filepath.find(".mygit") != std::string::npos)
            continue;

          // Read, Hash, Compress, and Store
          std::ifstream file(filepath, std::ios::binary);
          std::stringstream buffer;
          buffer << file.rdbuf();
          std::string sha1 = mygit::write_object(buffer.str());

          // Add to Index RAM state
          index.add(filepath, sha1);
          std::cout << "Added " << filepath << "\n";
        }
      } else {
        // The Single File Path (What you originally wrote)
        std::ifstream file(target_path, std::ios::binary);
        if (!file.is_open()) {
          throw std::runtime_error("Cannot open file: " + target_path);
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string sha1 = mygit::write_object(buffer.str());
        index.add(target_path, sha1);
        std::cout << "Added " << target_path << "\n";
      }

      // Save the Index to disk once the loop is finished
      index.write_to_disk();
      return 0;

    } catch (const std::exception &e) {
      std::cerr << "Error: " << e.what() << "\n";
      return 1;
    }
  }

  else if (command == "write-tree") {
    try {
      std::string tree_sha = mygit::write_tree();
      std::cout << tree_sha << "\n";
      return 0;
    } catch (const std::exception &e) {
      std::cerr << "Error: " << e.what() << "\n";
      return 1;
    }
  }

  else if (command == "commit-tree") {
    if (argc < 5 || std::string(argv[3]) != "-m") {
      std::cerr << "Usage: mygit commit-tree <tree_sha> -m \"<message>\"\n";
      return 1;
    }

    std::string tree_sha = argv[2];
    std::string message = argv[4];

    try {
      std::string commit_sha = mygit::create_commit(tree_sha, message);
      std::cout << commit_sha << "\n";
      return 0;
    } catch (const std::exception &e) {
      std::cerr << "Error: " << e.what() << "\n";
      return 1;
    }
  }

  else if (command == "branch") {
    if (argc < 4) {
      std::cerr << "Usage: mygit branch <branch_name> <commit_sha>\n";
      return 1;
    }

    std::string branch_name = argv[2];
    std::string commit_sha = argv[3];

    try {
      mygit::create_branch(branch_name, commit_sha);
      std::cout << "Branch '" << branch_name << "' created at commit "
                << commit_sha << "\n";
      return 0;
    } catch (const std::exception &e) {
      std::cerr << "Error: " << e.what() << "\n";
      return 1;
    }
  }

  else if (command == "checkout") {
    if (argc < 3) {
      std::cerr << "Usage: mygit checkout <branch_name>\n";
      return 1;
    }
    try {
      mygit::checkout(argv[2]);
      return 0;
    } catch (const std::exception &e) {
      std::cerr << "Error: " << e.what() << "\n";
      return 1;
    }
  }

  else {
    std::cerr << "Unknown command: " << command << "\n";
    return 1;
  }
}