#include <iostream>
#include <string>
#include <fstream>
#include <cctype>
#include "dictionary_manager.hpp"
#include "commands.hpp"
#include <common/FwdList/FwdList.hpp>
#include <common/HashTable/hash_table.hpp>

shramko::ForwardList< std::string > splitString(const std::string& str)
{
  shramko::ForwardList< std::string > tokens;
  std::string currentToken;
  size_t start = 0;
  size_t end = str.length();

  while (start < end && std::isspace(static_cast< unsigned char >(str[start])))
  {
    start++;
  }

  for (size_t i = start; i < end; ++i)
  {
    if (std::isspace(static_cast< unsigned char >(str[i])))
  {
      if (!currentToken.empty())
      {
        tokens.addToBack(currentToken);
        currentToken.clear();
      }
    }
    else
    {
      currentToken += str[i];
    }
  }

  if (!currentToken.empty())
  {
    tokens.addToBack(currentToken);
  }
  return tokens;
}

int main(int argc, char* argv[])
{
  DictionaryManager dm;
  shramko::HashTable< std::string, CommandFunction > commandMap = createCommandMap();

  if (argc == 2)
  {
    std::string file = argv[1];
    if (!dm.createDict("main"))
    {
      std::cout << "DICT EXISTS\n";
    }
    else if (!dm.loadFromFile("main", file))
    {
      std::cout << "FILE NOT FOUND\n";
    }
  }

  std::string line;
  while (std::getline(std::cin, line))
  {
    if (line.empty())
    {
      continue;
    }

    shramko::ForwardList< std::string > tokens = splitString(line);
    if (tokens.isEmpty())
    {
      continue;
    }

    auto it = tokens.begin();
    std::string commandName = *it;
    ++it;

    shramko::ForwardList< std::string > args;
    for (; it != tokens.end(); ++it)
    {
      args.addToBack(*it);
    }

    auto cmdIt = commandMap.find(commandName);
    if (cmdIt != commandMap.end())
    {
      try
      {
        cmdIt->second(args, dm, std::cout);
      }
      catch (const std::exception& e)
      {
        std::cout << "ERROR Unhandled exception: " << e.what() << "\n";
      }
      catch (...)
      {
        std::cout << "ERROR Unknown unhandled exception occurred.\n";
      }
    }
    else
    {
      std::cout << "INVALID COMMAND\n";
    }
  }

  return 0;
}
