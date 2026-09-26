# CMake Keyword Reference

A plain-English reference for the CMake commands and keywords used in
GPS_ONE, plus a few common ones you're likely to run into as the project grows.

---

## Project setup

### `cmake_minimum_required(VERSION x.y)`
Declares the oldest CMake version this project is written for. CMake
refuses to configure if the installed version is older than this, since
newer syntax/features might not exist yet.

### `project(NAME)`
Names the overall project and triggers CMake to detect and set up the
C/C++ compilers it'll use. Should appear once, near the top, right after
`cmake_minimum_required`.

### `set(VARIABLE value)`
Assigns a value to a CMake variable. Used constantly — e.g.
`set(CMAKE_CXX_STANDARD 26)` sets the C++ standard version for the whole
project.

### `CMAKE_CXX_STANDARD`
A built-in variable that controls which C++ standard (11, 17, 20, 26...)
the compiler is told to use.

### `CMAKE_CURRENT_SOURCE_DIR`
A built-in variable that always equals "the folder the currently-executing
`CMakeLists.txt` lives in." Used in `target_include_directories(... ${CMAKE_CURRENT_SOURCE_DIR})`
so each project's headers are findable relative to itself, no matter
where it sits in the tree.

---

## Defining targets (the "projects")

A **target** is anything CMake can build: a library or an executable.
Once defined, you refer to it by name everywhere else — never by file path.

### `add_library(NAME STATIC|SHARED source.cpp ...)`
Defines a library target compiled from the listed source files.
- **STATIC** — compiled into a `.a` file and baked directly into whatever
  links it. Simple, no runtime file to ship separately. Used throughout
  this project (`NMEA`, `UDP`, `EG_PROJECT`).
- **SHARED** — compiled into a `.so` (Linux) / `.dll` (Windows) that's
  loaded at runtime. Useful for plugins or when multiple programs share
  one copy, but not something you need for this project's structure.

You don't list header (`.h`) files here — only `.cpp` files. Headers are
found automatically via `#include` as long as the folder is on the
include path (see `target_include_directories` below).

### `add_executable(NAME source.cpp ...)`
Defines a runnable program. Used for `GPS_ONE` (the app) and `UDP_Tests`
(the test binary). Unlike libraries, nothing else can link *against* an
executable — it's an end product.

---

## Wiring targets together

### `target_include_directories(TARGET SCOPE dir)`
Adds a folder to a target's `#include` search path, so source files can
write `#include "header.h"` instead of a long relative path.

### `target_link_libraries(TARGET SCOPE dependency ...)`
Links one target against another (or against a system library like `m`
for math functions). This is how dependencies between your "projects"
are declared — e.g. `target_link_libraries(UDP PUBLIC NMEA)` means UDP
needs NMEA to build and run.

### Scope keywords: `PUBLIC` / `PRIVATE` / `INTERFACE`
These control how far a setting (an include path or a link dependency)
propagates to *other* targets that link against this one.

| Keyword | This target gets it? | Targets that link this one get it too? |
|---|---|---|
| `PUBLIC` | Yes | Yes |
| `PRIVATE` | Yes | No |
| `INTERFACE` | No | Yes |

**Rule of thumb used in this project:**
- Libraries with internal dependencies (`NMEA` needing `minmea`, `UDP`
  needing `NMEA`) use `PUBLIC`, so the dependency chain passes down
  automatically — nothing downstream has to re-declare it.
- Final executables (`GPS_ONE`, `UDP_Tests`) use `PRIVATE`, since nothing
  ever links against an executable further.
- `INTERFACE` is rare in this project — it's mainly for header-only
  libraries (no `.cpp` to compile, just settings to pass along). You may
  encounter it if you ever add a header-only dependency.

---

## Multi-directory projects

### `add_subdirectory(folder)`
Tells CMake "also read the `CMakeLists.txt` inside this folder, and
include whatever targets it defines in the overall build." This is how
the top-level file pulls in `NMEA/`, `UDP/`, `Test/`, etc.

**Order matters** — a folder must be added *after* any other folder whose
target it links against. If `EG_PROJECT` links `NMEA`, then
`add_subdirectory(NMEA)` must come before `add_subdirectory(EG_PROJECT)`.

---

## Testing

### `enable_testing()`
Turns on CMake/CTest's test-tracking machinery for the whole project.
Must be called before any `add_subdirectory(...)` that registers tests.

### `include(GoogleTest)`
Loads CMake's built-in helper functions for integrating GoogleTest,
including `gtest_discover_tests()`.

### `gtest_discover_tests(TARGET)`
Scans a built test executable and registers each individual `TEST(...)`
as its own separate entry in CTest, so `ctest` (or CLion's test runner)
can run/report them individually instead of as one lump result.

### `ctest` (command line)
The tool that actually *runs* registered tests, from a build directory
(`ctest` or `ctest --output-on-failure`). CLion's test runner UI calls
this under the hood.

---

## Fetching external dependencies

### `include(FetchContent)`
Loads CMake's module for downloading external projects' source code at
configure time, instead of requiring them to be installed system-wide.

### `FetchContent_Declare(name URL ...)`
Registers *what* you want and *where* to get it from (a URL to a zip/tar,
or a git repo + tag). Doesn't download anything yet — just records the request.

### `FetchContent_MakeAvailable(name)`
Actually performs the download (if not already cached) and runs the
fetched project's own `CMakeLists.txt`, making its targets (e.g. `gtest`,
`gtest_main` from GoogleTest) available to link against.

---

## Common ones you haven't used yet, but likely will

### `find_package(NAME)`
Looks for an *already-installed* library on the system (as opposed to
`FetchContent`, which downloads one). Common for system libraries like
OpenSSL, Boost, or Qt. You'd reach for this instead of `FetchContent` if
a dependency is expected to be installed via a package manager rather
than pulled fresh from GitHub each time.

### `option(NAME "description" ON|OFF)`
Defines a user-toggleable build switch, e.g.
`option(BUILD_TESTS "Build the test suite" ON)`. Lets you flip features
on/off at configure time without editing the CMake files, e.g.
`cmake -DBUILD_TESTS=OFF ..`.

### `if() / elseif() / else() / endif()`
Basic conditional logic inside a `CMakeLists.txt`, often paired with
`option()` — e.g. only calling `add_subdirectory(Test)` if
`BUILD_TESTS` is `ON`.

### `CMAKE_BUILD_TYPE`
Controls optimization/debug settings — common values: `Debug`, `Release`,
`RelWithDebInfo`. You've likely seen this already in CLion's build
profile dropdown ("Debug | GDB" in your screenshot) — CLion sets this
under the hood per profile.

### `install(TARGETS ... DESTINATION ...)`
Defines what gets copied where if someone runs `cmake --install`. Not
needed for local development, but relevant if you ever package/ship
GPS_ONE as a distributable build.

### `message(STATUS "text")`
Prints a message during CMake configuration — useful for debugging your
`CMakeLists.txt` files themselves (e.g. "Is this variable set to what I think?").

---

## Quick mental model

Think of each `CMakeLists.txt` as answering three questions for its folder:
1. **What am I?** (`add_library` or `add_executable`)
2. **Where are my headers?** (`target_include_directories`)
3. **What do I depend on?** (`target_link_libraries`, with `PUBLIC`/`PRIVATE` deciding whether that dependency passes further downstream)

And the top-level `CMakeLists.txt` answers one more:
4. **Which folders/projects exist, and in what order should they be processed?** (`add_subdirectory`)