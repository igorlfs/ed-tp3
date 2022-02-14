#include "inverseIndex.hpp"
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

using std::ifstream;
using std::make_pair;
namespace fs = std::filesystem;

void InverseIndex::createIndex(const string &corpusDirName,
                               const string &stopWordsFileName) {
    this->documents = getDocuments(corpusDirName);
    this->stopWords = getStopWords(stopWordsFileName);
    int i = 1;
    for (auto doc : documents) {
        ifstream document;
        document.open(doc);
        // TODO: Checar se abriu corretamente
        while (true) {
            string str, term;
            document >> str;
            for (const char c : str) {
                if (isalnum(tolower(c))) term += c;
            }
            // TODO: Converter para lowercase
            if (document.eof()) break;
            // TODO: Checar FAIL
            if (isStopWord(term)) continue;
            // Olha se o termo já está no índice
            if (!isInIndex(term, i))
                this->index[term].push_back(make_pair(i, 1));
            else incrementInDoc(term, i);
        }
        document.close();
        // TODO: Checar se fechou corretamente
        i++;
    }
}

list<string> InverseIndex::getStopWords(const string &stopWordsFileName) {
    ifstream stopWordsFile;
    stopWordsFile.open(stopWordsFileName);
    // TODO: Checar se abriu corretamente
    list<string> stopWords;
    while (true) {
        string str;
        stopWordsFile >> str;
        if (stopWordsFile.eof()) break;
        // TODO: Checar FAIL
        stopWords.push_back(str);
    }
    stopWordsFile.close();
    // TODO: Checar se fechou corretamente
    return stopWords;
}

list<string> InverseIndex::getDocuments(const string &corpusDirName) {
    list<string> documentNames;
    for (const auto &entry : fs::directory_iterator(corpusDirName)) {
        documentNames.push_back(entry.path());
    }
    documentNames.sort();
    return documentNames;
}

bool InverseIndex::isStopWord(const string &term) {
    for (auto it : this->stopWords)
        if (it == term) return true;

    return false;
}

bool InverseIndex::isInIndex(const string &term, const int &i) {
    if (this->index.find(term) == this->index.end()) return false;

    for (auto it : this->index[term])
        if (it.first == i) return true;

    return false;
}

void InverseIndex::incrementInDoc(const string &term, const int &i) {
    for (auto it = this->index[term].begin(); it != this->index[term].end();
         ++it) {
        if (it->first == i) {
            it->second++;
            break;
        }
    }
}
