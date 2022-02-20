#include "inverseIndex.hpp"
#include "mergeSort.hpp"
#include "msgassert.hpp"
#include "quickSort.hpp"
#include <cmath>
#include <filesystem>
#include <fstream>

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
        document.open(documentName);
        erroAssert(document.is_open(), "Erro ao abrir arquivo do corpus");
        while (true) {
            string term;
            document >> term;
            if (document.eof()) break;
            erroAssert(document.good(), "Erro ao ler do arquivo de stopwords");
            if (this->stopWords.find(term)) continue;
            int pos = hash(term);
            handleCollisions(term, pos);
            if (!isInList(documentName, index[pos])) {
                if (index[pos].empty())
                    index[pos].getHead()->setItem(make_pair(term, 0));
                index[pos].insertEnd(make_pair(documentName, 1));
            } else incrementInDoc(documentName, index[pos]);
        }
        document.close();
        erroAssert(!document.is_open(), "Erro ao fechar arquivo do corpus");
        p = p->getNext();
    }
    return index;
}

int InverseIndex::getFrequency(const string &id,
                               LinkedList<pair<string, int>> &list) const {
    Cell<pair<string, int>> *p = list.getHead()->getNext();
    while (p != nullptr) {
        if (p->item.first == id) return p->item.second;
        p = p->getNext();
    }

    // Essa função só deve ser chamada quando é garantido que o id está na lista
    throw "Não deve chegar aqui";
}

void InverseIndex::setStopWords(const string &filename) {
    ifstream stopWordsFile;
    stopWordsFile.open(filename);
    erroAssert(stopWordsFile.is_open(),
               "Erro ao abrir arquivo com as stopwords");
    while (true) {
        string str;
        stopWordsFile >> str;
        if (stopWordsFile.eof()) break;
        erroAssert(stopWordsFile.good(), "Erro ao ler do arquivo de stopwords");
        this->stopWords.insertEnd(str);
    }
    stopWordsFile.close();
    erroAssert(!stopWordsFile.is_open(),
               "Erro ao fechar arquivo com as stopwords");
}

void InverseIndex::setDocuments(const string &directory) {
    namespace fs = std::filesystem;
    this->numberOfDocuments =
        distance(fs::directory_iterator(directory), fs::directory_iterator{});
    string docs[numberOfDocuments];
    int i = 0;
    for (const auto &entry : fs::directory_iterator(directory)) {
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
    ifstream queryFile;
    queryFile.open(filename);
    erroAssert(queryFile.is_open(), "Erro ao abrir arquivo da consulta");
    while (true) {
        string str;
        queryFile >> str;
        if (queryFile.eof()) break;
        erroAssert(queryFile.good(), "Erro ao ler do arquivo da consulta");
        this->query.insertEnd(str);
    }
    queryFile.close();
    erroAssert(!queryFile.is_open(), "Erro ao fechar arquivo da consulta");
}

void InverseIndex::setIDs(string *docsIDs) {
    Cell<string> *p = this->documents.getHead()->getNext();
    for (int i = 0; i < this->numberOfDocuments; ++i, p = p->getNext()) {
        const string str = p->getItem();
        const unsigned long start = str.rfind('/') + 1;
        erroAssert(start != string::npos, "O path do documento não contém '/'");
        const unsigned long end = str.rfind('.');
        if (end != string::npos) docsIDs[i] = str.substr(start, end - start);
        else docsIDs[i] = str.substr(start);
    }
}

int InverseIndex::hash(const string &s) const {
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

void InverseIndex::handleCollisions(const string &s, int &pos) const {
    bool collides = true;
    int n = 0;
    do {
        if (!this->index[pos].empty() &&
            s != this->index[pos].getHead()->getItem().first) {
            pos = (pos + 1) % this->M;
            n++;
            erroAssert(n != this->M,
                       "Índice cheio, impossível inserir palavra");
        } else collides = false;
    } while (collides);
}

void InverseIndex::clearFile(const string &filename) const {
    fstream fs(filename, fstream::in | fstream::out);
    erroAssert(fs.is_open(), "Erro ao abrir arquivo para limpeza");
    while (!fs.eof()) {
        char c = fs.get();
        fs.seekp((fs.tellp() - static_cast<std::streampos>(1)));
        if (isalpha(c)) fs.put(tolower(c));
        else fs.put(' ');
        fs.seekp(fs.tellp());
    }
    fs.close();
    erroAssert(!fs.is_open(), "Erro ao fechar arquivo para limpeza");
}

bool InverseIndex::isInList(const string &id,
                            LinkedList<pair<string, int>> &list) const {
    Cell<pair<string, int>> *p = list.getHead()->getNext();

    while (p != nullptr) {
        if (p->getItem().first == id) return true;
        p = p->getNext();
    }

    return false;
}

void InverseIndex::incrementInDoc(const string &id,
                                  LinkedList<pair<string, int>> &list) const {
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
    const int D = this->numberOfDocuments;
    setQuery(inputFileName);
    string docsIDs[D];
    setIDs(docsIDs);
    long double documentWeights[D];
    calculateNormalizers(documentWeights);
    long double normQuery[D];
    memset(normQuery, 0, sizeof(normQuery));
    Cell<string> *p = this->query.getHead()->getNext();

    while (p != nullptr) {
        Cell<string> *q = this->documents.getHead()->getNext();
        int i = 0;
        string term = p->getItem();
        int pos = hash(term);
        handleCollisions(term, pos);
        if (this->index[pos].empty()) goto skipPos;
        while (q != nullptr) {
            if (isInList(q->getItem(), this->index[pos])) {
                int freqTerm = getFrequency(q->getItem(), index[pos]);
                long double numDocsTerm = index[pos].getSize();
                long double weight = freqTerm * log(D / numDocsTerm);
                normQuery[i] += weight;
            }
            i++;
            q = q->getNext();
        }
    skipPos:
        p = p->getNext();
    }
    for (int i = 0; i < D; ++i) {
        normQuery[i] = normQuery[i] / documentWeights[i];
    }
    mergeSort(normQuery, docsIDs, 0, D - 1);
    print(outputFileName, docsIDs, normQuery);
}

void InverseIndex::calculateNormalizers(long double *documentWeights) {
    Cell<string> *p = this->documents.getHead()->getNext();
    const int D = this->numberOfDocuments;
    int i = 0;
    while (p != nullptr) {
        long double weight = 0;
        const string documentName = p->getItem();
        LinkedList<int> hashes;
        for (int j = 0; j < M; ++j) {
            if (isInList(documentName, index[j])) hashes.insertEnd(j);
        }
        Cell<int> *q = hashes.getHead()->getNext();
        while (q != nullptr) {
            int freqTerm = getFrequency(documentName, index[q->getItem()]);
            long double numDocsTerm = index[q->getItem()].getSize();
            weight += pow((freqTerm * log(D / numDocsTerm)), 2);
            q = q->getNext();
        }
        weight = sqrt(weight);
        documentWeights[i] = weight;
        i++;
        p = p->getNext();
    }
}

void InverseIndex::print(const string &filename, const string *documentIDs,
                         const long double *normQuery) const {
    ofstream outputFile;
    outputFile.open(filename);
    erroAssert(outputFile.is_open(), "Erro ao abrir arquivo do ranking");
    for (int i = 0; i < this->numberOfDocuments; ++i) {
        if (normQuery[i] <= 0 || i == 10) break;
        outputFile << documentIDs[i] << ' ';
        erroAssert(outputFile.good(), "Erro ao escrever no arquivo do ranking");
    }
    outputFile.put('\n');
    outputFile.close();
    erroAssert(!outputFile.is_open(), "Erro ao abrir arquivo do ranking");
}
