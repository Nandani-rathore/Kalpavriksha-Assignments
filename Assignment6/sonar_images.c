#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<string.h>
#include<errno.h>
#include<time.h>

int takeNInput(){
    char input[100];
    char *end;
    long n;
    
    printf("Enter the value of n:");

    if(fgets(input , sizeof(input) , stdin) == NULL){
        printf("Invalid input\n");
        return 0;
    }

    if (strchr(input, '\n') == NULL && !feof(stdin)) {
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF) {
        }
        printf("Input is too long.\n");
        return 0;
    }

    errno = 0;
    n = strtol(input , &end , 10);

    if(end == input){
        printf("please enter a number.\n");
        return 0;
    }

    if(errno == ERANGE){
        printf("Enter no. is too big or too small.");
        return 0;
    }

    while(isspace((unsigned char)*end)){
        end++;
    }

    if(*end != '\0'){
        printf("invalid input! please enter a integer only.");
        return 0;
    }

    if(n < 2 || n > 10){
        printf("n must be between 2 and 10.\n");
        return 0;
    }

    return (int)n;
}


void freePartialArray(int ***arr, int rows,int cell, int n)
{
    for (int i = 0; i < rows; i++) {
        int limit = n;

        if (i == rows - 1 && cell >= 0) {
            limit = cell;
        }

        for (int j = 0; j < limit; j++) {
            free(*(*(arr + i) + j));
        }

        free(*(arr + i));
    }

    free(arr);
}


int ***createArray(int n){
    int ***arr = malloc(n *sizeof(int **));

    if(arr == NULL){
        printf("Memory allocation failed.\n");
        return NULL;
    }

    for(int i = 0 ; i < n ; i++){
        *(arr + i) = malloc(n * sizeof(int *));

        if(*(arr  + i) == NULL){
            printf("Row allocation failed.\n"); 

            freePartialArray(arr , i,-1 , n);
            return NULL;
        }

        for(int j = 0 ; j < n ; j++){
            *(*(arr + i) + j) = malloc(sizeof(int));

            if(*(*(arr + i) + j) == NULL){
                printf("Cell allocation failed.\n");
                freePartialArray(arr , i+1 , j , n);
                return NULL;
            } 
        }
    }
    return arr;
}


void takeArrayInput(int ***arr , int n){
    for(int i = 0;i<n;i++){
        for(int j = 0;j<n;j++){
        *(*(*(arr + i) + j)) = rand() % 256;
        }
    }
}

void display(int ***arr , int n){

    for(int i = 0 ; i < n ; i++){
        for(int j = 0 ; j < n ; j++){
            printf("%d  " , *(*(*(arr + i) + j)));
        }
        printf("\n");
    }
}

void rotateMatrix(int ***arr , int n){
    // rotated 90 degree clockwise 
    // transpose + reverse = 90 degree clockwise

    for(int i = 0 ; i < n ; i++){
        for(int j =  i+1 ; j < n ; j++){
            int *temp = *(*(arr + i) + j);
            *(*(arr + i) + j) = *(*(arr + j) + i);
            *(*(arr + j) + i) = temp;
        }
    }

    // reverse
    for(int i = 0 ; i < n ; i++){
        int p = 0 , q = n-1;
        while(p < q){
            int *temp = *(*(arr + i) + p);
            *(*(arr + i) + p) = *(*(arr + i) + q);
            *(*(arr + i) + q) = temp;
            p++;
            q--;
        }
    }
}

void copyRow(int *temp , int ***arr , int row , int n){
    for(int j = 0 ; j < n ; j++){
        *(temp + j) = *(*(*(arr + row) + j));
    }
}

int applySmoothFilter(int ***arr , int n ){
    
    int *prev = malloc(n * sizeof(int));
    int *curr = malloc(n * sizeof(int));
    int *next = malloc(n * sizeof(int));
    int *output = malloc(n * sizeof(int));

    if (prev == NULL || curr == NULL ||
        next == NULL || output == NULL) {
        printf("Memory allocation failed during smoothing.\n");
        free(prev);
        free(curr);
        free(next);
        free(output);
        return 0;
    }

    // initilize previous 
    for (int j = 0; j < n; j++) {
        *(prev + j) = 0;
    }

    copyRow(curr , arr , 0 , n);

    if(n > 1){
        copyRow(next , arr , 1 , n);
    }

    for(int i = 0 ; i < n ; i++){
        for(int j = 0 ; j < n ; j++){
            int sum = 0;
            int count = 0;

            for(int x = i-1 ; x <= i+1 ; x++){
                for(int y = j-1 ; y <= j+1 ; y++){
                    if(x < 0 || y < 0 || x >= n || y >= n){
                        continue;
                    }

                    if(x == i-1){
                        sum += *(prev + y);
                    }
                    else if(x == i){
                        sum += *(curr + y);
                    }
                    else{
                        sum += *(next + y);
                    }
                    
                    count++;
                    }
                }
                *(output + j) = sum / count;
            }

            for(int j = 0 ; j < n ; j++){
                *(*(*(arr + i) + j)) = *(output + j);
            }

        int *temp = prev;
        prev = curr;
        curr = next;
        next = temp;

        if(i + 2 < n){
            copyRow(next , arr , i+2 , n);
        }  
    }

    free(prev);
    free(curr);
    free(next);
    free(output);

    return 1;
}

void freeMemory(int ***arr , int n){
    for(int i = 0 ; i < n ; i++){
        for(int j = 0 ; j < n ; j++){
            free(*(*(arr + i)+j));
        }

        free(*(arr+i));
    }

    free(arr);
}

int main(){
    // take input n with validations
    int n = takeNInput();
    if(n == 0){
        return 1;
    }

    srand((unsigned int)time(NULL));
    int ***arr = createArray(n);
    if(arr == NULL){
        return 1;
    }

    // Take input
    takeArrayInput(arr , n);
    printf("\nThe Original matrix is:\n");
    display(arr , n);

    // rotate matrix 
    rotateMatrix(arr , n);
    printf("\nThe matrix after 90 degree rotation is :\n");
    display(arr , n);

    // apply smooth filter 3*3 -> avg of itself and neighbour
    if(!applySmoothFilter(arr , n)){
        freeMemory(arr , n);
        return 1;
    }
    printf("\nArray after applying 3*3 Smoothing filter is:\n");
    display(arr , n);

    // free the memory
    freeMemory(arr , n);

    return 0;

}