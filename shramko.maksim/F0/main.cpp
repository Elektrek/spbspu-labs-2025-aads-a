#include <string>
#include <limits>
#include <iostream>
#include <functional>
#include <memory>
#include "commands.hpp"

int main(int argc, char * argv[])
{
  using namespace freq_analysis;
  DictCollection dicts;

  if (argc == 2)
  {
    std::string arg = argv[1];
    if (arg == "--help")
    {
      printHelp(std::cout);
      return 0;
    }
  }

  shramko::HashTable< std::string, std::function< void() > > cmds;
  cmds["create"] = std::bind(create, std::ref(std::cin), std::ref(dicts), std::ref(std::cout));
  cmds["add"] = std::bind(addWord, std::ref(std::cin), std::ref(dicts), std::ref(std::cout));
  cmds["increment"] = std::bind(incrementWord, std::ref(std::cin), std::ref(dicts), std::ref(std::cout));
  cmds["search"] = std::bind(searchWord, std::ref(std::cin), std::cref(dicts), std::ref(std::cout));
  cmds["delete"] = std::bind(deleteWord, std::ref(std::cin), std::ref(dicts), std::ref(std::cout));
  cmds["dump"] = std::bind(dumpDict, std::ref(std::cin), std::cref(dicts), std::ref(std::cout));
  cmds["top"] = std::bind(topWords, std::ref(std::cin), std::cref(dicts), std::ref(std::cout));
  cmds["bot"] = std::bind(botWords, std::ref(std::cin), std::cref(dicts), std::ref(std::cout));
  cmds["minfreq"] = std::bind(minFreqWords, std::ref(std::cin), std::cref(dicts), std::ref(std::cout));
  cmds["maxfreq"] = std::bind(maxFreqWords, std::ref(std::cin), std::cref(dicts), std::ref(std::cout));
  cmds["median"] = std::bind(medianFreq, std::ref(std::cin), std::cref(dicts), std::ref(std::cout));

  std::string command;
  while (std::getline(std::cin >> command, '\n'))
  {
    try
    {
      cmds.at(command)();
    }
    catch (const std::out_of_range &)
    {
      std::cout << "<INVALID COMMAND>\n";
    }
    catch (const std::runtime_error & e)
    {
      printError(e.what(), std::cout);
    }
    catch (const std::exception & e)
    {
      std::cout << e.what() << '\n';
    }
  }
  return 0;
}
