#ifndef DICTIONARY_MANAGER_HPP
#define DICTIONARY_MANAGER_HPP

#include <string>
#include "UBST.hpp"
#include "hash_table.hpp"

class DictionaryManager
{
 public:
  DictionaryManager() = default;
  ~DictionaryManager() = default;

  DictionaryManager(const DictionaryManager&) = delete;
  DictionaryManager& operator=(const DictionaryManager&) = delete;

  DictionaryManager(DictionaryManager&&) noexcept = default;
  DictionaryManager& operator=(DictionaryManager&&) noexcept = default;

  bool createDict(const std::string& name);
  bool addWord(const std::string& dict_name, const std::string& word, int freq = 1);
  bool removeWord(const std::string& dict_name, const std::string& word);
  bool getFreq(const std::string& dict_name, const std::string& word, int& freq) const;
  const shramko::UBstTree< std::string, int >* getDict(const std::string& name) const;
  shramko::UBstTree< std::string, int >* getDictMutable(const std::string& name);
  bool loadFromFile(const std::string& dict_name, const std::string& filename);

 private:
  shramko::HashTable< std::string, shramko::UBstTree< std::string, int > > dicts_;
};

#endif
