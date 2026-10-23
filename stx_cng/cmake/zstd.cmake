# Private Zstandard 1.5.7 archive for stx_cng: library sources only (no
# programs, dictionary builder, deprecated or legacy formats), single
# threaded. The format uses one-shot level-3 compression, so the unused
# block-compressor strategies are compiled out; the bytes written are
# unchanged. Built with Zstandard's own conventions, not the house warnings.
set(stx_cng_zstd_lib "${STX_CNG_ZSTD_ROOT}/lib")
set(stx_cng_zstd_sources
    ${stx_cng_zstd_lib}/common/debug.c
    ${stx_cng_zstd_lib}/common/entropy_common.c
    ${stx_cng_zstd_lib}/common/error_private.c
    ${stx_cng_zstd_lib}/common/fse_decompress.c
    ${stx_cng_zstd_lib}/common/pool.c
    ${stx_cng_zstd_lib}/common/threading.c
    ${stx_cng_zstd_lib}/common/xxhash.c
    ${stx_cng_zstd_lib}/common/zstd_common.c
    ${stx_cng_zstd_lib}/compress/fse_compress.c
    ${stx_cng_zstd_lib}/compress/hist.c
    ${stx_cng_zstd_lib}/compress/huf_compress.c
    ${stx_cng_zstd_lib}/compress/zstd_compress.c
    ${stx_cng_zstd_lib}/compress/zstd_compress_literals.c
    ${stx_cng_zstd_lib}/compress/zstd_compress_sequences.c
    ${stx_cng_zstd_lib}/compress/zstd_compress_superblock.c
    ${stx_cng_zstd_lib}/compress/zstd_double_fast.c
    ${stx_cng_zstd_lib}/compress/zstd_fast.c
    ${stx_cng_zstd_lib}/compress/zstd_lazy.c
    ${stx_cng_zstd_lib}/compress/zstd_ldm.c
    ${stx_cng_zstd_lib}/compress/zstd_opt.c
    ${stx_cng_zstd_lib}/compress/zstd_preSplit.c
    ${stx_cng_zstd_lib}/compress/zstdmt_compress.c
    ${stx_cng_zstd_lib}/decompress/huf_decompress.c
    ${stx_cng_zstd_lib}/decompress/zstd_ddict.c
    ${stx_cng_zstd_lib}/decompress/zstd_decompress.c
    ${stx_cng_zstd_lib}/decompress/zstd_decompress_block.c)
set(stx_cng_zstd_definitions
    ZSTD_LEGACY_SUPPORT=0
    ZSTD_EXCLUDE_GREEDY_BLOCK_COMPRESSOR
    ZSTD_EXCLUDE_LAZY_BLOCK_COMPRESSOR
    ZSTD_EXCLUDE_LAZY2_BLOCK_COMPRESSOR
    ZSTD_EXCLUDE_BTLAZY2_BLOCK_COMPRESSOR
    ZSTD_EXCLUDE_BTOPT_BLOCK_COMPRESSOR
    ZSTD_EXCLUDE_BTULTRA_BLOCK_COMPRESSOR)
# Zstandard's x86-64 Huffman decoder is preprocessed assembly for
# GNU-compatible compilers (its own macros disable it elsewhere). Without it
# the portable C decoder runs.
if(NOT MSVC AND CMAKE_SYSTEM_PROCESSOR MATCHES "^(amd64|AMD64|x86_64|X86_64)$")
    enable_language(ASM)
    list(APPEND stx_cng_zstd_sources ${stx_cng_zstd_lib}/decompress/huf_decompress_amd64.S)
else()
    list(APPEND stx_cng_zstd_definitions ZSTD_DISABLE_ASM)
endif()
add_library(stx_cng_zstd STATIC ${stx_cng_zstd_sources})
target_compile_definitions(stx_cng_zstd PRIVATE ${stx_cng_zstd_definitions})
target_include_directories(stx_cng_zstd SYSTEM PUBLIC
    $<BUILD_INTERFACE:${stx_cng_zstd_lib}>)
set_target_properties(stx_cng_zstd PROPERTIES POSITION_INDEPENDENT_CODE ON C_VISIBILITY_PRESET hidden)
