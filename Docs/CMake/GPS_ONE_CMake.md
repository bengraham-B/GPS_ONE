# GPS_ONE — Adding New Projects & Files to the CMake Build

This project is structured like separate C#-style "projects" — each folder
(`NMEA/`, `UDP/`, `external/minmea/`, `Test/`) is its own CMake **library
target** with its own `CMakeLists.txt`. `main.cpp` just references them,
the way `Program.cs` would reference other project assemblies.

This guide covers two common tasks:
1. Adding a **brand new project/module** (folder + library), using an example called `EG_PROJECT`.
2. Adding a **new file inside an existing project** that's already hooked up.

---

## 1. Adding a new project (e.g. `EG_PROJECT`)

### Step 1 — Create the folder and source files

```
GPS_ONE/
└── EG_PROJECT/
    ├── CMakeLists.txt
    ├── eg_project.cpp
    └── eg_project.h
```

### Step 2 — Write `EG_PROJECT/CMakeLists.txt`

```cmake
# Create a library target named "EG_PROJECT", compiled from eg_project.cpp.
# STATIC = compiled into a .a file and baked into whatever links against it.
add_library(EG_PROJECT STATIC eg_project.cpp)

# Let anything that links EG_PROJECT do #include "eg_project.h" without
# needing to know this folder's path.
# PUBLIC = both this target AND anything that links it inherit the setting.
target_include_directories(EG_PROJECT PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# If EG_PROJECT depends on another module (e.g. it calls into NMEA),
# link it here. PUBLIC means anything that links EG_PROJECT automatically
# gets NMEA (and everything NMEA itself PUBLIC-links) passed along too —
# this is a "transitive dependency."
# Delete this line entirely if EG_PROJECT has no internal dependencies.
target_link_libraries(EG_PROJECT PUBLIC NMEA)
```

> **PUBLIC vs PRIVATE, quick reminder:**
> `PUBLIC` = "I need this, and so does anyone who links me."
> `PRIVATE` = "I need this, but it's my own business — don't pass it downstream."
> Use `PUBLIC` on libraries with dependencies other code needs transitively.
> Use `PRIVATE` on the final executable (`GPS_ONE`, `UDP_Tests`), since nothing links an executable further.

### Step 3 — Hook it into the top-level `CMakeLists.txt`

Add **one line** to pull the new folder into the build:

```cmake
add_subdirectory(external/minmea)
add_subdirectory(NMEA)
add_subdirectory(UDP)
add_subdirectory(EG_PROJECT)   # <-- new
```

> **Order matters.** If `EG_PROJECT` depends on `NMEA` (as in the example
> above), `add_subdirectory(NMEA)` must appear **before**
> `add_subdirectory(EG_PROJECT)` — CMake needs to know the `NMEA` target
> exists before `EG_PROJECT/CMakeLists.txt` tries to link against it.

### Step 4 — Link it wherever it's actually used

If `GPS_ONE` (the app) itself needs `EG_PROJECT`:

```cmake
target_link_libraries(GPS_ONE PRIVATE UDP EG_PROJECT)
```

If only the tests need it:

```cmake
target_link_libraries(UDP_Tests
    PRIVATE
        UDP
        NMEA
        EG_PROJECT   # <-- new
        gtest
        gtest_main
)
```

You only need to link it **where it's directly used** — if `UDP` already
links `EG_PROJECT` PUBLIC-ly, anything that links `UDP` gets it for free.

### Checklist for adding a new project
- [ ] New folder with source files
- [ ] New `CMakeLists.txt` inside it (`add_library`, `target_include_directories`, `target_link_libraries` if it depends on other modules)
- [ ] `add_subdirectory(YourFolder)` added to top-level `CMakeLists.txt`, in the correct dependency order
- [ ] `target_link_libraries(...)` updated on whichever target(s) actually use it (`GPS_ONE`, `UDP_Tests`, or another library)
- [ ] Reload/reconfigure CMake (CLion does this automatically, or re-run the CMake step manually)

---

## 2. Adding a new file inside an existing project (e.g. `NMEA/`)

This is much simpler — you're adding a `.cpp`/`.h` pair to a folder that
**already has its own `CMakeLists.txt`**.

### Step 1 — Create the files

```
NMEA/
├── CMakeLists.txt
├── nmea.cpp
├── nmea.h
├── nmea_utils.cpp   # <-- new
└── nmea_utils.h      # <-- new
```

### Step 2 — Add the new `.cpp` to the existing `add_library(...)` call

Open `NMEA/CMakeLists.txt` and add the new source file to the list:

```cmake
add_library(NMEA STATIC
    nmea.cpp
    nmea_utils.cpp   # <-- new
)
```

That's it for headers too — **you don't need to list `.h` files** in
`add_library()`. As long as `nmea_utils.h` is `#include`d by a `.cpp` file
that's already part of the target, CMake and your compiler find it
automatically via the `target_include_directories` already set up for
`NMEA` (since it points at the whole folder).

### Step 3 — Nothing else to do

Because `NMEA/CMakeLists.txt` already defines all its include paths and
link requirements once, and everything else in the project links against
the `NMEA` **target** (not individual files), no other `CMakeLists.txt`
needs to change. `UDP`, `GPS_ONE`, and `UDP_Tests` all automatically pick
up the new file the next time the project reconfigures/builds.

### Checklist for adding a file to an existing project
- [ ] New `.cpp`/`.h` created inside the existing project folder
- [ ] New `.cpp` file added to that folder's existing `add_library(...)` source list
- [ ] Headers `#include`d correctly — no CMake changes needed for them
- [ ] Reload/reconfigure CMake

---

## Quick reference: the two scenarios side by side

| | New project (`EG_PROJECT`) | New file in existing project |
|---|---|---|
| New folder needed? | Yes | No |
| New `CMakeLists.txt` needed? | Yes | No |
| Top-level `CMakeLists.txt` changes? | Yes — add `add_subdirectory(...)` | No |
| `target_link_libraries(...)` changes elsewhere? | Yes — wherever it's used | No |
| Changes inside the project's own `CMakeLists.txt`? | N/A (it's new) | Yes — add `.cpp` to `add_library(...)` list |