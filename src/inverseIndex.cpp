#include "inverseIndex.hpp"
#include "quickSort.hpp"
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
        const string documentName = p->getItem();
        // TODO: Checar se abriu corretamente
        document.open(documentName);
        while (true) {
            string str;
            document >> str;
            // TODO: tratar isso melhor
            /* for (const char c : str) { */
            /*     if (isalpha(tolower(c))) term += c; */
            /* } */
            if (document.eof()) break;
            // TODO: Checar FAIL
            if (this->stopWords.find(str)) continue;
            const int pos = hash(str);
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

int InverseIndex::getFrequency(const string &id,
                               LinkedList<pair<string, int>> &list) {
    Cell<pair<string, int>> *p = list.getHead()->getNext();
    while (p != nullptr) {
        if (p->item.first == id) return p->item.second;
        p = p->getNext();
    }
    // Essa função só deve ser chamada quando é garantido que o id está na lista
    throw "Não deve chegar aqui";
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
    namespace fs = std::filesystem;
    this->numberOfDocuments = distance(fs::directory_iterator(corpusDirName),
                                       fs::directory_iterator{});
    string docs[numberOfDocuments];
    int i = 0;
    for (const auto &entry : fs::directory_iterator(corpusDirName)) {
        docs[i] = entry.path();
        i++;
    }
    quickSort(docs, i);
    for (int i = 0; i < this->numberOfDocuments; ++i) {
        this->documents.insertEnd(docs[i]);
    }
}
}

int InverseIndex::hash(const string &s) {
    const int p = 53;
    const int m = this->M;
    int hash_value = 0;
    int p_pow = 1;
    for (const char c : s) {
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
void InverseIndex::calculateNormalizers(double *documentWeights) {
    Cell<string> *p = this->documents.getHead()->getNext();
    const int D = this->numberOfDocuments;
    int i = 0;
    while (p != nullptr) {
        double weight = 0;
        const string documentName = p->getItem();
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
        documentWeights[i] = weight;
        i++;
        p = p->getNext();
    }
}
}
