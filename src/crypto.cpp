#include "mygit/crypto.hpp"

#include <cstddef>
#include <iomanip>
#include <openssl/evp.h>
#include <sstream>
#include <stdexcept>
#include <string>

namespace mygit {

std::string compute_sha1(const std::string &data) {
  // Create a digest context that will hold the SHA-1 state.
  EVP_MD_CTX *context = EVP_MD_CTX_new();
  if (context == nullptr) {
    throw std::runtime_error("Failed to create OpenSSL digest context");
  }

  // Configure the context to use the SHA-1 algorithm.
  if (EVP_DigestInit_ex(context, EVP_sha1(), nullptr) != 1) {
    EVP_MD_CTX_free(context);
    throw std::runtime_error("Failed to initialize SHA-1 digest");
  }

  // Feed the input bytes into the hash function.
  if (EVP_DigestUpdate(context, data.data(), data.size()) != 1) {
    EVP_MD_CTX_free(context);
    throw std::runtime_error("Failed to update SHA-1 digest");
  }

  // Buffer for the resulting digest.
  unsigned char digest[EVP_MAX_MD_SIZE];
  unsigned int digestLength = 0;

  // Finalize the computation and retrieve the hash bytes.
  if (EVP_DigestFinal_ex(context, digest, &digestLength) != 1) {
    EVP_MD_CTX_free(context);
    throw std::runtime_error("Failed to finalize SHA-1 digest");
  }

  EVP_MD_CTX_free(context);

  // Convert the raw digest bytes into a hexadecimal string.
  std::stringstream hexStream;

  for (unsigned int i = 0; i < digestLength; ++i) {
    hexStream << std::hex << std::setw(2) << std::setfill('0')
              << static_cast<int>(digest[i]);
  }

  return hexStream.str();
}

std::string hex_to_raw(const std::string &hex_str) {
  if (hex_str.length() != 40) {
    throw std::invalid_argument("Hex string must be exactly 40 characters");
  }
  std::string raw;
  raw.reserve(20);
  for (size_t i = 0; i < 40; i += 2) {
    // Grab 2 hex characters at a time (e.g., "25")
    std::string byteString = hex_str.substr(i, 2);
    // Convert them to a single raw char/byte
    char byte = static_cast<char>(strtol(byteString.c_str(), nullptr, 16));
    raw.push_back(byte);
  }
  return raw;
}
std::string raw_to_hex(const std::string &raw) {
  if (raw.length() != 20) {
        throw std::invalid_argument("Raw SHA-1 must be exactly 20 bytes");
    }
    std::stringstream ss;
    for (unsigned char c : raw) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c);
    }
    return ss.str();
  }

} // namespace mygit

/*
Git identifies objects by the SHA-1 of their raw contents. This function creates
an OpenSSL digest context, configures it for SHA-1, feeds the object bytes into
the algorithm, retrieves the 20-byte digest, converts it into Git's 40-character
hexadecimal representation, and returns it.
*/