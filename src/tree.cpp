/*
[mode] [file_path]\0[20_byte_raw_sha1] : format of git tree
*/

#include "mygit/crypto.hpp"
#include "mygit/database.hpp"
#include "mygit/index.hpp"
#include "mygit/tree.hpp"
#include <sstream>
#include <vector>

namespace mygit {

std::vector<TreeEntry> parse_tree(const std::string & tree_data){
  std::vector<TreeEntry> entries;
  size_t pos = 0;

  while(pos < tree_data.size()){
    size_t space_pos = tree_data.find(' ', pos);
    if(space_pos == std::string::npos) break;
    std::string mode = tree_data.substr(pos, space_pos - pos);
    pos = space_pos + 1;

    size_t null_pos = tree_data.find('\0', pos);
    if(null_pos == std::string::npos) break;
    std::string name = tree_data.substr(pos, null_pos - pos);
    pos = null_pos + 1;

    std::string raw_sha1 = tree_data.substr(pos, 20);
    std::string hex_sha1 = raw_to_hex(raw_sha1);
    pos += 20;

    entries.push_back({mode, name, hex_sha1});
  }
  return entries;
}

std::string write_tree() {
  // 1. Load the current staging area into memory
  Index index;
  index.read_from_disk();

  std::string tree_content = "";

  // 2. Build the Tree content
  for (const auto &entry :
       index.get_entries()) { // We made entries private earlier, so we'll need
                              // to add
    // a getter, or for now, let's assume we can access it.
    // File mode for a standard text file is 100644
    // The format is: "100644 filename\0<20_byte_raw_sha>"
    std::string mode = "100644";
    std::string raw_sha1 = hex_to_raw(entry.sha1);

    tree_content += mode + " " + entry.path + '\0' + raw_sha1;
  }

  // 3. Write it to the object database with the type "tree"
  // Our write_object function handles the Git header, hashing, and Zlib
  // compression automatically!
  return write_object(tree_content, "tree");
}

} // namespace mygit