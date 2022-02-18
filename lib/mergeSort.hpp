#include <string>

using std::string;

template <typename T>
void merge(T array[], string secondArray[], int const &left, int const &mid,
           int const &right) {
    const int leftSize = mid - left + 1;
    const int rightSize = right - mid;

    T *leftArray = new T[leftSize], *rightArray = new T[rightSize];
    string *secLeftArray = new string[leftSize],
           *secRightArray = new string[rightSize];

    for (int i = 0; i < leftSize; i++) {
        leftArray[i] = array[left + i];
        secLeftArray[i] = secondArray[left + i];
    }
    for (int i = 0; i < rightSize; i++) {
        rightArray[i] = array[mid + 1 + i];
        secRightArray[i] = secondArray[mid + 1 + i];
    }

    int indexLeft = 0, indexRight = 0, indexMerged = left;

    // Merge the temp arrays back into array[left..right]
    while (indexLeft < leftSize && indexRight < rightSize) {
        if (leftArray[indexLeft] >= rightArray[indexRight]) {
            array[indexMerged] = leftArray[indexLeft];
            secondArray[indexMerged] = secLeftArray[indexLeft];
            indexLeft++;
        } else {
            array[indexMerged] = rightArray[indexRight];
            secondArray[indexMerged] = secRightArray[indexRight];
            indexRight++;
        }
        indexMerged++;
    }

    // Copy the remaining elements of left[], if there are any
    while (indexLeft < leftSize) {
        array[indexMerged] = leftArray[indexLeft];
        secondArray[indexMerged] = secLeftArray[indexLeft];
        indexLeft++;
        indexMerged++;
    }

    // Copy the remaining elements of right[], if there are any
    while (indexRight < rightSize) {
        array[indexMerged] = rightArray[indexRight];
        secondArray[indexMerged] = secRightArray[indexRight];
        indexRight++;
        indexMerged++;
    }

    delete[] leftArray;
    delete[] rightArray;
    delete[] secLeftArray;
    delete[] secRightArray;
}

template <typename T>
void mergeSort(T array[], string secondArray[], int const &begin,
               int const &end) {
    if (begin >= end) return;

    int mid = begin + (end - begin) / 2;
    mergeSort(array, secondArray, begin, mid);
    mergeSort(array, secondArray, mid + 1, end);
    merge(array, secondArray, begin, mid, end);
}
