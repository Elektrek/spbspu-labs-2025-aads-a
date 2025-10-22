#include "dictionary_manager.hpp"
#include <fstream>
#include <cctype>
#include <UBST/UBST.hpp>
#include <HashTable/hash_table.hpp>
#include <FwdList/FwdList.hpp>
#include <algorithm>

bool DictionaryManager::createDict(const std::string& name)
{
  auto inserted = dicts_.insert({name, shramko::UBstTree< std::string, int >()});
  return inserted.second;
}

bool DictionaryManager::addWord(const std::string& dict_name, const std::string& word, int freq)
{
  auto* dict = getDictMutable(dict_name);
  if (!dict)
  {
    return false;
  }
  (*dict)[word] += freq;
  return true;
}

bool DictionaryManager::removeWord(const std::string& dict_name, const std::string& word)
{
  auto* dict = getDictMutable(dict_name);
  if (!dict)
  {
    return false;
  }
  return dict->erase(word) > 0;
}

bool DictionaryManager::getFreq(const std::string& dict_name, const std::string& word, int& freq) const
{
  const auto* dict = getDict(dict_name);
  if (!dict)
  {
    return false;
  }
  auto it = dict->find(word);
  if (it == dict->cend())
  {
    return false;
  }
  freq = it->second;
  return true;
}

const shramko::UBstTree< std::string, int >* DictionaryManager::getDict(const std::string& name) const
{
  auto it = dicts_.find(name);
  if (it == dicts_.cend())
  {
    return nullptr;
  }
  return &(it->second);
}

shramko::UBstTree< std::string, int >* DictionaryManager::getDictMutable(const std::string& name)
{
  auto it = dicts_.find(name);
  if (it == dicts_.end())
  {
    return nullptr;
  }
  return &(it->second);
}

bool DictionaryManager::loadFromFile(const std::string& dict_name, const std::string& filename)
{
  auto* dict = getDictMutable(dict_name);
  if (!dict)
  {
    return false;
  }
  dict->clear();
  std::ifstream file(filename);
  if (!file)
  {
    return false;
  }
  std::string word;
  while (file >> word)
  {
    std::transform(word.begin(), word.end(), word.begin(), ::tolower);
    word.erase(
      std::remove_if(
        word.begin(), word.end(),
        [](unsigned char c)
        {
          return !std::isalpha(c);
        }
      ),
      word.end()
    );
    if (!word.empty())
    {
      addWord(dict_name, word, 1);
    }
  }
  return true;
}
