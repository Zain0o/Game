# C Program Game

## Overview
This is a text-based adventure game written in C. Players navigate through various rooms, interact with items, and manage their inventory while exploring the game's world. The game utilizes CSV files for defining rooms, connections, and items, making it extensible and easy to customize.

## Features
- **Room Navigation**: Move between connected rooms.
- **Item Management**: Pick up and drop items in different rooms.
- **Inventory System**: View and manage collected items.
- **Data-Driven**: Rooms, connections, and items are loaded dynamically from CSV files.

## Files
### Source Files
- **`game.c`**: Contains the core game logic and functions.

### Data Files
- **`rooms.csv`**: Defines rooms with their names and descriptions.
- **`connections.csv`**: Specifies the connections between rooms.
- **`items.csv`**: Lists items available in each room along with their properties.

### Executable
- **`game.exe`**: Precompiled executable for running the game on Windows.

## How to Run
1. **Compile the Game**:
   - Use a C compiler like `gcc` to compile the `game.c` file:
     ```bash
     gcc game.c -o game
     ```
   - Ensure you have the data files (`rooms.csv`, `connections.csv`, `items.csv`) in the same directory as the executable.

2. **Run the Game**:
   - On Windows:
     ```cmd
     game.exe
     ```
   - On Linux/Mac:
     ```bash
     ./game
     ```

## Game Mechanics
1. **Start the Game**:
   - The player begins in a specified room.
   - A description of the room is displayed.

2. **Menu Options**:
   - Look around the room for a description and items.
   - Move to a connected room.
   - Manage inventory (view, pick up, or drop items).
   - Quit the game.

3. **Room and Connection Logic**:
   - Rooms are dynamically connected based on the `connections.csv` file.
   - Navigation is restricted to predefined connections.

4. **Item Interaction**:
   - Items can be picked up if they are present in the room and are collectible.
   - Items can be dropped into any room, making them accessible later.

## CSV File Format
### `rooms.csv`
- **Format**: `Room Name, Room Description`
- Example:
  ```
  "Living Room", "A cozy room with a fireplace."
  "Kitchen", "A room filled with cooking utensils."
  ```

### `connections.csv`
- **Format**: `Room1 Index, Room2 Index, Room3 Index`
- Example:
  ```
  0,1,2
  1,2,3
  ```

### `items.csv`
- **Format**: `Room Index, "Item Name", "Item Description", Collectible (1 or 0)`
- Example:
  ```
  0, "Key", "A shiny golden key.", 1
  1, "Book", "An old dusty book.", 1
  ```

