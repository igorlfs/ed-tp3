#pragma once

template <typename T> class LinearList {
  public:
    // Construtores
    LinearList() { this->size = 0; }

    // Getters
    int getSize() const { return this->size; };

    // Capacidade

    // @brief determina se a fila está vazia
    // @return true se vazia; false caso contrário
    bool empty() const { return (this->size == 0); }

    // Modificadores
    virtual void clear() = 0;

  protected:
    int size;
};
