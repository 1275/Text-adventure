# C vs C++ Quick Reference for C++ Developers

This guide explains key differences between C and C++ to help you understand this codebase.

## 🔑 Core Differences

### 1. **No Classes or Objects**
```cpp
// C++ ❌
class Player {
    int health;
public:
    Player() : health(100) {}
    void takeDamage(int amount);
};

// C ✅
typedef struct {
    int health;
} Player;

void player_take_damage(Player *p, int amount); // No member functions!
```

**Key Points:**
- Structs are just data containers (no methods)
- No constructors/destructors - use init/cleanup functions
- No private/public - all struct members are public
- Functions are global - use naming conventions like `player_init()`, `player_take_damage()`

---

### 2. **Pointers Instead of References**
```cpp
// C++ ❌
void modify(int& value) {
    value = 42;
}
int x = 10;
modify(x);

// C ✅
void modify(int *value) {
    *value = 42;
}
int x = 10;
modify(&x);  // Pass address with &
```

**Key Points:**
- C has NO references - only pointers
- `&` gets the address of a variable
- `*` dereferences a pointer (gets the value)
- `->` accesses struct member through pointer: `player->health` = `(*player).health`

---

### 3. **No Standard Library (STL)**
```cpp
// C++ ❌
std::vector<int> numbers;
std::string name = "Hero";
std::map<int, std::string> items;

// C ✅
int numbers[100];        // Fixed-size array
char name[50] = "Hero";  // String is char array
// No easy map - must implement your own or use library
```

**Key Points:**
- No `std::vector`, `std::string`, `std::map`, etc.
- Arrays are fixed size and don't know their length
- Strings are `char[]` arrays ending with `\0` (null terminator)
- Must track array sizes manually or use constants

---

### 4. **Manual Memory Management**
```cpp
// C++ ❌
auto player = std::make_unique<Player>();
std::vector<int> items; // Automatically manages memory

// C ✅
Player *player = malloc(sizeof(Player));  // Allocate
player_init(player);
// ... use player ...
free(player);  // Must manually free!

// Or use stack allocation (automatic)
Player player;  // No malloc/free needed
player_init(&player);
```

**Key Points:**
- Stack allocation (no malloc): Automatic cleanup when function exits
- Heap allocation (malloc): YOU must call `free()` or leak memory
- No RAII, no smart pointers, no destructors
- This code mostly uses stack allocation to avoid manual memory management

---

### 5. **No Function Overloading**
```cpp
// C++ ❌
void print(int x);
void print(float x);
void print(const std::string& s);

// C ✅
void print_int(int x);
void print_float(float x);
void print_string(const char *s);
```

**Key Points:**
- Each function needs a unique name
- Use naming conventions: `player_init()`, `enemy_init()`, etc.
- No operator overloading either

---

### 6. **Header Files and Includes**
```cpp
// C++ ❌
#include <iostream>
#include <vector>
using namespace std;

// C ✅
#include <stdio.h>   // Standard I/O
#include <stdlib.h>  // Standard library
#include "player.h"  // Your headers
// No namespaces in C!
```

**Common C Headers:**
- `stdio.h` - printf, scanf, FILE operations
- `stdlib.h` - malloc, free, rand, exit
- `string.h` - strcpy, strlen, strcmp
- `math.h` - sqrt, pow, sin, cos
- `time.h` - time functions

---

### 7. **Input/Output**
```cpp
// C++ ❌
std::cout << "Hello " << name << std::endl;
std::cin >> value;

// C ✅
printf("Hello %s\n", name);  // Format specifiers
scanf("%d", &value);          // Must pass address!
```

**Format Specifiers:**
- `%d` - int
- `%f` - float/double
- `%s` - string (char*)
- `%c` - char
- `%p` - pointer address
- `%x` - hexadecimal

---

### 8. **Booleans**
```cpp
// C++ ❌
bool isAlive = true;

// C ✅
int isAlive = 1;  // 0 = false, non-zero = true
// Or use C99's stdbool.h
#include <stdbool.h>
bool isAlive = true;
```

---

### 9. **Structs and Typedef**
```cpp
// C Without typedef ❌ (verbose)
struct Player {
    int health;
};
struct Player player;  // Must use 'struct' keyword

// C With typedef ✅ (common pattern)
typedef struct {
    int health;
} Player;
Player player;  // Cleaner!
```

---

### 10. **Initialization**
```cpp
// C++ ❌
Player player(100, 50);  // Constructor
std::vector<int> nums = {1, 2, 3};

// C ✅
Player player = {100, 50};  // Aggregate initialization
int nums[] = {1, 2, 3};     // Array initialization
Player p = {0};             // Zero-initialize all members
```

---

## 📝 Common Patterns in This Codebase

### Pattern 1: Init Functions (Manual Constructors)
```c
void player_init(Player *p, PlayerClass class) {
    p->health = 100;
    p->level = 1;
    // ... more initialization
}
```

### Pattern 2: Pass-by-Pointer for Modification
```c
void player_take_damage(Player *p, int damage) {
    p->health -= damage;
}
```

### Pattern 3: Const Pointers for Read-Only
```c
void player_print_status(const Player *p) {
    printf("HP: %d\n", p->health);
    // p->health = 0; // ERROR: can't modify const
}
```

### Pattern 4: Returning Data via Pointer
```c
const ClassDefinition* get_all_class_definitions(int *out_count) {
    *out_count = 3;  // Fill in the count
    return definitions;  // Return the array
}
```

### Pattern 5: Error Handling (No Exceptions)
```c
int player_add_item(Player *p, const Item *item) {
    if (p->inv_count >= MAX_INVENTORY) {
        return 0;  // Failure
    }
    // ... add item ...
    return 1;  // Success
}
```

---

## 🎯 Quick Translation Guide

| C++ | C |
|-----|---|
| `std::string name` | `char name[256]` |
| `std::vector<int> v` | `int v[100]` + `int v_count` |
| `player.method()` | `player_method(&player)` |
| `new Player()` | `malloc(sizeof(Player))` |
| `delete player` | `free(player)` |
| `bool` | `int` (0=false, 1=true) |
| `std::cout << x` | `printf("%d", x)` |
| `std::cin >> x` | `scanf("%d", &x)` |
| `try/catch` | Check return values |
| `nullptr` | `NULL` |
| `&reference` | `*pointer` |

---

## 🚨 Common Gotchas for C++ Developers

1. **Forgetting & in scanf**: `scanf("%d", &value)` not `scanf("%d", value)`
2. **Buffer overflows**: Arrays don't check bounds - YOUR responsibility
3. **Uninitialized variables**: No default constructors - memory is garbage
4. **Forgetting \0**: Strings must be null-terminated
5. **scanf leaves \n**: Must consume newline after reading numbers
6. **Pointer arithmetic**: `arr[i]` is `*(arr + i)` - easy to mess up

---

## 💡 Why These Differences Exist

C was designed in 1972 for:
- **Simplicity**: Minimal language, close to hardware
- **Performance**: No hidden costs (no virtual functions, RTTI, etc.)
- **Portability**: Works on tiny embedded systems with 64MB RAM
- **Explicitness**: You see exactly what the code does

C++ added (starting 1985):
- Object-oriented features
- Generic programming (templates)
- Standard library (STL)
- Automatic memory management helpers
- Exceptions and RAII

This game targets the **LuckFox Pico** (64MB RAM), so C's lightweight nature is perfect!

---

## 📚 Next Steps

1. Read through `main.c` - it's now heavily documented
2. Look at `player.h` and `player.c` - see struct + functions pattern
3. Practice: Try adding a new function to the Player
4. Remember: In C, YOU are the compiler's safety net! 🛡️
