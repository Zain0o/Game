#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Forward declarations
typedef struct room room;
typedef struct item item;

// Structs
typedef struct {
    room *room1;
    room *room2;
} connection;

struct item {
    char *name;
    char *description;
    bool collectible;
};

struct room {
    char *name;
    char *description;
    connection *connections;
    int numConnections;
    item *items;
    int numItems;
};

typedef struct {
    room *currentRoom;
    item *inventory;
    int numInventoryItems;
} player;

typedef struct {
    room *rooms;
    int numRooms;
    player *player;
} game;

// Function prototypes
void createConnection(room *room1, room *room2);
room *loadRooms();
player *createPlayer(room *currentRoom);
game *createGame();
void printRoomDescription(room *room);
int getMenuChoice();
int getRoomChoice(room *currentRoom);
void movePlayer(player *player, int choice);
void pickUpItem(player *p, room *r, int itemIndex);
void dropItem(player *p, room *r, int itemIndex);
void playGame();
void loadItems(room *rooms, int numRooms); // Add this line


// Functions implementations (omitted for brevity, use your existing functions)

void loadConnections(room *rooms, int numRooms)
{
    FILE *file = fopen("connections.csv", "r");
    if (file == NULL)
    {
        printf("Error opening file\n");
        exit(1);
    }
    int roomIndex1, roomIndex2, roomIndex3;
    while (fscanf(file, "%d,%d,%d", &roomIndex1, &roomIndex2, &roomIndex3) == 3)
    {
        createConnection(&rooms[roomIndex1], &rooms[roomIndex2]);
        createConnection(&rooms[roomIndex1], &rooms[roomIndex3]);
        createConnection(&rooms[roomIndex2], &rooms[roomIndex3]);
    }
    fclose(file);
}

void createConnection(room *room1, room *room2)
{
    // Check if the connection already exists in room1
    // Check if the connection already exists in room1
    for (int i = 0; i < room1->numConnections; i++)
    {
        if (room1->connections[i].room2 == room2)
        {
            printf("Connection between %s and %s already exists.\n", room1->name, room2->name);
            return;
        }
    }

    // If we reach this point, it means the connection does not exist and we need to create it
    connection newConnection;
    newConnection.room1 = room1;
    newConnection.room2 = room2;

    // Add to room1's connections
    room1->connections = realloc(room1->connections, sizeof(connection) * (room1->numConnections + 1));
    room1->connections[room1->numConnections] = newConnection;
    room1->numConnections++;

    // Add to room2's connections only if room1 and room2 are not the same
    if (room1 != room2)
    {
        room2->connections = realloc(room2->connections, sizeof(connection) * (room2->numConnections + 1));
        room2->connections[room2->numConnections] = newConnection;
        room2->numConnections++;
    }
}

// a function to load the rooms from a file
// the file is called rooms.csv, and has a room name and room description on each line
// the function returns an array of rooms
room *loadRooms()
{
    // open the file
    FILE *file = fopen("rooms.csv", "r");

    // check that the file opened successfully
    if (file == NULL)
    {
        printf("Error opening file\n");
        exit(1);
    }

    // count the number of lines in the file
    int numLines = 0;
    char line[500];
    while (fgets(line, 500, file) != NULL)
    {
        numLines++;
    }

    // rewind the file
    rewind(file);

    // allocate memory for the rooms
    room *rooms = malloc(sizeof(room) * numLines);
    if (rooms == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }

    // read the rooms from the file
    for (int i = 0; i < numLines; i++)
    {
        fgets(line, 500, file);
        line[strlen(line) - 1] = '\0'; // remove newline character

        char *name = strtok(line, "\"");
        char *endofname = strtok(NULL, "\"");

        char *description = strtok(NULL, "\"");
        char *endofdesc = strtok(NULL, "\0");

        name[endofname - name] = '\0';

        rooms[i].name = malloc(sizeof(char) * strlen(name) + 1);
        strcpy(rooms[i].name, name);

        rooms[i].description = malloc(sizeof(char) * strlen(description) + 1);
        strcpy(rooms[i].description, description);

        rooms[i].connections = NULL;
        rooms[i].numConnections = 0;
        rooms[i].items = NULL; // Initialize items as NULL
        rooms[i].numItems = 0;  // Initialize numItems as 0
    }

    fclose(file);

    // Load connections
    loadConnections(rooms, numLines);

    // Load items
    loadItems(rooms, numLines); // Load items after initializing rooms

    return rooms;
}


void loadItems(room *rooms, int numRooms) {
    printf("Loading items...\n");
    FILE *file = fopen("items.csv", "r");
    if (file == NULL) {
        perror("Error opening items file");
        exit(1);
    }

    char line[1024];
    int lineNum = 0;
    while (fgets(line, sizeof(line), file)) {
        lineNum++;
        // Remove potential newline character at the end of the line
        line[strcspn(line, "\r\n")] = 0;
        printf("Processing line %d: %s\n", lineNum, line);

        int roomIndex;
        char itemName[100], itemDesc[256];
        int collectible;

        if (sscanf(line, "%d, \"%[^\"]\", \"%[^\"]\", %d", &roomIndex, itemName, itemDesc, &collectible) != 4) {
            printf("Warning: Line %d is not properly formatted.\n", lineNum);
            continue;
        }

        if (roomIndex < 0 || roomIndex >= numRooms) {
            printf("Warning: Invalid room index %d on line %d.\n", roomIndex, lineNum);
            continue;
        }

        bool isDuplicate = false;
        for (int i = 0; i < rooms[roomIndex].numItems; i++) {
            if (strcmp(rooms[roomIndex].items[i].name, itemName) == 0) {
                isDuplicate = true;
                printf("Duplicate item '%s' found in room %d, skipping.\n", itemName, roomIndex);
                break;
            }
        }

        if (!isDuplicate) {
            rooms[roomIndex].items = realloc(rooms[roomIndex].items, sizeof(item) * (rooms[roomIndex].numItems + 1));
            if (rooms[roomIndex].items == NULL) {
                perror("Failed to allocate memory for new item");
                fclose(file);
                exit(1);
            }

            char *itemNameCopy = strdup(itemName);
            char *itemDescCopy = strdup(itemDesc);
            if (!itemNameCopy || !itemDescCopy) {
                perror("Failed to allocate memory for item name/description");
                free(itemNameCopy); // Free if one allocation succeeded
                free(itemDescCopy); // Free if one allocation succeeded
                fclose(file);
                exit(1);
            }

            rooms[roomIndex].items[rooms[roomIndex].numItems].name = itemNameCopy;
            rooms[roomIndex].items[rooms[roomIndex].numItems].description = itemDescCopy;
            rooms[roomIndex].items[rooms[roomIndex].numItems].collectible = collectible;
            rooms[roomIndex].numItems++;
            printf("Item '%s' added to room %d.\n", itemName, roomIndex);
        }
    }

    fclose(file);
    printf("Item loading complete.\n");
}






// let's have a function to create a player
player *createPlayer(room *currentRoom)
{
    // we need to allocate memory for the player
    player *player = malloc(sizeof(player));
    // we need to set the current room
    player->currentRoom = currentRoom;
    player->inventory = NULL; // Initialize inventory as empty
    player->numInventoryItems = 0; // No items initially
    // we need to return the player
    return player;
}

// let's have a function to create a game
game *createGame()
{
    // we need to allocate memory for the game
    game *game = malloc(sizeof(game));
    // we need to load the rooms
    printf("debug - about to load rooms\n");
    game->rooms = loadRooms();
    printf("debug - rooms loaded\n");
    // we need to set the number of rooms
    game->numRooms = 10;

    // Load connections after the rooms are loaded and before the player is created
    // loadConnections(game->rooms, game->numRooms);

    // we need to create the player
    game->player = createPlayer(&game->rooms[0]);
    // we need to return the game
    return game;
}

// let's have a function to print the room description
void printRoomDescription(room *room)
{
    printf("You are in the %s.\n", room->name);
    printf("%s\n", room->description);
}

// let's have a function to print a menu and get user choice, return the choice
int getMenuChoice()
{
    int choice;
    printf("What would you like to do?\n");
    printf("1. Look around\n");
    printf("2. Move to another room\n");
    printf("3. Manage inventory\n");
    printf("4. Quit\n"); // New option for inventory
    scanf("%d", &choice);
    // we need to check that the choice is valid
    while (choice < 1 || choice > 4)
    {
        printf("Invalid choice, please try again\n");
        scanf("%d", &choice);
    }
    return choice;
}

// a function to get user choice of room to move to
int getRoomChoice(room *currentRoom)
{
    int choice;
    printf("Which room would you like to move to?\n");
    for (int i = 0; i < currentRoom->numConnections; i++)
    {
        printf("%d. %s\n", i + 1, currentRoom->connections[i].room2->name);
    }
    scanf("%d", &choice);
    // we need to check that the choice is valid
    while (choice < 1 || choice > currentRoom->numConnections)
    {
        printf("Invalid choice, please try again\n");
        scanf("%d", &choice);
    }
    return choice;
}

// a function to move the player to another room, and describe it to the user
void movePlayer(player *player, int choice)
{
    player->currentRoom = player->currentRoom->connections[choice - 1].room2;
    printRoomDescription(player->currentRoom);
}



void pickUpItem(player *p, room *r, int itemIndex) {
    if (itemIndex < 0 || itemIndex >= r->numItems) {
        printf("Invalid item index.\n");
        return;
    }

    // Resize player's inventory to add a new item
    p->inventory = realloc(p->inventory, sizeof(item) * (p->numInventoryItems + 1));
    if (p->inventory == NULL) {
        printf("Failed to allocate memory for inventory.\n");
        exit(1);
    }

    // Add item to player's inventory
    p->inventory[p->numInventoryItems] = r->items[itemIndex];
    p->numInventoryItems++;

    // Shift remaining items in room's inventory
    for (int i = itemIndex; i < r->numItems - 1; i++) {
        r->items[i] = r->items[i + 1];
    }

    // Resize room's inventory to remove the picked item
    r->items = realloc(r->items, sizeof(item) * (r->numItems - 1));
    r->numItems--;

    printf("Picked up: %s\n", p->inventory[p->numInventoryItems - 1].name);
}


void dropItem(player *p, room *r, int itemIndex) {
    if (itemIndex < 0 || itemIndex >= p->numInventoryItems) {
        printf("Invalid item index.\n");
        return;
    }

    // Resize room's inventory to add a new item
    r->items = realloc(r->items, sizeof(item) * (r->numItems + 1));
    if (r->items == NULL) {
        printf("Failed to allocate memory for room items.\n");
        exit(1);
    }

    // Add item to room's inventory
    r->items[r->numItems] = p->inventory[itemIndex];
    r->numItems++;

    // Shift remaining items in player's inventory
    for (int i = itemIndex; i < p->numInventoryItems - 1; i++) {
        p->inventory[i] = p->inventory[i + 1];
    }

    // Resize player's inventory to remove the dropped item
    p->inventory = realloc(p->inventory, sizeof(item) * (p->numInventoryItems - 1));
    p->numInventoryItems--;

    printf("Dropped: %s\n", r->items[r->numItems - 1].name);
}



// let's have a function to play the game which is the main function
void playGame()
{
    // we need to create the game
    printf("Welcome to the game\n");

    game *game = createGame();

    // we need to print the room description
    printRoomDescription(game->player->currentRoom);

    // we need to loop until the user quits
    bool quit = false;
    while (!quit)
    {
        // we need to print the menu and get the user choice
        int choice = getMenuChoice();
        // we need to carry out the user choice
        if (choice == 1)
        {
            // we need to print the room description
            printRoomDescription(game->player->currentRoom);
        }
        else if (choice == 2)
        {
            // we need to get the user choice of room to move to
            int choice = getRoomChoice(game->player->currentRoom);
            // we need to move the player to the room
            movePlayer(game->player, choice);
        }
        else if (choice == 4)
        {
            // we need to quit
            quit = true;
        }
        else if (choice == 3) 
        {
            int inventoryChoice;
            printf("Inventory Management:\n");
            printf("1. View Inventory\n");
            printf("2. Pick up an item from the room\n");
            printf("3. Drop an item from your inventory\n");
            printf("4. Return to the main menu\n");
            scanf("%d", &inventoryChoice);

            switch (inventoryChoice) 
            {
                case 1: // View Inventory
                    printf("Your Inventory:\n");
                    if (game->player->numInventoryItems == 0) 
                    {
                        printf("You have no items.\n");
                    } else {
                        for (int i = 0; i < game->player->numInventoryItems; i++) {
                            printf("%d. %s - %s\n", i + 1, game->player->inventory[i].name, game->player->inventory[i].description);
                        }
                    }
                    break;

                case 2: // Pick up an item
                    printf("Items in the room:\n");
                    if (game->player->currentRoom->numItems == 0) {
                        printf("There are no items to pick up in this room.\n");
                    } else {
                        for (int i = 0; i < game->player->currentRoom->numItems; i++) {
                            printf("%d. %s - %s\n", i + 1, game->player->currentRoom->items[i].name, game->player->currentRoom->items[i].description);
                        }
                        printf("Select the item number you want to pick up: ");
                        int itemChoice;
                        scanf("%d", &itemChoice);
                        if (itemChoice < 1 || itemChoice > game->player->currentRoom->numItems) {
                            printf("Invalid choice.\n");
                        } else {
                            pickUpItem(game->player, game->player->currentRoom, itemChoice - 1);
                        }
                    }
                    break;

                case 3: // Drop an item
                    printf("Your Inventory:\n");
                    if (game->player->numInventoryItems == 0) {
                        printf("You have no items to drop.\n");
                    } else {
                        for (int i = 0; i < game->player->numInventoryItems; i++) {
                            printf("%d. %s - %s\n", i + 1, game->player->inventory[i].name, game->player->inventory[i].description);
                        }
                        printf("Select the item number you want to drop: ");
                        int itemChoice;
                        scanf("%d", &itemChoice);
                        if (itemChoice < 1 || itemChoice > game->player->numInventoryItems) {
                            printf("Invalid choice.\n");
                        } else {
                            dropItem(game->player, game->player->currentRoom, itemChoice - 1);
                        }
                    }
                    break;

                case 4: // Return to the main menu
                    break;

                default:
                    printf("Invalid choice.\n");
                    break;
            }
        }

    }
}


int main() {
    playGame();
    return 0;
}
