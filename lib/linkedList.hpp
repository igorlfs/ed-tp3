#pragma once

#include "cell.hpp"
#include "linearList.hpp"
#include <ostream>

template <typename T> class LinkedList : public LinearList<T> {
  public:
    // Construtores
    LinkedList();

    // Destrutores
    ~LinkedList();

    // Getters
    Cell<T> *getHead() { return this->head; }

    // Modificadores
    void insertEnd(const T &c);
    T removeBeg();
    void clear() override;

    // Lookup
    bool find(const T &t) const;

  private:
    Cell<T> *head;
    Cell<T> *tail;
};
