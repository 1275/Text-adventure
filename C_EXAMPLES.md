# Practical C Examples for main.c

This guide shows common C patterns used in main.c with detailed explanations.

## Example 1: Understanding scanf and getchar

### The Problem: Reading Input
```c
// Reading a number
int choice;
scanf("%d", &choice);  // Reads "42" but leaves "\n" in buffer
```

**What happens:**
1. User types: `42` + presses Enter
2. Buffer contains: `'4'` `'2'` `'\n'`
3. scanf reads: `'4'` `'2'` and converts to int 42
4. **Buffer still has:** `'\n'` ← This will bite you!

### The Solution: Clear the Buffer
```c
int choice;
if (scanf("%d", &choice) != 1) {
    // User didn't enter a number - clear the garbage
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}
```

### Why Two getchar() Calls?
```c
scanf("%d", &choice);  // Reads number, leaves '\n'
getchar();             // Consumes the '\n' from scanf
getchar();             // NOW waits for user to press Enter
```

---

## Example 2: Pointers and Addresses

### Stack Variables
```c
int health = 100;      // Variable on stack
int *ptr = &health;    // ptr holds the ADDRESS of health

printf("%d\n", health);   // Prints: 100
printf("%p\n", &health);  // Prints: 0x7fff12345678 (address)
printf("%p\n", ptr);      // Prints: 0x7fff12345678 (same)
printf("%d\n", *ptr);     // Prints: 100 (dereference)

*ptr = 50;                // Changes health through pointer
printf("%d\n", health);   // Prints: 50
```

### Why Functions Need Pointers
```c
// ❌ This DOESN'T work - changes local copy
void set_health_wrong(int health) {
    health = 100;  // Only changes the parameter
}

// ✅ This WORKS - changes the original
void set_health_right(int *health) {
    *health = 100;  // Dereferences and changes original
}

int hp = 50;
set_health_wrong(hp);   // hp is still 50
set_health_right(&hp);  // hp is now 100!
```

---

## Example 3: Structs and Member Access

### The Two Ways to Access Members
```c
typedef struct {
    int health;
    int level;
} Player;

// Method 1: Direct access (dot operator)
Player player;
player.health = 100;
player.level = 1;

// Method 2: Through pointer (arrow operator)
Player *ptr = &player;
ptr->health = 100;     // Same as (*ptr).health
ptr->level = 1;
```

### Why We Use Pointers for Structs
```c
// ❌ BAD: Copies entire struct (expensive!)
void print_player_bad(Player p) {
    printf("HP: %d\n", p.health);
}

// ✅ GOOD: Just passes address (cheap!)
void print_player_good(const Player *p) {
    printf("HP: %d\n", p->health);
}

Player player = {100, 1};
print_player_bad(player);   // Copies all bytes of player
print_player_good(&player); // Just passes 8-byte address
```

---

## Example 4: Arrays and Pointers

### Arrays ARE Pointers
```c
int numbers[5] = {10, 20, 30, 40, 50};

// These are equivalent:
numbers[0]    // First element
*numbers      // Dereference array (same thing!)

numbers[2]    // Third element
*(numbers + 2)  // Pointer arithmetic (same thing!)

// Arrays decay to pointers
void process(int *arr, int size) {
    // arr is a pointer to first element
}
process(numbers, 5);  // Works! Array becomes pointer
```

### String Literals
```c
char message[256] = "Hello";

// What's in memory:
// message[0] = 'H'
// message[1] = 'e'
// message[2] = 'l'
// message[3] = 'l'
// message[4] = 'o'
// message[5] = '\0'  ← Null terminator (marks end)
// message[6-255] = garbage (uninitialized)
```

---

## Example 5: Aggregate Initialization

### Struct Initialization
```c
typedef struct {
    int x;
    int y;
} Position;

// Method 1: Initialize with values
Position pos = {25, 25};  // x=25, y=25

// Method 2: Zero-initialize everything
Position pos = {0};       // x=0, y=0

// Method 3: Partial initialization
Position pos = {10};      // x=10, y=0 (rest zeroed)
```

### Why {0} is Magic
```c
typedef struct {
    int health;
    int level;
    int gold;
    char name[50];
} Player;

Player p = {0};  // Everything becomes 0/NULL!
// p.health = 0
// p.level = 0
// p.gold = 0
// p.name[0-49] = 0 (empty string)
```

---

## Example 6: The Game Loop Pattern

### Basic Structure
```c
int running = 1;  // Loop control flag

while (running) {
    // 1. Get input
    char command;
    scanf(" %c", &command);
    
    // 2. Update game state
    handle_command(command, &running, &player, &world);
    
    // 3. Check for exit conditions
    if (player.health <= 0) {
        running = 0;
    }
    
    // 4. Render
    render_game(&player, &world);
}
```

### Why Pass running by Pointer?
```c
void handle_quit(int *running) {
    *running = 0;  // Changes the actual variable in main()
}

int running = 1;
handle_quit(&running);  // running is now 0
// Loop exits!
```

---

## Example 7: Return Values vs Out Parameters

### C++ Style (Return Values)
```cpp
// C++ would do this:
std::vector<Item> get_items() {
    std::vector<Item> items;
    items.push_back(sword);
    items.push_back(potion);
    return items;  // Modern C++ moves, no copy
}
```

### C Style (Out Parameters)
```c
// C does this instead:
const Item* get_items(int *out_count) {
    static Item items[2] = {sword, potion};
    *out_count = 2;  // Fill in the count via pointer
    return items;     // Return pointer to array
}

// Usage:
int count;
const Item *items = get_items(&count);
for (int i = 0; i < count; i++) {
    printf("%s\n", items[i].name);
}
```

**Why?**
- C can't return arrays or complex structures easily
- Return pointers + size instead
- Caller needs to know size (arrays don't track length)

---

## Example 8: Error Handling

### No Exceptions - Use Return Codes
```c
// Return 0 for failure, 1 for success
int player_add_item(Player *p, const Item *item) {
    if (p->inv_count >= MAX_INVENTORY) {
        return 0;  // Failure - inventory full
    }
    
    p->inventory[p->inv_count++] = *item;
    return 1;  // Success
}

// Usage:
if (player_add_item(&player, &sword)) {
    printf("Added sword!\n");
} else {
    printf("Inventory full!\n");
}
```

### Sometimes Return NULL
```c
const ClassDefinition* get_class_definition(PlayerClass class) {
    // ... search for class ...
    if (found) {
        return &definition;
    }
    return NULL;  // Failure - not found
}

// Usage: Check for NULL!
const ClassDefinition *def = get_class_definition(CLASS_WARRIOR);
if (def) {
    printf("Found: %s\n", def->name);
} else {
    printf("Class not found!\n");
}
```

---

## Example 9: Memory Lifetime

### Stack (Automatic)
```c
void function() {
    int x = 42;           // Created
    Player p = {100, 1};  // Created
    
    // ... use x and p ...
    
}  // x and p DESTROYED HERE (automatic)
```

### Heap (Manual)
```c
void function() {
    int *x = malloc(sizeof(int));  // You allocate
    *x = 42;
    
    Player *p = malloc(sizeof(Player));
    player_init(p, CLASS_WARRIOR);
    
    // ... use them ...
    
    free(x);  // YOU must free!
    free(p);  // Or memory leaks!
}
```

**This game uses mostly stack allocation to avoid manual memory management!**

---

## Example 10: Printf Format Specifiers

### Common Formats
```c
int health = 100;
float damage = 15.5;
char name[] = "Hero";
int *ptr = &health;

printf("Integer: %d\n", health);           // Integer: 100
printf("Float: %.2f\n", damage);           // Float: 15.50
printf("String: %s\n", name);              // String: Hero
printf("Pointer: %p\n", ptr);              // Pointer: 0x7fff...
printf("Hex: %x\n", health);               // Hex: 64
printf("Char: %c\n", name[0]);             // Char: H
printf("Padded: %3d/%3d\n", 5, 100);      // Padded:   5/100
printf("Left-align: %-10s!\n", name);      // Left-align: Hero      !
```

---

## 🎓 Study Tips

1. **Read main.c top to bottom** - it's now a tutorial!
2. **Type out examples** - don't just read, practice
3. **Use printf debugging** - no fancy debugger needed
4. **Check return values** - that's C's error handling
5. **Watch for buffer overflows** - arrays don't protect you!
6. **When in doubt, check for NULL** - pointers can be dangerous

---

## 🐛 Debugging Tips

### Print Everything
```c
printf("DEBUG: health=%d, x=%d, y=%d\n", player.health, pos.x, pos.y);
```

### Check Pointer Before Using
```c
if (player == NULL) {
    printf("ERROR: player is NULL!\n");
    return;
}
```

### Verify Array Bounds
```c
if (index < 0 || index >= MAX_INVENTORY) {
    printf("ERROR: Invalid index %d\n", index);
    return;
}
```

Remember: In C, the computer trusts you completely. Don't break that trust! 🤝
