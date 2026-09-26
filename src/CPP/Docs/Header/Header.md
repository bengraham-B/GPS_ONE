# Header Files in GPS_ONE

## Section 1 — Header Files in General

### What a header file actually is

A header file (`.h` / `.hpp`) is **not compiled on its own** — it's a block
of text that gets pasted, verbatim, into any `.cpp` file that `#include`s
it, before that `.cpp` file is compiled. That's the entire mechanism:
`#include` is a literal copy-paste instruction handled by the
**preprocessor**, a step that runs before the real compiler even sees
your code.

Headers exist to solve one problem: **letting multiple `.cpp` files agree
on the shape of something** (a class, a function signature, a struct)
without duplicating that definition in every file, and without every file
needing to see every other file's implementation details. In our project,
this often means files in **different project folders** (different CMake
targets) agreeing on a shape via a shared header.

The typical split:

| File | Contains | Compiled? |
|---|---|---|
| `.h` (header) | *Declarations* — "this class/function exists, here's its shape" | No — only copy-pasted into `.cpp` files |
| `.cpp` (source) | *Definitions* — "here's what the function actually does" | Yes — this is what the compiler turns into object code |

### A minimal example

**`nmea.h`** (declaration — the "contract" for the `NMEA` project):
```cpp
#pragma once
#include <string>

class nmea {
public:
    explicit nmea(std::string message);   // constructor exists, takes a string
    NMEAResult parseNMEASentance();       // this function exists, returns NMEAResult
private:
    std::string message;                  // this data member exists
};
```

**`nmea.cpp`** (definition — the actual implementation, same `NMEA` project):
```cpp
#include "nmea.h"

nmea::nmea(std::string message) {
    this->message = message;
}

NMEAResult nmea::parseNMEASentance() {
    // ... actual parsing logic lives here ...
}
```

**`UDP_Receiver.cpp`** (a consumer, in a *different* project — `UDP/`):
```cpp
#include "nmea.h"   // gets the declaration — knows nmea exists and what it can do

void someFunction() {
    nmea NMEA(message);                      // compiler knows this constructor exists
    NMEAResult result = NMEA.parseNMEASentance(); // compiler knows this returns NMEAResult
}
```

`UDP_Receiver.cpp` never needs to see `nmea.cpp` — it only needs the
*shape* from `nmea.h` to compile. The actual implementation gets linked
in later, at the **linking** stage (after compiling, when the linker
stitches all the separately-compiled `.o` files together into one binary).
This is also why `target_link_libraries(UDP PUBLIC NMEA)` in CMake exists
separately from the header include — the header gives you the
*declarations* to compile against; the CMake link gives you the actual
*compiled code* to run against, and (because it's `PUBLIC`) also passes
`NMEA`'s include path along to `UDP`, which is what lets
`#include "nmea.h"` be found at all from inside a different project
folder.

### Include guards — why they're needed

If the same header gets `#include`d twice into the same `.cpp` file
(directly once, and again indirectly through another header — easy to
happen once you're pulling headers across project folders), the compiler
would see the same class/struct declared twice and error out. Include
guards prevent this by making the second `#include` a no-op.

Two equivalent styles — you'll see both:

```cpp
// Style 1: #pragma once — modern, simplest, supported by all major compilers
#pragma once

// Style 2: traditional guard — works everywhere, more verbose
#ifndef GPS_ONE_UDP_TESTS_H
#define GPS_ONE_UDP_TESTS_H
// ... header contents ...
#endif // GPS_ONE_UDP_TESTS_H
```

Your `UDP_tests.h` already uses Style 2. Either is fine — just pick one
per file and don't skip it.

---

## Section 2 — Linking multiple source files into one header

Sometimes you want several `.cpp` files to share **one** header, rather
than a strict 1-to-1 `file.h` / `file.cpp` pairing. This is completely
normal when the files are closely related (e.g. several small classes
that make up one logical "module"), and in our structure it happens
**within a single project folder** — it doesn't involve any other
project's `CMakeLists.txt`.

**The mechanism is nothing special** — any number of `.cpp` files can
`#include` the same `.h` file. The header just needs to declare
everything each `.cpp` file needs to know about the others.

### Example: one shared header, multiple source files

Say `NMEA/` grows to include a second file, `nmea_utils.cpp`, and you
want both `nmea.cpp` and `nmea_utils.cpp` to share declarations through
a single header instead of two separate ones — all still inside the
`NMEA` project.

**`NMEA/nmea.h`** (shared by both `.cpp` files):
```cpp
#pragma once
#include "minmea.h"

// Declared here so BOTH nmea.cpp and nmea_utils.cpp can use it
struct NMEAResult {
    bool valid = false;
    minmea_sentence_gga GGA{};
};

class nmea {
public:
    explicit nmea(std::string message);
    NMEAResult parseNMEASentance();
    minmea_sentence_gga nmeaGGA();
private:
    std::string message;
    void printGGA(const minmea_sentence_gga& GGA);
};

// A free function, implemented in nmea_utils.cpp instead of nmea.cpp,
// but declared here so anyone including nmea.h can call it
double gpsCoordinateToDecimal(minmea_float_t coord);
```

**`NMEA/nmea.cpp`** (implements the `nmea` class):
```cpp
#include "nmea.h"
// ... nmea::nmea(...), nmea::parseNMEASentance(), etc ...
```

**`NMEA/nmea_utils.cpp`** (implements the free function, same header):
```cpp
#include "nmea.h"

double gpsCoordinateToDecimal(minmea_float_t coord) {
    return minmea_tocoord(&coord);
}
```

**CMake side** — both `.cpp` files just get added to the same library's
source list; nothing else changes, and no *other* project's
`CMakeLists.txt` needs to know:
```cmake
add_library(NMEA STATIC
    nmea.cpp
    nmea_utils.cpp
)
```

Any other project that wants either the class or the free function still
just includes the **one** header and links the **one** target:
```cpp
#include "nmea.h"

nmea NMEA(message);
double lat = gpsCoordinateToDecimal(someCoord);
```
```cmake
target_link_libraries(UDP PUBLIC NMEA)   # unchanged — still just NMEA
```

This is one of the benefits of one-target-per-folder: growing a project
internally (adding files, sharing a header) never ripples out into other
projects' CMake files. `UDP` automatically gets `nmea_utils.cpp`'s
contribution the next time it's rebuilt, without anyone touching
`UDP/CMakeLists.txt`.

### When to split into one header per file instead

One shared header works well when the files are tightly related (like
above). If the files represent genuinely separate concerns, prefer
separate headers — and, following the "new project" pattern, probably a
separate project folder entirely with its own `CMakeLists.txt`. As a
rough guide: if you'd naturally describe two files as "part of the same
thing," share a header; if you'd describe them as "two different things
that happen to sit near each other," give them separate headers (and
possibly separate targets).

---

## Section 3 — Linking other C++ libraries (yours or external) inside a header

A header can `#include` another library's header exactly the same way a
`.cpp` file can. There's no special syntax — you're just choosing
**where** the include lives: in the `.h` file (so every consumer of that
header gets it too) versus in the `.cpp` file (kept private to that
implementation). In our multi-project structure, this choice also
determines what the matching `target_link_libraries` scope has to be.

### The rule specific to our structure

> **If a header in Project A includes a header from Project B, Project
> A's `target_link_libraries` line for Project B must be `PUBLIC`** —
> otherwise anything that links Project A won't inherit Project B's
> include path, and will fail to compile the moment it includes Project
> A's header.

### Example 1 — including your own project's library in a header

`UDP_Receiver.h` (in the `UDP` project) needs to reference `nmea`-related
types directly in its declarations, so the include belongs in the
**header**, and crosses into the `NMEA` project:

```cpp
// UDP/UDP_Receiver.h
#pragma once
#include "nmea.h"   // ← your own NMEA library's header, included here

class UDP_Receiver {
public:
    explicit UDP_Receiver(int port);
    std::string ReceiveMessage() const;
    void UDP_ReceiverService() const;   // uses `nmea` internally
private:
    int sockfd;
    int port;
};
```

```cmake
# UDP/CMakeLists.txt
add_library(UDP STATIC UDP_Receiver.cpp)
target_link_libraries(UDP PUBLIC NMEA)   # ← must be PUBLIC, not PRIVATE
```

Because `nmea.h` is included in `UDP_Receiver.h` (not just
`UDP_Receiver.cpp`), anything that includes `UDP_Receiver.h` transitively
gets `nmea.h` too — including `GPS_ONE` and `UDP_Tests`, neither of which
mention `NMEA` directly in their own `#include`s, but both of which
compile code that reaches `nmea.h` through `UDP_Receiver.h`. That's
exactly why `UDP`'s link to `NMEA` has to be `PUBLIC` — it has to pass
`NMEA`'s include path forward to them.

### Example 2 — including an external library in a header

Same idea, just pointing at a third-party header instead of one of your
own, and one project further out. `nmea.h` already does exactly this:

```cpp
// NMEA/nmea.h
#pragma once
#include "minmea.h"   // ← external library, included here

struct NMEAResult {
    minmea_sentence_gga GGA{};   // uses minmea's type directly, so the
                                  // include has to be in the header, not
                                  // just nmea.cpp
    bool valid = false;
};
```

```cmake
# NMEA/CMakeLists.txt
target_link_libraries(NMEA PUBLIC minmea)   # ← must be PUBLIC, not PRIVATE
```

Because the header itself uses `minmea_sentence_gga` as a member type,
every project including `nmea.h` needs `minmea.h` available too — hence
the include living at the header level, and the link being `PUBLIC`. This
also means the include path chain is two hops long:
`minmea → NMEA → UDP → GPS_ONE`/`UDP_Tests` — every link in that chain
has to stay `PUBLIC`, or the chain breaks at whichever link is `PRIVATE`
and everything further out fails to compile.

### Example 3 — when NOT to put the include in the header

If a project uses another project's code only *internally*, and those
types never appear in its own header's declarations, keep the include in
the `.cpp` file instead — and the corresponding CMake link can then stay
`PRIVATE`, since nothing downstream needs to inherit it:

```cpp
// EG_PROJECT/eg_project.h — no mention of NMEA anywhere, clean public interface
#pragma once
#include <string>

class EG_PROJECT {
public:
    std::string doSomething();
};
```

```cpp
// EG_PROJECT/eg_project.cpp — uses nmea only internally, so it includes it here
#include "eg_project.h"
#include "nmea.h"   // private implementation detail

std::string EG_PROJECT::doSomething() {
    // uses nmea here, but nothing about that leaks into the header's
    // public interface
}
```

```cmake
# EG_PROJECT/CMakeLists.txt
target_link_libraries(EG_PROJECT PRIVATE NMEA)   # PRIVATE is correct —
                                                   # nothing that links
                                                   # EG_PROJECT needs
                                                   # NMEA's headers
```

This keeps `eg_project.h` lightweight — anyone including it doesn't need
`nmea.h` at all, and nothing downstream is forced to recompile if
`NMEA`'s internals change.

### The rule to remember

> **If a type from another project appears in a header's declarations
> (as a member variable, return type, or parameter type), the `#include`
> must go in the header, and that project's `target_link_libraries` entry
> for the other project must be `PUBLIC`.**
> **If the other project is only used inside function bodies in the
> `.cpp` file, keep the `#include` in the `.cpp` file, and the
> corresponding link can stay `PRIVATE`** — it's an implementation detail
> the header's consumers don't need to know about, and nothing
> downstream should be forced to depend on it.

This mirrors the `PUBLIC` vs `PRIVATE` distinction from the CMake
reference doc directly — a header-level include *requires* a `PUBLIC`
link to work correctly across our project folders, while a `.cpp`-level
include pairs naturally with `PRIVATE`.