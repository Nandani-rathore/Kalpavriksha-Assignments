#include<stdio.h>

struct User{
    int id;
    char name[100];
    int age ; 
};

//  first we need to create a file so that 
// we can apply all the operation on that file

void createfile(){
    FILE *fptr ;
    fptr = fopen("usersfile.txt", "a");

    if(fptr == NULL){
        printf("error occour in file creation");
        return ;
    }
    else{
        printf("File exists\n\n");
        fclose(fptr);
    }
}


void createUser(){
    FILE *fptr ;
    struct User user ; 
    fptr = fopen("usersfile.txt" , "a");

    if(fptr == NULL){
        printf("error occur in file opening\n");
        return ;
    }
    else{
        printf("Enter user ID :");
        scanf("%d" , &user.id);

        printf("Enter user Name :");
        scanf(" %[^\n]", user.name);

        printf("Enter user age :");
        scanf("%d" , &user.age);

        fprintf(fptr , "%d|%s|%d\n" , user.id,
        user.name , user.age);

        fclose(fptr);
        printf("User added successfully\n");
    }
}

void displayAll(){
    FILE *fptr;
    struct User user ;
    fptr = fopen("usersfile.txt" , "r");

    if(fptr == NULL){
        printf("error occur in file opening");
        return ;
    }
    else{

        printf("All Users are :\n");
      
        while(fscanf(fptr, "%d|%[^|]|%d",
                     &user.id, user.name, &user.age) == 3){
            printf("ID : %d\n" , user.id);
            printf("Name : %s\n" , user.name);
            printf("Age : %d\n\n" , user.age);
        }  
    }
    fclose(fptr);
}


void modifyDetail(){
    FILE *fptr;
    FILE *temp ;

    int searchId ;
    int found = 0;

    struct User user ;

    fptr = fopen("usersfile.txt" , "r");
    temp = fopen("temp.txt" , "w");
    // write mode because if not exists create new one 

    if(fptr == NULL){
        printf("File not exist");
        return ;
    }

    if(temp == NULL){
        printf("error occur in temp file creation");
        fclose(fptr);
        return ;
    }

    printf("\nEnter user ID for updation :");
    scanf("%d" , &searchId);

    while(fscanf(fptr , "%d|%[^|]|%d" , &user.id , user.name , &user.age) == 3){
        if(user.id == searchId){
            printf("Enter new Name : ");
            scanf(" %[^\n]" , user.name);

            printf("Enter new age :");
            scanf("%d" , &user.age);

            found = 1;
        }

        fprintf(temp , "%d|%s|%d\n" , user.id , user.name , user.age);


    }
    fclose(fptr);
    fclose(temp);

    remove("usersfile.txt");
    rename("temp.txt" , "usersfile.txt");

    if(found == 1){
        printf("\nUser record updated successfully.\n");
    }
    else{
        printf("\nUser Id not found");
    }
}

void deleteUser(){
    FILE *fptr ;
    FILE *temp ;

    struct  User user;
    int found = 0;
    int searchId ;

    fptr = fopen("usersfile.txt" , "r");
    temp = fopen("temp.txt" , "w");

    if(fptr == NULL){
        printf("file not found\n");
        return;
    }

    if(temp == NULL){
        printf("error occuring in file creation");
        return ;
    }

    printf("\nEnter user ID which have to be deleted : ");
    scanf("%d" , &searchId);

    while(fscanf(fptr , "%d|%[^|]|%d" ,&user.id , user.name , &user.age ) == 3){
        if(user.id == searchId){
            found = 1;
            continue;
        }
        fprintf(temp , "%d|%s|%d\n" , user.id , user.name,user.age);
    }

    fclose(fptr);
    fclose(temp);

    remove("usersfile.txt");
    rename("temp.txt" , "usersfile.txt");

    if(found == 1){
        printf("User deleted successfully\n");
    }else{
        printf("wrong Id is entered\n");
    }

}

int main(){
    int choice;
    createfile();

    while(1){
        printf("------ Menu Driven ------\n\n");
        printf("Press 1 : create new user\n");
        printf("Press 2 : display  all users\n");
        printf("Press 3 : modify user details\n");
        printf("Press 4 : Delete a user\n");
        printf("Press 5 : Exit\n");

        printf("Enter your choice : ");
        scanf("%d" , &choice);

        switch (choice)
        {
        case 1:
            createUser();
            break;
        case 2:
            displayAll();
            break;
        
        case 3:
            modifyDetail();
            break;

        case 4:
            deleteUser();
            break;

        case 5:
            printf("Programme is ended. Thank you !");
            return 0 ;
            

        default:
            printf("Invlaid choice entered");
            break;
        }
    }
    return 0 ;
    
}