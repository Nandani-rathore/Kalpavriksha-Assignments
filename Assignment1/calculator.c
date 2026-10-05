#include<stdio.h>
#include<string.h>

// function

int checkPriority(char curr ){
    if(curr == '*' || curr == '/'){
        return 2;
    }else{
        return 1;
    }
}

int applyOperator(int second , int first , char op , int *result){

    if(op == '+'){
        *result = second + first ;
    }else if(op == '-'){
        *result = second - first ;
    }else if(op == '*'){
        *result = second * first ;
    }else if(op == '/'){
        if(first == 0){
            printf("Error: Division by zero.\n");
            return 0;
        }
        *result = second / first ;
    }
    return 1;
}

int isOperator(char ch){
    if(ch == '+' || ch == '-' || ch == '*' || ch == '/'){
        return 1 ;
    }
    return 0; // not an operator
}

int readNumber(char str[] , int *i){
    int num = 0;

    while (str[*i] >= '0' && str[*i] <= '9'){
        num = num * 10 + (str[*i] - '0');
        (*i)++;
    }
    return num ;
    
}
int evaluate(char str[] , int numStack[] , int *numtop , char opStack[] , int *optop){
    int i = 0;
    int expectNumber = 1;
        
    while(str[i] != '\0'){
        if(str[i] == ' ' || str[i] == '\n') i++;

        else if(isOperator(str[i])){
            if(expectNumber == 1){
                printf("Error : Invalid expression\n");
                return 0 ;
            }
            if(*optop == -1){
                opStack[++(*optop)] = str[i];
            }
            else{
                while(*optop != -1 &&  checkPriority(opStack[*optop]) >= checkPriority(str[i])){
                    int first = numStack[(*numtop)--];
                    int second = numStack[(*numtop)--];
                    char calculatedOp = opStack[(*optop)--];
                                
                    int a;

                    if(applyOperator(second, first, calculatedOp, &a) == 0) {
                        return 0;
                    }

                    numStack[++(*numtop)] = a;
                }
                opStack[++(*optop)] = str[i];    
                }
            expectNumber = 1;
            i++;
            }

        else if(str[i] >= '0' && str[i] <= '9' ) {

            if(expectNumber == 0){
                printf("Error : Invalid expression\n");
                return 0 ;
            }

            int num = readNumber(str , &i);
            (*numtop) += 1;
            numStack[(*numtop)] = num ;   

            expectNumber = 0;
        }else{
            printf("Invalid expression\n");
            return 0;
        }
            
    }
    // expression end ho gyi but abhi bhe 1 no expected h it means error 
    if(expectNumber == 1) {
        printf("Error: Invalid expression.\n");
        return 0;
    }
    return 1;
}

// main function 

int main(){
    printf("----------- Console Based Calculator -----------");
    char str[100] ;

    printf("\nEnter the string :");
    fgets(str , 100 , stdin);

    int numStack[100];
    int numtop = -1;

    char opStack[100];
    int optop = -1;

    // now start the traversal in the string 
    
    int status = evaluate(str , numStack , &numtop , opStack , &optop);

    // after string traversal now calculate your final answer

    if(status == 0){
        return 0 ;
    }
    // else status is not zero 

    while(optop != -1){
        int f = numStack[numtop--];
        int s = numStack[numtop--];
        char op = opStack[optop--];
        int a ;

        if(applyOperator(s, f, op, &a) == 0) {
            return 0;
        }
        numStack[++numtop] = a;
    }

    printf("\nThe answer is :%d" , numStack[numtop]);

    return 0;
}