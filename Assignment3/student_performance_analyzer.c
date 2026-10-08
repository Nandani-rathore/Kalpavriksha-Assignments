#include<stdio.h>
#include<string.h>
#define MAX_STUDENTS 100
#define MIN_MARKS  0
#define MAX_MARKS 100

struct student{
    int roll_no ;
    char name[100];
    int subject1;
    int subject2;
    int subject3;
};


int valid_students(int n){
    if(n < 1 || n > MAX_STUDENTS){
        printf("Invalid no of students\n");
        return 0;
    }
    return 1 ;
}

int valid_marks(int marks){
    if(marks < MIN_MARKS || marks > MAX_MARKS){
        return  0;
    }
    return 1;
}

// check that the marks are in the range or not (0 - 100)
int valid_student_marks(struct student *s)
{
    if(!valid_marks((*s).subject1)){
        return 0;
    }

    if(!valid_marks((*s).subject2)){
        return 0;
    }

    if(!valid_marks((*s).subject3)){
        return 0;
    }

    return 1;
}

// take the input 

void input_students(struct student *s)
{
    char line[200];

    // Ek poori single line read karega: Roll_Number Full_Name Marks1 Marks2 Marks3
    if (fgets(line, sizeof(line), stdin) != NULL) {
        // %d roll no , %99[^0-9] = name , then %d marks 
        sscanf(line, "%d %99[^0-9] %d %d %d", 
               &s->roll_no, s->name, &s->subject1, &s->subject2, &s->subject3);

        // Name ke end me bache trailing spaces ko hatana
        int len = strlen(s->name);
        while (len > 0 && (s->name[len - 1] == ' ' || s->name[len - 1] == '\t')) {
            s->name[len - 1] = '\0';
            len--;
        }
    }
}


int Total_marks(int s1 , int s2 , int s3){
    return s1 + s2 + s3 ;
}

float Avg_marks(int total){
    return total / 3.0;
}

char Grade(float avg){
    if(avg >= 85 && avg <= 100){
        return 'A';
    }
    else if(avg >= 70 && avg < 85){
        return 'B';
    }
    else if(avg >= 50 && avg < 70){
        return 'C';
    }
    else if(avg >= 35 && avg < 50){
        return 'D';
    }
    else{
        return 'F';
    }
}

void performance(char grade){
    if(grade == 'A'){
        printf("Performance : ***** \n");
    }
    else if(grade == 'B'){
        printf("Performance : **** \n");
    }
    else if(grade == 'C'){
        printf("Performance : *** \n");
    }
    else if(grade == 'D'){
        printf("Performance : ** \n");
    }
}

void recurssion(int i ,int n){
    if(i > n) return;

    printf("%d  ", i);
    recurssion(i+1 , n);
}

int main(){
    printf("------------- Welcome to the Student Performance analyzer -------------");
    int n ; 
    printf("\nEnter the no of student :");
    scanf("%d" , &n);
    // check constraint : 1 <= n <= 100
    if(!valid_students(n)){
        printf("Invalid n value:");
        return 0 ;
    }
    // Buffer me bacha hua '\n' clear karna taaki fgets sahi se kaam kare
    while (getchar() != '\n');
    struct student stud[n];

    for(int i = 0; i < n; i++){
        while(1){
            printf("\nEnter student%d details:\n", i + 1);
            printf("Roll_no , name , marks1 , marks2 , marks3 (seperated by space) :\n");
            input_students(&stud[i]);
            if(valid_student_marks(&stud[i])){
                break;
            }
            printf("Invalid marks! Marks must be between 0 and 100.\n");
            printf("Please enter the student details again.\n");
        }
    }
    
    printf("\nStudents Details :\n\n");
    for(int i =0;i<n;i++){
        printf("Roll no = %d\n",stud[i].roll_no);
        printf("Name = %s\n",stud[i].name);
       
        int total = Total_marks(stud[i].subject1 , stud[i].subject2, stud[i].subject3);
        printf("Total Marks : %d\n" ,total);
        
        float avg = Avg_marks(total);
        printf("Average Marks = %.2f\n" , avg);

        char grade = Grade(avg);
        printf("Grade = %c\n" , grade);

        if(avg < 35){
            printf("\n");
            continue;
        }
        // agar avg 35 sai kam h to performance fxn skip ho jayega
        performance(grade);
        printf("\n\n");
        
    }
    printf("\nList of Roll Numbers (via recursion):");
    recurssion(1 , n);
    return 0 ;
}