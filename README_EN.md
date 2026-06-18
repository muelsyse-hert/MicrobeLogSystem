# MicrobeLogSystem - Microorganism Culture Management System

An interactive command-line system for managing and tracking microorganism strain cultivation processes. Now with persistent file-based storage!

## Features

- ✅ **Strain Management**: Register new strains with efficient binary search tree (BST) storage and query
- ✅ **Culture Logging**: Record detailed cultivation data including date, pH value, temperature, gas environment, and observations for each strain
- ✅ **Status Tracking**: Manage strain cultivation status (UNTOUCHED, CULTURING, ENDED)
- ✅ **Data Query**: Fast strain search with detailed information and complete log retrieval
- ✅ **Persistent Storage**: Automatic saving of all data to JSON files (NEW!)
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
gcc -Iinclude main.c src/storage_file.c src/*.c -o MicrobeLogSystem.exe
```

Compilation flags explained:
- `-Iinclude`: Specify header file search path
- `main.c src/storage_file.c src/*.c`: Compile main file, storage module, and all source files
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
6. Save Data to Storage
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
- **Note**: Data is automatically saved after each entry

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
  3. Updated status is saved to file

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

#### Option 6: Save Data to Storage
- **Function**: Manually save all strains and logs to persistent storage (JSON format)
- **Steps**:
  1. System saves all data to `data/` directory
  2. Data is organized as:
     - `data/strains.json` - Index of all strains
     - `data/logs/[strain_name].json` - Individual strain logs
  3. Confirmation message appears when save is complete

**Note**: Data is automatically saved on exit and when adding/modifying strains, but you can use this option to manually trigger a save.

#### Option 0: Exit System
- **Function**: Automatically save all data and safely exit the program with complete resource cleanup

## Data Storage

### Data Structures in Memory

All data is stored in **RAM** using the following structures during runtime:

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
| Strain Metadata | JSON File (`data/strains.json`) | Limited by disk space | ✅ Yes |
| Culture Logs | JSON Files (`data/logs/*.json`) | Limited by disk space | ✅ Yes |
| Strain Status | JSON File (`data/logs/[strain_name].json`) | Limited by disk space | ✅ Yes |
| Working Data in Memory | RAM (BST + Linked Lists) | Limited by available system memory | ❌ No |

### 📁 Data Directory Structure

When you run the program, it automatically creates a `data/` directory:

```
data/
├── strains.json              # Index of all strains (metadata)
└── logs/
    ├── E.coli_001.json       # Logs for strain E.coli_001
    ├── B.subtilis_002.json   # Logs for strain B.subtilis_002
    └── ...
```

### Data Persistence & Auto-Save

**New Feature**: The system now supports persistent data storage!

- ✅ **Automatic Save**: Data is automatically saved when you:
  - Add a new culture log (Option 2)
  - End a culture (Option 3)
  - Exit the program (Option 0)
  - Use the manual save option (Option 6)

- ✅ **JSON Format**: All data is stored in human-readable JSON format for easy inspection and portability

- ✅ **File-Based**: Data persists on disk and can be accessed even after program restart

### ⚠️ Important Notes

- **Data Persistence**: ✅ Data is now automatically saved to disk in JSON format
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
│   ├── culture_status.h         # Culture status module declarations
│   └── storage.h                # Storage abstraction layer interface (NEW!)
├── src/                         # Source file directory
│   ├── daily_log_list_demo.c    # Log module implementation
│   ├── strain_archive_bst_demo.c # Strain BST module implementation
│   ├── culture_status_demo.c    # Culture status module implementation
│   └── storage_file.c           # File storage backend implementation (NEW!)
├── data/                        # Data directory (auto-created)
│   ├── strains.json             # Strain index
│   └── logs/                    # Individual strain log files
└── README_EN.md                 # This document
```

### Storage Layer Architecture

The system uses an **abstraction layer** for data storage:

```
Application (main.c)
        ↓
Storage Interface (storage.h)
        ↓
File Storage Backend (storage_file.c)
        ↓
JSON Files (data/)
```

This design allows easy migration to database storage in the future.

## Module Descriptions

### 1. **common.h** - Shared Data Structures
- Defines all data structures used by all modules
- Defines culture status enumeration

### 2. **storage.h** - Storage Abstraction Layer (NEW!)
- Defines the storage interface used by the application
- Supports multiple backend implementations:
  - File-based storage (JSON format)
  - Future database support (reserved interface)
- Provides functions for loading, saving, and deleting strains

### 3. **storage_file.c** - File Storage Backend (NEW!)
- Implements JSON-based file storage
- Automatically creates `data/` directory and subdirectories
- Sanitizes strain names for safe filename generation
- Provides auto-save functionality

### 4. **app.h** - Application Layer Interface
- Declares module entry declarations

### 5. **daily_log_list.h** - Log Module
- Creates and manages log linked lists
- Supports log addition, query, and display
- Handles log memory deallocation

### 6. **strain_archive_bst.h** - Strain Archive Module
- Manages strains using binary search tree
- Supports strain insertion, search, and display
- In-order traversal for alphabetical display

### 7. **culture_status.h** - Culture Status Module
- Manages strain cultivation status
- Displays detailed strain information
- Marks culture as ended

## Code Highlights

✨ **Persistent Data Storage**
- JSON-based file storage for human-readable data
- Automatic saving on critical operations
- Manual save option for user control
- Designed for easy migration to database backends

✨ **Excellent Memory Management**
- Uses `fgets` instead of `scanf` to avoid input buffer issues
- Comprehensive dynamic memory allocation and deallocation
- Complete memory cleanup on program exit
- Proper handling of file operations

✨ **Modular Design**
- Storage layer abstraction allows backend switching
- Each module has independent header and implementation files
- Main file handles menu flow and calls appropriate modules
- Easy to extend with new storage backends (SQL, NoSQL, etc.)

✨ **Robust Input Handling**
- Integer and floating-point input validation
- Proper newline character handling
- User re-prompting for invalid input

✨ **Production-Ready File Handling**
- Safe filename generation from strain names
- Automatic directory creation
- Error handling for file I/O operations

## Data Format Examples

### Example: strains.json
```json
{
  "timestamp": 1718611200,
  "strains": [
    {
      "name": "E.coli_001",
      "id": 1001,
      "status": 1,
      "log_count": 5
    },
    {
      "name": "B.subtilis_002",
      "id": 1002,
      "status": 2,
      "log_count": 3
    }
  ]
}
```

### Example: logs/E.coli_001.json
```json
{
  "strain_name": "E.coli_001",
  "strain_id": 1001,
  "status": 1,
  "logs": [
    {
      "date": "2026-06-17",
      "ph_value": 7.20,
      "temperature": 37.50,
      "gas_env": "aerobic",
      "observation": "Initial culture started"
    },
    {
      "date": "2026-06-18",
      "ph_value": 7.15,
      "temperature": 37.30,
      "gas_env": "aerobic",
      "observation": "Normal growth observed"
    }
  ]
}
```

## Usage Example Scenario

### Scenario: Record E. coli cultivation process

```
1. Program starts and initializes storage to data/ directory
2. Select option 1 to register strain "E.coli_lab" with ID 1001
   - Data automatically saved to data/logs/E.coli_lab.json
3. Select option 2 to add cultivation logs
   - Date: 2026-06-17
   - pH: 7.2
   - Temperature: 37.5
   - Gas: aerobic
   - Observation: Initial culture started
   - Data automatically saved after entry
4. Continue adding more log records
5. Select option 4 to query strain status
6. Select option 3 to mark culture as ended
   - Updated status saved to data/logs/E.coli_lab.json
7. Select option 5 to view all strains
8. Select option 0 to exit
   - All data is saved and program closes safely
```

## Frequently Asked Questions

**Q: Where is my data stored?**
A: Data is automatically saved to a `data/` directory in JSON format:
- `data/strains.json` - Contains metadata for all strains
- `data/logs/[strain_name].json` - Contains detailed logs for each strain

**Q: How many strains can I manage?**
A: Limited by available disk space. Each strain occupies approximately 100-200 bytes (excluding logs), and each log entry occupies approximately 370 bytes.

**Q: Are there time constraints for logging records?**
A: No. You can record data for any time period.

**Q: Will my data be lost if the program crashes?**
A: Data is automatically saved after each operation, so you will only lose the current operation being performed at the time of crash.

**Q: Can I edit the JSON files directly?**
A: Yes, but use caution. The JSON format must remain valid. It's recommended to use the program interface for modifications.

**Q: How can I backup my data?**
A: Simply copy the entire `data/` folder to a safe location. The JSON format is portable and can be read by any text editor.

**Q: Can previously recorded logs be modified?**
A: Not through the program interface currently, but you can manually edit the JSON files. A delete function is planned for future versions.

## Future Enhancement: Database Support

This system is designed to support database backends in the future. The abstraction layer in `storage.h` allows easy implementation of:
- SQLite for local databases
- PostgreSQL for server-based systems
- MySQL/MariaDB for distributed systems
- NoSQL databases (MongoDB, etc.)

Migration to a database backend will only require implementing the `StorageContext` interface without changing the application code!

## Technical Details

- **Language**: C99 standard
- **Compilation**: Standard compiler toolchain (GCC/Clang/MSVC)
- **Data Format**: JSON (human-readable and portable)
- **Time Complexity**: Strain search O(log n), log addition O(1)
- **Space Complexity**: O(n*m), where n is the number of strains and m is the number of logs per strain
- **File Storage**: Automatic directory creation and file management

---

**Last Updated**: 2026-06-17
