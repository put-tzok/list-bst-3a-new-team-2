#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>


unsigned int ns[] = {5000,10000, 15000, 20000, 25000, 30000, 35000, 40000, 45000,
 50000,55000,60000};

// odnosnik zawierajacy klucz i odnosniki do prawego i lewega sasiada
struct odnosnik {
  int klucz;
  struct odnosnik* left;
  struct odnosnik* right;
};

// tworzenie korzenia drzewa
struct odnosnik* korzen = NULL;

struct odnosnik** tree_search(struct odnosnik** candidate, int wartosc) {
  if (!candidate)
    return nullptr;

  auto temp = *candidate;
  if (!temp)
    return candidate;

  if (wartosc < temp->klucz)
    return tree_search(&(temp->left), wartosc);
  if (wartosc > temp->klucz)
    return tree_search(&(temp->right), wartosc);
  return candidate;
}

struct odnosnik* tree_insert(int wartosc) {
  auto odnosnikptr = tree_search(&korzen, wartosc);
  *odnosnikptr = new odnosnik{ wartosc, nullptr, nullptr };
  return *odnosnikptr;
}

struct odnosnik** tree_maximum(struct odnosnik** candidate) {
  if (*candidate && (*candidate)->right)
    return tree_maximum(&(*candidate)->right);
  return candidate;
}

void tree_delete(int wartosc) {
  auto odnosnikptr = tree_search(&korzen, wartosc);
  auto temp = *odnosnikptr;
  if (!temp)
    return;

  if (!temp->left && !temp->right)
    *odnosnikptr = nullptr;
  else if (temp->left && !temp->right)
    *odnosnikptr = temp->left;
  else if (!temp->left && temp->right)
    *odnosnikptr = temp->right;
  else {
    auto max = tree_maximum(&temp->left);
    temp->klucz = (*max)->klucz;
    *max = (*max)->left;
  }
}

unsigned int tree_size(struct odnosnik* element) {
  if (!element)
    return 0;
  return tree_size(element->left) + 1 + tree_size(element->right);
}

/*
 * Fill an array with increasing values.
 *
 * Parameters:
 *      int *t:     pointer to the array
 *      int n:      number of elements in the array
 */
void fill_increasing(int* t, int n) {
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
void shuffle(int* t, int n) {
  for (int i = n - 1; i > 0; i--) {
    int j = rand() % i;
    int temp = t[i];
    t[i] = t[j];
    t[j] = temp;
  }
}

/*
 * Check if tree is a valid BST.
 *
 * Parameters:
 *      struct node *element:   pointer to node to be checked
 *
 * Returns:
 *      bool:                   true if subtree rooted in "element" is a BST
 */
bool is_bst(struct odnosnik* element) {
  // empty tree is a valid BST
  if (element == NULL) {
    return true;
  }
  // leaf node is valid
  if (element->left == NULL && element->right == NULL) {
    return true;
  }
  // only right subnodes check it recursively
  if (element->left == NULL && element->right != NULL) {
    return (element->klucz < element->right->klucz) && is_bst(element->right);
  }
  // only left subnodes check it recursively
  if (element->left != NULL && element->right == NULL) {
    return (element->klucz > element->left->klucz) && is_bst(element->left);
  }
  // both subnodes present? check both recursively
  return (element->klucz > element->left->klucz)
    && (element->klucz < element->right->klucz)
    && is_bst(element->left)
    && is_bst(element->right);
}

void insert_increasing(int* t, int n) {
  for (int i = 0; i < n; i++) {
    tree_insert(t[i]);
  }
}

void insert_random(int* t, int n) {
  shuffle(t, n);
  for (int i = 0; i < n; i++) {
    tree_insert(t[i]);
  }
}


void polowienie_binarne(int* t, int p, int r) {
  if (p == r)
  {
    tree_insert(t[p]);
  }
  else if ((r - p) == 1)
  {
    tree_insert(t[p]);
    tree_insert(t[r]);
  }
  else
  {
    int q = p + (r - p) / 2;
    tree_insert(t[q]);
    polowienie_binarne(t, p, q - 1);
    polowienie_binarne(t, q + 1, r);
  }
}

void insert_binary(int* t, int n) {
  polowienie_binarne(t, 0, n - 1);
}

const char* insert_names[] = { "Increasing", "Random", "Binary" };
void (*insert_functions[])(int*, int) = { insert_increasing, insert_random, insert_binary };

int main(int argc, char** argv) {
  for (unsigned int i = 0; i < sizeof(insert_functions) / sizeof(*insert_functions); i++) {
    void (*insert)(int*, int) = insert_functions[i];

    for (unsigned int j = 0; j < sizeof(ns) / sizeof(*ns); j++) {
      unsigned int n = ns[j];

      // crate an array of increasing integers: 0, 1, 2, ...
      int* t = reinterpret_cast<int*>(malloc(n * sizeof(*t)));
      fill_increasing(t, n);

      // insert data using one of methods
      clock_t insertion_time = clock();
      insert(t, n);
      insertion_time = clock() - insertion_time;

      assert(tree_size(korzen) == n);       // after all insertions, tree size must be `n`
      assert(is_bst(korzen));               // after all insertions, tree must be valid BST

      // reorder array elements before searching
      shuffle(t, n);

      // search for every element in the order present in array `t`
      clock_t search_time = clock();
      for (unsigned int k = 0; k < n; k++) {
        struct odnosnik** podnosnik = tree_search(&korzen, t[k]);
        struct odnosnik* iter = *podnosnik;
        assert(iter != NULL);           // found element cannot be NULL
        assert(iter->klucz == t[k]);      // found element must contain the expected value
      }
      search_time = clock() - search_time;

      // reorder array elements before deletion
      shuffle(t, n);

      // delete every element in the order present in array `t`
      for (unsigned int l = 0, m = n; l < n; l++, m--) {
        assert(tree_size(korzen) == m);   // tree size must be equal to the expected value
        tree_delete(t[l]);
        assert(is_bst(korzen));           // after deletion, tree must still be valid BST
      }
      assert(tree_size(korzen) == 0);       // after all deletions, tree has size zero

      free(korzen);
      free(t);

      printf("%d\t%s\t%f\t%f\n",
        n,
        insert_names[i],
        (double)insertion_time / CLOCKS_PER_SEC ,
        (double)search_time / CLOCKS_PER_SEC);
    }
  }
  return 0;
}
