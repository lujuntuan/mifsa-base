/*********************************************************************************
 *Copyright(C): Juntuan.Lu, 2020-2030, All rights reserved.
 *Author:  Juntuan.Lu
 *Version: 1.0
 *Date:  2022/04/01
 *Email: 931852884@qq.com
 *Description:
 *Others:
 *Function List:
 *History:
 **********************************************************************************/

#ifndef MIFSA_UTILS_DIR_H
#define MIFSA_UTILS_DIR_H

#include "mifsa/base/define.h"
#include <string>
#include <vector>

MIFSA_NAMESPACE_BEGIN

namespace Utils {
extern MIFSA_EXPORT bool mkPath(const std::string& path) noexcept;
extern MIFSA_EXPORT bool exists(const std::string& path) noexcept;
extern MIFSA_EXPORT bool remove(const std::string& path) noexcept;
extern MIFSA_EXPORT bool removeAll(const std::string& path) noexcept;
extern MIFSA_EXPORT bool isFile(const std::string& path) noexcept;
extern MIFSA_EXPORT bool isDir(const std::string& path) noexcept;
extern MIFSA_EXPORT bool setCurrentPath(const std::string& path) noexcept;
extern MIFSA_EXPORT std::string getCurrentPath() noexcept;
extern MIFSA_EXPORT std::vector<std::string> getFiles(const std::string& path, const std::string& suffix = "") noexcept;
extern MIFSA_EXPORT std::vector<std::string> getAllFiles(const std::string& path, const std::string& suffix = "") noexcept;
extern MIFSA_EXPORT std::vector<std::string> getFilesByName(const std::string& path, bool maxFirst = true) noexcept;
extern MIFSA_EXPORT std::vector<std::string> getFilesByLastDate(const std::string& path, bool newFirst = true) noexcept;
extern MIFSA_EXPORT std::vector<std::string> getDirsByName(const std::string& path, bool maxFirst = true) noexcept;
extern MIFSA_EXPORT std::vector<std::string> getDirsByLastDate(const std::string& path, bool newFirst = true) noexcept;
extern MIFSA_EXPORT std::string getPathFileName(const std::string& path) noexcept;
extern MIFSA_EXPORT std::string getPathBaseName(const std::string& path) noexcept;
extern MIFSA_EXPORT std::string getPathSuffixName(const std::string& path) noexcept;
extern MIFSA_EXPORT std::string getTempDir() noexcept;
extern MIFSA_EXPORT void removeSubOldDirs(const std::string& dir, int keepCount) noexcept;
extern MIFSA_EXPORT std::string getCfgPath(const std::string& fileName, const std::string& envName = "", const std::string& packageName = "");
}

MIFSA_NAMESPACE_END

#endif // MIFSA_UTILS_DIR_H
