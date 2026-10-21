Zig build system script for [libspng](https://libspng.org/).

Requires Zig 0.17.x. Build with `zig build --release=fast` and run the
in-memory PNG encode/decode tests with `zig build test --release=fast`.
Both commands support `-Dlinkage=static` (the default) or `-Dlinkage=dynamic`.
When linking statically on Windows, define `SPNG_STATIC` before including `spng.h`.

Run the optional upstream comparison suite against a PNG file with
`zig build test:testsuite -Dlibpng -- path/to/image.png`.

This build script is licensed under the same BSD 2-Clause License as libspng.
