#pragma once

#include <filesystem>
#include <list>
#include <utility>

namespace copy_dir_helper {
    namespace fs = std::filesystem;
    class CopyDirHelper {
    public:
        using AnsType = std::list< std::pair< fs::path, fs::path > >;
    private:
        AnsType mmmDirs;
        AnsType mmmFiles;
    public:
        CopyDirHelper(fs::path, fs::path);
        const AnsType & getCopyDirs();
        const AnsType & getCopyFiles();
        ~CopyDirHelper();
    };

}



