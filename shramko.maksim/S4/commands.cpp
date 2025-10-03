#include "commands.hpp"
#include <stdexcept>

namespace shramko
{
  void print(const TreeOfTrees& trees, std::istream& in, std::ostream& out)
  {
    std::string dictName;
    in >> dictName;
    TreeOfTrees::const_iterator it = trees.find(dictName);
    if (it == trees.cend())
    {
      throw std::invalid_argument("Invalid dictionary name");
    }
    const BasicTree& tree = it->second;
    if (tree.empty())
    {
      out << "<EMPTY>\n";
      return;
    }
    out << dictName;
    for (BasicTree::const_iterator tree_it = tree.cbegin(); tree_it != tree.cend(); ++tree_it)
    {
      out << " " << tree_it->first << " " << tree_it->second;
    }
    out << "\n";
  }

  void complement(TreeOfTrees& trees, std::istream& in, std::ostream& out)
  {
    std::string newDict, firstDict, secondDict;
    in >> newDict >> firstDict >> secondDict;
    TreeOfTrees::const_iterator first_it = trees.find(firstDict);
    TreeOfTrees::const_iterator second_it = trees.find(secondDict);
    if (first_it == trees.cend())
    {
      throw std::invalid_argument("First dictionary not found");
    }
    if (second_it == trees.cend())
    {
      throw std::invalid_argument("Second dictionary not found");
    }
    BasicTree result;
    for (BasicTree::const_iterator it = first_it->second.cbegin(); it != first_it->second.cend(); ++it)
    {
      if (second_it->second.find(it->first) == second_it->second.cend())
      {
        result[it->first] = it->second;
      }
    }
    trees[newDict] = result;
  }

  void intersect(TreeOfTrees& trees, std::istream& in, std::ostream& out)
  {
    std::string newDict, firstDict, secondDict;
    in >> newDict >> firstDict >> secondDict;
    TreeOfTrees::const_iterator first_it = trees.find(firstDict);
    TreeOfTrees::const_iterator second_it = trees.find(secondDict);
    if (first_it == trees.cend())
    {
      throw std::invalid_argument("First dictionary not found");
    }
    if (second_it == trees.cend())
    {
      throw std::invalid_argument("Second dictionary not found");
    }
    BasicTree result;
    for (BasicTree::const_iterator it = first_it->second.cbegin(); it != first_it->second.cend(); ++it)
    {
      auto sec_it = second_it->second.find(it->first);
      if (sec_it != second_it->second.cend())
      {
        result[it->first] = it->second;
      }
    }
    trees[newDict] = result;
  }

  void unite(TreeOfTrees& trees, std::istream& in, std::ostream& out)
  {
    std::string newDict, firstDict, secondDict;
    in >> newDict >> firstDict >> secondDict;
    TreeOfTrees::const_iterator first_it = trees.find(firstDict);
    TreeOfTrees::const_iterator second_it = trees.find(secondDict);
    if (first_it == trees.cend())
    {
      throw std::invalid_argument("First dictionary not found");
    }
    if (second_it == trees.cend())
    {
      throw std::invalid_argument("Second dictionary not found");
    }
    BasicTree result = first_it->second;
    for (BasicTree::const_iterator it = second_it->second.cbegin(); it != second_it->second.cend(); ++it)
    {
      if (result.find(it->first) == result.cend())
      {
        result[it->first] = it->second;
      }
    }
    trees[newDict] = result;
  }
}
