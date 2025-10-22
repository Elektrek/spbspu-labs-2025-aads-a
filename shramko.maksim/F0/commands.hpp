#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <utility>

#include <UBST/UBST.hpp>
#include <HashTable/hash_table.hpp>
#include <FwdList/FwdList.hpp>

namespace freq_analysis
{
  using FrequencyDict = shramko::UBstTree< std::string, size_t >;
  using DictCollection = shramko::HashTable< std::string, FrequencyDict >;

  void create(std::istream & in, DictCollection & dicts, std::ostream & out);
  void addWord(std::istream & in, DictCollection & dicts, std::ostream & out);
  void incrementWord(std::istream & in, DictCollection & dicts, std::ostream & out);
  void searchWord(std::istream & in, const DictCollection & dicts, std::ostream & out);
  void deleteWord(std::istream & in, DictCollection & dicts, std::ostream & out);
  void dumpDict(std::istream & in, const DictCollection & dicts, std::ostream & out);
  void topWords(std::istream & in, const DictCollection & dicts, std::ostream & out);
  void botWords(std::istream & in, const DictCollection & dicts, std::ostream & out);
  void minFreqWords(std::istream & in, const DictCollection & dicts, std::ostream & out);
  void maxFreqWords(std::istream & in, const DictCollection & dicts, std::ostream & out);
  void medianFreq(std::istream & in, const DictCollection & dicts, std::ostream & out);

  void printHelp(std::ostream & out);
  void printError(const std::string & message, std::ostream & out);
}

#endif
