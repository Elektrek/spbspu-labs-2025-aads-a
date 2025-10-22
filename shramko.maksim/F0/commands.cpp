#include "commands.hpp"
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <sstream>
#include <FwdList/FwdList.hpp>
#include <UBST/UBST.hpp>
#include <HashTable/hash_table.hpp>

void create(const shramko::ForwardList< std::string >& args, DictionaryManager& dm, std::ostream& os)
{
  if (args.getSize() != 1)
  {
    os << "INVALID COMMAND\n";
    return;
  }
  auto it = args.cbegin();
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
  auto it = args.cbegin();
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
  auto it = args.cbegin();
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
  auto it = args.cbegin();
  std::string dict_name = *it;
  ++it;
  std::string word = *it;
  const auto* dict = dm.getDict(dict_name);
  if (!dict)
  {
    os << "DICT NOT FOUND\n";
    return;
  }
  auto dit = dict->find(word);
  if (dit == dict->cend())
  {
    os << "NOT FOUND\n";
  }
  else
  {
    os << "FOUND: freq=" << dit->second << "\n";
  }
}

void delete_(const shramko::ForwardList< std::string >& args, DictionaryManager& dm, std::ostream& os)
{
  if (args.getSize() != 2)
  {
    os << "INVALID COMMAND\n";
    return;
  }
  auto it = args.cbegin();
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
  auto it = args.cbegin();
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
  os << "dump: ";
  bool first = true;
  for (auto dit = dict->cbegin(); dit != dict->cend(); ++dit)
  {
    if (!first)
    {
      os << " ";
    }
    os << dit->first << ":" << dit->second;
    first = false;
  }
  os << " \n";
}

void top(const shramko::ForwardList< std::string >& args, DictionaryManager& dm, std::ostream& os)
{
  if (args.getSize() != 2)
  {
    os << "INVALID COMMAND\n";
    return;
  }
  auto it = args.cbegin();
  std::string dict_name = *it;
  ++it;
  std::string nstr = *it;
  size_t N;
  try
  {
    N = std::stoul(nstr);
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
  if (dict->empty())
  {
    os << "EMPTY DICT\n";
    return;
  }
  std::vector<std::pair<std::string, int>> vec;
  vec.reserve(dict->size());
  for (auto dit = dict->cbegin(); dit != dict->cend(); ++dit)
  {
    vec.emplace_back(dit->first, dit->second);
  }
  std::sort(vec.begin(), vec.end(), [](const auto& a, const auto& b)
  {
    if (a.second != b.second)
    {
      return a.second > b.second;
    }
    return a.first < b.first;
  });
  os << "top " << N << ": ";
  size_t cnt = 0;
  for (const auto& p : vec)
  {
    if (cnt >= N)
    {
      break;
    }
    os << p.first << ":" << p.second << " ";
    ++cnt;
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
  auto it = args.cbegin();
  std::string dict_name = *it;
  ++it;
  std::string nstr = *it;
  size_t N;
  try
  {
    N = std::stoul(nstr);
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
  if (dict->empty())
  {
    os << "EMPTY DICT\n";
    return;
  }
  std::vector<std::pair<std::string, int>> vec;
  vec.reserve(dict->size());
  for (auto dit = dict->cbegin(); dit != dict->cend(); ++dit)
  {
    vec.emplace_back(dit->first, dit->second);
  }
  std::sort(vec.begin(), vec.end(), [](const auto& a, const auto& b)
  {
    if (a.second != b.second)
    {
      return a.second < b.second;
    }
    return a.first < b.first;
  });
  os << "bot " << N << ": ";
  size_t cnt = 0;
  for (const auto& p : vec)
  {
    if (cnt >= N)
    {
      break;
    }
    os << p.first << ":" << p.second << " ";
    ++cnt;
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
  auto it = args.cbegin();
  std::string dict_name = *it;
  ++it;
  std::string minstr = *it;
  int minf;
  try
  {
    minf = std::stoi(minstr);
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
  os << "minfreq " << minf << ": ";
  bool first = true;
  for (auto dit = dict->cbegin(); dit != dict->cend(); ++dit)
  {
    if (dit->second >= minf)
    {
      if (!first)
      {
        os << " ";
      }
      os << dit->first << ":" << dit->second;
      first = false;
    }
  }
  os << " \n";
}

void maxfreq(const shramko::ForwardList< std::string >& args, DictionaryManager& dm, std::ostream& os)
{
  if (args.getSize() != 2)
  {
    os << "INVALID COMMAND\n";
    return;
  }
  auto it = args.cbegin();
  std::string dict_name = *it;
  ++it;
  std::string maxstr = *it;
  int maxf;
  try
  {
    maxf = std::stoi(maxstr);
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
  os << "maxfreq " << maxf << ": ";
  bool first = true;
  for (auto dit = dict->cbegin(); dit != dict->cend(); ++dit)
  {
    if (dit->second > maxf)
    {
      if (!first)
      {
        os << " ";
      }
      os << dit->first << ":" << dit->second;
      first = false;
    }
  }
  os << " \n";
}

void median(const shramko::ForwardList< std::string >& args, DictionaryManager& dm, std::ostream& os)
{
  if (args.getSize() != 1)
  {
    os << "INVALID COMMAND\n";
    return;
  }
  auto it = args.cbegin();
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
  std::vector<int> freqs;
  freqs.reserve(dict->size());
  for (auto dit = dict->cbegin(); dit != dict->cend(); ++dit)
  {
    freqs.push_back(dit->second);
  }
  std::sort(freqs.begin(), freqs.end());
  size_t sz = freqs.size();
  double med = 0.0;
  if (sz % 2 == 1)
  {
    med = freqs[sz / 2];
  }
  else
  {
    med = (static_cast<double>(freqs[sz / 2 - 1]) + freqs[sz / 2]) / 2.0;
  }
  os << "median: " << med << "\n";
}

shramko::HashTable< std::string, CommandFunction > createCommandMap()
{
  shramko::HashTable< std::string, CommandFunction > commandMap;
  commandMap.insert("create", create);
  commandMap.insert("add", add);
  commandMap.insert("increment", increment);
  commandMap.insert("search", search);
  commandMap.insert("delete", delete_);
  commandMap.insert("dump", dump);
  commandMap.insert("top", top);
  commandMap.insert("bot", bot);
  commandMap.insert("minfreq", minfreq);
  commandMap.insert("maxfreq", maxfreq);
  commandMap.insert("median", median);
  return commandMap;
}
