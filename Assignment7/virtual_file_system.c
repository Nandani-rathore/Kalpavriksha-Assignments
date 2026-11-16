#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define BLOCK_SIZE 512
#define NUM_BLOCKS 1024
#define MAX_NAME 50


unsigned char** virtualDisk = NULL;
struct FreeBlock *freeHead = NULL;
struct FreeBlock *freeTail = NULL;

struct FileNode *root = NULL;
struct FileNode *cwd = NULL;

int usedBlocks = 0;


struct FreeBlock{
    int index;
    // struct FreeBlock *prev;
    struct FreeBlock *next;
};


struct FileNode{
    char name[MAX_NAME + 1];
    int isDirectory;    // 1 -> directory , 0 -> file
    int fileSize;  // stores actual size of file content
    int blockCount;

    int *blockPointer;

    struct FileNode *parent;
    struct FileNode *children;
    struct FileNode *next;

};

int initializeVirtualDisk(){
    virtualDisk = (unsigned char**)malloc(NUM_BLOCKS * sizeof(unsigned char*));
    if(virtualDisk == NULL){
        printf("Memory allocation failed.\n");
        return 0;
    }

    for(int  i = 0; i < NUM_BLOCKS ; i++){
        virtualDisk[i] = (unsigned char *)calloc(BLOCK_SIZE , sizeof(unsigned char));

        if(virtualDisk[i] == NULL){
            printf("Memory allocation failed for block %d.\n",i);
            for(int j = 0 ; j < i ; j++){
                free(virtualDisk[j]);
            }
            
            free(virtualDisk);
            virtualDisk = NULL ;

            return 0;
        }
    }
    printf("Virtual disk initilized sucessfully.\n");
    return 1;
}


// track that which blocks are free and which are allocated

int initializeFreeBlockList(){

    for(int i = 0; i < NUM_BLOCKS; i++){

        struct FreeBlock *newNode =
            (struct FreeBlock *)malloc(sizeof(struct FreeBlock));

        if(newNode == NULL){
            printf("Memory allocation failed for free block.\n");

            struct FreeBlock *temp = freeHead;

            while(temp != NULL){
                struct FreeBlock *nextNode = temp->next;
                free(temp);
                temp = nextNode;
            }

            freeHead = NULL;
            freeTail = NULL;

            return 0;
        }

        newNode->index = i;
        newNode->next = NULL;

        if(freeHead == NULL){
            freeHead = newNode;
            freeTail = newNode;
        }
        else{
            freeTail->next = newNode;
            freeTail = newNode;
        }
    }

    printf("Free Block list is initialized successfully.\n");
    return 1;
}


int initializeRootDirectory(){
    root = (struct FileNode *)calloc(1 , sizeof(struct FileNode));

    if(root == NULL){
        printf("Memory allocation is failed for root directory.\n");
        return 0;
    }

    strcpy(root->name , "/");
    root->isDirectory = 1;
    root->fileSize = 0;
    root->blockCount = 0;
    root->blockPointer = NULL;
    root->parent = NULL;
    root->children = NULL;
    root->next = NULL;

    cwd = root;
    
    printf("root directory initialized sucessfully.\n");
    return 1;
}


// combine all 3 functions into one fuction

int initializeFileSystem(){
    if(!initializeVirtualDisk()){
        return 0;
    }

    if(!initializeFreeBlockList()){
        return 0;
    }

    if(!initializeRootDirectory()){
        return 0;
    }

    printf("\nVirtual file system initialized sucessfully.\n");
    return 1;
}


void addChild(struct FileNode *parent , struct FileNode* newNode){

    if(parent->children == NULL){
        parent->children = newNode;
        newNode->next = newNode;
    }
    // parent directory already have a children
    else{
        struct FileNode *temp = parent->children;

        while(temp->next != parent->children){
            temp = temp->next;
        }

        temp->next = newNode;
        newNode->next = parent->children;
    }

    newNode->parent = parent;
}


//  create directory 

struct FileNode* createNode(char name[], int isDirectory) {

    struct FileNode *newNode =
        (struct FileNode *)calloc(1, sizeof(struct FileNode));

    if (newNode == NULL) {
        printf("Memory allocation failed.\n");
        return NULL;
    }

    strcpy(newNode->name, name);

    newNode->isDirectory = isDirectory;
    newNode->fileSize = 0;
    newNode->blockCount = 0;
    newNode->blockPointer = NULL;

    newNode->parent = NULL;
    newNode->children = NULL;
    newNode->next = NULL;

    return newNode;
}


// mkdir docs -> new directory (documents) is created
int createDirectory(char name[]){

    if(cwd == NULL || cwd->isDirectory == 0){
        printf("Invalid current directory.\n");
        return 0;
    }

    // empty name if
    if(strlen(name) == 0){
        printf("Directory name cannot be empty.\n");
        return 0;
    }

    // check name length
    if(strlen(name) > MAX_NAME){
        printf("Directory name is very long.\n");
        return 0;
    }

    // director name cant contain '/'
    if(strchr(name , '/') != NULL){
        printf("Invalid Directory name.\n");
        return 0;
    }

    // special names are not allowed 
    if(strcmp(name , ".") == 0 || strcmp(name , "..") == 0){
        printf("Invalid directory name.\n");
        return 0;
    }

    // check duplicate name 

    if(cwd->children != NULL){
        struct FileNode *temp = cwd->children;

        do{
            if(strcmp(temp->name , name) == 0){
                printf("A file or directory is exist already.\n");
                return 0;
            }
            temp = temp->next;
        }while(temp != cwd->children);
    }


    // if everyhing is fine create a new directory node
    struct FileNode* newNode = createNode(name , 1);

    if(newNode == NULL){
        return 0;
    }

    // add children
    addChild(cwd , newNode);

    printf("directory %s is created sucessfully.\n" , name);
    return 1;
}


// when user type create abc.txt -> file name only not content yet
int createFile(char name[])
{
    // check cwd
    if(cwd == NULL || cwd->isDirectory == 0){
        printf("Invalid current directory.\n");
        return 0;
    }

    // Check empty name
    if (strlen(name) == 0) {
        printf("File name cannot be empty.\n");
        return 0;
    }

    // Check maximum name length
    if (strlen(name) > MAX_NAME) {
        printf("File name is too long.\n");
        return 0;
    }

    // File name cannot contain '/'
    if (strchr(name, '/') != NULL) {
        printf("Invalid file name.\n");
        return 0;
    }

    // Special names are not allowed
    if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) {
        printf("Invalid file name.\n");
        return 0;
    }

    // Check duplicate name
    if (cwd->children != NULL) {

        struct FileNode *temp = cwd->children;

        do {
            if (strcmp(temp->name, name) == 0) {
                printf("A file or directory with this name already exists.\n");
                return 0;
            }

            temp = temp->next;

        } while (temp != cwd->children);
    }


    // Create new file node
    struct FileNode *newNode = createNode(name, 0);

    if (newNode == NULL) {
        return 0;
    }

    // Add file to current directory
    addChild(cwd, newNode);

    printf("File '%s' created successfully.\n", name);

    return 1;

}


// if data is write 
// block comes from head head = head->next

int allocateBlock(){
    // check for free block
    if(freeHead == NULL){
        printf("Disk is full. No free block is available.\n");
        return -1;
    }

    struct FreeBlock *temp = freeHead;

    int blockIndex = temp->index;

    freeHead = freeHead->next;

    if(freeHead == NULL){
        freeTail = NULL;
    }

    free(temp);
    usedBlocks++;

    //clear the allocated disk block
    memset(virtualDisk[blockIndex] , 0 , BLOCK_SIZE);
    return blockIndex;
}


void freeBlock(int blockIndex){
    // validate block index
    if(blockIndex < 0 || blockIndex >= NUM_BLOCKS){
        printf("Invalid block index.\n");
        return;
    }

    struct FreeBlock *temp = freeHead;

    while(temp != NULL){
        if(temp->index == blockIndex){
            printf("Block is already free.\n");
            return;
        }

        temp = temp->next;
    }

    struct FreeBlock *newNode = (struct FreeBlock*)malloc(sizeof(struct FreeBlock));

    if(newNode == NULL){
        printf("memory allocation failed during freeing block.\n");
        return;
    }

    newNode->index = blockIndex;
    newNode->next = NULL;

    //clear disk block
    memset(virtualDisk[blockIndex] , 0 , BLOCK_SIZE);

    // if free list is empty

    if(freeHead == NULL){
        freeHead = newNode;
        freeTail = newNode;
    }else{

        freeTail->next = newNode;
        freeTail = newNode;
    }

    if(usedBlocks > 0){
        usedBlocks--;
    }

}


// write command then 
// jis file mai data write karna h , data
int writeFile(struct FileNode *file , char content[]){

    if(file == NULL || file->isDirectory == 1){
        printf("Invalid file.\n");
        return 0;
    }

    int length = strlen(content);

    int requiredBlocks = (length + BLOCK_SIZE - 1) / BLOCK_SIZE;

    int *newBlockPointer = NULL;

    if(requiredBlocks > 0){
        newBlockPointer = (int *)malloc(requiredBlocks * sizeof(int));

        if(newBlockPointer == NULL){
            printf("Memory allocation failed.\n");
            return 0;
        }
    }

    int availableBlocks = file->blockCount;

    struct FreeBlock *temp = freeHead;

    while (temp != NULL) {
        availableBlocks++;
        temp = temp->next;
    }

    if (requiredBlocks > availableBlocks) {
        printf("Not enough disk space.\n");
        free(newBlockPointer);
        return 0;
    }

    int reusedBlocks = requiredBlocks;

    if (reusedBlocks > file->blockCount) {
        reusedBlocks = file->blockCount;
    }

    // Reuse existing blocks first.
    for (int i = 0; i < reusedBlocks; i++) {
        newBlockPointer[i] = file->blockPointer[i];
    }

    // Allocate additional blocks if required.
    for (int i = reusedBlocks; i < requiredBlocks; i++) {

        int blockIndex = allocateBlock();

        if (blockIndex == -1) {
            // Return newly allocated blocks to the free list.
            for (int j = reusedBlocks; j < i; j++) {
                freeBlock(newBlockPointer[j]);
            }

            free(newBlockPointer);
            return 0;
        }

        newBlockPointer[i] = blockIndex;
    }

    // Copy content into the allocated blocks.
    int copiedBytes = 0;

    for (int i = 0; i < requiredBlocks; i++) {

        int bytesToCopy = length - copiedBytes;

        if (bytesToCopy > BLOCK_SIZE) {
            bytesToCopy = BLOCK_SIZE;
        }

        memset(virtualDisk[newBlockPointer[i]], 0, BLOCK_SIZE);

        memcpy(
            virtualDisk[newBlockPointer[i]],
            content + copiedBytes,
            bytesToCopy
        );

        copiedBytes += bytesToCopy;
    }

    // Free extra old blocks if the new content is smaller.
    for (int i = requiredBlocks; i < file->blockCount; i++) {
        freeBlock(file->blockPointer[i]);
    }

    free(file->blockPointer);

    file->blockPointer = newBlockPointer;
    file->blockCount = requiredBlocks;
    file->fileSize = length;

    printf("File written successfully.\n");

    return 1;
}


struct FileNode* findChild(struct FileNode *parent, char name[]) {

    if (parent == NULL || parent->children == NULL) {
        return NULL;
    }

    struct FileNode *temp = parent->children;

    do {
        if (strcmp(temp->name, name) == 0) {
            return temp;
        }

        temp = temp->next;

    } while (temp != parent->children);

    return NULL;
}


int readFile(struct FileNode *file) {

    if (file == NULL || file->isDirectory == 1) {
        printf("Invalid file.\n");
        return 0;
    }

    if (file->fileSize == 0) {
        printf("File is empty.\n");
        return 1;
    }

    int remainingBytes = file->fileSize;

    for (int i = 0; i < file->blockCount; i++) {

        int bytesToRead = remainingBytes;

        if (bytesToRead > BLOCK_SIZE) {
            bytesToRead = BLOCK_SIZE;
        }

        fwrite(virtualDisk[file->blockPointer[i]],
               1,
               bytesToRead,
               stdout);

        remainingBytes -= bytesToRead;
    }

    printf("\n");

    return 1;
}



int deleteFile(char name[]) {

    if (cwd == NULL || cwd->children == NULL) {
        printf("File not found.\n");
        return 0;
    }

    struct FileNode *temp = cwd->children;
    struct FileNode *prev = NULL;

    do {

        if (strcmp(temp->name, name) == 0) {

            if (temp->isDirectory == 1) {
                printf("This is a directory. Use rmdir.\n");
                return 0;
            }

            if (prev == NULL) {

                struct FileNode *last = cwd->children;

                while (last->next != cwd->children) {
                    last = last->next;
                }

                if (last == cwd->children) {
                    cwd->children = NULL;
                } else {
                    cwd->children = temp->next;
                    last->next = cwd->children;
                }

            } else {
                prev->next = temp->next;
            }

            for (int i = 0; i < temp->blockCount; i++) {
                freeBlock(temp->blockPointer[i]);
            }

            free(temp->blockPointer);
            free(temp);

            printf("File deleted successfully.\n");
            return 1;
        }

        prev = temp;
        temp = temp->next;

    } while (temp != cwd->children);

    printf("File not found.\n");
    return 0;
}



int removeDirectory(char name[]) {

    if (cwd == NULL || cwd->children == NULL) {
        printf("Directory not found.\n");
        return 0;
    }

    struct FileNode *temp = cwd->children;
    struct FileNode *prev = NULL;

    do {

        if (strcmp(temp->name, name) == 0) {

            if (temp->isDirectory == 0) {
                printf("This is a file. Use delete.\n");
                return 0;
            }

            if (temp->children != NULL) {
                printf("Directory is not empty.\n");
                return 0;
            }

            if (prev == NULL) {

                struct FileNode *last = cwd->children;

                while (last->next != cwd->children) {
                    last = last->next;
                }

                if (last == cwd->children) {
                    cwd->children = NULL;
                } else {
                    cwd->children = temp->next;
                    last->next = cwd->children;
                }

            } else {
                prev->next = temp->next;
            }

            free(temp);

            printf("Directory removed successfully.\n");
            return 1;
        }

        prev = temp;
        temp = temp->next;

    } while (temp != cwd->children);

    printf("Directory not found.\n");
    return 0;
}

void listDirectory() {

    if (cwd == NULL) {
        printf("Current directory not found.\n");
        return;
    }

    if (cwd->children == NULL) {
        printf("Directory is empty.\n");
        return;
    }

    struct FileNode *temp = cwd->children;

    printf("\nContents of %s:\n", cwd->name);

    do {

        if (temp->isDirectory == 1) {
            printf("[DIR]  %s\n", temp->name);
        } else {
            printf("[FILE] %s (%d bytes)\n",
                   temp->name, temp->fileSize);
        }

        temp = temp->next;

    } while (temp != cwd->children);
}


int changeDirectory(char name[]) {

    if(cwd == NULL){
        printf("Current directory is invalid.\n");
        return 0;
    }

    if (strcmp(name, "/") == 0) {
        cwd = root;
        return 1;
    }

    if (strcmp(name, "..") == 0) {

        if (cwd->parent != NULL) {
            cwd = cwd->parent;
        }

        return 1;
    }

    struct FileNode *target = findChild(cwd, name);

    if (target == NULL) {
        printf("Directory not found.\n");
        return 0;
    }

    if (target->isDirectory == 0) {
        printf("Cannot enter a file.\n");
        return 0;
    }

    cwd = target;

    return 1;
}


void printWorkingDirectory() {

    if (cwd == root) {
        printf("/\n");
        return;
    }

    struct FileNode *path[100];
    int count = 0;

    struct FileNode *temp = cwd;

    while (temp != NULL && temp != root) {

        if (count == 100) {
            printf("Path is too deep.\n");
            return;
        }

        path[count] = temp;
        count++;

        temp = temp->parent;
    }

    printf("/");

    for (int i = count - 1; i >= 0; i--) {

        printf("%s", path[i]->name);

        if (i != 0) {
            printf("/");
        }
    }

    printf("\n");
}




void showDiskUsage() {

    int freeBlocks = 0;

    struct FreeBlock *temp = freeHead;

    while (temp != NULL) {
        freeBlocks++;
        temp = temp->next;
    }

    printf("\nVirtual Disk Information\n");

    printf("Total Blocks : %d\n", NUM_BLOCKS);
    printf("Used Blocks  : %d\n", usedBlocks);
    printf("Free Blocks  : %d\n", freeBlocks);

    printf("Block Size   : %d bytes\n", BLOCK_SIZE);

    printf("Total Space  : %d bytes\n",
           NUM_BLOCKS * BLOCK_SIZE);

    printf("Used Space   : %d bytes\n",
           usedBlocks * BLOCK_SIZE);

    printf("Free Space   : %d bytes\n",
           freeBlocks * BLOCK_SIZE);
}


void freeDirectoryChildren(struct FileNode *directory) {

    if (directory == NULL || directory->children == NULL) {
        return;
    }

    struct FileNode *first = directory->children;
    struct FileNode *temp = first;

    do {

        struct FileNode *nextNode = temp->next;

        if (temp->isDirectory == 1) {
            freeDirectoryChildren(temp);
        }

        free(temp->blockPointer);
        free(temp);

        temp = nextNode;

    } while (temp != first);

    directory->children = NULL;
}

void cleanupFileSystem() {

    // Free all files and directories.
    if (root != NULL) {
        freeDirectoryChildren(root);
        free(root);
        root = NULL;
        cwd = NULL;
    }

    // Free free-block linked list.
    struct FreeBlock *temp = freeHead;

    while (temp != NULL) {

        struct FreeBlock *nextNode = temp->next;

        free(temp);

        temp = nextNode;
    }

    freeHead = NULL;
    freeTail = NULL;

    // Free virtual disk blocks.
    if (virtualDisk != NULL) {

        for (int i = 0; i < NUM_BLOCKS; i++) {
            free(virtualDisk[i]);
        }

        free(virtualDisk);
        virtualDisk = NULL;
    }

    usedBlocks = 0;

    printf("Virtual File System memory cleaned successfully.\n");
}




int processCommand(char input[]) {

    char *cursor = input;

    // Skip leading spaces.
    while (*cursor == ' ' || *cursor == '\t') {
        cursor++;
    }

    if (*cursor == '\0') {
        return 1;
    }

    // Extract command.
    char *command = cursor;

    while (*cursor != '\0' &&
           *cursor != ' ' &&
           *cursor != '\t') {
        cursor++;
    }

    if (*cursor != '\0') {
        *cursor = '\0';
        cursor++;
    }

    // Skip spaces before arguments.
    while (*cursor == ' ' || *cursor == '\t') {
        cursor++;
    }

    // Exit command.
    if (strcmp(command, "exit") == 0) {
        return 0;
    }

    // Commands without arguments.
    if (strcmp(command, "ls") == 0) {
        listDirectory();
        return 1;
    }

    if (strcmp(command, "pwd") == 0) {
        printWorkingDirectory();
        return 1;
    }

    if (strcmp(command, "df") == 0) {
        showDiskUsage();
        return 1;
    }

    // Remaining commands require an argument.
    if (*cursor == '\0') {
        printf("Please provide an argument.\n");
        return 1;
    }

    char *name = cursor;

    // Extract filename/directory name.
    while (*cursor != '\0' &&
           *cursor != ' ' &&
           *cursor != '\t') {
        cursor++;
    }

    char *content = NULL;

    if (*cursor != '\0') {
        *cursor = '\0';
        cursor++;

        while (*cursor == ' ' || *cursor == '\t') {
            cursor++;
        }

        content = cursor;
    }

    // mkdir command.
    if (strcmp(command, "mkdir") == 0) {
        createDirectory(name);
    }

    // create command.
    else if (strcmp(command, "create") == 0) {
        createFile(name);
    }

    // write command.
    else if (strcmp(command, "write") == 0) {

        if (content == NULL) {
            printf("Please provide file content.\n");
        } else {

            struct FileNode *file = findChild(cwd, name);

            if (file == NULL) {
                printf("File not found.\n");
            } else {
                writeFile(file, content);
            }
        }
    }

    // read command.
    else if (strcmp(command, "read") == 0) {

        struct FileNode *file = findChild(cwd, name);

        if (file == NULL) {
            printf("File not found.\n");
        } else {
            readFile(file);
        }
    }

    // delete command.
    else if (strcmp(command, "delete") == 0) {
        deleteFile(name);
    }

    // rmdir command.
    else if (strcmp(command, "rmdir") == 0) {
        removeDirectory(name);
    }

    // cd command.
    else if (strcmp(command, "cd") == 0) {
        changeDirectory(name);
    }

    else {
        printf("Invalid command.\n");
    }

    return 1;
}


int main(){

    if(!initializeFileSystem()){
        printf("File System initialization failed.\n");
        cleanupFileSystem();
        return 1;
    }

    char input[4096];

    int running = 1;

    while(running){

        printf("\nVFS> ");

        if(fgets(input, sizeof(input), stdin) == NULL){

            if(feof(stdin)){
                break;
            }

            printf("Input reading error. Please try again.\n");
            clearerr(stdin);
            continue;
        }

        int length = strlen(input);

        // Check whether complete input was read
        if(length > 0 && input[length - 1] != '\n' && !feof(stdin)){

            int ch;

            // Discard remaining characters of this input line
            while((ch = getchar()) != '\n' && ch != EOF);

            printf("Command is too long. Please enter a shorter command.\n");

            continue;
        }

        // Remove newline character
        input[strcspn(input, "\n")] = '\0';

        // Process user command
        running = processCommand(input);
    }

    cleanupFileSystem();

    printf("Virtual File System closed.\n");

    return 0;
}