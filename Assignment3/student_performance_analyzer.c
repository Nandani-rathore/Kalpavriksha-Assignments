#include<stdio.h>
#include<string.h>
#define MAX_STUDENTS 100
#define MIN_MARKS  0
#define MAX_MARKS 100
#define SUBJECTS 3

struct student{
    int roll_no ;
    char name[100];
    int marks[SUBJECTS];
};

int validStudents(int n){
    if(n < 1 || n > MAX_STUDENTS){
        return 0 ;
    }
    return 1 ;
}

int validMarks(int marks){
    if(marks < MIN_MARKS || marks > MAX_MARKS){
        return  0;
    }
    return 1;
}

// Validate that marks are between 0 and 100.
int validStudentMarks(struct student *s) {
    for (int i = 0; i < SUBJECTS; i++) {
        if (!validMarks(s->marks[i])) {
            return 0;
        }
    }
    return 1;
}

// take input
int inputStudents(struct student *s) {
    char line[256];
    char name[100];
    int roll, m1, m2, m3;
    int used = 0;

    if (fgets(line, sizeof(line), stdin) == NULL) {
        return 0;
    }

    // Reject an input line that is too long
    if (strchr(line, '\n') == NULL && !feof(stdin)) {
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {
        }
        return 0;
    }

    // Read roll number, name and three marks
    if (sscanf(line, "%d %99[^0-9] %d %d %d %n",
               &roll, name, &m1, &m2, &m3, &used) != 5) {
        return 0;
    }

    // Allow only spaces or tabs after the last mark
    while (line[used] == ' ' || line[used] == '\t' ||
           line[used] == '\r' || line[used] == '\n') {
        used++;
    }

    if (line[used] != '\0') {
        return 0;
    }

    // Validate roll number and marks
    if (roll <= 0 ||
        !validMarks(m1) ||
        !validMarks(m2) ||
        !validMarks(m3)) {
        return 0;
    }

    // Remove spaces at the end of the name
    int len = strlen(name);

    while (len > 0 &&
           (name[len - 1] == ' ' || name[len - 1] == '\t')) {
        name[--len] = '\0';
    }

    if (len == 0 || strlen(name) >= sizeof(s->name)) {
        return 0;
    }

    s->roll_no = roll;
    strcpy(s->name, name);
    s->marks[0] = m1;
    s->marks[1] = m2;
    s->marks[2] = m3;

    return 1;
}

int inputAllStudents(struct student stud[], int n) {
    for (int i = 0; i < n; i++) {
        printf("\nEnter details of student %d:\n", i + 1);
        if (!inputStudents(&stud[i])) {
            printf("Invalid input! Please enter valid details.\n");
            return 0;
        }
    }
    return 1;
}

int totalMarks(int marks[]){
    int total = 0;

    for(int i =0;i<SUBJECTS;i++){
        total += marks[i];
    }
    return total ;
}

float avgMarks(int total){
    return (float) total / SUBJECTS;
}

char grade(float avg){
    if(avg >= 85 ){
        return 'A';
    }
    else if(avg >= 70 ){
        return 'B';
    }
    else if(avg >= 50 ){
        return 'C';
    }
    else if(avg >= 35 ){
        return 'D';
    }
    else{
        return 'F';
    }
}

void printPerformance(char grade_value){
    if(grade_value == 'A'){
        printf("Performance : ***** \n");
    }
    else if(grade_value == 'B'){
        printf("Performance : **** \n");
    }
    else if(grade_value == 'C'){
        printf("Performance : *** \n");
    }
    else if(grade_value == 'D'){
        printf("Performance : ** \n");
    }
}

void printRollNumbers(struct student stud[], int i, int n) {
    if (i >= n) {
        return;
    }
    printf("%d  ", stud[i].roll_no);
    printRollNumbers(stud, i + 1, n);
}

void printStudent(struct student *s) {
    printf("Roll: %d\n", s->roll_no);
    printf("Name: %s\n", s->name);

    int total = totalMarks(s->marks);
    printf("Total: %d\n", total);

    float avg = avgMarks(total);
    printf("Average: %.2f\n", avg);

    char student_grade = grade(avg);
    printf("Grade: %c\n", student_grade);

    if (avg >= 35) {
        printPerformance(student_grade);
    }

    printf("\n");  
}

void printAllStudents(struct student stud[], int n) {
    printf("\nStudents Details:\n\n");

    for (int i = 0; i < n; i++) {
        printStudent(&stud[i]);
    }
}

int main(){
    int n ;

    printf("------------- Welcome to the Student Performance analyzer -------------\n");
    printf("Enter the no of student :");
 
    if (scanf("%d", &n) != 1) {
        printf("Invalid input! Enter an integer.\n");
        return 1;
    }

    if (!validStudents(n)) {
        printf("Invalid number of students! Enter 1 to %d.\n",
            MAX_STUDENTS);
        return 1;
    }

    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        // Clear remaining characters from the input line.
    }

    struct student stud[n];
    if (!inputAllStudents(stud, n)) {
        return 1;
    }
    
    printAllStudents(stud, n);
    
    printf("List of Roll Numbers :");
    printRollNumbers(stud, 0, n);

    printf("\n");
    return 0 ;
}