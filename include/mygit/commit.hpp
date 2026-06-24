#pragma once
#include <string>

namespace mygit {

    // Formats a commit object, writes it to the database, 
    // and returns the 40-char SHA-1 hash of the new commit.
    std::string create_commit(const std::string& tree_sha, const std::string& message);
    
}