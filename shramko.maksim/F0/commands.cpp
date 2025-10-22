#include "commands.hpp"
#include <sstream>
#include <stdexcept>
#include <vector>
#include <algorithm>

namespace
{
  bool dictExists(const std::string & name, const freq_analysis::DictCollection & dicts)
  {
    return dicts.find(name) != dicts.cend();
  }

  bool wordExists(const std::string & dictName, const std::string & word, const freq_analysis::DictCollection & dicts)
  {
    auto dit = dicts.find(dictName);
    if (dit == dicts.cend())
    {
      return false;
    }
    return dit->second.find(word) != dit->second.cend();
  }
}

void freq_analysis::create(std::istream & in, DictCollection & dicts, std::ostream & out)
{
  std::string name;
  if (!(in >> name))
  {
    throw std::runtime_error("invalid arguments for create");
  }
  if (dictExists(name, dicts))
  {
    throw std::runtime_error("DICT EXISTS");
  }
  dicts[name] = FrequencyDict{};
  out << "OK: Dictionary '" << name << "' created\n";
}

void freq_analysis::addWord(std::istream & in, DictCollection & dicts, std::ostream & out)
{
  std::string dictName, word, opt;
  size_t freq = 1;
  if (!(in >> dictName >> word))
  {
    throw std::runtime_error("invalid arguments for add");
  }
  if (!dictExists(dictName, dicts))
  {
    throw std::runtime_error("DICT NOT FOUND");
  }
  if (in >> opt && opt.substr(0, 5) == "freq:")
  {
    try
    {
      freq = std::stoul(opt.substr(5));
    }
    catch (...)
    {
      throw std::runtime_error("invalid frequency");
    }
  }
  else if (in >> opt)
  {
    throw std::runtime_error("invalid arguments for add");
  }
  auto & dict = dicts.at(dictName);
  dict[word] = freq;
  out << "OK: Added '" << word << "' with freq=" << freq << "\n";
}

void freq_analysis::incrementWord(std::istream & in, DictCollection & dicts, std::ostream & out)
{
  std::string dictName, word, opt;
  size_t inc = 1;
  if (!(in >> dictName >> word))
  {
    throw std::runtime_error("invalid arguments for increment");
  }
  if (!dictExists(dictName, dicts))
  {
    throw std::runtime_error("DICT NOT FOUND");
  }
  if (in >> opt && opt.substr(0, 3) == "by:")
  {
    try
    {
      inc = std::stoul(opt.substr(3));
    }
    catch (...)
    {
      throw std::runtime_error("invalid increment value");
    }
  }
  else if (in >> opt)
  {
    throw std::runtime_error("invalid arguments for increment");
  }
  auto & dict = dicts.at(dictName);
  dict[word] += inc;
  out << "OK: Incremented '" << word << "' by " << inc << "\n";
}

void freq_analysis::searchWord(std::istream & in, const DictCollection & dicts, std::ostream & out)
{
  std::string dictName, word;
  if (!(in >> dictName >> word))
  {
    throw std::runtime_error("invalid arguments for search");
  }
  if (!dictExists(dictName, dicts))
  {
    throw std::runtime_error("DICT NOT FOUND");
  }
  const auto & dict = dicts.at(dictName);
  auto it = dict.find(word);
  if (it == dict.cend())
  {
    out << "NOT FOUND\n";
    return;
  }
  out << "FOUND: freq=" << it->second << "\n";
}

void freq_analysis::deleteWord(std::istream & in, DictCollection & dicts, std::ostream & out)
{
  std::string dictName, word;
  if (!(in >> dictName >> word))
  {
    throw std::runtime_error("invalid arguments for delete");
  }
  if (!dictExists(dictName, dicts))
  {
    throw std::runtime_error("DICT NOT FOUND");
  }
  auto & dict = dicts.at(dictName);
  if (dict.erase(word) == 0)
  {
    throw std::runtime_error("WORD NOT FOUND");
  }
  out << "OK: Deleted '" << word << "'\n";
}

void freq_analysis::dumpDict(std::istream & in, const DictCollection & dicts, std::ostream & out)
{
  std::string dictName;
  if (!(in >> dictName))
  {
    throw std::runtime_error("invalid arguments for dump");
  }
  if (!dictExists(dictName, dicts))
  {
    throw std::runtime_error("DICT NOT FOUND");
  }
  const auto & dict = dicts.at(dictName);
  out << "dump: ";
  bool first = true;
  dict.traverse_lnr([&](const auto & p)
  {
    if (!first)
    {
      out << " ";
    }
    out << p.first << ':' << p.second;
    first = false;
  });
  out << " \n";
}

void freq_analysis::topWords(std::istream & in, const DictCollection & dicts, std::ostream & out)
{
  std::string dictName, nStr;
  size_t N;
  if (!(in >> dictName >> nStr))
  {
    throw std::runtime_error("invalid arguments for top");
  }
  try
  {
    N = std::stoul(nStr);
  }
  catch (...)
  {
    throw std::runtime_error("invalid N for top");
  }
  if (!dictExists(dictName, dicts))
  {
    throw std::runtime_error("DICT NOT FOUND");
  }
  const auto & dict = dicts.at(dictName);
  shramko::ForwardList<std::pair<size_t, std::string>> fl;
  dict.traverse_lnr([&](const auto & p)
  {
    fl.addToBack({p.second, p.first});
  });
  std::vector<std::pair<size_t, std::string>> v(fl.cbegin(), fl.cend());
  std::sort(v.begin(), v.end(), [](const auto & a, const auto & b)
  {
    if (a.first != b.first)
    {
      return a.first > b.first;
    }
    return a.second < b.second;
  });
  out << "top " << N << ": ";
  size_t cnt = 0;
  for (const auto & pr : v)
  {
    if (cnt >= N) break;
    out << pr.second << ':' << pr.first << " ";
    ++cnt;
  }
  out << "\n";
}

void freq_analysis::botWords(std::istream & in, const DictCollection & dicts, std::ostream & out)
{
  std::string dictName, nStr;
  size_t N;
  if (!(in >> dictName >> nStr))
  {
    throw std::runtime_error("invalid arguments for bot");
  }
  try
  {
    N = std::stoul(nStr);
  }
  catch (...)
  {
    throw std::runtime_error("invalid N for bot");
  }
  if (!dictExists(dictName, dicts))
  {
    throw std::runtime_error("DICT NOT FOUND");
  }
  const auto & dict = dicts.at(dictName);
  shramko::ForwardList<std::pair<size_t, std::string>> fl;
  dict.traverse_lnr([&](const auto & p)
  {
    fl.addToBack({p.second, p.first});
  });
  std::vector<std::pair<size_t, std::string>> v(fl.cbegin(), fl.cend());
  std::sort(v.begin(), v.end(), [](const auto & a, const auto & b)
  {
    if (a.first != b.first)
    {
      return a.first < b.first;
    }
    return a.second < b.second;
  });
  out << "bot " << N << ": ";
  size_t cnt = 0;
  for (const auto & pr : v)
  {
    if (cnt >= N) break;
    out << pr.second << ':' << pr.first << " ";
    ++cnt;
  }
  out << "\n";
}

void freq_analysis::minFreqWords(std::istream & in, const DictCollection & dicts, std::ostream & out)
{
  std::string dictName, minStr;
  size_t minF;
  if (!(in >> dictName >> minStr))
  {
    throw std::runtime_error("invalid arguments for minfreq");
  }
  try
  {
    minF = std::stoul(minStr);
  }
  catch (...)
  {
    throw std::runtime_error("invalid min for minfreq");
  }
  if (!dictExists(dictName, dicts))
  {
    throw std::runtime_error("DICT NOT FOUND");
  }
  const auto & dict = dicts.at(dictName);
  shramko::ForwardList<std::pair<std::string, size_t>> fl;
  dict.traverse_lnr([&](const auto & p)
  {
    if (p.second >= minF)
    {
      fl.addToBack({p.first, p.second});
    }
  });
  std::vector<std::pair<std::string, size_t>> v(fl.cbegin(), fl.cend());
  std::sort(v.begin(), v.end());
  out << "minfreq " << minF << ": ";
  for (const auto & pr : v)
  {
    out << pr.first << ':' << pr.second << " ";
  }
  out << "\n";
}

void freq_analysis::maxFreqWords(std::istream & in, const DictCollection & dicts, std::ostream & out)
{
  std::string dictName, maxStr;
  size_t maxF;
  if (!(in >> dictName >> maxStr))
  {
    throw std::runtime_error("invalid arguments for maxfreq");
  }
  try
  {
    maxF = std::stoul(maxStr);
  }
  catch (...)
  {
    throw std::runtime_error("invalid max for maxfreq");
  }
  if (!dictExists(dictName, dicts))
  {
    throw std::runtime_error("DICT NOT FOUND");
  }
  const auto & dict = dicts.at(dictName);
  shramko::ForwardList<std::pair<std::string, size_t>> fl;
  dict.traverse_lnr([&](const auto & p)
  {
    if (p.second > maxF)
    {
      fl.addToBack({p.first, p.second});
    }
  });
  std::vector<std::pair<std::string, size_t>> v(fl.cbegin(), fl.cend());
  std::sort(v.begin(), v.end());
  out << "maxfreq " << maxF << ": ";
  for (const auto & pr : v)
  {
    out << pr.first << ':' << pr.second << " ";
  }
  out << "\n";
}

void freq_analysis::medianFreq(std::istream & in, const DictCollection & dicts, std::ostream & out)
{
  std::string dictName;
  if (!(in >> dictName))
  {
    throw std::runtime_error("invalid arguments for median");
  }
  if (!dictExists(dictName, dicts))
  {
    throw std::runtime_error("DICT NOT FOUND");
  }
  const auto & dict = dicts.at(dictName);
  if (dict.empty())
  {
    throw std::runtime_error("EMPTY DICT");
  }
  shramko::ForwardList<size_t> fl;
  dict.traverse_lnr([&](const auto & p)
  {
    fl.addToBack(p.second);
  });
  std::vector<size_t> freqs(fl.cbegin(), fl.cend());
  std::sort(freqs.begin(), freqs.end());
  size_t s = freqs.size();
  double med;
  if (s % 2 == 1)
  {
    med = freqs[s / 2];
  }
  else
  {
    med = (static_cast<double>(freqs[s / 2 - 1]) + freqs[s / 2]) / 2.0;
  }
  out << "median: " << med << "\n";
}

void freq_analysis::printHelp(std::ostream & out)
{
  out << std::left;
  out << "Available commands:\n\n";
  constexpr size_t cmdWidth = 30;
  constexpr size_t numWidth = 3;
  out << std::setw(numWidth) << "1." << std::setw(cmdWidth) << "create <name>" << "create a new dictionary\n";
  out << std::setw(numWidth) << "2." << std::setw(cmdWidth) << "add <dict> <word> [freq:N]" << "add or update word frequency\n";
  out << std::setw(numWidth) << "3." << std::setw(cmdWidth) << "increment <dict> <word> [by:N]" << "increment word frequency\n";
  out << std::setw(numWidth) << "4." << std::setw(cmdWidth) << "search <dict> <word>" << "search word frequency\n";
  out << std::setw(numWidth) << "5." << std::setw(cmdWidth) << "delete <dict> <word>" << "delete word\n";
  out << std::setw(numWidth) << "6." << std::setw(cmdWidth) << "dump <dict>" << "dump dictionary contents\n";
  out << std::setw(numWidth) << "7." << std::setw(cmdWidth) << "top <dict> <N>" << "top N words by frequency\n";
  out << std::setw(numWidth) << "8." << std::setw(cmdWidth) << "bot <dict> <N>" << "bottom N words by frequency\n";
  out << std::setw(numWidth) << "9." << std::setw(cmdWidth) << "minfreq <dict> <min>" << "words with freq >= min\n";
  out << std::setw(numWidth) << "10." << std::setw(cmdWidth) << "maxfreq <dict> <max>" << "words with freq > max\n";
  out << std::setw(numWidth) << "11." << std::setw(cmdWidth) << "median <dict>" << "median frequency\n";
}

void freq_analysis::printError(const std::string & message, std::ostream & out)
{
  out << message << "\n";
}
