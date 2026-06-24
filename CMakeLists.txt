cmake_minimum_required(VERSION 3.10)
project(MyGit CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)

# ONLY ONE add_executable line here!
# Update this line to include the new files:
# Update this line:
add_executable(mygit src/main.cpp src/init.cpp src/crypto.cpp src/zlib_utils.cpp src/database.cpp src/index.cpp src/tree.cpp src/commit.cpp src/branch.cpp src/checkout.cpp)


target_include_directories(mygit PRIVATE include)

# Link the libraries
target_link_libraries(mygit PRIVATE OpenSSL::Crypto ZLIB::ZLIB)