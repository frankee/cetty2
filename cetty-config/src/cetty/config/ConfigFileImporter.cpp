/*
 * Copyright (c) 2010-2012 frankee zhou (frankee.zhou at gmail dot com)
 *
 * Distributed under under the Apache License, version 2.0 (the "License").
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

#include <cetty/config/ConfigFileImporter.h>

#include <yaml-cpp/node/node.h>
#include <boost/algorithm/string.hpp>
#include <cetty/util/StringUtil.h>

#include <cetty/logging/LoggerHelper.h>

namespace cetty {
namespace config {

using namespace cetty::util;
using namespace cetty::logging;

static const std::string IMPORT_KEY = "#import";

class ImportLine {
public:
    ImportLine(const std::string& file, const std::string& line)
        : folderPath_(file), parentPath_(folderPath_.parent_path()) {
        init(line);
    }

    int getImports(std::vector<std::string>* matches) {
        BOOST_ASSERT(matches && "matches should not be null.");

        boost::filesystem::directory_iterator end;

        if (boost::filesystem::exists(folderPath_)) {
            boost::filesystem::directory_iterator pos(folderPath_);
            bool isMatchAll = isAllMatchedMode();

            for (; pos != end; ++pos) {
                if (is_directory(*pos)) {
                    continue;
                }

                if (isMatchAll || match(pos->path().filename().string())) {
                    matches->push_back(pos->path().string());
                }
            }
        }

        return static_cast<int>(matches->size());
    }

private:
    void init(const std::string& str) {
        std::string::size_type pos = str.find(IMPORT_KEY);

        if (pos == str.npos) {
            LOG_ERROR << "This is not the import line: " << str;
            return;
        }

        std::string subStr = str.substr(pos + IMPORT_KEY.size());
        boost::algorithm::trim(subStr);

        if (subStr.empty()) {
            LOG_ERROR << "the import line is illegal: " << str;
            return;
        }

        // filter out the double quoter
        if (subStr[0] == '\"') {
            std::size_t size = subStr.size();

            if (size > 1 && subStr[size - 1] == '\"') {
                subStr = subStr.substr(1, size - 2);
                boost::algorithm::trim(subStr);
            }
            else {
                LOG_ERROR << "the import line is illegal: " << str;
                return;
            }
        }

        folderPath_ = subStr;
        std::string parentPathStr = folderPath_.parent_path().string();

        if (!folderPath_.is_complete() && parentPath_.string().compare(parentPathStr) != 0) {
            folderPath_ = (parentPath_ /= parentPathStr);
        }

        // parse simple * match
        std::string fileName = folderPath_.filename().string();

        if (fileName.find("*") == fileName.npos) {
            fullMatch_ = fileName;
        }
        else if (fileName.compare("*") == 0) {
            // match all mode.
        }
        else {
            std::vector<std::string> keyWords;
            cetty::util::StringUtil::split(fileName, "*", &keyWords);

            //             if (1 == keyWords.size()) {
            //                 fullMatch_ = p.filename().string();
            //             }
            //
            //             if (2 == keyWords.size()) {
            //                 prefixMatch_ = keyWords[0];
            //                 postfixMatch_ = keyWords[1];
            //                 fullMatch_ = p.filename().string();
            //             }
        }
    }

    bool match(const std::string& filename) const {
        if (!fullMatch_.empty()) {
            return fullMatch_ == filename;
        }

        if (!prefixMatch_.empty())  {
            if (StringUtil::hasPrefixString(filename, prefixMatch_)) {
                if (!postfixMatch_.empty()) {
                    return StringUtil::hasSuffixString(filename, postfixMatch_);
                }
                else {
                    return true;
                }
            }
            else {
                return false;
            }
        }

        if (!postfixMatch_.empty()) {
            return StringUtil::hasSuffixString(filename, postfixMatch_);
        }

        return false;
    }

    bool isAllMatchedMode() const {
        return fullMatch_.empty() &&
               prefixMatch_.empty() &&
               postfixMatch_.empty();
    }

private:
    std::string fullMatch_;
    std::string prefixMatch_;
    std::string postfixMatch_;

    boost::filesystem::path folderPath_;
    boost::filesystem::path parentPath_;
};

ConfigFileImporter::ConfigFileImporter() {
}

int ConfigFileImporter::find(const std::string& file,
                             std::vector<std::string>* imports) {
    if (imports == NULL) {
        LOG_ERROR << "the imports is null!";
        return 0;
    }

    std::fstream filestream;
    filestream.open(file.c_str(), std::fstream::in);

    if (!filestream.is_open()) {
        LOG_WARN << "the file " << file << " does not exist.";
        return 0;
    }
    else {
        filestream.close();
    }

    // prevent to circular import
    if (std::find(imports->begin(), imports->end(), file) == imports->end()) {
        imports->push_back(file);
    }
    else {
        return 0;
    }

    // load importers
    std::vector<std::string> importLines;
    getImportLines(file, &importLines);

    for (std::size_t i = 0; i < importLines.size(); ++i) {
        ImportLine line(file, importLines[i]);
        std::vector<std::string> files;
        line.getImports(&files);

        for (std::size_t j = 0; j < files.size(); ++j) {
            find(files[j], imports);
        }
    }

    return static_cast<int>(imports->size());
}

int ConfigFileImporter::getImportLines(const std::string& file, std::vector<std::string>* lines) {
    BOOST_ASSERT(lines && "the lines is null!");

    std::fstream filestream;
    filestream.open(file.c_str(), std::fstream::in);

    std::string line;

    while (!filestream.eof()) {
        std::getline(filestream, line);
        boost::algorithm::trim(line);

        if (line.find(IMPORT_KEY) != std::string::npos) {
            lines->push_back(line);
        }
    }

    filestream.close();
    return static_cast<int>(lines->size());
}

}
}
