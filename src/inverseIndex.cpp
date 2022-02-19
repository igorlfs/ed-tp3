#include "inverseIndex.hpp"
#include "mergeSort.hpp"
#include "msgassert.hpp"
#include "quickSort.hpp"
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>

using std::distance;
using std::fstream;
using std::ifstream;
using std::make_pair;
using std::ofstream;

LinkedList<pair<string, int>> *
InverseIndex::createIndex(const string &corpusDirName,
                          const string &stopWordsFileName) {
    clearFile(stopWordsFileName);
    setStopWords(stopWordsFileName);
    setDocuments(corpusDirName);
    Cell<string> *p = this->documents.getHead()->getNext();
    while (p != nullptr) {
        clearFile(p->getItem());
        ifstream document;
        const string documentName = p->getItem();
        // TODO: Checar se abriu corretamente
        document.open(documentName);
        while (true) {
            string str;
            document >> str;
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

void InverseIndex::setQuery(const string &filename) {
    clearFile(filename);
    ifstream stopWordsFile;
    stopWordsFile.open(filename);
    // TODO: Checar se abriu corretamente
    while (true) {
        string str;
        stopWordsFile >> str;
        // TODO: tratar números e afins
        if (stopWordsFile.eof()) break;
        // TODO: Checar FAIL
        this->query.insertEnd(str);
    }
    stopWordsFile.close();
    // TODO: Checar se fechou corretamente
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

void InverseIndex::clearFile(const string &filename) {
    fstream fs(filename, fstream::in | fstream::out);
    if (fs.is_open()) {
        while (!fs.eof()) {
            char c = fs.get();
            if (isalpha(c)) {
                fs.seekp((fs.tellp() - static_cast<std::streampos>(1)));
                fs.put(tolower(c));
                fs.seekp(fs.tellp());
            } else {
                fs.seekp((fs.tellp() - static_cast<std::streampos>(1)));
                fs.put(' ');
                fs.seekp(fs.tellp());
            }
        }
        fs.close();
    }
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

void InverseIndex::process(const string &inputFileName,
                           const string &outputFileName) {
    setQuery(inputFileName);

    const int D = this->numberOfDocuments;
    string docsIds[D];
    Cell<string> *r = this->documents.getHead()->getNext();
    for (int i = 0; i < D; ++i, r = r->getNext()) {
        const string str = r->getItem();
        const unsigned long start = str.rfind('/') + 1;
        erroAssert(start != string::npos, "O path do documento não contém '/'");
        const unsigned long end = str.rfind('.');
        if (end != string::npos) docsIds[i] = str.substr(start, end - start);
        else docsIds[i] = str.substr(start);
    }

    double documentWeights[D];
    calculateNormalizers(documentWeights);
    double normQuery[D];
    memset(normQuery, 0, sizeof(normQuery));
    Cell<string> *p = this->query.getHead()->getNext();

    while (p != nullptr) {
        string term = p->getItem();
        int pos = hash(term);
        if (this->index[pos].empty()) {
            p = p->getNext();
            continue;
        }
        Cell<string> *q = this->documents.getHead()->getNext();
        int i = 0;
        while (q != nullptr) {
            if (isInIndex(q->getItem(), this->index[pos])) {
                int freqTerm = getFrequency(q->getItem(), index[pos]);
                double numDocsTerm = index[pos].getSize();
                double weight = freqTerm * log(D / numDocsTerm);
                normQuery[i] += weight;
            }
            i++;
            q = q->getNext();
        }
        p = p->getNext();
    }
    for (int i = 0; i < D; ++i) {
        normQuery[i] = normQuery[i] / documentWeights[i];
    }
    mergeSort(normQuery, docsIds, 0, D - 1);
    print(outputFileName, docsIds, normQuery);
}

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

void InverseIndex::print(const string &outputFileName,
                         const string *documentIDs, const double *normQuery) {
    ofstream outputFile;
    outputFile.open(outputFileName);
    for (int i = 0; i < this->numberOfDocuments; ++i) {
        if (normQuery[i] <= 0 || i == 10) break;
        outputFile << documentIDs[i] << ' ';
    }
    outputFile.put('\n');
}
