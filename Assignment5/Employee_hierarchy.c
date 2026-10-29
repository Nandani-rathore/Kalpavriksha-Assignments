
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>

struct Employee {
    int Employee_ID;
    char name[100];
    char designation[100];
    struct Employee *manager;
    struct Employee **reports;
    int reportCount;
};


// input function

int readInteger(const char *prompt, int *value)
{
    char line[100], *end;
    printf("%s", prompt);

    if (fgets(line, sizeof(line), stdin) == NULL)
        return -1;

    if (strchr(line, '\n') == NULL && !feof(stdin)) {
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF) {}
        printf("Input too long.\n");
        return 0;
    }

    errno = 0;
    long n = strtol(line, &end, 10);

    while (isspace((unsigned char)*end))
        end++;

    if (end == line || *end != '\0' || errno == ERANGE ||
        n < INT_MIN || n > INT_MAX) {
        printf("Invalid integer.\n");
        return 0;
    }

    *value = (int)n;
    return 1;
}


int readText(const char *prompt, char *text, size_t size)
{
    while (1) {
        printf("%s", prompt);

        if (fgets(text, size, stdin) == NULL)
            return 0;

        if (strchr(text, '\n') == NULL && !feof(stdin)) {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF) {}
            printf("Input too long. Try again.\n");
            continue;
        }

        text[strcspn(text, "\n")] = '\0';

        int valid = 0;
        for (int i = 0; text[i] != '\0'; i++) {
            if (!isspace((unsigned char)text[i])) {
                valid = 1;
                break;
            }
        }

        if (valid)
            return 1;

        printf("Input cannot be empty.\n");
    }
}


// search function

int findEmployeeIndex(struct Employee **employees, int count, int id)
{
    for (int i = 0; i < count; i++)
        if (employees[i]->Employee_ID == id)
            return i;

    return -1;
}


struct Employee *findEmployee(struct Employee **employees, int count, int id)
{
    int index = findEmployeeIndex(employees, count, id);
    return index == -1 ? NULL : employees[index];
}


// display and count function
void displayEmployeeHierarchy(struct Employee *e, int level)
{
    if (e == NULL)
        return;

    for (int i = 0; i < level; i++)
        printf("    ");

    printf("ID: %d | Name: %s | Designation: %s\n",
           e->Employee_ID, e->name, e->designation);

    for (int i = 0; i < e->reportCount; i++)
        displayEmployeeHierarchy(e->reports[i], level + 1);
}


int countSubordinates(struct Employee *e)
{
    if (e == NULL)
        return 0;

    int total = 0;

    for (int i = 0; i < e->reportCount; i++)
        total += 1 + countSubordinates(e->reports[i]);

    return total;
}


void displayCompleteHierarchy(struct Employee **employees, int count)
{
    printf("\n--- Complete Employee Hierarchy ---\n");

    if (count == 0) {
        printf("No employees found.\nTotal employees: 0\n");
        return;
    }

    for (int i = 0; i < count; i++)
        if (employees[i]->manager == NULL)
            displayEmployeeHierarchy(employees[i], 0);

    printf("\nTotal employees: %d\n", count);
}


void displaySingleHierarchy(struct Employee **employees, int count)
{
    int id;
    int result = readInteger("Enter employee ID: ", &id);

    if (result != 1)
        return;

    struct Employee *e = findEmployee(employees, count, id);

    if (e == NULL) {
        printf("Employee not found.\n");
        return;
    }

    displayEmployeeHierarchy(e, 0);
    printf("Total employees under %s: %d\n",
           e->name, countSubordinates(e));
}


// add employee

int addEmployee(struct Employee ***employees, int *count)
{
    int id, result;

    while (1) {
        result = readInteger("Enter employee ID: ", &id);

        if (result == -1)
            return 0;
        if (result == 0)
            continue;
        if (id <= 0) {
            printf("ID must be positive.\n");
            continue;
        }
        if (findEmployee(*employees, *count, id) != NULL) {
            printf("Employee ID already exists.\n");
            continue;
        }
        break;
    }

    struct Employee *e = calloc(1, sizeof(struct Employee));

    if (e == NULL) {
        printf("Memory allocation failed.\n");
        return 0;
    }

    e->Employee_ID = id;

    if (!readText("Enter name: ", e->name, sizeof(e->name)) ||
        !readText("Enter designation: ",
                  e->designation, sizeof(e->designation))) {
        free(e);
        return 0;
    }

    struct Employee *manager = NULL;

    if (*count == 0) {
        printf("This employee will be the CEO.\n");
    } else {
        while (1) {
            int managerId;
            result = readInteger("Enter manager ID: ", &managerId);

            if (result == -1) {
                free(e);
                return 0;
            }
            if (result == 0)
                continue;
            if (managerId == 0) {
                printf("Every non-CEO employee needs a manager.\n");
                continue;
            }

            manager = findEmployee(*employees, *count, managerId);

            if (manager != NULL)
                break;

            printf("Manager not found. Try again.\n");
        }
    }

    // Resize the main employee array
    struct Employee **temp = realloc(
        *employees, (*count + 1) * sizeof(struct Employee *)
    );

    if (temp == NULL) {
        free(e);
        printf("Memory allocation failed.\n");
        return 0;
    }

    *employees = temp;

    // Add employee to the manager's reports
    if (manager != NULL) {
        struct Employee **reports = realloc(
            manager->reports,
            (manager->reportCount + 1) * sizeof(struct Employee *)
        );

        if (reports == NULL) {
            free(e);
            printf("Memory allocation failed.\n");
            return 0;
        }

        manager->reports = reports;
        manager->reports[manager->reportCount++] = e;
        e->manager = manager;
    }

    (*employees)[*count] = e;
    (*count)++;

    printf("Employee added successfully.\n");
    return 1;
}


// remove employee

int removeEmployee(struct Employee ***employees, int *count, int id)
{
    int index = findEmployeeIndex(*employees, *count, id);

    if (index == -1) {
        printf("Employee not found.\n");
        return 0;
    }

    struct Employee **list = *employees;
    struct Employee *target = list[index];
    struct Employee *manager = target->manager;

    if (manager == NULL) {
        printf("CEO cannot be removed.\n");
        return 0;
    }

    int children = target->reportCount;
    struct Employee *destination = NULL;
    int siblings = 0;

    // Select the sibling with the fewest direct reports
    for (int i = 0; i < manager->reportCount; i++) {
        struct Employee *s = manager->reports[i];

        if (s == target)
            continue;

        siblings++;

        if (destination == NULL ||
            s->reportCount < destination->reportCount)
            destination = s;
    }

    // No sibling: transfer reports to the superior
    if (destination == NULL)
        destination = manager;

    int managerCount = manager->reportCount - 1;

    if (destination == manager)
        managerCount += children;

    // Allocate replacement arrays before changing relationships
    struct Employee **newManagerReports = NULL;
    struct Employee **newDestinationReports = NULL;

    if (managerCount > 0) {
        newManagerReports = malloc(
            managerCount * sizeof(struct Employee *)
        );

        if (newManagerReports == NULL) {
            printf("Memory allocation failed. Employee not removed.\n");
            return 0;
        }
    }

    if (destination != manager && children > 0) {
        newDestinationReports = malloc(
            (destination->reportCount + children) *
            sizeof(struct Employee *)
        );

        if (newDestinationReports == NULL) {
            free(newManagerReports);
            printf("Memory allocation failed. Employee not removed.\n");
            return 0;
        }
    }

    // Copy manager's reports
    int k = 0;

    for (int i = 0; i < manager->reportCount; i++)
        if (manager->reports[i] != target)
            newManagerReports[k++] = manager->reports[i];

    // No siblings: add the removed employee's children to the manager
    if (destination == manager)
        for (int i = 0; i < children; i++)
            newManagerReports[k++] = target->reports[i];

    // Sibling exists: copy their reports and append transferred children
    if (destination != manager && children > 0) {
        k = 0;

        for (int i = 0; i < destination->reportCount; i++)
            newDestinationReports[k++] = destination->reports[i];

        for (int i = 0; i < children; i++)
            newDestinationReports[k++] = target->reports[i];
    }

    free(manager->reports);
    manager->reports = newManagerReports;
    manager->reportCount = managerCount;

    if (destination != manager && children > 0) {
        free(destination->reports);
        destination->reports = newDestinationReports;
        destination->reportCount += children;
    }

    // Update transferred employees' manager pointers
    for (int i = 0; i < children; i++)
        target->reports[i]->manager = destination;

    free(target->reports);
    free(target);

    // Remove target from the main array
    for (int i = index; i < *count - 1; i++)
        list[i] = list[i + 1];

    (*count)--;

    if (*count == 0) {
        free(*employees);
        *employees = NULL;
    }

    printf("Employee removed successfully.\n");

    if (children > 0) {
        if (siblings > 0)
            printf("Reports transferred to sibling with fewest direct reports.\n");
        else
            printf("No siblings. Reports transferred to the superior.\n");
    }

    return 1;
}


// free the memory 
void freeAllEmployees(struct Employee **employees, int count)
{
    for (int i = 0; i < count; i++) {
        free(employees[i]->reports);
        free(employees[i]);
    }

    free(employees);
}

// menu driven 
void menuDriven(void)
{
    struct Employee **employees = NULL;
    int count = 0, choice;

    while (1) {
        printf("\n----- Employee Hierarchy System -----\n");
        printf("1. Add employee\n");
        printf("2. Remove employee\n");
        printf("3. Display one employee's hierarchy\n");
        printf("4. Display complete hierarchy\n");
        printf("5. Exit\n");

        int result = readInteger("Enter choice: ", &choice);

        if (result == -1)
            break;
        if (result == 0)
            continue;

        switch (choice) {
            case 1:
                addEmployee(&employees, &count);
                break;

            case 2: {
                int id;
                if (readInteger("Enter employee ID to remove: ", &id) == 1)
                    removeEmployee(&employees, &count, id);
                break;
            }

            case 3:
                displaySingleHierarchy(employees, count);
                break;

            case 4:
                displayCompleteHierarchy(employees, count);
                break;

            case 5:
                freeAllEmployees(employees, count);
                printf("Memory released. Goodbye!\n");
                return;

            default:
                printf("Invalid choice. Enter 1 to 5.\n");
        }
    }

    freeAllEmployees(employees, count);
    printf("Program ended. Memory released.\n");
}


int main(void)
{
    menuDriven();
    return 0;
}
