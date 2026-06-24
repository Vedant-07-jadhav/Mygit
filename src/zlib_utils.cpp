#include "mygit/zlib_utils.hpp"
#include <zlib.h>
#include <stdexcept>
#include <vector>
#include <cstdint> 
#include <cstring>

namespace mygit {

std::string compress_data(const std::string& data) {
    // Calculate maximum required buffer size for compressed data
    uLongf compressed_size = compressBound(data.size());
    std::vector<uint8_t> compressed_buffer(compressed_size);

    // Perform the compression
    int result = compress(
        compressed_buffer.data(), &compressed_size,
        reinterpret_cast<const Bytef*>(data.data()), data.size()
    );

    if (result != Z_OK) {
        throw std::runtime_error("Zlib compression failed");
    }

    // Return as a std::string
    return std::string(reinterpret_cast<char*>(compressed_buffer.data()), compressed_size);
}

std::string decompress_data(const std::string& compressed_data) {
    z_stream zs;
    memset(&zs, 0, sizeof(zs));

    if (inflateInit(&zs) != Z_OK) {
        throw std::runtime_error("inflateInit failed while decompressing.");
    }

    zs.next_in = (Bytef*)compressed_data.data();
    zs.avail_in = compressed_data.size();

    int ret;
    char outbuffer[32768]; // 32KB chunk size
    std::string outstring;

    // Loop to decompress chunk by chunk
    do {
        zs.next_out = reinterpret_cast<Bytef*>(outbuffer);
        zs.avail_out = sizeof(outbuffer);

        ret = inflate(&zs, 0);

        if (outstring.size() < zs.total_out) {
            outstring.append(outbuffer, zs.total_out - outstring.size());
        }

    } while (ret == Z_OK);

    inflateEnd(&zs);

    if (ret != Z_STREAM_END) {
        throw std::runtime_error("Exception during zlib decompression");
    }

    return outstring;
}

} // namespace mygit