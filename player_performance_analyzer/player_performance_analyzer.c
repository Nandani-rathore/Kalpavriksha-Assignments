
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <ctype.h>
#include <math.h>

#include "Players_data.h"

#define MAX_TEAMS 10
#define MIN_PLAYER_IN_TEAM 11
#define MAX_PLAYER_IN_TEAM 50
#define MAX_NAME_LENGTH 50
#define MAX_TOTAL_PLAYER 500

typedef struct Player {
    int playerId;
    char name[MAX_NAME_LENGTH + 1];
    char teamName[MAX_NAME_LENGTH + 1];
    char role[20];

    int totalRuns;
    float battingAverage;
    float strikeRate;
    int wickets;
    float economyRate;

    float performanceIndex;
} PlayerRecord;

typedef struct Team {
    int teamId;
    char name[MAX_NAME_LENGTH + 1];

    int totalPlayers;
    int battingPlayerCount;
    float averageBattingStrikeRate;
} teamRecord;

typedef struct {
    int teamIndex;
    int playerIndex;
} HeapNode;


PlayerRecord playerRecords[MAX_TOTAL_PLAYER];
teamRecord teamRecords[MAX_TEAMS];

int currPlayerCount = 0;
int currentTeamCount = 0;


void displayPlayerHeader(int showTeam)
{
    printf("\n");

    printf("%-6s %-22s ", "ID", "Player Name");

    if (showTeam == 1)
    {
        printf("%-18s ", "Team");
    }

    printf("%-13s %-10s %-10s %-10s %-9s %-10s %-10s\n",
           "Role", "Runs", "Average", "StrikeRate",
           "Wickets", "Economy", "Perf.Index");

    printf("====================================================================================================================\n");
}


void displayPlayerRow(PlayerRecord p, int showTeam)
{
    printf("%-6d %-22s ", p.playerId, p.name);

    if (showTeam == 1)
    {
        printf("%-18s ", p.teamName);
    }

    printf("%-13s %-10d %-10.2f %-10.2f %-9d %-10.2f %-10.2f\n",
           p.role,
           p.totalRuns,
           p.battingAverage,
           p.strikeRate,
           p.wickets,
           p.economyRate,
           p.performanceIndex);
}


// input functions

void readLine(char input[], int size, const char prompt[]) {
    int length;
    int ch;

    while (1) {
        printf("%s", prompt);

        if (fgets(input, size, stdin) == NULL) {
            printf("\nInput ended. Exiting program.\n");
            exit(EXIT_SUCCESS);
        }

        length = strlen(input);

        if (length > 0 && input[length - 1] == '\n') {
            input[length - 1] = '\0';
            return;
        }

        if (feof(stdin)) {
            return;
        }

        while ((ch = getchar()) != '\n' && ch != EOF) {
        }

        printf("Input is too long. Try again.\n");
    }
}


int readIntegerInput() {
    char input[100];
    char *endPointer;
    long value;

    while (1) {
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("\nInput ended. Exiting program.\n");
            exit(EXIT_SUCCESS);
        }

        errno = 0;
        value = strtol(input, &endPointer, 10);

        if (endPointer == input || errno == ERANGE) {
            printf("Invalid input! Enter an integer: ");
            continue;
        }

        while (isspace((unsigned char)*endPointer)) {
            endPointer++;
        }

        if (*endPointer != '\0') {
            printf("Extra characters are not allowed. Try again: ");
            continue;
        }

        if (value < INT_MIN || value > INT_MAX) {
            printf("Number out of range. Try again: ");
            continue;
        }

        return (int)value;
    }
}


float readFloatInput() {
    char input[100];
    char *endPointer;
    float value;

    while (1) {
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("\nInput ended. Exiting program.\n");
            exit(EXIT_SUCCESS);
        }

        errno = 0;
        value = strtof(input, &endPointer);

        if (endPointer == input || errno == ERANGE || !isfinite(value)) {
            printf("Invalid number! Try again: ");
            continue;
        }

        while (isspace((unsigned char)*endPointer)) {
            endPointer++;
        }

        if (*endPointer != '\0') {
            printf("Extra characters are not allowed. Try again: ");
            continue;
        }

        if (value < 0) {
            printf("Negative values are not allowed. Try again: ");
            continue;
        }

        return value;
    }
}

// performance calculation

float calculatePerformanceIndex(PlayerRecord p) {
    float index = 0;

    if (strcmp(p.role, "Batsman") == 0) {
        index = (p.battingAverage * p.strikeRate) / 100;
    }
    else if (strcmp(p.role, "Bowler") == 0) {
        index = (p.wickets * 2) + (100 - p.economyRate);
    }
    else if (strcmp(p.role, "All-rounder") == 0) {
        index = ((p.battingAverage * p.strikeRate) / 100)
                + (p.wickets * 2);
    }

    return index;
}



// initilize structure data
int initializeData() {
    int i, j;

    float strikeRateSum[MAX_TEAMS] = {0};

    if (teamCount < 1 || teamCount > MAX_TEAMS) {
        printf("Invalid number of teams in dataset!\n");
        return 0;
    }

    if (playerCount < 0 || playerCount > MAX_TOTAL_PLAYER) {
        printf("Invalid number of players in dataset!\n");
        return 0;
    }

    currentTeamCount = teamCount;
    currPlayerCount = 0;

    for (i = 0; i < currentTeamCount; i++) {
        teamRecords[i].teamId = i + 1;

        strcpy(teamRecords[i].name, teams[i]);

        teamRecords[i].totalPlayers = 0;
        teamRecords[i].battingPlayerCount = 0;
        teamRecords[i].averageBattingStrikeRate = 0;
    }

    for (i = 0; i < playerCount; i++) {

        int found = 0;
        int teamIndex = -1;

        for (j = 0; j < currentTeamCount; j++) {
            if (strcmp(players[i].team, teamRecords[j].name) == 0) {
                teamIndex = j;
                found = 1;
                break;
            }
        }

        if (!found) {
            printf("Unknown team for player ID %d!\n", players[i].id);
            return 0;
        }

        if (teamRecords[teamIndex].totalPlayers >= MAX_PLAYER_IN_TEAM) {
            printf("Maximum team capacity exceeded!\n");
            return 0;
        }

        playerRecords[i].playerId = players[i].id;

        strcpy(playerRecords[i].name, players[i].name);
        strcpy(playerRecords[i].teamName, players[i].team);
        strcpy(playerRecords[i].role, players[i].role);

        playerRecords[i].totalRuns = players[i].totalRuns;
        playerRecords[i].battingAverage = players[i].battingAverage;
        playerRecords[i].strikeRate = players[i].strikeRate;
        playerRecords[i].wickets = players[i].wickets;
        playerRecords[i].economyRate = players[i].economyRate;

        playerRecords[i].performanceIndex =
            calculatePerformanceIndex(playerRecords[i]);

        teamRecords[teamIndex].totalPlayers++;

        if (strcmp(playerRecords[i].role, "Batsman") == 0 ||
            strcmp(playerRecords[i].role, "All-rounder") == 0) {

            strikeRateSum[teamIndex] += playerRecords[i].strikeRate;
            teamRecords[teamIndex].battingPlayerCount++;
        }

        currPlayerCount++;
    }

    for (i = 0; i < currentTeamCount; i++) {

        if (teamRecords[i].battingPlayerCount > 0) {

            teamRecords[i].averageBattingStrikeRate =
                strikeRateSum[i] / teamRecords[i].battingPlayerCount;
        }
    }

    return 1;
}


//  menu

void displayMenu() {

    printf("\n==============================================================\n");
    printf("1. Add Player to Team\n");
    printf("2. Display Players of a Specific Team\n");
    printf("3. Display Teams by Average Batting Strike Rate\n");
    printf("4. Display Top K Players of a Specific Team by Role\n");
    printf("5. Display all Players of specific role Across All Teams by performance index\n");
    printf("6. Exit\n");
    printf("==============================================================\n");

    printf("Enter your choice: ");
}



// search team by id
int findTeamById(int teamId) {

    int left = 0;
    int right = currentTeamCount - 1;

    while (left <= right) {

        int mid = left + (right - left) / 2;

        if (teamRecords[mid].teamId == teamId) {
            return mid;
        }
        else if (teamRecords[mid].teamId < teamId) {
            left = mid + 1;
        }
        else {
            right = mid - 1;
        }
    }

    return -1;
}


int getAvailableTeamIndex() {

    int teamId;
    int teamIndex;

    while (1) {

        printf("\nEnter Team ID (1-%d): ", currentTeamCount);
        teamId = readIntegerInput();

        teamIndex = findTeamById(teamId);

        if (teamIndex == -1) {
            printf("Invalid Team ID! Try again.\n");
            continue;
        }

        if (teamRecords[teamIndex].totalPlayers >= MAX_PLAYER_IN_TEAM) {
            printf("This team already has maximum 50 players.\n");
            continue;
        }

        printf("Selected Team: %s\n", teamRecords[teamIndex].name);

        return teamIndex;
    }
}


int getUniquePlayerId() {

    int playerId;
    int i;
    int duplicate;

    while (1) {

        printf("\nEnter Player ID (1-1000): ");
        playerId = readIntegerInput();

        if (playerId < 1 || playerId > 1000) {
            printf("Invalid Player ID! Enter ID between 1 and 1000.\n");
            continue;
        }

        duplicate = 0;

        for (i = 0; i < currPlayerCount; i++) {

            if (playerRecords[i].playerId == playerId) {
                duplicate = 1;
                break;
            }
        }

        if (duplicate) {
            printf("Player ID already exists! Enter another ID.\n");
            continue;
        }

        return playerId;
    }
}


void getPlayerName(char name[]) {

    while (1) {

        readLine(name, MAX_NAME_LENGTH + 2, "\nEnter Player Name: ");

        if (strlen(name) == 0) {
            printf("Name cannot be empty!\n");
            continue;
        }

        if (strlen(name) > MAX_NAME_LENGTH) {
            printf("Name must not exceed 50 characters.\n");
            continue;
        }

        return;
    }
}


void getPlayerRole(char role[]) {

    char input[100];

    while (1) {

        printf("\nSelect Player Role:\n");
        printf("1. Batsman\n");
        printf("2. Bowler\n");
        printf("3. All-rounder\n");

        readLine(input, sizeof(input), "Enter your choice: ");

        if (strcmp(input, "1") == 0 ||
            strcmp(input, "Batsman") == 0 ||
            strcmp(input, "batsman") == 0) {

            strcpy(role, "Batsman");
            return;
        }

        if (strcmp(input, "2") == 0 ||
            strcmp(input, "Bowler") == 0 ||
            strcmp(input, "bowler") == 0) {

            strcpy(role, "Bowler");
            return;
        }

        if (strcmp(input, "3") == 0 ||
            strcmp(input, "All-rounder") == 0 ||
            strcmp(input, "all-rounder") == 0) {

            strcpy(role, "All-rounder");
            return;
        }

        printf("Invalid role! Please select 1, 2, or 3.\n");
    }
}




void getBattingDetails(PlayerRecord *player) {

    while (1) {

        printf("\nEnter Total Runs: ");
        player->totalRuns = readIntegerInput();

        if (player->totalRuns < 0) {
            printf("Runs cannot be negative!\n");
            continue;
        }

        break;
    }

    printf("Enter Batting Average: ");
    player->battingAverage = readFloatInput();

    printf("Enter Strike Rate: ");
    player->strikeRate = readFloatInput();
}


void getBowlingDetails(PlayerRecord *player) {

    while (1) {

        printf("\nEnter Wickets: ");
        player->wickets = readIntegerInput();

        if (player->wickets < 0) {
            printf("Wickets cannot be negative!\n");
            continue;
        }

        break;
    }

    printf("Enter Economy Rate: ");
    player->economyRate = readFloatInput();
}



// add player
void addPlayerToTeam() {

    PlayerRecord newPlayer;
    int teamIndex;

    if (currPlayerCount >= MAX_TOTAL_PLAYER) {
        printf("\nPlayer storage is full!\n");
        return;
    }

    teamIndex = getAvailableTeamIndex();

    newPlayer.playerId = getUniquePlayerId();

    getPlayerName(newPlayer.name);
    getPlayerRole(newPlayer.role);

    getBattingDetails(&newPlayer);
    getBowlingDetails(&newPlayer);

    strcpy(newPlayer.teamName, teamRecords[teamIndex].name);

    newPlayer.performanceIndex =
        calculatePerformanceIndex(newPlayer);

    playerRecords[currPlayerCount] = newPlayer;

    currPlayerCount++;

    teamRecords[teamIndex].totalPlayers++;

    if (strcmp(newPlayer.role, "Batsman") == 0 ||
        strcmp(newPlayer.role, "All-rounder") == 0) {

        int oldCount = teamRecords[teamIndex].battingPlayerCount;

        float oldSum =
            teamRecords[teamIndex].averageBattingStrikeRate * oldCount;

        teamRecords[teamIndex].battingPlayerCount++;

        teamRecords[teamIndex].averageBattingStrikeRate =
            (oldSum + newPlayer.strikeRate) /
            teamRecords[teamIndex].battingPlayerCount;
    }

    printf("\nPlayer added successfully!\n");
    printf("Player ID: %d\n", newPlayer.playerId);
    printf("Player Name: %s\n", newPlayer.name);
    printf("Team: %s\n", newPlayer.teamName);
    printf("Performance Index: %.2f\n", newPlayer.performanceIndex);
}



// display teams player
void displayTeamPlayers() {

    int teamId;
    int teamIndex;
    int i;

    while (1) {

        printf("\nEnter Team ID: ");
        teamId = readIntegerInput();

        teamIndex = findTeamById(teamId);

        if (teamIndex == -1) {
            printf("Invalid Team ID! Try again.\n");
            continue;
        }

        break;
    }

    printf("\nPlayers of Team %s:\n", teamRecords[teamIndex].name);

    displayPlayerHeader(0);

    for (i = 0; i < currPlayerCount; i++) {

        if (strcmp(playerRecords[i].teamName,
                   teamRecords[teamIndex].name) == 0) {

            displayPlayerRow(playerRecords[i], 0);
        }
    }

    printf("=======================================================================================\n");

    printf("Total Players: %d\n",
           teamRecords[teamIndex].totalPlayers);

    printf("Average Batting Strike Rate: %.2f\n",
           teamRecords[teamIndex].averageBattingStrikeRate);
}


// sort teams
void displaySortedTeams() {

    teamRecord sortedTeams[MAX_TEAMS];
    teamRecord temp;

    int i, j;

    for (i = 0; i < currentTeamCount; i++) {
        sortedTeams[i] = teamRecords[i];
    }

    for (i = 0; i < currentTeamCount - 1; i++) {

        for (j = 0; j < currentTeamCount - i - 1; j++) {

            if (sortedTeams[j].averageBattingStrikeRate <
                sortedTeams[j + 1].averageBattingStrikeRate) {

                temp = sortedTeams[j];
                sortedTeams[j] = sortedTeams[j + 1];
                sortedTeams[j + 1] = temp;
            }
        }
    }

    printf("\nTeams Sorted by Average Batting Strike Rate\n");

    printf("===============================================================\n");
    printf("%-6s %-22s %-15s %-15s\n",
           "ID", "Team Name", "Avg Bat SR", "Total Players");
    printf("===============================================================\n");

    for (i = 0; i < currentTeamCount; i++) {

        printf("%-6d %-22s %-15.2f %-15d\n",
               sortedTeams[i].teamId,
               sortedTeams[i].name,
               sortedTeams[i].averageBattingStrikeRate,
               sortedTeams[i].totalPlayers);
    }

    printf("===============================================================\n");
}



// performance comparision
int comparePerformance(const void *a, const void *b) {

    const PlayerRecord *p1 = (const PlayerRecord *)a;
    const PlayerRecord *p2 = (const PlayerRecord *)b;

    if (p1->performanceIndex < p2->performanceIndex) {
        return 1;
    }

    if (p1->performanceIndex > p2->performanceIndex) {
        return -1;
    }

    return 0;
}



// display the top K players
void displayTopKPlayers() {

    int teamId;
    int teamIndex;
    int k;
    int count = 0;
    int i;

    char role[20];

    PlayerRecord selectedPlayers[MAX_PLAYER_IN_TEAM];

    while (1) {

        printf("\nEnter Team ID: ");
        teamId = readIntegerInput();

        teamIndex = findTeamById(teamId);

        if (teamIndex == -1) {
            printf("Invalid Team ID! Try again.\n");
            continue;
        }

        break;
    }

    getPlayerRole(role);

    while (1) {

        printf("Enter K (Top K Players): ");
        k = readIntegerInput();

        if (k <= 0) {
            printf("K must be greater than 0.\n");
            continue;
        }

        break;
    }

    for (i = 0; i < currPlayerCount; i++) {

        if (strcmp(playerRecords[i].teamName,
                   teamRecords[teamIndex].name) == 0 &&
            strcmp(playerRecords[i].role, role) == 0) {

            selectedPlayers[count] = playerRecords[i];
            count++;
        }
    }

    if (count == 0) {
        printf("No players found for this role!\n");
        return;
    }

    qsort(selectedPlayers, count, sizeof(PlayerRecord),
          comparePerformance);

    if (k > count) {
        k = count;
    }

    printf("\nTop %d %s of Team %s:\n",
           k, role, teamRecords[teamIndex].name);

    displayPlayerHeader(0);

    for (i = 0; i < k; i++) {
        displayPlayerRow(selectedPlayers[i], 0);
    }

    printf("=======================================================================================\n");
}


// heap concept - doubt

void heapPush(HeapNode heap[], int *size, HeapNode node,
              PlayerRecord data[MAX_TEAMS][MAX_PLAYER_IN_TEAM]) {

    int i = (*size)++;

    while (i > 0) {

        int parent = (i - 1) / 2;

        if (data[heap[parent].teamIndex][heap[parent].playerIndex].performanceIndex >=
            data[node.teamIndex][node.playerIndex].performanceIndex) {
            break;
        }

        heap[i] = heap[parent];
        i = parent;
    }

    heap[i] = node;
}


HeapNode heapPop(HeapNode heap[], int *size,
                 PlayerRecord data[MAX_TEAMS][MAX_PLAYER_IN_TEAM]) {

    HeapNode result = heap[0];
    HeapNode last = heap[--(*size)];

    int i = 0;

    while (2 * i + 1 < *size) {

        int child = 2 * i + 1;

        if (child + 1 < *size &&
            data[heap[child + 1].teamIndex][heap[child + 1].playerIndex].performanceIndex >
            data[heap[child].teamIndex][heap[child].playerIndex].performanceIndex) {

            child++;
        }

        if (data[last.teamIndex][last.playerIndex].performanceIndex >=
            data[heap[child].teamIndex][heap[child].playerIndex].performanceIndex) {
            break;
        }

        heap[i] = heap[child];
        i = child;
    }

    heap[i] = last;

    return result;
}



// display all the players
void displayAllPlayersByRole() {

    PlayerRecord data[MAX_TEAMS][MAX_PLAYER_IN_TEAM];

    int counts[MAX_TEAMS] = {0};
    int positions[MAX_TEAMS] = {0};

    HeapNode heap[MAX_TEAMS];

    int heapSize = 0;

    char role[20];

    int i, j;

    getPlayerRole(role);

    /* Collect players team-wise */

    for (i = 0; i < currPlayerCount; i++) {

        for (j = 0; j < currentTeamCount; j++) {

            if (strcmp(playerRecords[i].teamName, teamRecords[j].name) == 0 &&
                strcmp(playerRecords[i].role, role) == 0) {

                data[j][counts[j]++] = playerRecords[i];
                break;
            }
        }
    }

    

    for (i = 0; i < currentTeamCount; i++) {

        qsort(data[i], counts[i], sizeof(PlayerRecord),
              comparePerformance);

        if (counts[i] > 0) {

            HeapNode node;

            node.teamIndex = i;
            node.playerIndex = 0;

            heapPush(heap, &heapSize, node, data);
        }
    }

    if (heapSize == 0) {
        printf("\nNo players found for this role!\n");
        return;
    }

    printf("\n%s of All Teams:\n", role);

    displayPlayerHeader(1);

    /* Merge sorted teams using Max Heap */

    while (heapSize > 0) {

        HeapNode current = heapPop(heap, &heapSize, data);

        PlayerRecord p =
            data[current.teamIndex][current.playerIndex];

        displayPlayerRow(p, 1);

        positions[current.teamIndex]++;

        if (positions[current.teamIndex] < counts[current.teamIndex]) {

            HeapNode next;

            next.teamIndex = current.teamIndex;
            next.playerIndex = positions[current.teamIndex];

            heapPush(heap, &heapSize, next, data);
        }
    }

    printf("===============================================================================================\n");
}



int main() {

    int choice;

    printf("========================================\n");
    printf("     ICC ODI PLAYER PERFORMANCE ANALYZER\n");
    printf("========================================\n");

    if (!initializeData()) {
        printf("Data initialization failed. Program terminated.\n");
        return 1;
    }

    printf("\nData initialized successfully!\n");
    printf("Total Teams: %d\n", currentTeamCount);
    printf("Total Players: %d\n", currPlayerCount);

    while (1) {

        displayMenu();

        choice = readIntegerInput();

        printf("\nYour choice: %d\n", choice);

        switch (choice) {

            case 1:
                addPlayerToTeam();
                break;

            case 2:
                displayTeamPlayers();
                break;

            case 3:
                displaySortedTeams();
                break;

            case 4:
                displayTopKPlayers();
                break;

            case 5:
                displayAllPlayersByRole();
                break;

            case 6:
                printf("\nExiting Program. Thank You!\n");
                return 0;

            default:
                printf("\nInvalid choice! Please enter a number between 1 and 6.\n");
        }
    }

    return 0;
}
