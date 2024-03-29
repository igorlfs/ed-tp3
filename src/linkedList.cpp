#include "linkedList.hpp"
#include "msgassert.hpp"
#include <iostream>

// @brief Constrói lista inicializando cabeça e cauda
template <typename T> LinkedList<T>::LinkedList() : LinearList<T>() {
    this->head = new (std::nothrow) Cell<T>;
    erroAssert(this->head, "Falha ao alocar dinamicamente a célula cabeça.");
    this->tail = this->head;
}

// @brief Destrói lista liberando células e cabeça
template <typename T> LinkedList<T>::~LinkedList() {
    clear();
    delete this->head;
}

// @brief insere uma nova célula no final da lista
// @param c, célula a ser inserida
template <typename T> void LinkedList<T>::insertEnd(const T &c) {
    Cell<T> *newCell = new (std::nothrow) Cell<T>;
    erroAssert(newCell, "Falha ao alocar dinamicamente a célula.");

    newCell->item = c;
    this->tail->next = newCell;
    this->tail = newCell;
    this->size++;
}

// @brief desaloca células da lista, faz cauda=cabeça, coloca tamanho 0
template <typename T> void LinkedList<T>::clear() {
    Cell<T> *p = this->head->next;

    while (p != nullptr) {
        this->head->next = p->next;
        delete p;
        p = this->head->next;
    }

    this->tail = this->head;
    this->size = 0;
}

// @brief verifica se a lista contém a URL
// @param t, item procurado
// @return true se item estiver na lista e false caso contraŕio
template <typename T> bool LinkedList<T>::find(const T &t) const {
    Cell<T> *p = this->head->next;

    while (p != nullptr) {
        if (p->item == t) return true;

        p = p->next;
    }

    return false;
}

// Defina aqui as instâncias usadas no programa
// (Idiossincrasia de C++ com templates)
template class LinkedList<std::string>;
template class LinkedList<std::pair<std::string, int>>;
