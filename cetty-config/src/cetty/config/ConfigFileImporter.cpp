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

#include <boost/algorithm/string.hpp>
#include <cetty/util/StringUtil.h>

#include <cetty/logging/LoggerHelper.h>

namespace cetty {
namespace config {

using namespace cetty::util;
using namespace cetty::logging;

class FileNameMatchPattern {
public:
    std::string prefix;
    std::string postfix;
    std::string fullName;
    boost::filesystem::path fatherPath;

    bool parse(const std::string& str, boost::filesystem::path* filePath);
    bool match(const std::string& filename) const;

    bool isMatchAll() const;
};

class IncludeLine {
public:
    boost::filesystem::path filePath;
    FileNameMatchPattern pattern;

    IncludeLine(const std::string& str, boost::filesystem::path filePath) {
        pattern.fatherPath = filePath.parent_path();
        parse(str);
    }

    bool parse(const std::string& str);
};

ConfigFileImporter::ConfigFileImporter() {
}

int ConfigFileImporter::find(const std::string& file,
                                  std::vector<std::string>* files) {
    if (files == NULL) {
        LOG_ERROR << "the files is null!";
        return 0;
    }

    if (!files->empty()) {

    }

    std::fstream filestream;
    filestream.open(file.c_str(), std::fstream::in);

    if (!filestream.is_open()) {
        filestream.close();
        std::cout<<"can not find the file"<<std::endl;
        return 0;
    }

    filestream.close();

    std::vector<IncludeLine> includes;
    int filesCnt = getFileIncludes(file, &includes);

    if (std::find(files->begin(), files->end(), file) == files->end()) {
        files->push_back(file);
    }
    else {
        return 0;
    }

    for (int i = 0; i < filesCnt; ++i) {
        const IncludeLine& include = includes[i];
        findFile(include.filePath, include.pattern, files);
    }

    return (int)files->size();
}

bool filterIncludeLine(const std::string line) {
    std::string::size_type pos = line.find("#include");

    if (pos != std::string::npos) {
        return true;
    }

    return false;
}

bool FileNameMatchPattern::parse(const std::string& str,
                                 boost::filesystem::path* filePath) {
    //处理相对路径
    boost::filesystem::path p(str);
    *filePath = p.parent_path();
    std::string pathStr = p.parent_path().string();

    if (!p.is_complete() && fatherPath.string().compare(p.parent_path().string()) != 0) {
        *filePath = (fatherPath /= pathStr);
    }

    //取出文件名 并分割
    std::vector<std::string> keyWords;
    cetty::util::StringUtil::split(p.filename().string(),"*", &keyWords);

    if (1 == keyWords.size()) {
        fullName = p.filename().string();
    }

    if (2 == keyWords.size()) {
        prefix = keyWords[0];
        postfix = keyWords[1];
        fullName = p.filename().string();
    }

    return true;
}

bool FileNameMatchPattern::isMatchAll() const {
    return true;
}

bool FileNameMatchPattern::match(const std::string& filename) const {
    if (prefix.empty() && postfix.empty()) {
        return true;
    }
    else if (!prefix.empty() && StringUtil::hasPrefixString(filename, prefix)) {
        if (!postfix.empty() && StringUtil::hasSuffixString(filename, postfix)) {
            return true;
        }
        else if (postfix.empty()) {
            return true;
        }
    }
    else if (prefix.empty()
             && !postfix.empty()
             && StringUtil::hasSuffixString(filename, postfix)) {
        return true;
    }

    return false;
}

bool IncludeLine::parse(const std::string& str) {
    std::string::size_type pos;
    std::string subStr;

    pos = str.find("#include");

    subStr = str.substr(pos+strlen("#include"));
    boost::algorithm::trim(subStr);  //除去空格 获得路径

    //有双引号就过滤 双引号
    if (subStr.find_first_of("\"") != subStr.npos) {
        if (subStr.find_first_of("\"") != subStr.find_last_of("\"")) {
            subStr = subStr.substr(1, subStr.find_last_of("\"")-1);
            boost::algorithm::trim(subStr);
        }
        else {
            std::cout<<"the includeline is wrong!"<<std::endl;
        }
    }

    pattern.parse(subStr, &filePath);

    return true;
}

int ConfigFileImporter::getFileIncludes(const std::string& file,
        std::vector<IncludeLine>* includes) {
    BOOST_ASSERT(includes && "the includes is null!");

    std::vector<std::string> lines;
    std::fstream filestream;
    filestream.open(file.c_str(), std::fstream::in);

    std::string line;

    while (!filestream.eof()) {
        std::getline(filestream, line);
        boost::algorithm::trim(line);

        if (!line.empty()) {
            lines.push_back(line);
        }
    }

    filestream.close();

    boost::filesystem::path p(file);
    std::string includeLineStr;

    for (size_t i = lines.size(); i > 0; --i) {
        // is include file
        if (!filterIncludeLine(lines[i-1])) {
            break;
        }

        includes->push_back(IncludeLine(lines[i-1], p));
    }

    return includes->size();
}

void ConfigFileImporter::findFile(const boost::filesystem::path& filePath,
                                       const FileNameMatchPattern& pattern,
                                       std::vector<std::string>* files) {
    boost::filesystem::directory_iterator end;

    if (boost::filesystem::exists(filePath)) {
        boost::filesystem::directory_iterator pos(filePath);

        for (; pos != end; ++pos) {
            if (pattern.isMatchAll()) {

            }

            if (is_directory(*pos)) {
                continue;
            }

            if (pattern.match(pos->path().filename().string())) { //如果是 * 所有文件都将匹配成功  即全部遍历

                find(pos->path().string(), files);
            }
        }
    }

}

}
}
