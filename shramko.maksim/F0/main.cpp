#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <cctype>
#include <stdexcept>
#include "dictionary_manager.hpp"
#include "commands.hpp"
#include <FwdList/FwdList.hpp>

shramko::ForwardList< std::string > splitString(const std::string& str)
{
  std::vector< std::string > temp_tokens;
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
        temp_tokens.push_back(currentToken);
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
    temp_tokens.push_back(currentToken);
  }

  shramko::ForwardList< std::string > tokens;
  for (const auto& tok : temp_tokens)
  {
    tokens.addToBack(tok);
  }
  return tokens;
}

int main(int argc, char* argv[])
{
  DictionaryManager dm;
  auto commandMap = createCommandMap();

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

    std::string commandName = tokens.getFront();
    tokens.removeFront();
    shramko::ForwardList< std::string > args = tokens;

    auto cmd_it = commandMap.find(commandName);
    if (cmd_it != commandMap.cend())
    {
      try
      {
        cmd_it->second(args, dm, std::cout);
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
