# Git Submodules

## GitHub Repo
 - ```https://github.com/kosma/minmea.git```
<hr>

## Adding minmea as a SubModule

### Command to Add Submodule
```shell
git submodule add https://github.com/kosma/minmea.git external/minmea
```

### Project Structure after adding Submodule
```
your-project/
├── CMakeLists.txt
├── main.cpp
├── .gitmodules
├── external/minmea/
│   ├── minmea.c
│   ├── minmea.h
│   └── ...
```

### Update CMakeLists.txt
```cmake
cmake_minimum_required(VERSION 3.26)
project(your_project)

set(CMAKE_CXX_STANDARD 17)

add_executable(your_project
    main.cpp
    external/minmea/minmea.c
)

target_include_directories(your_project PRIVATE external/minmea)
target_link_libraries(your_project m)
```

<hr/>

## Fetching/Pulling Updates

### Get the latest version
```shell
git submodule update --remote --merge
```

### Commit it
```shell
git add external/minmea
git commit -m "Update minmea submodule to latest"
```

### Note
 - A git submodule is not tracked as normal files in the project, the parent repo just stores a pointer to the specific commit hash of the submodule.
 - Updating it ```git submodule update --remote --merge```, this just moves the pointer forward to the new commit but, this is just a change in the working directory.
 - At this point the parent repo does not know about it which requires it to be staged and commited, just the same as a file in the parent project.

1. ```git submodule update --remote --merge → updates the pointer locally```
2. ```git add external/minmea → stages the new pointer value```
3. ```git commit → records it in your project's history```

<hr>

## Further Understanding

### Making edits in the submodule
 - The parent repo does not look inside of the submodule file contents, it only tracks which commit hash the submodule is pointed to.
 - If you edit a file inside of ```external/minmea``` but actually do not commit it inside og the submodule, the parent at this point will just see the sub module as having uncommited changes (dirty) but no new commit has t point to.
 - Therefore, running ```git add external/minmea & git commit -m ""``` in the parent directory will not save the changes you made.
   - They just remain local changes in the submodule. 

### Correct Sequence if you make changes inside of the submodule
```shell
# 1. Go into the submodule
cd external/minmea

# 2. Commit your change there first
git add minmea.c
git commit -m "Local patch to minmea"

# 3. Go back to the parent project
cd ../..

# 4. Now the parent can see the submodule points to a new commit
git add external/minmea
git commit -m "Update minmea submodule pointer (local patch)"
```

### Edit submodule in a new Branch
- When you git submodule add or git submodule update, the submodule is often left in a detached HEAD state (not on a branch). If you commit while detached and then switch commits without pushing/branching, you can lose that commit entirely — it won't be on any branch and can eventually get garbage collected.

#### To safely edit minmea locally, checkout a branch first:

```shell
cd external/minmea
git checkout -b local-patch    # or checkout master if you intend to track upstream
# ...make your edit...
git add minmea.c
git commit -m "Local patch"
cd ../..
git add external/minmea
git commit -m "Point to patched minmea"
```

<hr>