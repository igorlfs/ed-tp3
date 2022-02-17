#pragma once

#include "cell.hpp"
#include "linearList.hpp"
#include <ostream>

using std::ostream;
using std::string;

template <typename T> class LinkedList : public LinearList<T> {
  public:
    // Construtores
    LinkedList();
    // Destrutores
    ~LinkedList();

    // Getters
    Cell<T> *getHead() { return this->head; }

    // Modificadores
    void insertBeg(const T &u);
    void insertEnd(const T &u);
    void insertPos(const T &u, const int &pos);
    T removeBeg();
    void clear() override;
    /* void escalona(ostream &out, const int &n); */
    /* void escalonaTudo(ostream &out); */

    // Lookup
    bool find(const T &str) const;

    // Operações
    /* void print(ostream &out) const; */

  private:
    Cell<T> *head;
    Cell<T> *tail;

    // Setters
    Cell<T> *setPos(const int &pos, const bool &before) const;
};
