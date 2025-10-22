#include "commands.hpp"
#include <ostream>
#include <stdexcept>
#include "dictionary_manager.hpp"
#include <FwdList/FwdList.hpp>
#include <HashTable/hash_table.hpp>

namespace shramko
{
  bool compareByFreqDesc(const std::pair< std::string, int >& a, const std::pair< std::string, int >& b)
  {
    if (a.second != b.second)
    {
      return a.second > b.second;
    }
    return a.first < b.first;
  }

  bool compareByFreqAsc(const std::pair< std::string, int >& a, const std::pair< std::string, int >& b)
  {
    if (a.second != b.second)
    {
      return a.second < b.second;
    }
    return a.first < b.first;
  }

  bool compareByKey(const std::pair< std::string, int >& a, const std::pair< std::string, int >& b)
  {
    return a.first < b.first;
  }
}

void create(const shramko::ForwardList< std::string >& args, DictionaryManager& dm, std::ostream& os)
{
  if (args.getSize() != 1)
  {
    os << "INVALID COMMAND\n";
    return;
  }
  auto it = args.begin();
  std::string name = *it;
  if (dm.createDict(name))
  {
    os << "OK: Dictionary '" << name << "' created\n";
  }
  else
  {
    os << "DICT EXISTS\n";
  }
}

void add(const shramko::ForwardList< std::string >& args, DictionaryManager& dm, std::ostream& os)
{
  if (args.getSize() != 2 && args.getSize() != 3)
  {
    os << "INVALID COMMAND\n";
    return;
  }
  auto it = args.begin();
  std::string dict_name = *it;
  ++it;
  std::string word = *it;
  int freq = 1;
  if (args.getSize() == 3)
  {
    ++it;
    std::string freq_part = *it;
    if (freq_part.substr(0, 5) != "freq:")
    {
      os << "INVALID COMMAND\n";
      return;
    }
    try
    {
      freq = std::stoi(freq_part.substr(5));
    }
    catch (...)
    {
      os << "INVALID COMMAND\n";
      return;
    }
  }
  if (dm.addWord(dict_name, word, freq))
  {
    os << "OK: Added '" << word << "' with freq=" << freq << "\n";
  }
  else
  {
    os << "DICT NOT FOUND\n";
  }
}

void increment(const shramko::ForwardList< std::string >& args, DictionaryManager& dm, std::ostream& os)
{
  if (args.getSize() != 2 && args.getSize() != 3)
  {
    os << "INVALID COMMAND\n";
    return;
  }
  auto it = args.begin();
  std::string dict_name = *it;
  ++it;
  std::string word = *it;
  int by = 1;
  if (args.getSize() == 3)
  {
    ++it;
    std::string by_part = *it;
    if (by_part.substr(0, 3) != "by:")
    {
      os << "INVALID COMMAND\n";
      return;
    }
    try
    {
      by = std::stoi(by_part.substr(3));
    }
    catch (...)
    {
      os << "INVALID COMMAND\n";
      return;
    }
  }
  if (dm.addWord(dict_name, word, by))
  {
    os << "OK: Incremented '" << word << "' by " << by << "\n";
  }
  else
  {
    os << "DICT NOT FOUND\n";
  }
}

void search(const shramko::ForwardList< std::string >& args, DictionaryManager& dm, std::ostream& os)
{
  if (args.getSize() != 2)
  {
    os << "INVALID COMMAND\n";
    return;
  }
  auto it = args.begin();
  std::string dict_name = *it;
  ++it;
  std::string word = *it;
  int freq = 0;
  if (dm.getFreq(dict_name, word, freq))
  {
    os << "FOUND: freq=" << freq << "\n";
  }
  else
  {
    os << "NOT FOUND\n";
  }
}

void delete_(const shramko::ForwardList< std::string >& args, DictionaryManager& dm, std::ostream& os)
{
  if (args.getSize() != 2)
  {
    os << "INVALID COMMAND\n";
    return;
  }
  auto it = args.begin();
  std::string dict_name = *it;
  ++it;
  std::string word = *it;
  if (dm.removeWord(dict_name, word))
  {
    os << "OK: Deleted '" << word << "'\n";
  }
  else
  {
    os << "WORD NOT FOUND\n";
  }
}

void dump(const shramko::ForwardList< std::string >& args, DictionaryManager& dm, std::ostream& os)
{
  if (args.getSize() != 1)
  {
    os << "INVALID COMMAND\n";
    return;
  }
  auto it = args.begin();
  std::string dict_name = *it;
  const auto* dict = dm.getDict(dict_name);
  if (!dict)
  {
    os << "DICT NOT FOUND\n";
    return;
  }
  os << "dump: ";
  for (auto dictIt = dict->cbegin(); dictIt != dict->cend(); ++dictIt)
  {
    os << dictIt->first << ":" << dictIt->second << " ";
  }
  os << "\n";
}

void top(const shramko::ForwardList< std::string >& args, DictionaryManager& dm, std::ostream& os)
{
  if (args.getSize() != 2)
  {
    os << "INVALID COMMAND\n";
    return;
  }
  auto it = args.begin();
  std::string dict_name = *it;
  ++it;
  std::string n_str = *it;
  int n = 0;
  try
  {
    n = std::stoi(n_str);
  }
  catch (...)
  {
    os << "INVALID COMMAND\n";
    return;
  }
  if (n <= 0)
  {
    os << "INVALID COMMAND\n";
    return;
  }
  const auto* dict = dm.getDict(dict_name);
  if (!dict)
  {
    os << "DICT NOT FOUND\n";
    return;
  }
  if (dict->empty())
  {
    os << "EMPTY\n";
    return;
  }
  shramko::ForwardList<std::pair<std::string, int> > items;
  for (auto dictIt = dict->cbegin(); dictIt != dict->cend(); ++dictIt)
  {
    items.addToBack(*dictIt);
  }
  items.sort(shramko::compareByFreqDesc);
  os << "top " << n << ": ";
  size_t count = 0;
  auto itemIt = items.begin();
  while (itemIt != items.end() && count < static_cast<size_t>(n))
  {
    os << itemIt->first << ":" << itemIt->second << " ";
    ++itemIt;
    ++count;
  }
  os << "\n";
}

void bot(const shramko::ForwardList< std::string >& args, DictionaryManager& dm, std::ostream& os)
{
  if (args.getSize() != 2)
  {
    os << "INVALID COMMAND\n";
    return;
  }
  auto it = args.begin();
  std::string dict_name = *it;
  ++it;
  std::string n_str = *it;
  int n = 0;
  try
  {
    n = std::stoi(n_str);
  }
  catch (...)
  {
    os << "INVALID COMMAND\n";
    return;
  }
  if (n <= 0)
  {
    os << "INVALID COMMAND\n";
    return;
  }
  const auto* dict = dm.getDict(dict_name);
  if (!dict)
  {
    os << "DICT NOT FOUND\n";
    return;
  }
  if (dict->empty())
  {
    os << "EMPTY\n";
    return;
  }
  shramko::ForwardList<std::pair<std::string, int> > items;
  for (auto dictIt = dict->cbegin(); dictIt != dict->cend(); ++dictIt)
  {
    items.addToBack(*dictIt);
  }
  items.sort(shramko::compareByFreqAsc);
  os << "BOT " << n << ": ";
  size_t count = 0;
  auto itemIt = items.begin();
  while (itemIt != items.end() && count < static_cast<size_t>(n))
  {
    os << itemIt->first << ":" << itemIt->second << " ";
    ++itemIt;
    ++count;
  }
  os << "\n";
}

void minfreq(const shramko::ForwardList< std::string >& args, DictionaryManager& dm, std::ostream& os)
{
  if (args.getSize() != 2)
  {
    os << "INVALID COMMAND\n";
    return;
  }
  auto it = args.begin();
  std::string dict_name = *it;
  ++it;
  std::string min_str = *it;
  int min_val = 0;
  try
  {
    min_val = std::stoi(min_str);
  }
  catch (...)
  {
    os << "INVALID COMMAND\n";
    return;
  }
  const auto* dict = dm.getDict(dict_name);
  if (!dict)
  {
    os << "DICT NOT FOUND\n";
    return;
  }
  shramko::ForwardList<std::pair<std::string, int> > items;
  for (auto dictIt = dict->cbegin(); dictIt != dict->cend(); ++dictIt)
  {
    if (dictIt->second >= min_val)
    {
      items.addToBack(*dictIt);
    }
  }
  items.sort(shramko::compareByKey);
  os << "minfreq " << min_val << ": ";
  for (auto itemIt = items.begin(); itemIt != items.end(); ++itemIt)
  {
    os << itemIt->first << ":" << itemIt->second << " ";
  }
  os << "\n";
}

void maxfreq(const shramko::ForwardList< std::string >& args, DictionaryManager& dm, std::ostream& os)
{
  if (args.getSize() != 2)
  {
    os << "INVALID COMMAND\n";
    return;
  }
  auto it = args.begin();
  std::string dict_name = *it;
  ++it;
  std::string max_str = *it;
  int max_val = 0;
  try
  {
    max_val = std::stoi(max_str);
  }
  catch (...)
  {
    os << "INVALID COMMAND\n";
    return;
  }
  const auto* dict = dm.getDict(dict_name);
  if (!dict)
  {
    os << "DICT NOT FOUND\n";
    return;
  }
  shramko::ForwardList<std::pair<std::string, int> > items;
  for (auto dictIt = dict->cbegin(); dictIt != dict->cend(); ++dictIt)
  {
    if (dictIt->second > max_val)
    {
      items.addToBack(*dictIt);
    }
  }
  items.sort(shramko::compareByKey);
  os << "maxfreq " << max_val << ": ";
  for (auto itemIt = items.begin(); itemIt != items.end(); ++itemIt)
  {
    os << itemIt->first << ":" << itemIt->second << " ";
  }
  os << "\n";
}

void median(const shramko::ForwardList< std::string >& args, DictionaryManager& dm, std::ostream& os)
{
  if (args.getSize() != 1)
  {
    os << "INVALID COMMAND\n";
    return;
  }
  auto it = args.begin();
  std::string dict_name = *it;
  const auto* dict = dm.getDict(dict_name);
  if (!dict)
  {
    os << "DICT NOT FOUND\n";
    return;
  }
  if (dict->empty())
  {
    os << "EMPTY DICT\n";
    return;
  }
  shramko::ForwardList< int > freqs;
  for (auto dictIt = dict->cbegin(); dictIt != dict->cend(); ++dictIt)
  {
    freqs.addToBack(dictIt->second);
  }
  freqs.sort();
  size_t sz = freqs.getSize();
  double med = 0.0;
  if (sz % 2 == 1)
  {
    auto freqIt = freqs.begin();
    for (size_t i = 0; i < sz / 2; ++i)
    {
      ++freqIt;
    }
    med = *freqIt;
  }
  else
  {
    auto it1 = freqs.begin();
    for (size_t i = 0; i < sz / 2 - 1; ++i)
    {
      ++it1;
    }
    auto it2 = it1;
    ++it2;
    med = (*it1 + *it2) / 2.0;
  }
  os << "median: " << med << "\n";
}

struct OneArgCaller
{
  void (*func)(const shramko::ForwardList< std::string >&, DictionaryManager&, std::ostream&);
  OneArgCaller(void (*f)(const shramko::ForwardList< std::string >&, DictionaryManager&, std::ostream&)):
    func(f)
  {}
  void operator()(const shramko::ForwardList< std::string >& args, DictionaryManager& dm, std::ostream& os) const
  {
    func(args, dm, os);
  }
};

shramko::HashTable< std::string, CommandFunction > createCommandMap()
{
  shramko::HashTable< std::string, CommandFunction > commandMap;
  commandMap.insert("create", OneArgCaller(create));
  commandMap.insert("add", OneArgCaller(add));
  commandMap.insert("increment", OneArgCaller(increment));
  commandMap.insert("search", OneArgCaller(search));
  commandMap.insert("delete", OneArgCaller(delete_));
  commandMap.insert("dump", OneArgCaller(dump));
  commandMap.insert("top", OneArgCaller(top));
  commandMap.insert("bot", OneArgCaller(bot));
  commandMap.insert("minfreq", OneArgCaller(minfreq));
  commandMap.insert("maxfreq", OneArgCaller(maxfreq));
  commandMap.insert("median", OneArgCaller(median));

  return commandMap;
}
