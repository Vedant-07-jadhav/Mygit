#include "mygit/checkout.hpp"
#include "mygit/database.hpp"
#include "mygit/tree.hpp"
#include "mygit/index.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace mygit {

void checkout(const std::string& branch_name) {
    fs::path branch_path = fs::path(".mygit") / "refs" / "heads" / branch_name;
    if (!fs::exists(branch_path)) {
        throw std::runtime_error("Fatal: Branch '" + branch_name + "' does not exist.");
    }

    // 1. Read the Commit SHA from the branch file
    std::ifstream branch_file(branch_path);
    std::string commit_sha;
    branch_file >> commit_sha;
    branch_file.close();

    // 2. Read the Commit Object from the Database
    std::string commit_data = read_object(commit_sha);

    // 3. Extract the Tree SHA from the Commit (First line is "tree <sha>")
    std::istringstream commit_stream(commit_data);
    std::string line, tree_label, tree_sha;
    commit_stream >> tree_label >> tree_sha; 
    
    if (tree_label != "tree") {
        throw std::runtime_error("Fatal: Corrupted commit object.");
    }

    // 4. Read and Parse the Tree Object
    std::string tree_data = read_object(tree_sha);
    std::vector<TreeEntry> tree_entries = parse_tree(tree_data);

    // 5. Overwrite the Working Directory
    // Note: In a production system, you'd check for unsaved changes here. 
    // Since we are pushing for the MVP finish line, we will force-overwrite.
    Index index;
    index.read_from_disk();
    
    for (const auto& entry : tree_entries) {
        // Pull the raw file data from our database
        std::string file_content = read_object(entry.sha1);

        // Write it to the actual file system
        std::ofstream out_file(entry.path, std::ios::binary | std::ios::trunc);
        out_file.write(file_content.data(), file_content.size());
        out_file.close();

        // Update the Index to reflect the restored file
        index.add(entry.path, entry.sha1);
    }
    
    index.write_to_disk();

    // 6. Update HEAD to point to the new branch
    std::ofstream head_file(fs::path(".mygit") / "HEAD", std::ios::trunc);
    head_file << "ref: refs/heads/" << branch_name << "\n";
    
    std::cout << "Switched to branch '" << branch_name << "'\n";
}

} // namespace mygit