#pragma once

#include "linkedList.hpp"
#include <string>
#include <utility>

using std::pair;
using std::string;

class InverseIndex {
  public:
    void createIndex(const string &corpusDirName,
                     const string &stopWordsFileName);
    int countValidWords();

  private:
    long long hash(const string &s);
    LinkedList<string> stopWords;
    LinkedList<string> documents;
    long long indexSize;

    void setStopWords(const string &stopWordsFileName);
    void setDocuments(const string &corpusDirName);
    bool isInIndex(const int &pos, const string &id,
                   LinkedList<pair<string, int>> *index);
    void incrementInDoc(const string &id, LinkedList<pair<string, int>> &index);
};
