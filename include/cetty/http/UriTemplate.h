#if !defined(CETTY_HTTP_URLTEMPLATE_H)
#define CETTY_HTTP_URLTEMPLATE_H

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

namespace cetty { namespace http { 

    // path
    // [/method_path:alias]/path{/parameter:alias}{.format:alias}{?parameter1:alias}{&parameter2:alias}
    // /method_path/path/parameter.format
    // parameter1
    // parameter2

    class UriTemplate {
    public:
        enum PathSegmentType {
            PATH_SEG_NORMAL = 0,
            PATH_SEG_ALIAS  = 1,
            PATH_SEG_PARAM  = 2,
        };

        struct PathSegment {
            int type;
            int nextSameTypeIndex;
            int nextDiffTypeIndex;
            std::string name;
            std::string alias;
        };

    public:
        UriTemplate(const std::string& templateStr);


        std::string expand(const NameValueCollection& values);

        //int extract(const std::string& uri, NameValueCollection* values);

        //int extract(const std::vector<std::string>& pathSegments,
        //    const NameValueCollection& queryParameters,
        //    NameValueCollection* values) {
        //}

        bool matchPath(const std::vector<std::string>& pathSegments) {
            if (pathSegments.size() != this->pathSegments.size()) {
                return false;
            }

            int j = (int)this->pathSegments.size();
            for (int i = 0; i < j; ++i) {
                if (this->pathSegments[i].type == PATH_SEG_PARAM) {
                    continue;
                }
                if (pathSegments[i] != this->pathSegments[i]) {
                    return false;
                }
            }
            return true;
        }

        int getFirstPathAliasSegmentIndex() const;
        int getFirstPathParamSegmentIndex() const;

        const std::string& getLabelAlias() const;
        const std::string& getParameterAlias(const std::string& parameter) const;

    private:
        //path
        std::vector<PathSegment> pathSegments;

        //label
        std::string label;
        std::string labelAlias;

        std::map<std::string, std::string> parameterAliases;
    };

    class UriTemplates {
    public:

    private:
        std::vector<UriTemplate> uriTemplates;
    };
}}

#endif //#if !defined(CETTY_HTTP_URLTEMPLATE_H)

// Local Variables:
// mode: c++
// End:
