#pragma once

template <typename T> class Cell {
  public:
    // Construtores
    Cell() { this->next = nullptr; }

    // Getters
    T getItem() const { return this->item; }
    Cell *getNext() const { return this->next; }

    // Setters
    void setItem(const T &t) { this->item = t; }

  private:
    T item;
    Cell *next;

    template <typename U> friend class LinkedList;
    friend class InverseIndex;
};
