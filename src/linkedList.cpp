#include "linkedList.hpp"
#include <iostream>

using std::cout;
using std::endl;
using std::pair;

// @brief Constrói lista inicializando cabeça e cauda
template <typename T> LinkedList<T>::LinkedList() : LinearList<T>() {
    this->head = new (std::nothrow) Cell<T>;
    /* erroAssert(this->head, "Falha ao alocar dinamicamente a célula."); */
    this->tail = this->head;
}

// @brief Destrói lista liberando células e cabeça
template <typename T> LinkedList<T>::~LinkedList() {
    clear();
    delete this->head;
}

// @brief Retorna a célula (antes) na posição pos
// @param pos, before (se quero a posição antes, assume-se falso)
template <typename T>
Cell<T> *LinkedList<T>::setPos(const int &pos, const bool &before) const {
    Cell<T> *p = this->head;
    for (int i = 1; i < pos; ++i)
        p = p->next;

    if (!before) p = p->next;

    return p;
}

// @brief insere uma nova célula no começo da lista
// @param u, URL a ser inserida
template <typename T> void LinkedList<T>::insertBeg(const T &u) {
    Cell<T> *newCell = new (std::nothrow) Cell<T>;
    /* erroAssert(newCell, "Falha ao alocar dinamicamente a célula."); */

    newCell->item = u;
    newCell->next = this->head->next;
    this->head->next = newCell;
    this->tail = newCell;
    this->size++;
}

// @brief insere uma nova célula no final da lista
// @param u, URL a ser inserida
template <typename T> void LinkedList<T>::insertEnd(const T &u) {
    Cell<T> *newCell = new (std::nothrow) Cell<T>;

    newCell->item = u;
    this->tail->next = newCell;
    this->tail = newCell;
    this->size++;
}

// @brief insere uma nova célula numa dada posição da lista
// @param u, URL a ser inserida, pos, posição de inserção
template <typename T>
void LinkedList<T>::insertPos(const T &u, const int &pos) {
    Cell<T> *newCell = new (std::nothrow) Cell<T>;

    Cell<T> *p = setPos(pos, true);

    newCell->item = u;
    newCell->next = p->next;
    p->next = newCell;
    this->size++;

    if (newCell->next == nullptr) this->tail = newCell;
}

// @brief remove a célula no começo da lista
// @return URL removida
template <typename T> T LinkedList<T>::removeBeg() {

    /* erroAssert(!this->empty(), "Falha ao remover item da lista: lista
     * vazia"); */
    Cell<T> *p = this->head->next;
    this->head->next = p->next;
    this->size--;

    if (this->head->next == nullptr) this->tail = this->head;

    T aux = p->item;
    delete p;

    return aux;
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
// @param str, URL procurada
// @return true se URL está na lista e false caso contraŕio
template <typename T> bool LinkedList<T>::find(const T &t) const {
    Cell<T> *p = this->head->next;

    while (p != nullptr) {
        if (p->item == t) return true;

        p = p->next;
    }

    return false;
}

template class LinkedList<string>;
template class LinkedList<pair<string, int>>;
