# Link-map audit: compression and PNG implementations (2026-10-21, stage I4)

Command: `python tools/audit_link_map.py <binaries>`. The tool reads each
binary's defined symbols with `llvm-nm` and sorts them into four groups:
- owned: stx_zlib and stx_png;
- a C zlib (including Chromium's `Cr_z_` copy);
- FreeType's bundled gzip;
- libpng.

`--forbid-foreign` makes any foreign symbol fail. Stage I5 enables that
mode in CI.

## Windows x64 (local Release build, LLVM 22.1.8)

| Binary | owned | C zlib | FreeType gzip | libpng |
|---|---|---|---|---|
| `GUI.Forms Gallery.exe` | 103 | 5 (`adler32`, `crc32`, `inflate`, `inflate_fast`, `inflate_table`, all local) | 9 (`FT_Gzip_Uncompress`, `ft_gzip_*`) | 0 |
| `third_party/freetype/libfreetype.a` | 0 | 5 (the same) | 9 | 0 |
| `libgui_forms_core.a` | 4 | 0 | 0 | 0 |
| `stx_zlib/libstx_zlib.a` | 54 | 0 | 0 | 0 |
| `stx_png/libstx_png.a` | 45 | 0 | 0 | 0 |

Every C-zlib symbol in the Gallery comes from FreeType's bundled copy
(`src/gzip/ftgzip.c`), compiled with internal visibility. No external zlib
is linked, and the link lines name no `-lz` or `libz`. The PNG code is
stx_png alone: WIC was removed in I2.

Decision 3A removes FreeType's copy by compiling `FT_CONFIG_OPTION_USE_ZLIB`
out. That is stage I5, after which this binary must report zero foreign
symbols.

## macOS and Linux

Not measured locally. Until I5, the Skia archives still carry libpng and
Chromium's zlib (`Cr_z_` symbols), although no GUI.Forms path calls them
since I2 and I3. Stage I5 removes them from the Skia build and runs this
audit with `--forbid-foreign` on all four CI platforms.
