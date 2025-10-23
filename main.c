/**
 * main.c - Entry point for the Dungeon Crawler game
 * 
 * C vs C++ Notes:
 * - In C, we use #include with .h files (no <vector>, <string>, etc.)
 * - stdio.h = C's version of <iostream> (printf instead of std::cout)
 * - stdlib.h = Basic utilities (malloc, rand, etc.)
 * - No namespaces in C (no "using namespace std")
 * - No function overloading - each function needs a unique name
 */

#include <stdio.h>   // Standard Input/Output: printf, scanf, getchar
#include <stdlib.h>  // Standard library: srand, rand, exit
#include <time.h>    // Time functions: time() for random seed
#include "dungeon.h" // Our custom dungeon/map types and functions
#include "player.h"  // Player struct and class definitions
#include "ui.h"      // User interface rendering functions

/**
 * main() - Program entry point
 * 
 * C vs C++:
 * - In C, main returns int (always)
 * - "void" in the parameters means no arguments (in C++ you'd write main())
 * - No try/catch in C - error handling is manual with return codes
 * 
 * @return 0 on success, non-zero on error (Unix convention)
 */
int main(void)
{
    /*
     * Seed the random number generator
     * 
     * C vs C++:
     * - C uses srand() + rand() (in C++ you'd use <random> with mt19937)
     * - time(NULL) returns current time as seed (NULL = no pointer passed)
     * - Cast to unsigned int because time_t might be different size
     * - This makes rand() produce different numbers each run
     */
    srand((unsigned int)time(NULL));

    // ========================================================================
    // CHARACTER CLASS SELECTION
    // ========================================================================
    
    /*
     * Display the game title using box-drawing characters
     * 
     * C string literals:
     * - Strings are char arrays, NOT std::string objects
     * - No automatic memory management
     * - \n is newline (like std::endl but doesn't flush)
     * - printf is like std::cout but uses format specifiers
     */
    printf("\n╔════════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                        DUNGEON CRAWLER ADVENTURE                               ║\n");
    printf("╚════════════════════════════════════════════════════════════════════════════════╝\n\n");
    printf("Choose your character class:\n\n");
    
    /*
     * Get all available character classes dynamically
     * 
     * C vs C++:
     * - Variables must be declared at the beginning of a block (C89/C90 style)
     *   or before use (C99+). We're using C11, so we can declare anywhere.
     * - "const ClassDefinition *" = pointer to constant data (like const* in C++)
     * - The & operator gets the ADDRESS of class_count (pass by reference in C)
     * - In C++, you'd use references (&count). C only has pointers!
     * - This function fills class_count and returns a pointer to an array
     */
    int class_count;
    const ClassDefinition *classes = get_all_class_definitions(&class_count);
    
    /*
     * Loop through and display each class
     * 
     * C vs C++:
     * - for loops work the same, but C99+ allows declaring i inside the loop
     * - Array indexing with [] works on pointers: classes[i] = *(classes + i)
     * - The . operator accesses struct members (same as C++)
     * - printf format specifiers: %d = int, %s = string (char*), %f = float
     * - No range-based for loops (for (auto& x : vec) in C++)
     */
    for (int i = 0; i < class_count; i++) {
        printf("  [%d] %s - %s\n", i + 1, classes[i].name, classes[i].description);
        printf("      • Max Health: %d\n", classes[i].max_health);
        printf("      • Base Damage: %d\n", classes[i].base_damage);
        printf("      • Base Defense: %d\n\n", classes[i].base_defense);
    }
    
    // ========================================================================
    // INPUT VALIDATION LOOP
    // ========================================================================
    
    /*
     * Initialize variables for class selection
     * 
     * C vs C++:
     * - PlayerClass is an enum (works same as C++)
     * - Must initialize variables (no default constructors in C)
     * - "int valid_input = 0" means false (C uses 0=false, non-zero=true)
     * - No bool type in older C (C99+ has _Bool, but int is more common)
     */
    PlayerClass selected_class = CLASS_WARRIOR;  // Default to first class
    int valid_input = 0;  // false
    
    while (!valid_input) {  // Loop until we get valid input
        printf("Enter your choice (1-%d): ", class_count);
        
        /*
         * Read user input with scanf
         * 
         * IMPORTANT C CONCEPT - scanf:
         * - scanf is like std::cin >> but more dangerous!
         * - "%d" = format specifier for integer
         * - &choice = pass address of variable (scanf NEEDS a pointer)
         * - Returns number of items successfully read (1 if successful)
         * - scanf leaves the newline '\n' in the input buffer!
         */
        int choice;
        if (scanf("%d", &choice) != 1) {
            /*
             * Input was not a number - clear the input buffer
             * 
             * C vs C++:
             * - No cin.clear() or cin.ignore() in C
             * - Must manually read characters until newline
             * - getchar() reads one character at a time
             * - EOF = End Of File (special value, usually -1)
             */
            int c;  // Store each character (int, not char, to handle EOF)
            while ((c = getchar()) != '\n' && c != EOF);
            printf("Invalid input! Please enter a number between 1 and %d.\n", class_count);
            continue;  // Skip to next loop iteration
        }
        
        /*
         * Validate the choice is in range
         * 
         * C vs C++:
         * - Logical operators work the same: && (and), || (or), ! (not)
         * - Array indexing: choice-1 because arrays are 0-indexed
         */
        if (choice >= 1 && choice <= class_count) {
            selected_class = classes[choice - 1].class_type;
            valid_input = 1;  // true - exit the loop
            printf("\nYou have chosen the %s class!\n", classes[choice - 1].name);
        } else {
            printf("Invalid choice! Please enter a number between 1 and %d.\n", class_count);
        }
    }
    
    /*
     * Wait for user to press Enter before starting
     * 
     * TRICKY C GOTCHA:
     * - Need TWO getchar() calls!
     * - First getchar() consumes the '\n' left by scanf
     * - Second getchar() waits for user to press Enter
     * - In C++, you'd use cin.ignore() and cin.get()
     */
    printf("\nPress Enter to begin your adventure...");
    getchar(); // consume newline from scanf
    getchar(); // wait for user to press Enter
    
    // ========================================================================
    // GAME INITIALIZATION
    // ========================================================================
    
    /*
     * Create player object
     * 
     * HUGE C vs C++ DIFFERENCE:
     * - In C++: Player player; calls the constructor
     * - In C: Player player; just allocates memory (no constructors!)
     * - Memory contains GARBAGE - must explicitly initialize
     * - That's why we call player_init() - it's our "manual constructor"
     * - Structs in C are just data (no methods, no constructors/destructors)
     */
    Player player;
    player_init(&player, selected_class);  // Pass address to initialize
    
    /*
     * Create and generate the map
     * 
     * C vs C++:
     * - Map map; allocates on the stack (same as C++)
     * - map_generate(&map) passes address to fill in the map data
     * - No "new" keyword - this is stack allocation
     * - Will be automatically freed when main() exits
     */
    Map map;
    map_generate(&map);

    /*
     * Initialize game state variables
     * 
     * C aggregate initialization:
     * - {MAP_CENTER, MAP_CENTER} initializes a Position struct
     * - In C++, you might use Position pos(MAP_CENTER, MAP_CENTER)
     * - {0} initializes all struct members to 0 (like memset to 0)
     * - Very common C idiom for zeroing structs
     */
    Position pos = {MAP_CENTER, MAP_CENTER};  // Start at center of map
    GameState state = STATE_EXPLORING;        // Game state enum
    BattleState battle = {0};                 // Zero-initialize all members
    battle.is_active = 0;                     // Explicitly set (redundant but clear)
    
    /*
     * Set up game loop variables
     * 
     * C arrays:
     * - char message[256] = creates array of 256 chars on stack
     * - In C++, you'd use std::string message = "...";
     * - String literals automatically null-terminated ('\0' at end)
     * - Fixed size - NO bounds checking! Overflow = undefined behavior
     * - Must ensure strings fit in the buffer (why we use 256)
     */
    int running = 1;  // true - game loop continues while this is 1
    char message[256] = "Whoa! You trigger a magical portal and find yourself in a mysterious dungeon...";

    // Render initial game state
    ui_render_game(&player, &pos, message, &map);

    // ========================================================================
    // MAIN GAME LOOP
    // ========================================================================
    
    /*
     * Game loop - runs until player quits or dies
     * 
     * C pattern:
     * - Simple while(running) loop
     * - No fancy event systems like in C++ game engines
     * - Blocking I/O with scanf (waits for input)
     * - Update, then render
     */
    while (running)
    {
        /*
         * Read a single character command
         * 
         * scanf details:
         * - " %c" = the space before %c consumes any whitespace
         * - This skips newlines, spaces, tabs before reading the char
         * - Without the space, you'd read the '\n' from previous input!
         * - Returns 1 if successful, 0 if no input, EOF if error
         */
        char command;
        if (scanf(" %c", &command) != 1) {
            break;  // Exit loop if scanf fails (EOF or error)
        }
        
        /*
         * Process the command
         * 
         * C function calls:
         * - Pass everything by pointer (&variable) to allow modification
         * - In C++, you'd use references: handle_command(..., Player& player, ...)
         * - C doesn't have references - only pointers
         * - The function can modify these variables through the pointers
         */
        handle_command(command, &running, &pos, &player, message, &map, &state, &battle);
        
        // ====================================================================
        // RENDERING
        // ====================================================================
        
        /*
         * Render the appropriate screen based on game state
         * 
         * C conditionals:
         * - Work exactly like C++
         * - No switch expressions (C++ 17+)
         * - State pattern but without polymorphism (no virtual functions)
         */
        if (running) {
            if (state == STATE_BATTLE) {
                ui_render_battle(&player, &battle, message);
            } else if (state == STATE_INVENTORY) {
                ui_render_inventory(&player, message);
            } else {
                ui_render_game(&player, &pos, message, &map);
            }
        }

        // ====================================================================
        // DEATH CHECK
        // ====================================================================
        
        /*
         * Check if player has died
         * 
         * C vs C++:
         * - Direct member access with . operator
         * - player is a value (on stack), not a pointer
         * - If it was a pointer: player->health instead of player.health
         * - No exceptions - just check the value and handle it
         */
        if (player.health <= 0)
        {
            // Display game over screen
            ui_clear_screen();
            printf("\n╔════════════════════════════════════════╗\n");
            printf("║         GAME OVER                      ║\n");
            printf("╚════════════════════════════════════════╝\n\n");
            printf("You have perished in the dungeon.\n");
            printf("Final Level: %d\n", player.level);
            printf("Gold Collected: %d\n", player.gold);
            printf("Final Position: [%d, %d]\n\n", pos.x, pos.y);
            running = 0;  // Stop the game loop
        }
    }

    // ========================================================================
    // CLEANUP
    // ========================================================================
    
    /*
     * Restore terminal state and exit
     * 
     * C vs C++:
     * - No destructors! Must manually clean up
     * - Luckily, all our data is on the stack (automatic storage)
     * - Stack variables are automatically freed when function exits
     * - If we used malloc(), we'd need to call free() here
     * - ui_show_cursor() restores the terminal to normal state
     */
    ui_show_cursor();
    return 0;  // Success! (Unix convention: 0 = success)
    
    /*
     * What happens when main() returns:
     * - All stack variables (player, map, pos, etc.) are destroyed
     * - No memory leaks because we didn't use malloc()
     * - In C++, destructors would be called here
     * - In C, the memory is just reclaimed by the OS
     */
}