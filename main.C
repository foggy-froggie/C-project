#include <R.h>
#include <Rinternals.h>
#include <stdio.h>
#include <stdlib.h>

#define ABS(x) ((x) < 0 ? -(x) : (x))

static const int *global_d = durations;
static const int *global_w = weights;

int comp(const void *a, const void *b) {
  int i = *(int *)a;
  int j = *(int *)b;

  if (global_d[i] > global_d[j])
    return -1;
  if (global_d[i] < global_d[j])
    return 1;

  if (global_w[i] > global_w[j])
    return -1;
  if (global_w[i] < global_w[j])
    return 1;

  return 0;
}

typedef struct {
  int *data;
  int size;
  int capacity;
} Heap;

void swap(int *a, int *b) {
  int temp = *a;
  *a = *b;
  *b = temp;
}

Heap *createHeap(int capacity) {
  Heap *h = (Heap *)malloc(sizeof(Heap));
  h->data = malloc(capacity * sizeof(int));
  h->size = 0;
  h->capacity = capacity;
  return h;
}

void upHeap(Heap *h, double *loads, int idx) {
  while (idx > 0) {
    int parent = (idx - 1) / 2;

    if (loads[h->data[parent]] <= loads[h->data[idx]])
      break;

    swap(&h->data[parent], &h->data[idx]);
    idx = parent;
  }
}

void downHeap(Heap *h, double *w, int idx) {
  while (1) {
    int l = 2 * idx + 1;
    int r = 2 * idx + 2;
    int best = idx;

    if (l < h->size && w[h->data[l]] < w[h->data[best]])
      best = l;

    if (r < h->size && w[h->data[r]] < w[h->data[best]])
      best = r;

    if (best == idx)
      break;

    swap(&h->data[best], &h->data[idx]);
    idx = best;
  }
}

void insert(Heap *h, int val, double *loads) {
  h->data[h->size] = val;
  upHeap(h, loads, h->size);
  h->size++;
}

int extractMin(Heap *h, double *w) {
  if (heap->size <= 0) {
    error("Próba pobrania elementu z pustego heap.");
  }

  int res = h->data[0];

  h->data[0] = h->data[h->size - 1];
  h->size--;

  downHeap(h, w, 0);

  return res;
}

void freeHeap(Heap *h) {
  free(h->data);
  free(h);
}

typedef struct node {
  int item_id;
  struct node *next;
} node;

typedef struct {
  node *head;
  int count;
  double load;
} person_load;

void pushFront(person_load *p_load, int item_id) {
  node *new_node = malloc(sizeof(node));
  new_node->item_id = item_id;
  new_node->next = p_load->head;
  p_load->head = new_node;
  p_load->count++;
}

node *getRandomNode(person_load *p_load) {
  if (p_load)
    int target = rand() % p_load->count;
  node *current = p_load->head;
  for (int i = 0; i < target; i++) {
    current = current->next;
  }
  return current;
}

SEXP optimize(SEXP resultSEXP, SEXP weightsSEXP, SEXP durationsSEXP,
              SEXP strengthSEXP, SEXP num_of_swaps) {
  double *weights = REAL(weightsSEXP);
  int *durations = INTEGER(durationsSEXP);
  double *strength = REAL(strengthSEXP);
  int *result = INTEGER(resultSEXP);

  int n_swaps = asInteger(num_of_swaps);

  int n = LENGTH(weightsSEXP);
  int p = LENGTH(strengthSEXP);

  SEXP new_result = PROTECT(allocVector(INTSXP, n));
  int *new_result_ptr = INTEGER(new_result);

  for (int i = 0; i < n; i++) {
    new_result_ptr[i] = result[i];
  }

  person_load *peoples_items = calloc(p, sizeof(person_load));

  for (int i = 0; i < n; i++) {
    int person_id = result[i] - 1;
    pushFront(&peoples_items[person_id], i);
    double effort = (weights[i] * durations[i]) / strength[person_id];
    peoples_items[person_id].load += effort;
  }

  for (int i = 0; i < n_swaps; i++) {
    int p1 = rand() % p;
    int p2 = rand() % p;

    if (peoples_items[p1].count > 0 && peoples_items[p2].count > 0) {
      node *node1 = getRandomNode(&peoples_items[p1]);
      node *node2 = getRandomNode(&peoples_items[p2]);

      int item1 = node1->item_id;
      int item2 = node2->item_id;

      double effort1 = (weights[item1] * durations[item1]) / strength[p1];
      double effort2 = (weights[item2] * durations[item2]) / strength[p2];

      double new_load_p1 = peoples_items[p1].load - effort1 +
                           (weights[item2] * durations[item2] / strength[p1]);
      double new_load_p2 = peoples_items[p2].load - effort2 +
                           (weights[item1] * durations[item1] / strength[p2]);

      if (ABS(new_load_p1 - new_load_p2) <
          ABS(peoples_items[p1].load - peoples_items[p2].load)) {

        node1->item_id = item2;
        node2->item_id = item1;

        peoples_items[p1].load = new_load_p1;
        peoples_items[p2].load = new_load_p2;

        new_result_ptr[item1] = p2 + 1;
        new_result_ptr[item2] = p1 + 1;
      }
    }
  }

  for (int i = 0; i < p; i++) {
    node *curr = peoples_items[i].head;
    while (curr != NULL) {
      node *next = curr->next;
      free(curr);
      curr = next;
    }
  }
  free(peoples_items);

  UNPROTECT(1);
  return new_result;
}

SEXP assign_items(SEXP weightsSEXP, SEXP durationsSEXP, SEXP strengthSEXP) {
  int n = LENGTH(weightsSEXP);
  int p = LENGTH(strengthSEXP);
  double *weights = REAL(weightsSEXP);
  int *durations = INTEGER(durationsSEXP);
  double *strength = REAL(strengthSEXP);

  int *sorted = malloc(n * sizeof(int));

  for (int i = 0; i < p; i++) {
    if (strength[i] <= 0)
      error("Siła musi być większa od zera!");
  }

  for (int i = 0; i < n; i++) {
    sorted[i] = i;
  }

  qsort(sorted, n, sizeof(int), comp);

  double *curr_weight = malloc(p * sizeof(double));

  for (int i = 0; i < p; i++)
    curr_weight[i] = 0;

  Heap *h = createHeap(p);

  for (int i = 0; i < p; i++)
    insert(h, i, curr_weight);

  SEXP result = PROTECT(allocVector(INTSXP, n));

  for (int i = 0; i < n; i++) {
    int best = extractMin(h, curr_weight);
    curr_weight[best] +=
        weights[sorted[i]] * durations[sorted[i]] / strength[best];
    INTEGER(result)[sorted[i]] = best + 1;
    insert(h, best, curr_weight);
  }

  free(sorted);
  freeHeap(h);
  free(curr_weight);

  UNPROTECT(1);
  return result;
}