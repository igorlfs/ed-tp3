#pragma once

template <typename T> class Cell {
  public:
    // Construtores
    Cell() { this->next = nullptr; }

    // Getters
    T getItem() const { return this->item; }
    Cell *getNext() const { return this->next; }

  private:
    T item;
    Cell *next;

    template <typename U> friend class LinkedList;
    friend class InverseIndex;
};
