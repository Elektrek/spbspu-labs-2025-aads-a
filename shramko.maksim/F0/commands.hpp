#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <functional>
#include <string>
#include <ostream>
#include "dictionary_manager.hpp"
#include "FwdList.hpp"

using CommandFunction = std::function< void(const shramko::ForwardList< std::string >&, DictionaryManager&, std::ostream&) >;

shramko::HashTable< std::string, CommandFunction > createCommandMap();

void create(const shramko::ForwardList< std::string >& args, DictionaryManager& dm, std::ostream& os);
void add(const shramko::ForwardList< std::string >& args, DictionaryManager& dm, std::ostream& os);
void increment(const shramko::ForwardList< std::string >& args, DictionaryManager& dm, std::ostream& os);
void search(const shramko::ForwardList< std::string >& args, DictionaryManager& dm, std::ostream& os);
void delete_(const shramko::ForwardList< std::string >& args, DictionaryManager& dm, std::ostream& os);
void dump(const shramko::ForwardList< std::string >& args, DictionaryManager& dm, std::ostream& os);
void top(const shramko::ForwardList< std::string >& args, DictionaryManager& dm, std::ostream& os);
void bot(const shramko::ForwardList< std::string >& args, DictionaryManager& dm, std::ostream& os);
void minfreq(const shramko::ForwardList< std::string >& args, DictionaryManager& dm, std::ostream& os);
void maxfreq(const shramko::ForwardList< std::string >& args, DictionaryManager& dm, std::ostream& os);
void median(const shramko::ForwardList< std::string >& args, DictionaryManager& dm, std::ostream& os);

#endif
