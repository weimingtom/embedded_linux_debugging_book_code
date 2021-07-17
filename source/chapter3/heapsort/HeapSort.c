#include <stdio.h>
#include <stdlib.h>

#define	N_ARR	10
#define	TRUE	1
#define	FALSE	0
#define	SWAP(a,b)	do{(a)=(a)^(b);(b)=(a)^(b);(a)=(a)^(b);}while(0)

int array[] = {0, 5, 1, 2, 7, 9, 0, 3, 4, 8, 6}; /* array[0] unused */

/* 
 * arr[s+1 ... t] satisfy the property of heap,
 * adapt arr[s] to make arr[s ... t] satisfy property
 * of heap
 * n is arr length
 */
int Sift(int *arr, int n, int s, int t)
{
    int i = s;
    int j = s * 2;
    int x = arr[i];
    int finish = FALSE;
    int y = arr[i];   /* save root */
    while ((j <= t) && !finish){
        if ((j < t) && (arr[j] > arr[j + 1])) j = j + 1;
        if (x <= arr[j]){
            finish = TRUE;
        }else{
            arr[i] = arr[j];
            i = j;
            j = 2 * i;
        }
    }
    arr[i] = y;
}

int HeapSort(int *arr, int n)
{
    int i;
    for (i = n/2; i >= 1; i--){
        Sift(arr, n, i, n);
    }
    for (i = n; i >= 2; i--){
        SWAP(arr[1], arr[i]);
        Sift(arr, n, 1, i - 1);
    }
}

int PrintArr(int *arr, int n)
{
    int i;
    for (i = 1; i < n; i++){
        printf("%d,", arr[i]);
    }
    printf("\n");
}

int main(void)
{
    HeapSort(array, N_ARR);
    PrintArr(array, N_ARR);
    return(0);
}

