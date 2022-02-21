#pragma once

#include "linkedList.hpp"
#include <string.h>
#include <utility>

using std::pair;
using std::string;

class InverseIndex {
  public:
    LinkedList<pair<string, int>> *createIndex(const string &corpusDirName,
                                               const string &stopWordsFileName);
    void process(const string &inputFileName, const string &outputFileName);

  private:
    static constexpr int M = 100003;
    int numberOfDocuments;
    LinkedList<pair<string, int>> index[M];
    LinkedList<string> stopWords;
    LinkedList<string> query;
    LinkedList<string> documents;

    int getFrequency(const string &id,
                     LinkedList<pair<string, int>> &list) const;
    void setFile(const string &filename);
    void setDocuments(const string &directory);
    void setIDs(string *docsIDs);
    int hash(const string &s) const;
    void handleCollisions(const string &s, int &pos) const;
    void clearFile(const string &filename) const;
    bool isInList(const string &id,
                  const LinkedList<pair<string, int>> &list) const;
    void incrementInList(const string &id,
                         LinkedList<pair<string, int>> &list) const;
    void calculateNormalizers(long double *documentWeights);
    void print(const string &filename, const string *documentIDs,
               const long double *normQuery) const;
};
