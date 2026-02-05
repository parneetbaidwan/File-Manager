/*
Parneet Baidwan - 251259638
Description: This header file declares the FileSystemService class and related data structures used to interact with the filesystem. The system establishes a clear separation between C++17 filesystem operations and its directory listing functions and file metadata retrieval capabilities and its directory and file operations which include create and delete and rename and copy and move functions.
February 1, 2026
*/

#ifndef FILESYSTEMSERVICE_H
#define FILESYSTEMSERVICE_H

#include <filesystem>
#include <string>
#include <vector>
#include <system_error>
#include <ctime>

namespace fs = std::filesystem;

// Model for one row in the UI listing 
struct FileItem
{
    fs::path fullPath;
    bool isDir = false;
    std::uintmax_t sizeBytes = 0;
    std::time_t modified = 0;
};

// Virtual clipboard 
struct VirtualClipboard
{
    fs::path source;
    bool isCut = false;
    bool hasItem = false;

    void Clear()
    {
        source.clear();
        isCut = false;
        hasItem = false;
    }
};

// Filesystem operations used by the GUI
class FileSystemService final
{
public:
    std::vector<FileItem> ListDirectory(const fs::path& dir, std::string& outErr) const;

    bool CreateDirectory(const fs::path& dir, const std::string& name, std::string& outErr) const;
    bool RenamePath(const fs::path& oldPath, const std::string& newName, std::string& outErr) const;
    bool RemoveRecursive(const fs::path& target, std::uintmax_t& outRemovedCount, std::string& outErr) const;

    bool PasteInto(const VirtualClipboard& clip,
                   const fs::path& destDir,
                   bool overwriteExisting,
                   std::string& outErr) const;

    static bool Exists(const fs::path& p);
    static bool IsDirectory(const fs::path& p);
    static fs::path CanonicalOrSame(const fs::path& p);
};

#endif // MAINFRAME_H
