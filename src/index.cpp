#include "mygit/index.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>

namespace fs = std::filesystem;

namespace mygit {

void Index::add(const std::string &path, const std::string &sha1) {
  // Get system-level file metadata using POSIX stat (or fallback to basic
  // values) For this milestone, we will use basic C++17 filesystem calls for
  // size
  uint32_t file_size = fs::file_size(path);

  // Note: C++17 std::filesystem::last_write_time is notoriously difficult to
  // cast to a UNIX timestamp safely across platforms. For this MVP, we will
  // mock the timestamps, but in a real system, you'd use the POSIX <sys/stat.h>
  // library.
  uint64_t mock_ctime = 0;
  uint64_t mock_mtime = 0;

  // Check if the file is already in the index. If so, update it.
  for (auto &entry : entries) {
    if (entry.path == path) {
      entry.sha1 = sha1;
      entry.file_size = file_size;
      return;
    }
  }

  // If not found, add a new entry
  entries.push_back({mock_ctime, mock_mtime, file_size, sha1, path});
}

void Index::write_to_disk() {
  fs::path index_path = fs::path(".mygit") / "index";

  // Open in binary mode, truncating (overwriting) the old file
  std::ofstream out(index_path, std::ios::binary | std::ios::trunc);
  if (!out.is_open()) {
    throw std::runtime_error("Fatal: Could not open .mygit/index for writing");
  }

  // Write the Header (Total number of entries)
  uint32_t count = entries.size();
  out.write(reinterpret_cast<const char *>(&count), sizeof(count));

  // Write each entry directly from RAM to the file stream
  for (const auto &entry : entries) {
    // Fixed-size integers
    out.write(reinterpret_cast<const char *>(&entry.ctime),
              sizeof(entry.ctime));
    out.write(reinterpret_cast<const char *>(&entry.mtime),
              sizeof(entry.mtime));
    out.write(reinterpret_cast<const char *>(&entry.file_size),
              sizeof(entry.file_size));

    // SHA-1 is always exactly 40 characters, so we write 40 bytes directly
    out.write(entry.sha1.data(), 40);

    // Path is variable-length. Write the length prefix first, then the string
    // data.
    uint16_t path_len = entry.path.size();
    out.write(reinterpret_cast<const char *>(&path_len), sizeof(path_len));
    out.write(entry.path.data(), path_len);
  }
}

void Index::read_from_disk() {
  entries.clear();
  fs::path index_path = fs::path(".mygit") / "index";

  // If there is no index file yet, just return an empty memory state
  if (!fs::exists(index_path))
    return;

  std::ifstream in(index_path, std::ios::binary);
  if (!in.is_open()) {
    throw std::runtime_error("Fatal: Could not open .mygit/index for reading");
  }

  // 1. Read the Header
  uint32_t count;
  in.read(reinterpret_cast<char *>(&count), sizeof(count));

  // 2. Read exact byte blocks into our variables
  for (uint32_t i = 0; i < count; ++i) {
    IndexEntry entry;

    in.read(reinterpret_cast<char *>(&entry.ctime), sizeof(entry.ctime));
    in.read(reinterpret_cast<char *>(&entry.mtime), sizeof(entry.mtime));
    in.read(reinterpret_cast<char *>(&entry.file_size),
            sizeof(entry.file_size));

    // Read 40 bytes for SHA-1
    entry.sha1.resize(40);
    in.read(entry.sha1.data(), 40);

    // Read the variable length path
    uint16_t path_len;
    in.read(reinterpret_cast<char *>(&path_len), sizeof(path_len));

    entry.path.resize(path_len);
    in.read(entry.path.data(), path_len);

    entries.push_back(entry);
  }
}

void Index::print_entries() const {
  std::cout << "Index contains " << entries.size() << " entries:\n";
  for (const auto &entry : entries) {
    std::cout << entry.sha1 << " " << entry.path
              << " (Size: " << entry.file_size << " bytes)\n";
  }
}

} // namespace mygit