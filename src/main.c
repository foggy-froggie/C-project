#define R_NO_REMAP
#include <R.h>
#include <Rinternals.h>
#include <stdio.h>
#include <stdlib.h>

int *global_d;
double *global_w;

int comp(const void* a, const void* b) {
    int i = *(int*)a;
    int j = *(int*)b;

    if (global_d[i] > global_d[j]) return -1;
    if (global_d[i] < global_d[j]) return 1;

    if (global_w[i] > global_w[j]) return -1;
    if (global_w[i] < global_w[j]) return 1;

    return 0;
}
#define R_NO_REMAP
typedef struct {
    int *data;
    double *loads;
    int size;
    int capacity;
} Heap;

void swap(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

Heap *createHeap(int capacity){
    Heap *h = (Heap*) malloc(sizeof(Heap));
    h->data = malloc(capacity * sizeof(int));
    h->loads = malloc(capacity * sizeof(double));
    h->size = 0;
    h->capacity = capacity;
    return h;
}

void upHeap(Heap *h, int idx){
    while(idx > 0){
        int parent = (idx - 1) / 2;

        if (h->loads[h->data[parent]] <= h->loads[h->data[idx]])
            break;
        swap(&h->data[parent], &h->data[idx]);
        idx = parent;
    }
}

void downHeap(Heap *h, int idx){
    while(1){
        int l = 2*idx + 1;
        int r = 2*idx + 2;
        int best = idx;

        if(l < h->size && h->loads[h->data[l]] < h->loads[h->data[best]])
            best = l;

        if(r < h->size && h->loads[h->data[r]] < h->loads[h->data[best]])
            best = r;

        if(best == idx) break;
        swap(&h->data[best], &h->data[idx]);
        idx = best;
    }
}

void insert(Heap *h, int val){
    h->data[h->size] = val;
    upHeap(h, h->size);
    h->size++;
}

int extractMin(Heap *h){
    if (h->size <= 0) {
        Rf_error("Empty heap");
    }

    int res = h->data[0];

    h->data[0] = h->data[h->size - 1];
    h->size--;

    downHeap(h, 0);
    return res;
}

void freeHeap(Heap *h) {
    free(h->data);
    free(h->loads);
    free(h);
}


SEXP assign_items(
    SEXP weightsSEXP,
    SEXP durationsSEXP,
    SEXP strengthSEXP
){
    if (!Rf_isReal(weightsSEXP)) Rf_error("weights must be type double.");
    if (!Rf_isInteger(durationsSEXP)) Rf_error("durations must be type integer.");
    if (!Rf_isReal(strengthSEXP)) Rf_error("strength must be type double.");

    int n = LENGTH(weightsSEXP);
    int p = LENGTH(strengthSEXP);

    if (LENGTH(durationsSEXP) != n) Rf_error("weights and durations must be the same lenght");
    if (p <= 0) Rf_error("There must be at least one person");

    double *weights = REAL(weightsSEXP);
    int *durations = INTEGER(durationsSEXP);
    double *strength = REAL(strengthSEXP);

    for(int i = 0; i < n; i++) {
        if(weights[i] <= 0) Rf_error("weight must be > 0!");
        if(durations[i] <= 0) Rf_error("duration must be > 0!");
    }
    for(int i = 0; i < p; i++) {
        if(strength[i] <= 0) Rf_error("strength must be > 0!");
    }

    global_d = durations;
    global_w = weights;

    int *sorted = malloc(n*sizeof(int));

    for (int i = 0; i < n ; i++){
        sorted[i] = i;
    }

    qsort(sorted, n, sizeof(int),comp);

    Heap *h = createHeap(p);
    for(int i = 0; i < p; i++) {
        h->data[i] = i;
        h->loads[i] = 0.0;
        h->size++;
    }

    SEXP result = PROTECT(Rf_allocVector(INTSXP, n));

    for(int i = 0; i < n; i++){
        int best = extractMin(h);
        double effort = (weights[sorted[i]]*durations[sorted[i]] / strength[best]);
        h->loads[best] += effort;
        INTEGER(result)[sorted[i]] = best + 1;
        insert(h, best);
    }

    free(sorted);
    freeHeap(h);
    UNPROTECT(1);
    return result;
 
}