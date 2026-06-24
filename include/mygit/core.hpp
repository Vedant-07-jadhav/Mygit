#pragma once // Prevents this file from being included multiple time by the compiler

namespace mygit {
  // initialise the empty mygit repo in current directory
  // returns true on success, false if a repo already exists or on failure
  bool init_repository();
}