# Testing in GPS_ONE

This doc covers C++ testing in general, then GoogleTest specifically, all
framed around our actual folder-per-project structure
(`NMEA/`, `UDP/`, `external/minmea/`, `Test/UDP_Tests/`).

---

## Section 1 — Testing in C++: a bit of background

Unlike some languages, C++ has **no built-in testing framework** — there's
no equivalent of Python's `unittest` or Java's JUnit baked into the
standard library. Testing is always done via a **third-party framework**
that you pull in as a dependency, compile into its own executable, and
run separately from your actual application.

### Why tests live in their own executable, not inside the app

A test suite is, functionally, a **second `main()`** — one that calls
into your code and checks the results, instead of running your program
for real (e.g. instead of `UDP_Receiver` actually opening a socket and
listening forever). Because C++ only allows one `main()` per executable,
tests can never live inside the same binary as your actual app — they
need their own `add_executable(...)` target entirely. This is exactly
why our structure has `UDP_Tests` as its own executable, separate from
`GPS_ONE`.

### Why this is *easier*, not harder, with our folder-per-project structure

This is the main payoff of splitting `NMEA`, `UDP`, and `minmea` into
their own **libraries** rather than compiling everything straight into
the `GPS_ONE` executable: a test executable can link against those
libraries directly and call their real code, without needing to link
(or duplicate) `main.cpp` or spin up an actual UDP socket. Testing
`NMEA`'s parsing logic in isolation only works cleanly *because* `NMEA`
is its own target with its own compiled `.a` file that anything —
`GPS_ONE`, or `UDP_Tests` — can link against independently.

### Common C++ testing frameworks (context)

| Framework | Notes |
|---|---|
| **GoogleTest (gtest)** | Most widely used; what we're using. Rich assertion macros, test fixtures, good IDE integration (including CLion). |
| **Catch2** | Header-only-friendly, simpler syntax, popular for smaller projects. |
| **doctest** | Similar to Catch2, extremely lightweight, near-zero compile overhead. |
| Plain `assert()` | No framework at all — works, but gives no pass/fail reporting, no test discovery, and stops at the first failure. |

We're using GoogleTest, so the rest of this doc focuses there.

---

## Section 2 — Google Tests

### [2.1] Setting up GoogleTest

We don't install GoogleTest system-wide — we pull its source directly
into the build using CMake's `FetchContent`, in the **top-level**
`CMakeLists.txt`:

```cmake
# top-level CMakeLists.txt
include(FetchContent)

FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
)
FetchContent_MakeAvailable(googletest)
```

This downloads GoogleTest (cached after the first run) and makes two
targets available project-wide: `gtest` (the framework itself) and
`gtest_main` (a ready-made `main()` that discovers and runs all your
`TEST(...)` blocks, so you don't write your own test runner).

Because this happens at the top level, **every** project folder
(`Test/`, or any future test folder) can link against `gtest`/
`gtest_main` without repeating the `FetchContent` setup.

### [2.2] Making a basic test

A GoogleTest test is written with the `TEST(GroupName, TestName)` macro,
plus assertion macros like `EXPECT_EQ`, `EXPECT_TRUE`, `ASSERT_EQ`, etc.

```cpp
#include <gtest/gtest.h>

TEST(BasicMath, AdditionWorks) {
    EXPECT_EQ(2 + 2, 4);
}

TEST(BasicMath, SubtractionWorks) {
    EXPECT_EQ(5 - 3, 2);
}
```

- **`EXPECT_*`** — records a failure but keeps running the rest of the test.
- **`ASSERT_*`** — stops that test immediately on failure (use when
  continuing would be meaningless, e.g. a null pointer check before
  dereferencing).

You don't need to write `main()` yourself — that's what `gtest_main`
provides, which is why we link it in `Test/CMakeLists.txt`.

### [2.3] Running tests, especially in CLion

Once the project is configured (and `gtest_discover_tests` has run — see
2.4), CLion shows each `TEST(...)` with a small green run-arrow gutter
icon next to it, the same as it does for `main()`. You can:

- Click the gutter icon next to an individual `TEST(...)` to run just that one.
- Right-click `UDP_Tests` in the CMake target list (or the file itself) → **Run 'UDP_Tests'** to run the whole suite.
- Use the test-results panel that opens after running — it shows pass/fail per test, with a red/green breakdown, and lets you re-run only failed tests.

From the command line (from your build directory, e.g.
`cmake-build-debug/`), the equivalent is:

```bash
ctest --output-on-failure
```

`--output-on-failure` prints the actual assertion failure text instead of
just pass/fail — worth always including.

### [2.4] Example with reference to our application structure

Say we want to test `nmea::parseNMEASentance()` from the `NMEA` project.
Because `NMEA` is its own library target, `UDP_Tests` can link it
directly and call real `nmea` code — no socket, no `main.cpp`, no
`UDP_Receiver` involved at all:

```cpp
// Test/UDP_Tests/NMEA_Test.cpp
#include <gtest/gtest.h>
#include "nmea.h"   // comes from the NMEA project — see Section 4 for how this resolves

TEST(NmeaParsing, RejectsInvalidChecksum) {
    nmea NMEA("$GPGGA,garbage,invalid,checksum*00");
    NMEAResult result = NMEA.parseNMEASentance();
    EXPECT_FALSE(result.valid);
}

TEST(NmeaParsing, AcceptsValidGGASentence) {
    nmea NMEA("$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47");
    NMEAResult result = NMEA.parseNMEASentance();
    EXPECT_TRUE(result.valid);
}
```

This works because `nmea` is fully self-contained logic (no socket, no
`while(true)` loop) — exactly the kind of code that's straightforward to
unit test in isolation, unlike `UDP_Receiver::UDP_ReceiverService()`,
which is tightly coupled to a live socket and would need to be refactored
(as discussed earlier, e.g. extracting `SentenceCounter`) before it could
be tested this cleanly.

---

## Section 3 — Including multiple tests in a dir in one header file

Just like production code (see the Header Files doc, Section 2), multiple
test `.cpp` files can share one header — useful for shared test helpers,
fixtures, or sample data reused across several test files in the same
folder.

### [3.1] Example with reference to our application structure

Say `Test/UDP_Tests/` grows to have both `Counter_Test.cpp` and a new
`NMEA_Test.cpp`, and both want to reuse the same sample NMEA sentences
instead of each hardcoding their own strings.

**`Test/UDP_Tests/UDP_tests.h`** (shared by every test file in this folder):
```cpp
#ifndef GPS_ONE_UDP_TESTS_H
#define GPS_ONE_UDP_TESTS_H

// Shared sample data, reused across multiple test files in this folder
inline const char* VALID_GGA_SAMPLE =
    "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47";

inline const char* INVALID_GGA_SAMPLE =
    "$GPGGA,garbage,invalid,checksum*00";

#endif //GPS_ONE_UDP_TESTS_H
```

**`Test/UDP_Tests/Counter_Test.cpp`**:
```cpp
#include <gtest/gtest.h>
#include "UDP_tests.h"
#include "nmea.h"

TEST(Counter, IncrementsOnValidSentence) {
    nmea NMEA(VALID_GGA_SAMPLE);
    // ... test counting logic here ...
}
```

**`Test/UDP_Tests/NMEA_Test.cpp`** (new file, same shared header):
```cpp
#include <gtest/gtest.h>
#include "UDP_tests.h"
#include "nmea.h"

TEST(NmeaParsing, AcceptsValidGGASentence) {
    nmea NMEA(VALID_GGA_SAMPLE);
    NMEAResult result = NMEA.parseNMEASentance();
    EXPECT_TRUE(result.valid);
}

TEST(NmeaParsing, RejectsInvalidChecksum) {
    nmea NMEA(INVALID_GGA_SAMPLE);
    NMEAResult result = NMEA.parseNMEASentance();
    EXPECT_FALSE(result.valid);
}
```

Both files stay consistent (one sample sentence to update instead of two)
and both get picked up the same way in `Test/CMakeLists.txt` — just add
the new `.cpp` to `add_executable(...)`'s source list, same as adding any
new file to an existing project (see the CMake guide, Section 2):

```cmake
add_executable(UDP_Tests
    UDP_Tests/Counter_Test.cpp
    UDP_Tests/NMEA_Test.cpp   # ← new
)
```

---

## Section 4 — How each dir and its tests are included in the project's `CMakeLists.txt`

This ties directly back to the CMake reference doc — testing in our
project isn't a special case, it follows the exact same
target-per-folder pattern as `NMEA` and `UDP`, just producing an
executable instead of a library, and pulling in `gtest`/`gtest_main` as
extra dependencies.

### [4.1] Example with reference to our application structure

**The chain, folder by folder:**

```
external/minmea/   → library "minmea"
NMEA/               → library "NMEA"    (links minmea, PUBLIC)
UDP/                → library "UDP"     (links NMEA, PUBLIC)
Test/UDP_Tests/     → executable "UDP_Tests" (links UDP, NMEA, gtest, gtest_main)
```

**`Test/CMakeLists.txt`**:
```cmake
# Builds the test executable itself
add_executable(UDP_Tests
    UDP_Tests/Counter_Test.cpp
    UDP_Tests/NMEA_Test.cpp
)

# Lets test files do #include "UDP_tests.h" without a relative path
target_include_directories(UDP_Tests PRIVATE UDP_Tests)

# Everything the tests need to compile and run:
target_link_libraries(UDP_Tests
    PRIVATE
        UDP        # the code under test — transitively pulls in NMEA + minmea too
        NMEA       # listed explicitly too, since test files include "nmea.h" directly
        gtest      # the GoogleTest framework
        gtest_main # provides main() automatically
)

# Registers each TEST(...) individually with CTest/CLion's test runner
include(GoogleTest)
gtest_discover_tests(UDP_Tests)
```

This is why `#include "nmea.h"` and `#include "UDP_Receiver.h"` both
resolve correctly inside `Counter_Test.cpp`/`NMEA_Test.cpp`, exactly as
explained in the Header Files doc: `UDP_Tests` directly links `NMEA`
(`PRIVATE` is fine here — `UDP_Tests` is a final executable, nothing
links against it further, same reasoning as `GPS_ONE`).

**Top-level `CMakeLists.txt`** — the piece that ties `Test/` into the
overall build, and where `gtest`/`gtest_main` themselves come from:
```cmake
enable_testing()          # must come before add_subdirectory(Test)
                           # so gtest_discover_tests() has CTest ready
add_subdirectory(Test)    # pulls in Test/CMakeLists.txt, same as any
                           # other project folder (NMEA, UDP, etc.)
```

**The full picture, start to finish:**
1. Top-level `FetchContent_MakeAvailable(googletest)` makes `gtest`/`gtest_main` targets exist, project-wide.
2. `enable_testing()` turns on CTest tracking before any test target is processed.
3. `add_subdirectory(Test)` reads `Test/CMakeLists.txt`, which defines the `UDP_Tests` executable and links it against `UDP`, `NMEA`, `gtest`, and `gtest_main`.
4. `gtest_discover_tests(UDP_Tests)` scans the built `UDP_Tests` binary and registers each `TEST(...)` as its own CTest entry.
5. Running `ctest`, or clicking the gutter icons in CLion, executes those registered tests and reports pass/fail individually.

Nothing about this differs, structurally, from how `UDP` links `NMEA` to
build the real app — the test executable is just one more target in the
same dependency graph, sitting alongside `GPS_ONE` rather than replacing
it.