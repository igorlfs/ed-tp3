#pragma once

template <typename T> class LinearList {
  public:
    // Construtores
    LinearList() { this->size = 0; }

    // Getters
    int getSize() const { return this->size; };

    // Capacidade
    bool empty() const { return (this->size == 0); }

    // Modificadores
    virtual void clear() = 0;

  protected:
    int size;
};
