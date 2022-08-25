# mzgl-starter
starter template for MZGL projects

To use, clone it, then also clone mzgl, and at the top of the CMakeLists.txt, change the line 

```
set(MZGL_ROOT "../koala/lib/mzgl")
```

to point to the mzgl folder you created.


to build: 

```
cmake -Bbuild -GNinja
cmake --build build --config Release

```