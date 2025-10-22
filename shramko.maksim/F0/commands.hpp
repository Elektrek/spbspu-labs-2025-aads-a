#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <functional>
#include "dictionary_manager.hpp"
#include <FwdList/FwdList.hpp>

namespace shramko
{
  class ForwardList;
}

using CommandFunction = std::function< void(const shramko::ForwardList< std::string >&, DictionaryManager&, std::ostream&) >;

shramko::HashTable< std::string, CommandFunction > createCommandMap();

#endif