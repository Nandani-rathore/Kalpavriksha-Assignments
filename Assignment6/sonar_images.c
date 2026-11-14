#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<string.h>
#include<errno.h>
#include<time.h>

int getMatrixSize(){
    char input[100];
    char *end;
    long n;
    
    while(1){
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
            continue;
        }

        errno = 0;
        n = strtol(input , &end , 10);

        if(end == input){
            printf("please enter a number.\n");
            continue;
        }

        if(errno == ERANGE){
            printf("Enter no. is too big or too small.\n");
            continue;
        }

        while(isspace((unsigned char)*end)){
            end++;
        }

        if(*end != '\0'){
            printf("invalid input! please enter a integer only.\n");
            continue;
        }

        if(n < 2 || n > 10){
            printf("n must be between 2 and 10.\n");
            continue;
        }

        return (int)n;
    }
    
}

int **createMatrix(int n){
    int **arr = malloc(n *sizeof(int *));

    if(arr == NULL){
        printf("Memory allocation failed.\n");
        return NULL;
    }

    for(int i = 0 ; i < n ; i++){
        *(arr + i) = malloc(n * sizeof(int));

        if(*(arr  + i) == NULL){
            printf("Row allocation failed.\n"); 

            for(int j = 0 ; j < i ; j++){
                free(*(arr + j));
            }

            free(arr);
            return NULL;
        }

    }
    return arr;
}



void generateSonarImage(int **arr, int n){
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            *(*(arr + i) + j) = rand() % 256;
        }
    }
}



void displayMatrix(int **arr, int n){

    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            printf("%d  ", *(*(arr + i) + j));
        }
        printf("\n");
    }
}


void rotateMatrixClockwise(int **arr , int n){
    // rotated 90 degree clockwise 
    // transpose + reverse = 90 degree clockwise

    for(int i = 0 ; i < n ; i++){
        for(int j =  i+1 ; j < n ; j++){
            int temp = *(*(arr + i) + j);
            *(*(arr + i) + j) = *(*(arr + j) + i);
            *(*(arr + j) + i) = temp;
        }
    }

    // reverse
    for(int i = 0 ; i < n ; i++){
        int p = 0 , q = n-1;
        while(p < q){
            int temp = *(*(arr + i) + p);
            *(*(arr + i) + p) = *(*(arr + i) + q);
            *(*(arr + i) + q) = temp;
            p++;
            q--;
        }
    }
}


void copyRow(int *temp, int **arr, int row, int n){

    for(int j = 0; j < n; j++){
        *(temp + j) = *(*(arr + row) + j);
    }
}


int applySmoothingFilter(int **arr , int n ){
    
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
                *(*(arr + i) + j) = *(output + j);
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

void freeMatrix(int **arr , int n){
    for(int i = 0 ; i < n ; i++){
        free(*(arr+i));
    }

    free(arr);
}

int main(){
    // take input n with validations
    int n = getMatrixSize();
    if(n == 0){
        return 1;
    }

    srand((unsigned int)time(NULL));
    int **arr = createMatrix(n);
    if(arr == NULL){
        return 1;
    }

    // generate random pixel values for the sonar image
    generateSonarImage(arr , n);
    printf("\nThe Original matrix is:\n");
    displayMatrix(arr , n);

    // rotate matrix 
    rotateMatrixClockwise(arr , n);
    printf("\nThe matrix after 90 degree rotation is :\n");
    displayMatrix(arr , n);

    // apply smooth filter 3*3 -> avg of itself and neighbour
    if(!applySmoothingFilter(arr , n)){
        freeMatrix(arr , n);
        return 1;
    }
    printf("\nArray after applying 3*3 Smoothing filter is:\n");
    displayMatrix(arr , n);

    // free the memory
    freeMatrix(arr , n);

    return 0;

}