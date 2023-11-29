//
//  main.c
//  Warship Rivals
//
//  Created by Егор Тыщук on 24.11.23..
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define SAVE_FILE ".save.txt"
#define GRID_SIZE 10

#define EMPTY 0
#define PLAYER_SHIP 1
#define PC_SHIP 2

#define GREEN_TEXT "\x1b[32m"
#define RED_TEXT "\x1b[31m"
#define BLUE_TEXT "\x1b[34m"
#define WHITE_TEXT "\x1b[37m"
#define RESET_TEXT "\x1b[0m"

void initializeGrid(int grid[GRID_SIZE][GRID_SIZE]) {
    for (int i = 0; i < GRID_SIZE; ++i)
        for (int j = 0; j < GRID_SIZE; ++j)
            grid[i][j] = EMPTY;
}

void printGrids(int player[GRID_SIZE][GRID_SIZE], int pc[GRID_SIZE][GRID_SIZE], int reveal) {
    printf(WHITE_TEXT "   a b c d e f g h i j\t\t   a b c d e f g h i j\n");

    for (int i = 0; i < GRID_SIZE; ++i) {
        // Print player's grid
        printf(WHITE_TEXT "%2d ", i + 1);
        for (int j = 0; j < GRID_SIZE; ++j) {
            switch (player[i][j]) {
                case EMPTY:
                    printf(reveal ? ". " : "  ");
                    break;
                case -1:
                    printf(RED_TEXT "O " RESET_TEXT); // Miss
                    break;
                case -2:
                    printf(BLUE_TEXT "X " RESET_TEXT); //Hit
                    break;
                default:
                    printf(GREEN_TEXT "%d " RESET_TEXT, player[i][j]); // Green for player
            }
        }

        // Separate player's and PC's grids with a tab
        printf("\t\t");
        printf(WHITE_TEXT "%2d ", i + 1);

        // Print PC's grid
        for (int j = 0; j < GRID_SIZE; ++j) {
            switch (pc[i][j]) {
                case EMPTY:
                    printf(reveal ? ". " : "  ");
                    break;
                case -1:
                    printf(RED_TEXT "O " RESET_TEXT); // Miss
                    break;
                case -2:
                    printf(BLUE_TEXT "X " RESET_TEXT); //Hit
                    break;
                default:
                    printf(reveal ? ". " : "  ");
            }
        }

        printf("\n");
    }
}



int isCollision(int grid[GRID_SIZE][GRID_SIZE], int row, int col, int length, int orientation) {
    for (int i = 0; i < length; ++i)
        if ((orientation == 0 && (col + i >= GRID_SIZE || grid[row][col + i] != EMPTY)) || (orientation == 1 && (row + i >= GRID_SIZE || grid[row + i][col] != EMPTY)))
            return 1;
    return 0;
}

void placeShip(int grid[GRID_SIZE][GRID_SIZE], int length) {
    int orientation, row, col;
    int collision;

    do {
        collision = 0;
        orientation = rand() % 2;
        row = rand() % GRID_SIZE;
        col = rand() % GRID_SIZE;
        collision = isCollision(grid, row, col, length, orientation);
    } while (collision);


    for (int i = 0; i < length; ++i) {
        if (orientation == 0) {
            grid[row][col + i] = length; // Use positive values for both player and PC
        } else {
            grid[row + i][col] = length; // Use positive values for both player and PC
        }
    }
}

int makeTurn(int grid[GRID_SIZE][GRID_SIZE], int row, int col) {
    if (grid[row][col] == EMPTY)
        return (grid[row][col] = -1), 0;
    if (grid[row][col] > 0) {
        grid[row][col] = -2;
        return 1;
    } else {
        return 2;
    }
}

void printDebugGrids(int playerGrid[GRID_SIZE][GRID_SIZE], int pcGrid[GRID_SIZE][GRID_SIZE]) {
    printf("Player's Grid:\n");
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            printf("%d ", playerGrid[i][j]);
        }
        printf("\n");
    }

    printf("\nComputer's Grid:\n");
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            printf("%d ", pcGrid[i][j]);
        }
        printf("\n");
    }
}

// Function to generate a random move for the PC
void generatePCMove(int* row, int* col) {
    *row = rand() % GRID_SIZE;
    *col = rand() % GRID_SIZE;
}

void clearConsole(void) {
#ifdef _WIN32
    system("cls"); // For Windows
#else
    system("clear"); // For Unix-like systems
#endif
}

void displayLogo(void) {
    // Fancy logo
    printf(GREEN_TEXT " __      __                     .__    .__         __________.__              .__\n");
    printf("/  \\    /  \\_____ _______  _____|  |__ |__|_____   \\______   \\__|__  _______  |  |   ______\n");
    printf("\\   \\/\\/   /\\__  \\\\_  __ \\/  ___/  |  \\|  \\____ \\   |       _/  \\  \\/ /\\__  \\ |  |  /  ___/\n");
    printf(" \\        /  / __ \\|  | \\/\\___ \\|   Y  \\  |  |_> >  |    |   \\  |\\   /  / __ \\|  |__\\___ \\\n");
    printf("  \\__/\\  /  (____  /__|  /____  >___|  /__|   __/   |____|_  /__| \\_/  (____  /____/____  >\n");
    printf("       \\/        \\/           \\/     \\/   |__|             \\/               \\/          \\/\n\n\n" RESET_TEXT);
}

void saveGame(int playerGrid[GRID_SIZE][GRID_SIZE], int pcGrid[GRID_SIZE][GRID_SIZE]) {
    FILE *file = fopen(SAVE_FILE, "a");
    if (file == NULL) {
        printf("Error opening the save file for writing.\n");
        exit(EXIT_FAILURE);
    }

    // Get the current timestamp
    time_t rawtime;
    struct tm *timeinfo;
    char timestamp[20];
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(timestamp, sizeof(timestamp), "%Y%m%d_%H%M%S", timeinfo);

    // Write the timestamp
    fprintf(file, "Save %s\n", timestamp);

    // Write the player's grid to the text file
    fprintf(file, "Player's Grid:\n");
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            fprintf(file, "%d ", playerGrid[i][j]);
        }
        fprintf(file, "\n");
    }

    // Write the PC's grid to the text file
    fprintf(file, "PC's Grid:\n");
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            fprintf(file, "%d ", pcGrid[i][j]);
        }
        fprintf(file, "\n");
    }

    fclose(file);
}

// Helper function to load a grid from the file
void loadGridFromFile(FILE *file, int grid[GRID_SIZE][GRID_SIZE]) {
    char line[256];

    for (int i = 0; i < GRID_SIZE; i++) {
        if (fgets(line, sizeof(line), file) != NULL) {
            // Check if the line contains grid data
            if (strstr(line, "Player's Grid:") != NULL || strstr(line, "PC's Grid:") != NULL) {
                i--; // Retry reading this line
                continue;
            }

            char *token = strtok(line, " ");
            for (int j = 0; j < GRID_SIZE; j++) {
                // Check if token is not NULL
                if (token != NULL) {
                    grid[i][j] = atoi(token);
                    token = strtok(NULL, " ");
                } else {
                    fprintf(stderr, "Error reading from file. Unexpected format.\n");
                    exit(EXIT_FAILURE);
                }
            }
        } else {
            fprintf(stderr, "Error reading from file.\n");
            exit(EXIT_FAILURE);
        }
    }
}

void loadGame(int playerGrid[GRID_SIZE][GRID_SIZE], int pcGrid[GRID_SIZE][GRID_SIZE]) {
    FILE *file = fopen(SAVE_FILE, "r");
    if (file == NULL) {
        printf("No save file found.\n");
        return;
    }

    char line[256];
    int saveCount = 0;
    int index = 0;

    printf("Available Saves:\n");

    // Display available saves with indices
    while (fgets(line, sizeof(line), file) != NULL) {
        if (strstr(line, "Save ") != NULL) {
            printf("%d. %s", saveCount, line);
            saveCount++;
        }
    }

    if (saveCount == 0) {
        printf("No saves found.\n");
        fclose(file);
        return;
    }

    // Ask user to choose a save to load
    printf("\nChoose a save to load (enter the index): ");
    scanf("%d", &index);

    if (index < 0 || index >= saveCount) {
        printf("Invalid index.\n");
        fclose(file);
        return;
    }

    // Reset file pointer to the beginning
    fseek(file, 0, SEEK_SET);

    // Find the chosen save in the file
    saveCount = 0;

    while (fgets(line, sizeof(line), file) != NULL) {
        if (strstr(line, "Save ") != NULL) {
            if (saveCount == index) {
                // Load the corresponding grids
                loadGridFromFile(file, playerGrid);
                loadGridFromFile(file, pcGrid);
                break;
            }
            saveCount++;
        }
    }

    if (feof(file)) {
        printf("Error reading the save file.\n");
    } else {
        printf("Game loaded successfully.\n");
    }

    fclose(file);
    printGrids(playerGrid, pcGrid, 1);
}

// Function to check if any ships remaining on the grid
int checkShipsRemaining(int grid[GRID_SIZE][GRID_SIZE]) {
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            if (grid[i][j] > 0) {
                return 1; // Ships remaining
            }
        }
    }
    return 0; // No ships remaining
}

int gameLoop(int playerGrid[GRID_SIZE][GRID_SIZE], int pcGrid[GRID_SIZE][GRID_SIZE], int save) {
    srand(time(NULL)); // Initialize random seed
    if (!save) {
        // Place ships for the player
        placeShip(playerGrid, 4); // 1 ship of length 4
        placeShip(playerGrid, 3); // 1st ship of length 3
        placeShip(playerGrid, 3); // 2nd ship of length 3
        placeShip(playerGrid, 2); // 1st ship of length 2
        placeShip(playerGrid, 2); // 2nd ship of length 2
        placeShip(playerGrid, 2); // 3rd ship of length 2
        placeShip(playerGrid, 1); // 1st ship of length 1
        placeShip(playerGrid, 1); // 2nd ship of length 1
        placeShip(playerGrid, 1); // 3rd ship of length 1
        placeShip(playerGrid, 1); // 4th ship of length 1
        
        
        // Place ships for the PC
        placeShip(pcGrid, 4); // 1 ship of length 4
        placeShip(pcGrid, 3); // 1st ship of length 3
        placeShip(pcGrid, 3); // 2nd ship of length 3
        placeShip(pcGrid, 2); // 1st ship of length 2
        placeShip(pcGrid, 2); // 2nd ship of length 2
        placeShip(pcGrid, 2); // 3rd ship of length 2
        placeShip(pcGrid, 1); // 1st ship of length 1
        placeShip(pcGrid, 1); // 2nd ship of length 1
        placeShip(pcGrid, 1); // 3rd ship of length 1
        placeShip(pcGrid, 1); // 4th ship of length 1
    }

    int turns = 0;
    int maxTurns = 30; // Maximum number of turns to prevent an infinite loop

    while (1) {

        // Make the player's turn
        int result;
        do {
            clearConsole();
            printGrids(playerGrid, pcGrid, 1); // Reveal the ships for the player
            // Take input for the player's turn
            char colChar;
            int row, col;

            printf("Make a turn (e.g., a5), or enter '0' to stop and save: ");
            scanf(" %c", &colChar);
            // Check if the user wants to stop the game
            if (colChar == '0') {
                printf("Stopping the game and saving results.\n");
                saveGame(playerGrid, pcGrid);
                return 0;
            }

            // Read the second part of the input
            scanf("%d", &row);

            // Check if the user wants to stop the game
            if (colChar == '0' || row == 0) {
                printf("Stopping the game and saving results.\n");
                saveGame(playerGrid, pcGrid);
                return 0;  // Exit the loop
            }

            // Convert column character to index
            col = colChar - 'a';

            // Validate input
            if (row < 1 || row > GRID_SIZE || col < 0 || col >= GRID_SIZE) {
                printf("Invalid input. Please enter a valid turn.\n");
                continue;
            }

            result = makeTurn(pcGrid, row - 1, col);

            // Print the result of the player's turn
            if (result == 0) {
                printf("Miss!\n");
            } else if (result == 1) {
                printf("Hit!\n");
                turns++;
            } else {
                printf("You already tried to hit this cell, try again\n");
                result = 1;
                continue; // Continue to the next iteration of the loop without incrementing turns
            }

            // Check for game over conditions
            if (turns >= maxTurns) {
                printf("Game over. Maximum number of turns reached.\n");
                break;
            }

        } while (result == 1); // Continue as long as the player keeps hitting the PC's ships




        // Check if all PC ships are sunk
        int pcShipsRemaining = checkShipsRemaining(pcGrid);
        int playerShipsRemaining = checkShipsRemaining(playerGrid);

        if (!pcShipsRemaining) {
            printf("Congratulations! You sank all the PC's ships!\n");
            break;
        } else if (!playerShipsRemaining) {
            printf("Game over. PC sank all your ships.\n");
            break;
        }

        // PC makes a random move
        int pcRow, pcCol;
        int sleepDuration = rand() % 1000 + 1000; // Declare sleep time between 1000 and 2000 milliseconds
        
        int pcResult; // Declare pcResult outside the loop
        
        usleep(sleepDuration * 1000);

        do {
            printf("PC makes a move.. \n");

            // Generate PC's move
            generatePCMove(&pcRow, &pcCol);

            // Process PC's move
            pcResult = makeTurn(playerGrid, pcRow, pcCol);

            // Print the result of the PC's move
            if (pcResult == 0) {
                printf("PC missed!\n");
            } else if (pcResult > 0) {
                printf("PC hit your ship!\n");
                // Optionally, you can increment turns here if needed
            } else {
                pcResult = 1;
                continue;
            }

            // Check for game over conditions
            if (turns >= maxTurns) {
                printf("Game over. Maximum number of turns reached.\n");
                break;
            }
            
            usleep(sleepDuration * 1000); // Sleep for the generated duration
            clearConsole();
            printGrids(playerGrid, pcGrid, 1); // Reveal the ships for the player
            
        } while (pcResult == 1); // Continue as long as the PC keeps hitting the player's ships

        clearConsole(); // Clear console
    }
    return 0;
}

int main(void) {
    clearConsole(); // Clear console
    displayLogo(); // Display logo
    
    int playerGrid[GRID_SIZE][GRID_SIZE];
    int pcGrid[GRID_SIZE][GRID_SIZE];
    
    initializeGrid(playerGrid);
    initializeGrid(pcGrid);

    printf("Choose option:\n");
    printf("1. Start New Game\n");
    printf("2. Load Save\n");
    printf("3. Quit\n");

    int option;
    scanf("%d", &option);

    while (option < 1 || option > 3) {
        clearConsole();
        displayLogo();
        printf("Invalid option. Please enter a valid option:\n");
        printf("1. Start New Game\n");
        printf("2. Load Save\n");
        printf("3. Quit\n");

        scanf("%d", &option);
    }

    switch (option) {
        case 1:
            gameLoop(playerGrid, pcGrid, 0);
            break;
        case 2:
            loadGame(playerGrid, pcGrid);
            gameLoop(playerGrid, pcGrid, 1);
            break;
        case 3:
            return 0;
        default:
            break;
    }

    return 0;
}
