//Task 1

#include <stdio.h>

void swap(int *x1, int *y2) {
    int z;
    z = *x1;
    *x1 = *y2;
    *y2 = z;
}

int main() {
    int x, y;
    scanf("%d%d", &x, &y);
    swap(&x, &y);
    printf("%d %d", x, y);
    return 0;
}




//Task 2 

#include <stdio.h>
#include <stdlib.h>

int main() {
    int n;
    
    scanf("%d", &n);
    
    int *arr = malloc(n * sizeof(*arr));
    
    for(int i = 0; i < n; i++) {
        scanf("%d", &arr[i]);
    }
    
    for(int i = 0; i < n/2; i++) {
        int temp = arr[i];
        arr[i] = arr[n-1-i];
        arr[n-1-i] = temp;
    }
    
    for(int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    
    free(arr);
    return 0;
}




//Task 3

#include <stdio.h>
#include <stdlib.h>

int main() {
    int n, m;
    scanf("%d %d", &n, &m);
    
    size_t ptr_size = sizeof(int*);
    size_t int_size = sizeof(int);
    
    int **arr = malloc(n * ptr_size);
    for(int i = 0; i < n; i++) {
        arr[i] = malloc(m * int_size);
    }
    
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < m; j++) {
            scanf("%d", &arr[i][j]);
        }
    }
    
    int **arr_trans = malloc(m * ptr_size);
    for(int j = 0; j < m; j++) {
        arr_trans[j] = malloc(n * int_size);
    }
    
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < m; j++) {
            arr_trans[j][i] = arr[i][j];
        }
    }
    
    for(int j = 0; j < m; j++) {
        for(int i = 0; i < n; i++) {
            printf("%d ", arr_trans[j][i]);
        }
        printf("\n");
    }
    
    for(int i = 0; i < n; i++) free(arr[i]);
    free(arr);
    for(int j = 0; j < m; j++) free(arr_trans[j]);
    free(arr_trans);
    
    return 0;
}




//Task 4

#include <stdio.h>
#include <stdlib.h>

int main() {
    int value;
    void *first = NULL, *last = NULL, *box = NULL;

    while(1) {
        scanf("%d", &value);
        if(value == 0) break;
        
        void *new_box = malloc(sizeof(int) + sizeof(void*));
        *(int*)new_box = value;
        *(void**)(new_box + sizeof(int)) = NULL;
        
        if(!first) first = new_box;
        else *(void**)(last + sizeof(int)) = new_box;
        last = new_box;
    }

    void *old = NULL, *next = NULL;
    box = first;
    while(box) {
        next = *(void**)(box + sizeof(int));
        *(void**)(box + sizeof(int)) = old;
        old = box;
        box = next;
    }
    first = old;

    box = first;
    while(box) {
        printf("%d ", *(int*)box);
        void *temp = box;
        box = *(void**)(box + sizeof(int));
        free(temp);
    }
    
    return 0;
}