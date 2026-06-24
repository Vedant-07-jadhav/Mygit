#pragma once
#include <string>

namespace mygit {
    // Switches the working directory to match the target branch
    void checkout(const std::string& branch_name);
}