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

int stringTraversal(char str[] , int numStack[] , int *numtop , char opStack[] , int *optop){
        int i = 0;
        
        while(str[i] != '\0'){
            if(str[i] == ' ' || str[i] == '\n') i++;

            else if(str[i] == '+' || str[i] == '-' || str[i] == '*' || str[i] == '/' ){
                if(*optop == -1){
                    opStack[++(*optop)] = str[i];
                }
                else{
                    char topelement = opStack[(*optop)];
                    int currpri = checkPriority(str[i]);
                    int stpri = checkPriority(topelement);
                    if(currpri > stpri){
                        opStack[++(*optop)] = str[i];
                    }else{
                        // par agar priority less ya same h to pop karna hoga

                        while(*optop != -1 &&  checkPriority(opStack[*optop]) >= checkPriority(str[i])){
                                int first = numStack[(*numtop)--];
                                int second = numStack[(*numtop)--];
                                char calculatedOp = opStack[(*optop)--];
                                int a = 0 ;

                                if(calculatedOp == '+'){
                                    a = second + first ;
                                }
                                else if(calculatedOp == '-'){
                                    a = second - first ; 
                                }
                                else if(calculatedOp == '*'){
                                    a = second * first ; 
                                }
                                else{
                                    if(first != 0) a = second / first ; 
                                    else{
                                        printf("Divisile by 0 is not possible");
                                        return 0 ;
                                    } 
                                }
                                numStack[++(*numtop)] = a;
                        }
                         opStack[++(*optop)] = str[i];    
                    }
                }
                i++;
            }

            else if(str[i] >= '0' && str[i] <= '9' ) {
                int num = 0;
                while(str[i] >= '0' && str[i] <= '9'){
                    num = num*10 + (str[i] - '0');    // subtract from '0' because input is string not no
                    i++;
                }
                (*numtop) += 1;
                numStack[(*numtop)] = num ; 
               
            }
            else{
                printf("Invalid character '%c' found in the expression\n", str[i]);
                return 0;
            }
            
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
    
    int status = stringTraversal(str , numStack , &numtop , opStack , &optop);

    // after string traversal now calculate your final answer

    if(status == 0){
        return 0 ;
    }
    // else status is not zero 

    while(optop != -1){
        int f = numStack[numtop--];
        int s = numStack[numtop--];
        char op = opStack[optop--];
        int a = 0;
        if(op == '+'){
            a = s+f;
        }else if( op == '-'){
            a = s - f;
        }else if( op == '*'){
            a = s * f;
        }else{
            if( f != 0) a = s/f;
            else {
                printf("Divide by 0 is not possible");
                return 0;
            }
        }

        numStack[++numtop] = a;
    }

    printf("\nThe answer is :%d" , numStack[numtop]);

    return 0;
}