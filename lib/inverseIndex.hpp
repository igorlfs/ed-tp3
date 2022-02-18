#pragma once

#include "linkedList.hpp"
#include <string>
#include <utility>

using std::pair;
using std::string;

class InverseIndex {
  public:
    LinkedList<pair<string, int>> *createIndex(const string &corpusDirName,
                                               const string &stopWordsFileName);
    void calculateNormalizers();

  private:
    static constexpr int M = 100003;
    long long hash(const string &s);
    LinkedList<pair<string, int>> index[M];
    LinkedList<string> stopWords;
    LinkedList<double> documentsWeights;
    LinkedList<string> documents;

    int getFrequency(const string &id, LinkedList<pair<string, int>> &list);
    void setStopWords(const string &stopWordsFileName);
    void setDocuments(const string &corpusDirName);
    int hash(const string &s);
    bool isInIndex(const string &id, LinkedList<pair<string, int>> &list);
    void incrementInDoc(const string &id, LinkedList<pair<string, int>> &list);
};
