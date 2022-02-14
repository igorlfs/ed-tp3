#pragma once

#include <list>
#include <map>
#include <string>
#include <utility>

using std::list;
using std::map;
using std::pair;
using std::string;

class InverseIndex {
  public:
    void createIndex(const string &corpusDirName,
                     const string &stopWordsFileName);
    static list<string> getStopWords(const string &stopWordsFileName);
    static list<string> getDocuments(const string &corpusDirName);
    bool isStopWord(const string &term);
    bool isInIndex(const string &term, const int &i);
    void incrementInDoc(const string &term, const int &i);

  private:
    map<string, list<pair<int, int>>> index;
    list<string> stopWords;
    list<string> documents;
};
