#define R_NO_REMAP
#include <R.h>
#include <Rinternals.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int og_idx;
    int duration;
    double weight;
} Item;

int comp(const void* a, const void* b) {
    const Item *itemA = (const Item*)a;
    const Item *itemB = (const Item*)b;

    if (itemA->duration > itemB->duration) return -1;
    if (itemA->duration < itemB->duration) return 1;

    if (itemA->weight > itemB->weight) return -1;
    if (itemA->weight < itemB->weight) return 1;

    return 0;
}

typedef struct {
    int *data; // Tablica indeksów osób w kopcu
    double *loads; // Obciążenie danej osoby
    double *strengths; // wskaźnik do tablicy siły
    int size;
} Heap;

void heap_swap(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

int heapComp(Heap *h, int idx_i, int idx_j) {
    int p_i = h->data[idx_i];
    int p_j = h->data[idx_j];

    // wybieramy tego który ma mniejsze obciążenie
    if (h->loads[p_i] < h->loads[p_j]) return 1;
    else if (h->loads[p_i] > h->loads[p_j]) return 0;
    // jeśli to samo obciązenie to silniejszy
    else if (h->strengths[p_i] > h->strengths[p_j]) return 1;
    return 0;
}


void downHeap(Heap *h, int idx){
    while(1){
        int l = 2*idx + 1;
        int r = 2*idx + 2;
        int best = idx;

        if(l < h->size && heapComp(h, l, best))
            best = l;

        if(r< h->size && heapComp(h, r, best))
            best = r;

        if(best == idx) break;
        heap_swap(&h->data[best], &h->data[idx]);
        idx = best;
    }
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

    Item *items = (Item *)R_alloc(n, sizeof(Item));
    if (items == NULL) Rf_error("memory allocation failed");

    for (int i = 0; i < n; i++) {
        items[i].og_idx = i;
        items[i].duration = durations[i];
        items[i].weight = weights[i];
    }

    qsort(items, n, sizeof(Item), comp);

    Heap h;
    h.data = (int *)R_alloc(p, sizeof(int));
    h.loads = (double *)R_alloc(p, sizeof(double));
    h.strengths = strength;
    h.size = p;

    for(int i = 0; i < p; i++) {
        h.loads[i] = 0.0;
    }

    if (h.data == NULL || h.loads == NULL) {
        free(items);
        if (h.data) free(h.data);
        if (h.loads) free(h.loads);
        Rf_error("memory allocation failed ");
    }


    for(int i = 0; i < p; i++) {
        h.data[i] = i;
    }

    for (int i = (p / 2) - 1; i >= 0; i--) {
        downHeap(&h, i);
    }

    SEXP result = PROTECT(Rf_allocVector(INTSXP, n));

    for(int i = 0; i < n; i++){
        int best = h.data[0];
        double effort = (items[i].weight*items[i].duration) / h.strengths[best];
        h.loads[best] += effort;
        INTEGER(result)[items[i].og_idx] = best + 1;
        downHeap(&h, 0);
    }
    
    UNPROTECT(1);
    return result;
 
}