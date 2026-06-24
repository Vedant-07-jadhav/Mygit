# MyGit
*A high-performance, content-addressable version control engine written from scratch in C++17.*

### 1. The Core Architecture

* **Content-Addressable Object Database:** Instead of storing files by their names, this storage engine identifies files strictly by the mathematical hash (SHA-1) of their contents.
* **Zlib DEFLATE Compression Engine:** To prevent massive disk bloat, the engine intercepts raw file data, compresses it using the `zlib` library, and writes it directly to the `.mygit/objects` directory using a 2-character fan-out folder structure.
* **High-Performance Binary Staging Cache (The Index):** A custom binary serialization format maps file paths to their hashes. To avoid O(N) file-read operations when checking for modifications, it implements an O(1) heuristic cache using OS-level `stat` metadata (`mtime` and `file_size`).
* **Directed Acyclic Graph (DAG) for History:** Implements Git's internal object model:
    * **Blobs:** Raw, compressed file data.
    * **Trees:** Directory structures that hold raw 20-byte binary pointers to Blobs (saving 50% disk space compared to hex strings).
    * **Commits:** Snapshot nodes containing timestamps, authorship, and a pointer to the root Tree.
* **Reference Management System:** A symbolic pointer system (`HEAD` and `refs/heads/`) to anchor the commit graph and manage active development branches.

### 2. The Command Line Interface

* `mygit init`: Initializes the hidden database structure (`.mygit/objects`, `refs`, `HEAD`).
* `mygit hash-object <file>`: Reads a file, compresses it, and stores it in the Object Database, returning its SHA-1 hash.
* `mygit cat-file -p <hash>`: Locates a compressed object by its hash, decompresses it, and streams the raw payload to standard output.
* `mygit add <path>`: A recursive directory crawler that hashes local files and safely serializes their metadata into the binary `.mygit/index` cache.
* `mygit write-tree`: Parses the binary index and generates a hierarchical Tree object representing the working directory's state.
* `mygit commit-tree <tree_sha> -m "<msg>"`: Formats and stores a Commit object, linking the Tree to the author and the current UNIX timestamp.
* `mygit branch <name> <commit_sha>`: Creates a human-readable text reference to a specific node in the DAG.
* `mygit checkout <branch>`: Safely reads a branch file, parses the target Commit and Tree, and dynamically overwrites the physical working directory to travel through history.

### 3. Technical Stack & Skills Demonstrated

* **Language:** C++17
* **Libraries:** `<filesystem>`, `<chrono>`, OpenSSL (`<openssl/sha.h>`), Zlib (`<zlib.h>`)
* **Concepts:** Binary File I/O, Data Serialization, Graph Traversal (DAG), Hash Functions, Pointer Management.

### Installation / Build

**Requirements:**
* A C++17-compatible compiler (gcc/clang)
* `cmake` (3.10+ recommended)
* Development headers for OpenSSL and zlib (e.g., `libssl-dev`, `zlib1g-dev`)

**Build steps (from the project root):**
```bash
mkdir -p build
cd build
cmake ..
make -j$(nproc)
```
After building, the `mygit` executable will be available in the `build` directory.

