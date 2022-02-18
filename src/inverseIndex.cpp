#include "inverseIndex.hpp"
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>

using std::cout; // Para depuração
using std::endl;
using std::ifstream;
using std::make_pair;
namespace fs = std::filesystem;

LinkedList<pair<string, int>> *
InverseIndex::createIndex(const string &corpusDirName,
                          const string &stopWordsFileName) {
    setStopWords(stopWordsFileName);
    setDocuments(corpusDirName);
    Cell<string> *p = this->documents.getHead()->getNext();
    while (p != nullptr) {
        ifstream document;
        string documentName = p->getItem();
        // TODO: Checar se abriu corretamente
        document.open(documentName);
        while (true) {
            string str;
            long long pos;
            document >> str;
            // TODO: tratar isso melhor
            /* for (const char c : str) { */
            /*     if (isalpha(tolower(c))) term += c; */
            /* } */
            if (document.eof()) break;
            // TODO: Checar FAIL
            if (this->stopWords.find(str)) continue;
            pos = hash(str);
            if (!isInIndex(documentName, index[pos]))
                index[pos].insertEnd(make_pair(documentName, 1));
            else incrementInDoc(documentName, index[pos]);
        }
        // TODO: Checar se fechou corretamente
        document.close();
        // TODO: cálcula peso do documento
        p = p->getNext();
    }
    return index;
}

void InverseIndex::calculateNormalizers() {
    Cell<string> *p = this->documents.getHead()->getNext();
    int D = this->documents.getSize();
    while (p != nullptr) {
        double weight = 0;
        string documentName = p->getItem();
        LinkedList<int> hashes;
        for (int i = 0; i < M; ++i) {
            if (isInIndex(documentName, index[i])) hashes.insertEnd(i);
        }

        Cell<int> *q = hashes.getHead()->getNext();
        while (q != nullptr) {
            int freqTerm = getFrequency(documentName, index[q->getItem()]);
            double numDocsTerm = index[q->getItem()].getSize();
            weight += pow((freqTerm * log(D / numDocsTerm)), 2);
            q = q->getNext();
        }
        weight = sqrt(weight);
        this->documentsWeights.insertEnd(weight);
        p = p->getNext();
    }
}

void InverseIndex::setStopWords(const string &stopWordsFileName) {
    ifstream stopWordsFile;
    stopWordsFile.open(stopWordsFileName);
    // TODO: Checar se abriu corretamente
    while (true) {
        string str;
        stopWordsFile >> str;
        if (stopWordsFile.eof()) break;
        // TODO: Checar FAIL
        this->stopWords.insertEnd(str);
    }
    stopWordsFile.close();
    // TODO: Checar se fechou corretamente
}

void InverseIndex::setDocuments(const string &corpusDirName) {
    for (const auto &entry : fs::directory_iterator(corpusDirName))
        this->documents.insertEnd(entry.path());
}

long long InverseIndex::hash(const string &s) {
    const int p = 53;
    const int m = this->M;
    long long hash_value = 0;
    long long p_pow = 1;
    for (char c : s) {
        hash_value = (hash_value + (c - 'a' + 1) * p_pow) % m;
        p_pow = (p_pow * p) % m;
    }
    return hash_value;
}

bool InverseIndex::isInIndex(const string &id,
                             LinkedList<pair<string, int>> &list) {
    Cell<pair<string, int>> *p = list.getHead()->getNext();

    while (p != nullptr) {
        if (p->getItem().first == id) return true;
        p = p->getNext();
    }

    return false;
}

void InverseIndex::incrementInDoc(const string &id,
                                  LinkedList<pair<string, int>> &list) {
    Cell<pair<string, int>> *p = list.getHead()->getNext();
    while (p != nullptr) {
        if (p->item.first == id) {
            p->item.second++;
            return;
        }
        p = p->getNext();
    }
}

int InverseIndex::getFrequency(const string &id,
                               LinkedList<pair<string, int>> &list) {
    Cell<pair<string, int>> *p = list.getHead()->getNext();
    while (p != nullptr) {
        if (p->item.first == id) {
            return p->item.second;
        }
        p = p->getNext();
    }
    return -1;
}
