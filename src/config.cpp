/* This file is part of the the dci project. Copyright (C) 2013-2021 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

#include <dci/config.hpp>
#include <boost/property_tree/ptree.hpp>

//TODO в бусте 1.73-1.75 ворнинг без этой штуки
//можно убрать на 1.76
#define BOOST_BIND_GLOBAL_PLACEHOLDERS

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/info_parser.hpp>

#include "idl-config.hpp"


namespace dci::config
{
    using namespace boost::property_tree;

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    dci::idl::gen::Config cnvt(const ptree& pt)
    {
        dci::idl::gen::Config res;
        res.value = pt.data();

        for(const auto& kv : pt)
        {
            res.children.emplace_back(std::make_tuple(kv.first, cnvt(kv.second)));
        }

        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    ptree cnvt(const dci::idl::gen::Config& c)
    {
        ptree res;
        res.data() = c.value;

        for(const auto& kv : c.children)
        {
            res.push_back(std::make_pair(std::get<0>(kv), cnvt(std::get<1>(kv))));
        }

        return res;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    namespace
    {
        bool ends_with(const std::string& str, const char* suffix)
        {
            size_t slen = strlen(suffix);

            if(str.size() < slen)
            {
                return false;
            }

            return str.size() - slen == str.find(suffix);
        }
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    ptree parse(const std::vector<std::string>& argv)
    {
        ptree pt;

        for(const std::string& arg : argv)
        {
            if(0 == arg.find('@'))
            {
                if(ends_with(arg, ".xml"))
                {
                    read_xml(arg.substr(1), pt);
                }
                else if(ends_with(arg, ".ini"))
                {
                    read_ini(arg.substr(1), pt);
                }
                else if(ends_with(arg, ".json"))
                {
                    read_json(arg.substr(1), pt);
                }
                else
                {
                    read_info(arg.substr(1), pt);
                }
                continue;
            }

            std::string::size_type eqPos = arg.find('=');

            if(std::string::npos == eqPos)
            {
                pt.put(arg.substr(0, eqPos), std::string{});
            }
            else
            {
                pt.put(arg.substr(0, eqPos), arg.substr(eqPos+1));
            }
        }

        return pt;
    }
}
