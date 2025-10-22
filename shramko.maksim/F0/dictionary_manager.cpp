#include "dictionary_manager.hpp"
#include <fstream>
#include <algorithm>
#include <cctype>
#include <stdexcept>

bool DictionaryManager::createDict(const std::string& name)
{
  auto result = dicts_.insert(name, shramko::UBstTree<std::string, int>{});
  return result.second;
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
  auto it = dict->find(word);
  if (it == dict->cend())
  {
    return false;
  }
  return dict->erase(word);
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

const shramko::UBstTree<std::string, int>* DictionaryManager::getDict(const std::string& name) const
{
  try
  {
    return &dicts_.at(name);
  }
  catch (const std::out_of_range&)
  {
    return nullptr;
  }
}

shramko::UBstTree<std::string, int>* DictionaryManager::getDictMutable(const std::string& name)
{
  try
  {
    return &dicts_.at(name);
  }
  catch (const std::out_of_range&)
  {
    return nullptr;
  }
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
    word.erase(std::remove_if(word.begin(), word.end(), [](unsigned char c)
    {
      return !std::isalpha(c);
    }), word.end());
    if (!word.empty())
    {
      (*dict)[word]++;
    }
  }
  return true;
}
