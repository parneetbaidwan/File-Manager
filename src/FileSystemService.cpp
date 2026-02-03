#include "FileSystemService.h"
#include <chrono>

static std::time_t ToTimeT(const fs::file_time_type& ftime)
{
    auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
        ftime - fs::file_time_type::clock::now() + std::chrono::system_clock::now()
    );
    return std::chrono::system_clock::to_time_t(sctp);
}

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

bool FileSystemService::Exists(const fs::path& p)
{
    std::error_code ec;
    return fs::exists(p, ec);
}

bool FileSystemService::IsDirectory(const fs::path& p)
{
    std::error_code ec;
    return fs::is_directory(p, ec);
}

fs::path FileSystemService::CanonicalOrSame(const fs::path& p)
{
    std::error_code ec;
    fs::path c = fs::canonical(p, ec);
    return ec ? p : c;
}
