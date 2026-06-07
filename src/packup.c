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

typedef struct {
    int *data; // Tablica indeksów osób w kopcu
    double *loads; // Tablica indeksów osób w kopcu
    double *strengths; // wskaźnik do tablicy siły
    int size;
} Heap;

void swap(int *a, int *b)
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
        swap(&h->data[best], &h->data[idx]);
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

    global_d = durations;
    global_w = weights;

    int *sorted = malloc(n*sizeof(int));

    for (int i = 0; i < n ; i++){
        sorted[i] = i;
    }

    qsort(sorted, n, sizeof(int),comp);

    Heap h;
    h.data = malloc(p * sizeof(int));
    h.loads = calloc(p, sizeof(double)); 
    h.strengths = strength;
    h.size = p;

    for(int i = 0; i < p; i++) {
        h.data[i] = i;
    }

    for (int i = (p / 2) - 1; i >= 0; i--) {
        downHeap(&h, i);
    }

    SEXP result = PROTECT(Rf_allocVector(INTSXP, n));

    for(int i = 0; i < n; i++){
        int best = h.data[0];
        double effort = (weights[sorted[i]]*durations[sorted[i]] / h.strengths[best]);
        h.loads[best] += effort;
        INTEGER(result)[sorted[i]] = best + 1;
        downHeap(&h, 0);
    }

    free(sorted);
    free(h.data);
    free(h.loads);

    UNPROTECT(1);
    return result;
 
}