#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <functional>
#include <string>
#include <ostream>
#include "dictionary_manager.hpp"
#include <FwdList/FwdList.hpp>

using CommandFunction = std::function< void(const shramko::FwdList< std::string >&, DictionaryManager&, std::ostream&) >;

shramko::hash_table< std::string, CommandFunction > createCommandMap();

void create(const shramko::FwdList< std::string >& args, DictionaryManager& dm, std::ostream& os);
void add(const shramko::FwdList< std::string >& args, DictionaryManager& dm, std::ostream& os);
void increment(const shramko::FwdList< std::string >& args, DictionaryManager& dm, std::ostream& os);
void search(const shramko::FwdList< std::string >& args, DictionaryManager& dm, std::ostream& os);
void delete_(const shramko::FwdList< std::string >& args, DictionaryManager& dm, std::ostream& os);
void dump(const shramko::FwdList< std::string >& args, DictionaryManager& dm, std::ostream& os);
void top(const shramko::FwdList< std::string >& args, DictionaryManager& dm, std::ostream& os);
void bot(const shramko::FwdList< std::string >& args, DictionaryManager& dm, std::ostream& os);
void minfreq(const shramko::FwdList< std::string >& args, DictionaryManager& dm, std::ostream& os);
void maxfreq(const shramko::FwdList< std::string >& args, DictionaryManager& dm, std::ostream& os);
void median(const shramko::FwdList< std::string >& args, DictionaryManager& dm, std::ostream& os);

#endif
