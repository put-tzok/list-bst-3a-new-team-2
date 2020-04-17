#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

unsigned int ns[] = {5000,10000, 15000, 20000, 25000, 30000, 35000, 40000, 45000,
 50000,55000,60000};
// punkt - zawiera klucz (wartoœæ) i wskaŸnik do nastêpnika
struct odnosnik {
    int klucz;
    struct odnosnik *next;
};

// g³owa, która rozpoczyna listê
struct odnosnik *glowa = NULL;



struct odnosnik* list_insert(int wartosc) {

    struct odnosnik *stworz_odnosnik = (odnosnik*)malloc(sizeof(*stworz_odnosnik));
    stworz_odnosnik->klucz = wartosc;
    stworz_odnosnik->next = glowa;
    glowa = stworz_odnosnik;

}

struct odnosnik *i;

struct odnosnik* list_search(int wartosc) {
    i= glowa;

    while(i != NULL && i->klucz != wartosc)
    {
      i = i->next;
    }
    return i;
}


void list_delete(int wartosc) {
    if (glowa->klucz == wartosc)
    {
        glowa = glowa->next;
    }
    else
    {
        struct odnosnik *prev = glowa;
        i = glowa->next;
        while (i != NULL && i->klucz != wartosc)
        {
            prev = i;
            i = i->next;
        }
        if (i != NULL)
        {
            prev->next = i->next;
        }
    }
}

unsigned int list_size() {
    int size = 0;
    i = glowa;

    while(i != NULL)
    {
      size = size + 1;
      i = i->next;
    }

    return size;
}

/*
 * Fill an array with increasing values.
 *
 * Parameters:
 *      int *t:     pointer to the array
 *      int n:      number of elements in the array
 */
void fill_increasing(int *t, int n) {
    for (int i = 0; i < n; i++) {
        t[i] = i;
    }
}

/*
 * Reorder array elements in a random way.
 *
 * Parameters:
 *      int *t:     pointer to the array
 *      int n:      number of elements in the array
 */
void shuffle(int *t, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % i;
        int temp = t[i];
        t[i] = t[j];
        t[j] = temp;
    }
}

int main() {
    bool no_yes[] = { false, true };

    for (unsigned int i = 0; i < sizeof(no_yes) / sizeof(*no_yes); i++) {
        bool enable_shuffle = no_yes[i];

        for (unsigned int j = 0; j < sizeof(ns) / sizeof(*ns); j++) {
            unsigned int n = ns[j];

            // always create an array of size `n` and fill it with increasing wartoscs
            int *t =(int*) malloc(n * sizeof(*t));
            fill_increasing(t, n);

            // if true, reorder array elements randomly
            if (enable_shuffle) {
                shuffle(t, n);
            }

            // insert elements in the order present in array `t`
            clock_t insertion_time = clock();
            for (unsigned int k = 0; k < n; k++) {
                struct odnosnik *i = list_insert(t[k]);
                assert(i != NULL);       // inserted element cannot be NULL
                assert(i->klucz == t[k]);  // inserted element must contain the expected value
            }
            insertion_time = clock() - insertion_time;

            // reorder array elements before searching
            shuffle(t, n);

            // search for every element in the order present in array `t`
            clock_t search_time = clock();
            for (unsigned int k = 0; k < n; k++) {
                struct odnosnik *i = list_search(t[k]);
                assert(i != NULL);       // found element cannot be NULL
                assert(i->klucz == t[k]);  // found element must contain the expected value
            }
            search_time = clock() - search_time;

            // reorder array elements before deletion
            shuffle(t, n);

            // delete every element in the order present in array `t`
            for (unsigned int k = 0, l = n; k < n; k++, l--) {
                assert(list_size() == l);   // list size must be equal to the expected value
                list_delete(t[k]);
            }
            assert(list_size() == 0);       // after all deletions, the list size is zero
            assert(glowa == NULL);           // after all deletions, the list's head is NULL

            free(t);

            printf("%d\t%s\t%f\t%f\n", n, enable_shuffle ? "true" : "false",
                    (double)insertion_time / CLOCKS_PER_SEC,
                    (double)search_time / CLOCKS_PER_SEC);
        }
    }
    return 0;
}
