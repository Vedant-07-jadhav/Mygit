// tree builder

#pragma once
#include <string>
#include <vector>

namespace mygit {

struct TreeEntry {
  std::string mode; // e.g., "100644" for a file, "40000" for a directory
  std::string path; 
  std::string sha1; // 40-character SHA-1 hash of the object
};
std::vector<TreeEntry> parse_tree(const std::string &tree_sha);

// Reads the current Index, formats a Git Tree object,
// writes it to the database, and returns the Tree's SHA-1 hash.
std::string write_tree();

} // namespace mygit