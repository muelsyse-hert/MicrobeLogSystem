# MicrobeLogSystem - Microorganism Culture Management System

An interactive command-line system for managing and tracking microorganism strain cultivation processes. Supports strain registration, culture log recording, status queries, and more.

## Features

- ✅ **Strain Management**: Register new strains with efficient binary search tree (BST) storage and query
- ✅ **Culture Logging**: Record detailed cultivation data including date, pH value, temperature, gas environment, and observations for each strain
- ✅ **Status Tracking**: Manage strain cultivation status (UNTOUCHED, CULTURING, ENDED)
- ✅ **Data Query**: Fast strain search with detailed information and complete log retrieval
- ✅ **Interactive Interface**: User-friendly menu-driven operation
- ✅ **Memory Management**: Comprehensive dynamic memory management with automatic resource cleanup on exit

## System Requirements

- **Operating System**: Windows/Linux/macOS (any system supporting GCC)
- **Compiler**: GCC 7.0 or later
- **Dependencies**: None

## Compilation and Running

### 1. Compile the Project

Open a terminal in the project root directory and run:

```bash
gcc -Iinclude main.c src/*.c -o MicrobeLogSystem.exe
```

Compilation flags explained:
- `-Iinclude`: Specify header file search path
- `main.c src/*.c`: Compile main file and all source files
- `-o MicrobeLogSystem.exe`: Output executable name (Windows); on Linux/macOS, use `MicrobeLogSystem`

### 2. Run the Program

After successful compilation, execute:

```bash
./MicrobeLogSystem.exe
```

Or on Linux/macOS:

```bash
./MicrobeLogSystem
```

## User Guide

After launching the program, you will see the main menu:

```
===========================================
  Microorganism Culture Management System
===========================================
1. Register New Strain
2. Append Culture Log
3. End Culture
4. Query Strain Status & Logs
5. Display All Strain Archives
0. Exit System
===========================================
```

### Menu Options in Detail

#### Option 1: Register New Strain
- **Function**: Add a new strain to the system
- **Steps**:
  1. Enter strain name (e.g., E.coli_001)
  2. Enter strain ID (integer, e.g., 1001)
  3. System automatically sets strain status to "UNTOUCHED"

```
Example:
Enter strain name: E.coli_001
Enter strain ID: 1001
```

#### Option 2: Append Culture Log
- **Function**: Add a new cultivation record for a registered strain
- **Steps**:
  1. Enter strain name (must be registered)
  2. Enter record date (format: YYYY-MM-DD, e.g., 2026-06-17)
  3. Enter pH value (floating-point, e.g., 7.2)
  4. Enter cultivation temperature (floating-point, unit: °C, e.g., 37.5)
  5. Enter gas environment (e.g., aerobic or anaerobic)
  6. Enter observation (e.g., strain growth is normal)

```
Example:
Enter strain name: E.coli_001
Enter date (YYYY-MM-DD): 2026-06-17
Enter pH value: 7.2
Enter temperature: 37.5
Enter gas environment: aerobic
Enter observation: Bacterial growth is normal, no contamination
```

#### Option 3: End Culture
- **Function**: Mark a strain's cultivation as completed
- **Steps**:
  1. Enter the name of the strain to end
  2. Strain status will change to "ENDED"

```
Example:
Enter strain name: E.coli_001
```

#### Option 4: Query Strain Status & Logs
- **Function**: View detailed information and all cultivation logs for a specific strain
- **Steps**:
  1. Enter strain name
  2. System displays basic strain information (name, ID, status) and complete logs

```
Example:
Enter strain name: E.coli_001

Sample Output:
Strain: E.coli_001
ID: 1001
Status: CULTURING
Logs:
Date: 2026-06-17 | pH: 7.20 | Temperature: 37.50°C | Gas: aerobic | Obs: Normal growth
Date: 2026-06-18 | pH: 7.15 | Temperature: 37.30°C | Gas: aerobic | Obs: Growth continues
```

#### Option 5: Display All Strain Archives
- **Function**: Display all registered strains using in-order tree traversal (alphabetically by strain name)
- **Output Format**: Strain name, ID, current status

```
Example:
Strain 1: B.subtilis_002, ID: 1002, Status: ENDED
Strain 2: E.coli_001, ID: 1001, Status: CULTURING
Strain 3: S.aureus_003, ID: 1003, Status: UNTOUCHED
```

#### Option 0: Exit System
- **Function**: Safely exit the program with automatic memory cleanup

## Data Storage

### Data Structures in Memory

All data is stored in **RAM** using the following structures:

#### 1. **Strain Tree (BST - Binary Search Tree)**
- Organizes all strains using a binary search tree with strain name as the sort key
- Supports fast search and insertion operations (O(log n) average case)
- Root node pointer: `StrainNode* root`

#### 2. **Strain Node Structure** (`StrainNode`)
```c
typedef struct StrainNode {
    char name[50];              // Strain name
    int strain_id;              // Strain ID
    CultureStatus status;       // Cultivation status (UNTOUCHED/CULTURING/ENDED)
    LogNode standard_data;      // Standard data
    LogNode* log_head;          // Log linked list head pointer
    LogNode* log_tail;          // Log linked list tail pointer
    struct StrainNode* left;    // Left child node
    struct StrainNode* right;   // Right child node
} StrainNode;
```

#### 3. **Log Record Structure** (`LogNode`)
```c
typedef struct LogNode {
    char date[15];              // Date (YYYY-MM-DD)
    float ph_value;             // pH value
    float temperature;          // Cultivation temperature (°C)
    char gas_env[50];           // Gas environment
    char observation[256];      // Observation record
    struct LogNode* next;       // Pointer to next log
} LogNode;
```

#### 4. **Culture Status Enumeration** (`CultureStatus`)
```c
typedef enum {
    STATUS_UNTOUCHED = 0,       // Not started
    STATUS_CULTURING = 1,       // In progress
    STATUS_ENDED = 2            // Ended
} CultureStatus;
```

### Storage Locations

| Data Type | Storage Location | Capacity | Persistent |
|-----------|-----------------|----------|-----------|
| Strain Information | RAM (BST) | Limited by available system memory | ❌ No |
| Culture Logs | RAM (Linked List) | Limited by available system memory | ❌ No |
| Strain Status | RAM | - | ❌ No |

### ⚠️ Important Notes

- **No Data Persistence**: All data during program execution is stored in memory and **will be lost after exiting**
- **Recommended Improvement**: For persistent storage, consider exporting data to CSV/JSON files or using a database
- **Character Limits**:
  - Strain name: 49 characters
  - Gas environment: 49 characters
  - Observation: 255 characters
  - Date: 14 characters

## Project Structure

```
MicrobeLogSystem/
├── main.c                       # Main program with menu and interaction logic
├── include/                     # Header file directory
│   ├── common.h                 # Shared data structures and enum definitions
│   ├── app.h                    # Module entry declarations
│   ├── daily_log_list.h         # Log module declarations
│   ├── strain_archive_bst.h     # Strain BST module declarations
│   └── culture_status.h         # Culture status module declarations
├── src/                         # Source file directory
│   ├── daily_log_list_demo.c    # Log module implementation
│   ├── strain_archive_bst_demo.c # Strain BST module implementation
│   └── culture_status_demo.c    # Culture status module implementation
└── README_EN.md                 # This document
```

## Module Descriptions

### 1. **common.h** - Shared Data Structures
- Defines all data structures used by all modules
- Defines culture status enumeration

### 2. **app.h** - Application Layer Interface
- Declares three demo functions (for future extensions)

### 3. **daily_log_list.h** - Log Module
- Creates and manages log linked lists
- Supports log addition, query, and display
- Handles log memory deallocation

### 4. **strain_archive_bst.h** - Strain Archive Module
- Manages strains using binary search tree
- Supports strain insertion, search, and display
- In-order traversal for alphabetical display

### 5. **culture_status.h** - Culture Status Module
- Manages strain cultivation status
- Displays detailed strain information
- Marks culture as ended

## Code Highlights

✨ **Excellent Memory Management**
- Uses `fgets` instead of `scanf` to avoid input buffer issues
- Comprehensive dynamic memory allocation and deallocation
- Complete memory cleanup on program exit

✨ **Modular Design**
- Each module has independent header and implementation files
- Main file handles only menu flow and module function calls
- Easy to extend and maintain

✨ **Robust Input Handling**
- Integer and floating-point input validation
- Proper newline character handling
- User re-prompting for invalid input

## Usage Example Scenario

### Scenario: Record E. coli cultivation process

```
1. Select option 1 to register strain "E.coli_lab" with ID 1001
2. Select option 2 to add cultivation logs
   - Date: 2026-06-17
   - pH: 7.2
   - Temperature: 37.5
   - Gas: aerobic
   - Observation: Initial culture started
3. Continue adding more log records
4. Select option 4 to query strain status
5. Select option 3 to mark culture as ended
```

## Frequently Asked Questions

**Q: How many strains can be managed simultaneously?**
A: Limited by available system memory. Each strain occupies approximately 100-200 bytes (excluding logs), and each log entry occupies approximately 370 bytes.

**Q: Are there time constraints for logging records?**
A: No. You can record data for any time period.

**Q: What happens to the data after exiting the program?**
A: All data is lost. Consider using option 5 to view all data before exiting and manually record if necessary.

**Q: Can previously recorded logs be modified?**
A: Not currently supported, but you can restart the program and re-enter the data.

## Technical Details

- **Language**: C99 standard
- **Compilation**: Standard compiler toolchain (GCC/Clang/MSVC)
- **Time Complexity**: Strain search O(log n), log addition O(1)
- **Space Complexity**: O(n*m), where n is the number of strains and m is the number of logs per strain

---

**Last Updated**: 2026-06-17
