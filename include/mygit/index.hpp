#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace mygit {
struct IndexEntry {
  uint64_t ctime;
  uint64_t mtime;
  uint64_t file_size;
  std::string sha1;
  std::string path;
};

class Index {
private:
  std::vector<IndexEntry> entries;

public:
  // Adds a file to the index (or updates it if it already exists)
  void add(const std::string &path, const std::string &sha1);

  // Saves the current memory state to the binary .mygit/index file
  void write_to_disk();

  // Loads the binary .mygit/index file into memory
  void read_from_disk();

  // Debugging helper
  void print_entries() const;

  const std::vector<IndexEntry> &get_entries() const { return entries; }
};
} // namespace mygit