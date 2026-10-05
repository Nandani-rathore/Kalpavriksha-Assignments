#include<stdio.h>
#define FILENAME "users.txt"

struct User{
    int id;
    char name[100];
    int age ; 
};

void clearInputBuffer(){
    int ch;

    while((ch = getchar()) != '\n' && ch != EOF){
    }
}


int readUser(FILE *fptr, struct User *user){
    return fscanf(fptr, "%d|%99[^|]|%d",
                  &user->id, user->name, &user->age) == 3;
}


void writeUser(FILE *fptr, const struct User *user){
    fprintf(fptr, "%d|%s|%d\n",
            user->id, user->name, user->age);
}




//  first we need to create a file so that 
// we can apply all the operation on that file

int idExists(int id){
    FILE *fptr;
    struct User user;

    fptr = fopen(FILENAME , "r");
    if(fptr == NULL){
        return 0 ;
    }

    while (readUser(fptr , &user)){
        if(user.id == id){
            fclose(fptr);
            return 1;
        }
    }
    fclose(fptr);
    return 0 ;
}




int replaceFile(){
    if(remove(FILENAME) != 0){
        return 0;
    }

    if(rename("temp.txt", FILENAME) != 0){
        return 0;
    }

    return 1;
}

void createfile(){
    FILE *fptr ;
    fptr = fopen(FILENAME, "a");

    if(fptr == NULL){
        printf("error occour in file creation");
        return ;
    }
    fclose(fptr);
}


void createUser(struct User user){
    FILE *fptr ;
   
    fptr = fopen(FILENAME , "a");

    if(fptr == NULL){
        printf("error occur in file opening\n");
        return ;
    }
    writeUser(fptr , &user);

    fclose(fptr);
    printf("User added successfully\n");
    }


void displayAll(){
    FILE *fptr;
    struct User user ;
    fptr = fopen(FILENAME , "r");

    if(fptr == NULL){
        printf("error occur in file opening");
        return ;
    }
    else{

        printf("All Users are :\n");
      
        while(readUser(fptr , &user)){
            printf("ID : %d\n" , user.id);
            printf("Name : %s\n" , user.name);
            printf("Age : %d\n\n" , user.age);
        }  
    }
    fclose(fptr);
}


void updateUser(int id, struct User newData){
    FILE *fptr;
    FILE *temp ;

    int found = 0;
    struct User user ;

    fptr = fopen(FILENAME , "r");
    
    // write mode because if not exists create new one 

    if(fptr == NULL){
        printf("File not exist");
        return ;
    }
    temp = fopen("temp.txt" , "w");

    if(temp == NULL){
        printf("error occur in temp file creation");
        fclose(fptr);
        return ;
    }

    while(readUser(fptr , &user)){
        if(user.id == id){
            user = newData;
            found = 1;
        }

        writeUser(temp , &user);

    }
    fclose(fptr);
    fclose(temp);

    if(!replaceFile()){
        printf("Error replacing file.\n");
        return;
    }

    if(found == 1){
        printf("\nUser record updated successfully.\n");
    }
    else{
        printf("\nUser Id not found");
    }
}

void deleteUser(int id){
    FILE *fptr ;
    FILE *temp ;

    struct  User user;
    int found = 0;

    fptr = fopen(FILENAME , "r");
    if(fptr == NULL){
        printf("file not found\n");
        return;
    }

    temp = fopen("temp.txt" , "w");
    if(temp == NULL){
        printf("error occuring in file creation");
        fclose(fptr);
        return ;
    }

    while(readUser(fptr , &user)){
        if(user.id == id ){
            found = 1;
            continue;
        }
        writeUser(temp , &user);
    }

    fclose(fptr);
    fclose(temp);

    if(!replaceFile()){
        printf("Error replacing file.\n");
        return;
    }

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
        {
            struct User user;

            printf("Enter user ID :");
            scanf("%d", &user.id);

            if(idExists(user.id)){
                printf("User ID already exists.\n");
                break;
            }

            printf("Enter user Name :");
            scanf(" %99[^\n]", user.name);
            clearInputBuffer();

            printf("Enter user age :");
            scanf("%d", &user.age);

            createUser(user);

            break;
        }
        case 2:
            displayAll();
            break;
        
        case 3:
        {
            int id;
            struct User newData;

            printf("\nEnter user ID for updation : ");
            scanf("%d", &id);

            if(!idExists(id)){
                printf("User ID does not exist. No updation performed.\n");
                break;
            }

            newData.id = id;

            printf("Enter new Name : ");
            scanf(" %99[^\n]", newData.name);
            clearInputBuffer();

            printf("Enter new age : ");
            scanf("%d", &newData.age);

            updateUser(id, newData);

            break;
        }

        case 4:
        {
            int id;

            printf("\nEnter user ID which have to be deleted : ");
            scanf("%d", &id);

            deleteUser(id);

            break;
        }

        case 5:
            printf("Programme is ended. Thank you !\n");
            return 0 ;
            

        default:
            printf("Invlaid choice entered\n");
            break;
        }
    }
    return 0 ;
    
}