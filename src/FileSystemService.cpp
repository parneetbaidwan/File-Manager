/*
Parneet Baidwan - 251259638
Description: The FileSystemService class implementation in this file uses the C++17 filesystem library. The system enables users to browse directory contents through its directory enumeration feature while it allows users to check file attributes and conduct file activities which include copying and moving and deleting and creating new directories and file system errors are managed by the system with platform-independent solutions.
February 1, 2026
*/


#include "FileSystemService.h"
#include <chrono>

/*
Function: ToTimeT
Description: Converts a std::filesystem::file_time_type into a std::time_t. This is used to
             display last-modified timestamps in a portable way by translating from the
             filesystem clock to system_clock. Conversion can be sensitive to platform clock
             differences, so this function centralizes the logic.
Parameters:
  - ftime: Filesystem timestamp (file_time_type) to convert.
Returns:
  - std::time_t: Converted timestamp suitable for formatting and display.
*/
static std::time_t ToTimeT(const fs::file_time_type& ftime)
{
    auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
        ftime - fs::file_time_type::clock::now() + std::chrono::system_clock::now()
    );
    return std::chrono::system_clock::to_time_t(sctp);
}

/*
Function: FileSystemService::ListDirectory
Description: Produces a list of FileItem entries for a directory, including metadata needed
             by the UI (name/type/size/last modified). Returns an empty vector on failure and
             sets outErr with a human-readable message (e.g., not a directory, permission errors).
Parameters:
  - dir: Directory path to enumerate.
  - outErr: Output string populated with an error message if listing fails; cleared on success.
Returns:
  - std::vector<FileItem>: Items in the directory (may be empty if directory is empty or on error).
*/
std::vector<FileItem> FileSystemService::ListDirectory(const fs::path& dir, std::string& outErr) const
{
    outErr.clear();
    std::vector<FileItem> items;

    std::error_code ec;
    if (!fs::exists(dir, ec) || !fs::is_directory(dir, ec))
    {
        outErr = "Not a directory: " + dir.string();
        return items;
    }

    fs::directory_iterator it(dir, ec);
    if (ec)
    {
        outErr = "Cannot iterate directory: " + ec.message();
        return items;
    }

    for (const auto& entry : it)
    {
        FileItem item;
        item.fullPath = entry.path();

        std::error_code e2;
        item.isDir = entry.is_directory(e2);
        if (e2) item.isDir = false;

        if (!item.isDir)
        {
            std::error_code e3;
            item.sizeBytes = fs::file_size(item.fullPath, e3);
            if (e3) item.sizeBytes = 0;
        }

        std::error_code e4;
        auto t = fs::last_write_time(item.fullPath, e4);
        item.modified = e4 ? 0 : ToTimeT(t);

        items.push_back(std::move(item));
    }

    return items;
}

/*
Function: FileSystemService::CreateDirectory
Description: Creates a new folder inside the given directory. Validates existence via filesystem
             operations and reports failures using outErr.
Parameters:
  - dir: Parent directory where the new folder should be created.
  - name: Name of the new directory (not a full path).
  - outErr: Output string populated with an error message if creation fails; cleared on success.
Returns:
  - bool: true if the directory was created successfully; false otherwise.
*/
bool FileSystemService::CreateDirectory(const fs::path& dir, const std::string& name, std::string& outErr) const
{
    outErr.clear();
    if (name.empty())
    {
        outErr = "Directory name cannot be empty.";
        return false;
    }

    const fs::path newDir = dir / fs::path(name);

    std::error_code ec;
    if (fs::exists(newDir, ec))
    {
        outErr = "That name already exists.";
        return false;
    }

    const bool ok = fs::create_directory(newDir, ec);
    if (!ok || ec)
    {
        outErr = "Create directory failed: " + (ec ? ec.message() : std::string("unknown error"));
        return false;
    }

    return true;
}

/*
Function: FileSystemService::RenamePath
Description: Renames a file or directory by changing only the filename component, keeping it
             within the same parent directory. Reports filesystem errors via outErr.
Parameters:
  - oldPath: Existing path to rename.
  - newName: New filename to apply (not a full path).
  - outErr: Output string populated with an error message if rename fails; cleared on success.
Returns:
  - bool: true if the rename succeeded; false otherwise.
*/
bool FileSystemService::RenamePath(const fs::path& oldPath, const std::string& newName, std::string& outErr) const
{
    outErr.clear();
    if (newName.empty())
    {
        outErr = "New name cannot be empty.";
        return false;
    }

    std::error_code ec;
    if (!fs::exists(oldPath, ec))
    {
        outErr = "Source does not exist.";
        return false;
    }

    const fs::path newPath = oldPath.parent_path() / fs::path(newName);
    if (fs::exists(newPath, ec))
    {
        outErr = "A file/directory with that name already exists.";
        return false;
    }

    fs::rename(oldPath, newPath, ec);
    if (ec)
    {
        outErr = "Rename failed: " + ec.message();
        return false;
    }

    return true;
}

/*
Function: FileSystemService::RemoveRecursive
Description: Deletes a file or directory. For directories, performs recursive removal using
             filesystem facilities. Reports the number of removed entries through outRemovedCount.
             On failure sets outErr with the reason (missing target, permission errors, etc.).
Parameters:
  - target: File or directory to delete.
  - outRemovedCount: Output count of removed filesystem entries (0 on failure).
  - outErr: Output string populated with an error message if deletion fails; cleared on success.
Returns:
  - bool: true if removal succeeded; false otherwise.
*/
bool FileSystemService::RemoveRecursive(const fs::path& target, std::uintmax_t& outRemovedCount, std::string& outErr) const
{
    outErr.clear();
    outRemovedCount = 0;

    std::error_code ec;
    if (!fs::exists(target, ec))
    {
        outErr = "Target does not exist.";
        return false;
    }

    outRemovedCount = fs::remove_all(target, ec);
    if (ec)
    {
        outErr = "Delete failed: " + ec.message();
        return false;
    }

    return true;
}

/*
Function: FileSystemService::PasteInto
Description: Executes a copy or move operation from the virtual clipboard into the destination
             directory. If overwriteExisting is false and a target exists, the function fails with
             an explanatory error. When cut is set in the clipboard, the operation moves; otherwise
             it copies. Handles directory vs file semantics using filesystem functions.
Parameters:
  - clip: VirtualClipboard describing the source path and whether it is a cut (move) or copy.
  - destDir: Directory that will receive the pasted item.
  - overwriteExisting: If true, allow replacing an existing destination entry.
  - outErr: Output string populated with an error message if paste fails; cleared on success.
Returns:
  - bool: true if the paste operation succeeded; false otherwise.
*/
bool FileSystemService::PasteInto(const VirtualClipboard& clip,
                                 const fs::path& destDir,
                                 bool overwriteExisting,
                                 std::string& outErr) const
{
    outErr.clear();
    if (!clip.hasItem)
    {
        outErr = "Clipboard is empty.";
        return false;
    }

    std::error_code ec;
    if (!fs::exists(clip.source, ec))
    {
        outErr = "Source no longer exists.";
        return false;
    }

    const fs::path dest = destDir / clip.source.filename();

    if (fs::exists(dest, ec))
    {
        if (!overwriteExisting)
        {
            outErr = "Destination exists (overwrite not allowed).";
            return false;
        }

        fs::remove_all(dest, ec);
        if (ec)
        {
            outErr = "Failed removing destination: " + ec.message();
            return false;
        }
    }

    if (clip.isCut)
    {
        fs::rename(clip.source, dest, ec);
        if (ec)
        {
            outErr = "Move failed: " + ec.message();
            return false;
        }
        return true;
    }

    if (fs::is_directory(clip.source, ec))
    {
        fs::copy(clip.source, dest, fs::copy_options::recursive, ec);
        if (ec)
        {
            outErr = "Copy directory failed: " + ec.message();
            return false;
        }
        return true;
    }

    fs::copy_file(clip.source, dest, fs::copy_options::overwrite_existing, ec);
    if (ec)
    {
        outErr = "Copy file failed: " + ec.message();
        return false;
    }

    return true;
}

/*
Function: FileSystemService::Exists
Description: Convenience wrapper to check whether a path exists in the filesystem.
Parameters:
  - p: Path to test.
Returns:
  - bool: true if p exists; false otherwise.
*/
bool FileSystemService::Exists(const fs::path& p)
{
    std::error_code ec;
    return fs::exists(p, ec);
}

/*
Function: FileSystemService::IsDirectory
Description: Convenience wrapper to check whether a path refers to a directory.
Parameters:
  - p: Path to test.
Returns:
  - bool: true if p is a directory; false otherwise.
*/
bool FileSystemService::IsDirectory(const fs::path& p)
{
    std::error_code ec;
    return fs::is_directory(p, ec);
}

/*
Function: FileSystemService::CanonicalOrSame
Description: Attempts to compute a canonical (normalized, absolute) version of a path. If canonical
             resolution fails (e.g., broken symlink or permission issue), returns the original path.
             This helps keep path comparisons and UI display stable.
Parameters:
  - p: Path to canonicalize if possible.
Returns:
  - fs::path: Canonical path on success, otherwise the original path.
*/
fs::path FileSystemService::CanonicalOrSame(const fs::path& p)
{
    std::error_code ec;
    fs::path c = fs::canonical(p, ec);
    return ec ? p : c;
}
