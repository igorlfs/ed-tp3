#pragma once

template <typename T> class Cell {
  public:
    // Construtores
    Cell() { this->next = nullptr; }

    // Getters
    T getItem() { return this->item; }
    Cell *getNext() { return this->next; }

  private:
    T item;
    Cell *next;

    template <typename U> friend class LinkedList;
    friend class InverseIndex;
};
