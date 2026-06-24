#pragma once
#include <string>

namespace mygit{
  void create_branch(const std::string& branch_name, const std::string& commit_sha);
}