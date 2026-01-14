# CMPSC 311 Systems Programming Labs

A collection of five labs building a complete storage system from the ground up: starting with C fundamentals, implementing a JBOD disk array, adding caching, and finally networking support.

**Course:** CMPSC 311 - Introduction to Systems Programming
**Institution:** Penn State
**Term:** Fall 2022
**Developer:** Garrett Fincke
**Grade:** 100% on all assignments

---

## Labs Overview

| Lab | Topic | Key Concepts |
|-----|-------|--------------|
| 1 | C Fundamentals | Arrays, strings, pointers, sorting |
| 2 | JBOD Basics | Disk mounting, block reads, bit manipulation |
| 3 | JBOD Read/Write | Cross-block I/O, address translation |
| 4 | Caching | LFU replacement, hit/miss tracking |
| 5 | Networking | TCP sockets, client-server architecture |

---

## Lab 1: C Programming Fundamentals

Basic C programming exercises covering core concepts.

**Implemented Functions:**
- `squareOfSmallest()` - Find and square the minimum array element
- `findMin()` - Linear search for minimum value
- `isPalindrome()` - Two-pointer string comparison
- `freqOfChar()` - Character frequency counter
- `sort()` - Selection sort implementation
- `twoSum()` - Find pair summing to target
- `decryptPointer()` - Pointer arithmetic with arrays

---

## Lab 2: JBOD Storage System

Introduction to Just a Bunch of Disks (JBOD) - a storage architecture using multiple disks as a single logical volume.

**Architecture:**
- 16 disks, 256 blocks per disk, 256 bytes per block
- Total capacity: 1 MB (16 x 256 x 256)
- Linear address space mapped across all disks

**Implemented Functions:**
| Function | Description |
|----------|-------------|
| `op_create()` | Pack disk/block/command into 32-bit operation |
| `mdadm_mount()` | Initialize the disk array |
| `mdadm_unmount()` | Shut down the disk array |
| `mdadm_read()` | Read bytes from linear address |

---

## Lab 3: Extended JBOD Operations

Full read/write support with cross-boundary I/O handling.

**New Capabilities:**
- Write operations across disk/block boundaries
- Trace-based testing with linear and random access patterns
- Partial block reads/writes at arbitrary offsets

**Key Challenge:** Handling reads/writes that span multiple blocks or disks requires:
1. Calculating disk ID from linear address (`addr / 65536`)
2. Calculating block ID (`(addr % 65536) / 256`)
3. Calculating byte offset within block (`addr % 256`)

---

## Lab 4: Cache Implementation

LFU (Least Frequently Used) cache layer to reduce disk I/O.

**Cache Design:**
```
┌─────────────────────────────────────────┐
│              Cache Entry                │
├─────────────────────────────────────────┤
│ valid | disk_num | block_num | accesses │
│ block[256]                              │
└─────────────────────────────────────────┘
```

**Implemented Functions:**
| Function | Description |
|----------|-------------|
| `cache_create()` | Allocate cache with 2-4096 entries |
| `cache_destroy()` | Free cache memory |
| `cache_lookup()` | Search cache, update hit stats |
| `cache_insert()` | Add block, evict LFU if full |
| `cache_update()` | Modify existing cache entry |

**Eviction Policy:** When cache is full, evict the entry with the lowest `num_accesses` count.

---

## Lab 5: Network Storage

Client-server architecture for remote JBOD access over TCP.

**Network Protocol:**
- Client sends JBOD operations to server
- Server executes on local disk array
- Results returned over socket connection

**New Files:**
- `net.c` / `net.h` - Socket connection and communication
- `jbod_server` - Server executable

---

## Building

Each lab has its own Makefile:

```bash
cd fa22-lab[X]-ggfincke
make
./tester    # Run test suite
```

## Project Structure

```
CMPSC311_projects/
├── fa22-lab1-ggfincke/
│   ├── student.c         # Lab solutions
│   ├── tester.c          # Test harness
│   └── Makefile
├── fa22-lab2-ggfincke/
│   ├── mdadm.c           # JBOD interface
│   ├── jbod.h            # JBOD definitions
│   └── Makefile
├── fa22-lab3-ggfincke/
│   ├── mdadm.c           # Extended JBOD
│   ├── traces/           # Test traces
│   └── Makefile
├── fa22-lab4-ggfincke/
│   ├── cache.c           # LFU cache
│   ├── mdadm.c           # Cache-integrated JBOD
│   └── Makefile
└── fa22-lab5-ggfincke/
    ├── net.c             # Network client
    ├── mdadm.c           # Network-integrated JBOD
    ├── jbod_server       # Server binary
    └── Makefile
```
