#include "inverseIndex.hpp"
#include "msgassert.hpp"
#include <iostream>
#include <unistd.h>

using std::cerr;

string inputFileName, outputFileName, corpusDirName, stopWordsFileName;

void help() {
    cerr << "Opções:\n";
    cerr << "\t-i <arq>\t\t(entrada: consulta) \n";
    cerr << "\t-o <arq>\t\t(saída: ranking)\n";
    cerr << "\t-c <diretório>\t\t(entrada: corpus)\n";
    cerr << "\t-s <arq>\t\t(entrada: stopwords) \n";
}

void parseArgs(const int &argc, char **argv) {
    extern char *optarg;

    int c;
    while ((c = getopt(argc, argv, "i:o:c:s:")) != EOF) {
        switch (c) {
            case 'i': inputFileName = optarg; break;
            case 'o': outputFileName = optarg; break;
            case 'c': corpusDirName = optarg; break;
            case 's': stopWordsFileName = optarg; break;
            case 'h':
            default: help(); exit(1);
        }
    }
    erroAssert(!inputFileName.empty(), "Nome do arquivo de consulta vazio");
    erroAssert(!outputFileName.empty(), "Nome do arquivo de ranking vazio");
    erroAssert(
        corpusDirName != ".",
        "Você não pode usar o diretório '.' como corpus, use um subdiretório");
    erroAssert(!corpusDirName.empty(), "Nome do diretório do corpus vazio");
    erroAssert(!stopWordsFileName.empty(),
               "Nome do arquivo de stopwords vazio");
}

int main(int argc, char *argv[]) {
    parseArgs(argc, argv);
    InverseIndex I;
    I.createIndex(corpusDirName, stopWordsFileName);
    I.process(inputFileName, outputFileName);
    return 0;
}
