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

const short INVALID_FREQUENCY = -1;

// @brief cria o índice dado um corpus e uma lista de stopwords
// @param corpusDirName, diretório com os documentos
// @param stopWordsFileName, nome do arquivo com as stopwords
void InverseIndex::createIndex(const string &corpusDirName,
                               const string &stopWordsFileName) {
    LinkedList<string> stopWords;
    // Define stop words e documentos
    setFile(stopWordsFileName, stopWords);
    setDocuments(corpusDirName);
    Cell<string> *p = this->documents.getHead()->getNext();
    // Loop pelos documentos para inserir palavra a palavra
    while (p != nullptr) {
        const string documentName = p->getItem();
        clearFile(documentName); // Sanitiza arquivo
        ifstream document;
        document.open(documentName);
        erroAssert(document.is_open(),
                   "Erro ao abrir arquivo do corpus: " << documentName);
        string term;
        while (document >> term) {
            erroAssert(document.good(),
                       "Erro ao ler do arquivo do corpus: " << documentName);
            if (stopWords.find(term)) continue; // Confere se é stopword
            int pos = hash(term);
            handleCollisions(term, pos);
            // Insere item na lista ou incrementa, a depender da presença
            if (!isInList(documentName, this->index[pos])) {
                if (this->index[pos].empty())
                    this->index[pos].getHead()->setItem(make_pair(term, 0));
                index[pos].insertEnd(make_pair(documentName, 1));
            } else incrementInList(documentName, this->index[pos]);
        }
        document.close();
        erroAssert(!document.is_open(),
                   "Erro ao fechar arquivo do corpus: " << documentName);
        p = p->getNext();
    }
}

// @brief cria o ranking com os documentos mais adequados
// @param inputFileName, arquivo com as queries
// @param outputFileName, arquivo que recebe o ranking
void InverseIndex::process(const string &inputFileName,
                           const string &outputFileName) {
    // Define e configura variáveis usadas nos cálculos
    const int D = this->numDocs;
    LinkedList<string> query;
    setFile(inputFileName, query);
    string docsIDs[D];
    setIDs(docsIDs);
    long double documentWeights[D];
    calculateNormalizers(documentWeights);
    long double normQuery[D];
    memset(normQuery, 0, sizeof(normQuery));
    Cell<string> *p = query.getHead()->getNext();

    // Loop nas palavras da query
    while (p != nullptr) {
        Cell<string> *q = this->documents.getHead()->getNext();
        int docTracker = 0;
        string term = p->getItem();
        int pos = hash(term);
        handleCollisions(term, pos);
        if (this->index[pos].empty()) goto skipPos;
        // Loop nos documentos
        while (q != nullptr) {
            int freqTerm = getFrequency(q->getItem(), index[pos]);
            // Se a busca tiver sucesso, calcule a norma
            if (freqTerm != INVALID_FREQUENCY) {
                long double numDocsTerm = index[pos].getSize();
                long double weight = freqTerm * log(D / numDocsTerm);
                normQuery[docTracker] += weight;
            }
            docTracker++;
            q = q->getNext();
        }
    skipPos:
        p = p->getNext();
    }

    // Normalize, ordene e imprima
    for (int i = 0; i < D; ++i) {
        normQuery[i] = normQuery[i] / documentWeights[i];
    }
    mergeSort(normQuery, docsIDs, 0, D - 1);
    print(outputFileName, docsIDs, normQuery);
}

// @brief recupera o número de vezes que uma palavra aparece em tal documento
// @param id, nome do documento
// @param list, lista associada à palavra que se deseja buscar
// @return frequência coressponde se presente, INVALID_FREQUENCY caso contrário
int InverseIndex::getFrequency(const string &id,
                               LinkedList<pair<string, int>> &list) const {
    Cell<pair<string, int>> *p = list.getHead()->getNext();
    while (p != nullptr) {
        if (p->item.first == id) return p->item.second;
        p = p->getNext();
    }

    // Se o item não está presente, retorne inválido
    return INVALID_FREQUENCY;
}

// @brief sanitiza e lê arquivo de entrada
// @param filename, nome do arquivo de entrada
// @param list, lista que recebe as palavras contidas no arquivo
void InverseIndex::setFile(const string &filename, LinkedList<string> &list) {
    clearFile(filename);
    ifstream inputFile;
    inputFile.open(filename);
    erroAssert(inputFile.is_open(), "Erro ao abrir arquivo " << filename);
    string str;
    while (inputFile >> str) {
        list.insertEnd(str);
        erroAssert(inputFile.good(), "Erro ao ler do arquivo " << filename);
    }
    inputFile.close();
    erroAssert(!inputFile.is_open(), "Erro ao fechar arquivo " << filename);
}

// @brief conta número de documentos, ordena e setta documentos do corpus
// @param directory, diretório do corpus
void InverseIndex::setDocuments(const string &directory) {
    namespace fs = std::filesystem;
    this->numDocs =
        distance(fs::directory_iterator(directory), fs::directory_iterator{});
    int IDs[numDocs];
    string extensions[numDocs];
    int i = 0;
    for (const auto &entry : fs::directory_iterator(directory)) {
        string filename = entry.path().filename();
        IDs[i] = stoi(filename);
        if (filename.find('.') != string::npos)
            extensions[i] = filename.substr(
                filename.rfind('.'), filename.length() - filename.rfind('.'));
        i++;
    }
    quickSort(IDs, i);
    for (int i = 0; i < this->numDocs; ++i) {
        this->documents.insertEnd(directory + "/" + std::to_string(IDs[i]) +
                                  extensions[i]);
    }
}

// @brief transforma um path num identificador numérico
// @param array com os IDs dos documentos
void InverseIndex::setIDs(string *docsIDs) {
    Cell<string> *p = this->documents.getHead()->getNext();
    for (int i = 0; i < this->numDocs; ++i, p = p->getNext()) {
        const string str = p->getItem();
        const unsigned long start = str.rfind('/') + 1;
        erroAssert(start != string::npos, "O path do documento não contém '/'");
        const unsigned long end = str.rfind('.');
        if (end != string::npos) docsIDs[i] = str.substr(start, end - start);
        else docsIDs[i] = str.substr(start);
    }
}

// @brief calcula o hash de uma string, baseado em
// https://cp-algorithms.com/string/string-hashing.html
// @param s, string a ser hasheada
// @return hash correspondente
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

// @brief empurra posição para as seguintes enquanto houver colisões
// @param s, termo da posição em questão
// @param pos, posição testada
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

// @brief reescreve arquivo só contendo letras minúsculas e espaços
// @param filename, nome do arquivo a ser sanitizado
void InverseIndex::clearFile(const string &filename) const {
    fstream fs(filename, fstream::in | fstream::out);
    erroAssert(fs.is_open(),
               "Erro ao abrir arquivo " << filename << " para limpeza");
    while (!fs.eof()) {
        char c = fs.get();
        fs.seekp((fs.tellp() - static_cast<std::streampos>(1)));
        if (isalpha(c)) fs.put(tolower(c));
        else fs.put(' ');
        fs.seekp(fs.tellp());
        erroAssert(!fs.bad(),
                   "Erro ao manipular arquivo " << filename << " para limpeza");
    }
    fs.close();
    erroAssert(!fs.is_open(),
               "Erro ao fechar arquivo " << filename << " para limpeza");
}

// @brief verifica se a lista de uma dada palavra contém dado documento
// @param id, nome do documento
// @param list, lista associada à palavra que se deseja checar
bool InverseIndex::isInList(const string &id,
                            const LinkedList<pair<string, int>> &list) const {
    Cell<pair<string, int>> *p = list.getHead()->getNext();

    while (p != nullptr) {
        if (p->getItem().first == id) return true;
        p = p->getNext();
    }

    return false;
}

// @brief verifica se a lista de uma dada palavra contém dado documento
// @param id, nome do documento cujo par receberá o incremento
// @param list, lista associada à palavra que será incrementada
void InverseIndex::incrementInList(const string &id,
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

// @brief calcula os normalizadores de cada documento
// @param documentWeights, vetor que recebe os pesos
void InverseIndex::calculateNormalizers(long double *documentWeights) {
    Cell<string> *p = this->documents.getHead()->getNext();
    // Loop nos docuemntos
    for (int i = 0; p != nullptr; ++i, p = p->getNext()) {
        long double weight = 0;
        const string documentName = p->getItem();
        // Loop nas posições do hash
        for (int j = 0; j < this->M; ++j) {
            if (this->index[j].empty()) continue;
            const int freqTerm = getFrequency(documentName, index[j]);
            if (freqTerm != INVALID_FREQUENCY) {
                const long double numDocsTerm = index[j].getSize();
                weight += pow((freqTerm * log(this->numDocs / numDocsTerm)), 2);
            }
        }
        weight = sqrt(weight);
        documentWeights[i] = weight;
    }
}

// @brief imprime o top 10 (se existir) no arquivo de saída
// @param filename, nome do arquivo de saída
// @param documentIDs, vetor com identificadores dos documentos
// @param normQuery, vetor da busca realiza
void InverseIndex::print(const string &filename, const string *documentIDs,
                         const long double *normQuery) const {
    const int D = this->numDocs;
    const short numRank = 11; // A contagem começa em D - 1
    ofstream outputFile;
    outputFile.open(filename);
    erroAssert(outputFile.is_open(), "Erro ao abrir arquivo do ranking");
    for (int i = D - 1; i >= 0; --i) {
        if (normQuery[i] <= 0 || i == D - numRank) break;
        outputFile << documentIDs[i] << ' ';
        erroAssert(outputFile.good(), "Erro ao escrever no arquivo do ranking");
    }
    outputFile.put('\n');
    outputFile.close();
    erroAssert(!outputFile.is_open(), "Erro ao abrir arquivo do ranking");
}
