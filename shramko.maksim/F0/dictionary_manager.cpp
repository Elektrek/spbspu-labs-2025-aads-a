#include "dictionary_manager.hpp"
#include <stdexcept>
#include <fstream>
#include <cctype>
#include <algorithm>

namespace shramko {

bool DictionaryManager::createDict(const std::string& name) {
    auto inserted = dicts_.insert(name, UBstTree<std::string, int>());
    return inserted.second;
}

UBstTree<std::string, int>* DictionaryManager::getDict(const std::string& name) {
    auto it = dicts_.find(name);
    if (it == dicts_.end()) {
        return nullptr;
    }
    return &it->second;
}

const UBstTree<std::string, int>* DictionaryManager::getDict(const std::string& name) const {
    auto it = dicts_.find(name);
    if (it == dicts_.end()) {
        return nullptr;
    }
    return &it->second;
}

bool DictionaryManager::addWord(const std::string& dictName, const std::string& word, int freq) {
    auto dict = getDict(dictName);
    if (dict == nullptr) {
        return false;
    }
    (*dict)[word] += freq;
    return true;
}

bool DictionaryManager::removeWord(const std::string& dictName, const std::string& word) {
    auto dict = getDict(dictName);
    if (dict == nullptr) {
        return false;
    }
    return dict->erase(word) > 0;
}

bool DictionaryManager::getFreq(const std::string& dictName, const std::string& word, int& freq) const {
    auto dict = getDict(dictName);
    if (dict == nullptr) {
        return false;
    }
    auto it = dict->find(word);
    if (it == dict->cend()) {
        return false;
    }
    freq = it->second;
    return true;
}

bool DictionaryManager::loadFromFile(const std::string& dictName, const std::string& filename) {
    auto dict = getDict(dictName);
    if (dict == nullptr) {
        return false;
    }
    dict->clear();
    std::ifstream file(filename);
    if (!file) {
        return false;
    }
    std::string word;
    while (file >> word) {
        std::transform(word.begin(), word.end(), word.begin(), ::tolower);
        word.erase(std::remove_if(word.begin(), word.end(), [](unsigned char c) { return !std::isalpha(c); }), word.end());
        if (!word.empty()) {
            addWord(dictName, word, 1);
        }
    }
    return true;
}

}