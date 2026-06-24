#include "mygit/commit.hpp"
#include "mygit/database.hpp"
#include <chrono>
#include <sstream>

namespace mygit { 
  std::string create_commit(const std::string &tree_sha, const std::string &message) {
    // Get the current time in seconds since epoch
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);

    // Format the commit object
    std::ostringstream commit_stream;
    commit_stream << "tree " << tree_sha << "\n";
    
    // Real Git reads ~/.gitconfig for this, but we will hardcode it for our engine
    commit_stream << "author Vedant <vedant@iitm.ac.in> " << time_t_now << " +0530\n";
    commit_stream << "committer Vedant <vedant@iitm.ac.in> " << time_t_now << " +0530\n";

    commit_stream << "\n";
    commit_stream << message << "\n";

    return write_object(commit_stream.str(), "commit");
  }
}// namespace mygit