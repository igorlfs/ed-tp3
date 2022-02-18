template <typename T>
void partition(const int &left, const int &right, int *i, int *j, T *r) {
    *i = left;
    *j = right;

    T pivot = r[(*i + *j) / 2];

    do {
        while (pivot > r[*i])
            (*i)++;
        while (pivot < r[*j])
            (*j)--;

        if (*i <= *j) {
            T aux = r[*i];
            r[*i] = r[*j];
            r[*j] = aux;

            (*i)++;
            (*j)--;
        }
    } while (*i <= *j);
}

template <typename T> void sort(const int &left, const int &right, T *v) {
    int i, j;

    partition(left, right, &i, &j, v);

    if (left < j) sort(left, j, v);
    if (i < right) sort(i, right, v);
}

template <typename T> void quickSort(T *v, const int &n) { sort(0, n - 1, v); }
