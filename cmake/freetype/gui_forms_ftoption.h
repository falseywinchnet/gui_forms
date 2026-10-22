/*
 * GUI.Forms' FreeType options (Z-stage decision 3A, 2026-10-19): the options
 * the pinned FreeType build generates, with its bundled zlib compiled out.
 * src/gzip/ftgzip.c then compiles to stubs returning
 * FT_Err_Unimplemented_Feature, so gzip-wrapped fonts and WOFF 1 fonts with
 * compressed tables are refused instead of being inflated by a second zlib.
 * Bundled fonts are uncompressed TTF/OTF. Selected for the freetype target
 * only, through FT_CONFIG_OPTIONS_H (see CMakeLists.txt).
 */
#ifndef GUI_FORMS_FTOPTION_H
#define GUI_FORMS_FTOPTION_H

#include <freetype/config/ftoption.h>

#undef FT_CONFIG_OPTION_USE_ZLIB
#undef FT_CONFIG_OPTION_SYSTEM_ZLIB

#endif
