#include<stdio.h>
#include<stdlib.h>
#include<string.h>
// stdlib is the library in which malloc() , calloc() and free() functions are declared

#define NAME_SIZE 100
#define MIN_TICKET 1
#define MAX_TICKET 20
#define STANDARD_PRICE 150
#define PREMIUM_PRICE 250 
#define RECLINER_PRICE 400

struct ticket{
    char *name ; 
    int *ticket_count ;
    int *category ;
};

// function decleration
struct ticket *createBooking(void);

int inputCustomerName(struct ticket *t1);
int inputNoOfTicket(struct ticket *t1);
int inputCategory(struct ticket *t1);

int ticketPrice(struct ticket *t1);
int processBooking(struct ticket *t1);

int calculateSubtotal(struct ticket *t1);
float calculateDiscount(struct ticket *t1, float subtotal);
float calculateFinalBill(float subtotal, float discount);

const char *ticketCategoryName(struct ticket *t1);
void printBill(struct ticket *t1, float subtotal,float discount, float bill);
void freeBooking(struct ticket *t1);

//  function defination 

struct ticket *createBooking(void){
    struct ticket *t1 = calloc(1,sizeof(struct ticket));

    if(t1 == NULL){
        return NULL;
    }

    t1->name = calloc(NAME_SIZE , sizeof(char));
    t1->ticket_count = malloc(sizeof(int)) ;
    t1->category = malloc(sizeof(int));


    if(t1->name == NULL || t1->ticket_count == NULL || t1->category == NULL){
        free(t1->name);
        free(t1->ticket_count);
        free(t1->category);
        free(t1);

        return NULL;
    }
    return t1;
}


int inputCustomerName(struct ticket *t1) {
    char line[200];
    char extra;

    printf("Enter customer name: ");

    if (fgets(line, sizeof(line), stdin) == NULL) {
        printf("Invalid customer name.\n");
        return 0;
    }

    if (sscanf(line, "%99s %c", t1->name, &extra) != 1) {
        printf("Please enter a single-word name only.\n");
        return 0;
    }

    return 1;
}

int inputNoOfTicket(struct ticket *t1){
    printf("Enter no. of tickets:");
    
    if(scanf("%d" , t1->ticket_count) != 1){
        printf("Invalid ticket count.\n");
        return 0;
    }

    if(*t1->ticket_count < MIN_TICKET || *t1->ticket_count > MAX_TICKET ){
        printf("Ticket count must be between 1 and 20.\n");
        return 0;
    }

    return 1;
}

int inputCategory(struct ticket *t1){
    printf("Enter ticket Categories:\n");
    printf("1.Standard Rs %d\n" , STANDARD_PRICE);
    printf("2.Premium Rs %d\n" ,PREMIUM_PRICE);
    printf("3.Recliner Rs %d\n" , RECLINER_PRICE);

    printf("Enter Category choice:");
    if (scanf("%d", t1->category) != 1) {
        printf("Invalid category input!\n");
        return 0;
    }

    if (ticketPrice(t1) == 0) {
        printf("Please choose category 1, 2, or 3.\n");
        return 0;
    }
    return 1;
}


int processBooking(struct ticket *t1) {
    if (!inputCustomerName(t1)) {
        return 0;
    }

    if (!inputNoOfTicket(t1)) {
        return 0;
    }

    if (!inputCategory(t1)) {
        return 0;
    }

    return 1;
}

int ticketPrice(struct ticket *t1){
    switch (*t1->category){
        case 1 :
            return STANDARD_PRICE;
        case 2 :
            return PREMIUM_PRICE;
        case 3 :
            return RECLINER_PRICE;
        default :
            return 0;
    }
}


int calculateSubtotal(struct ticket *t1){
    int price = ticketPrice(t1);
    int no_of_tickets = *t1->ticket_count;
    return price * no_of_tickets;
}


float calculateDiscount(struct ticket *t1 , float subtotal){
    if(*t1->ticket_count >= 5){
        return ( subtotal * 10 ) / 100 ;
    }
    else{
        return 0;
    }
}


float calculateFinalBill(float subtotal ,float discount){
    return subtotal - discount ; 
}

const char *ticketCategoryName(struct ticket *t1) {
    switch (*t1->category) {
        case 1:
            return "Standard";

        case 2:
            return "Premium";

        case 3:
            return "Recliner";

        default:
            return "Invalid Category";
    }
}

void printBill(struct ticket *t1 , float subtotal , float discount , float bill ){
    printf("\n ------- Total Bill -------\n");
    printf("Customer :%s\n" , t1->name);
    printf("Ticket Category :%s\n" ,ticketCategoryName(t1));
    printf("Tickets :%d\n" , *t1->ticket_count);
    printf("Subtotal :%.2f\n" , subtotal);
    printf("Discount :%.2f\n" , discount);
    printf("Final Bill :%.2f\n" , bill);
} 


void freeBooking(struct ticket *t1) {
    if (t1 == NULL) {
        return;
    }

    free(t1->name);
    free(t1->ticket_count);
    free(t1->category);
    free(t1);
}

int main(){
    printf("----------- Movie Ticket Booking System -----------\n");
    struct  ticket *t1 = createBooking();

    if (t1 == NULL) {
        printf("Memory allocation failed.\n");
        return 1;
    }

    if(!processBooking(t1)){
        freeBooking(t1);
        return 1;
    }

    float subtotal = calculateSubtotal(t1);
    float discount = calculateDiscount(t1 , subtotal);
    float finalbill = calculateFinalBill(subtotal , discount);

    printBill(t1,subtotal,discount,finalbill);

    freeBooking(t1);
    return 0 ;

}