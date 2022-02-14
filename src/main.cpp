#include "inverseIndex.hpp"
#include <iostream>
#include <unistd.h>

using std::cerr;

string inputFileName, outputFileName, corpusDirName, stopWordsFileName;

void help() {
    cerr << "Opções:\n";
    cerr << "\t-i <arq>\t\t(entrada: consulta) \n";
    cerr << "\t-o <arq>\t\t(saída: ranking)\n";
    cerr << "\t-c <folder>\t\t(entrada: corpus)\n";
    cerr << "\t-s <arq>\t\t(entrada: stop words) \n";
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

    // TODO: checar se os arquivos não são vazios usando o erroAsser()
}

int main(int argc, char *argv[]) {
    parseArgs(argc, argv);
    InverseIndex I;
    I.createIndex(corpusDirName, stopWordsFileName);
    return 0;
}
