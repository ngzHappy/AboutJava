#include "CopyDirHelper.hpp"

namespace copy_dir_helper {

    CopyDirHelper::CopyDirHelper(fs::path argFrom, fs::path argTo) {

        AnsType varAboutToCopy;
        varAboutToCopy.emplace_back(std::move(argFrom), std::move(argTo));

        while (!varAboutToCopy.empty()) {

            mmmDirs.splice(
                mmmDirs.end(),
                varAboutToCopy,
                varAboutToCopy.begin());

            auto & varItem =
                mmmDirs.back();

            fs::directory_iterator varEnd;
            fs::directory_iterator varPos{ varItem.first };

            for (; varPos != varEnd; ++varPos) {
                const auto & varFileFullPath = varPos->path();
                if (varPos->is_directory()) {
                    varAboutToCopy.emplace_back(
                        varFileFullPath,
                        varItem.second / varFileFullPath.filename());
                } else {
                    mmmFiles.emplace_back(
                        varFileFullPath,
                        varItem.second / varFileFullPath.filename());
                }
            }
        }
    }

    const CopyDirHelper::AnsType & CopyDirHelper::getCopyDirs() {
        return mmmDirs;
    }

    const CopyDirHelper::AnsType & CopyDirHelper::getCopyFiles() {
        return mmmFiles;
    }

    CopyDirHelper::~CopyDirHelper() {
    }

}












