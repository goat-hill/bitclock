/*******************************************************************************
 * Size: 16 px
 * Bpp: 1
 * Opts: --bpp 1 --format lvgl --font Overpass/Overpass-SemiBold.ttf --symbols
 *0123456789ABCDEFGHIJGKLMNOPQRSTUVWXYZ· ₂ₓ --size 16 --output sublabel.c
 ******************************************************************************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#ifndef SUBLABEL
#define SUBLABEL 1
#endif

#if SUBLABEL

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */
    0x0,

    /* U+0030 "0" */
    0x3c, 0x7e, 0x66, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0x66, 0x7e, 0x3c,

    /* U+0031 "1" */
    0x3f, 0xf3, 0x33, 0x33, 0x33, 0x33,

    /* U+0032 "2" */
    0x3c, 0x7e, 0xe3, 0x3, 0x3, 0x6, 0x1c, 0x38, 0x60, 0xc0, 0xff, 0xff,

    /* U+0033 "3" */
    0x39, 0xff, 0x18, 0x30, 0x67, 0x8f, 0x3, 0x7, 0x8f, 0xf3, 0xc0,

    /* U+0034 "4" */
    0x3, 0x3, 0x83, 0xc3, 0x63, 0x31, 0x99, 0x8d, 0xff, 0xff, 0x81, 0x80, 0xc0,
    0x60,

    /* U+0035 "5" */
    0x7e, 0x7e, 0x60, 0x40, 0x5c, 0xfe, 0x67, 0x3, 0x3, 0x47, 0xfe, 0x7c,

    /* U+0036 "6" */
    0x0, 0x1c, 0x38, 0x70, 0x60, 0xdc, 0xfe, 0xe7, 0xc3, 0xc3, 0xe7, 0x7e, 0x3c,

    /* U+0037 "7" */
    0xff, 0xfc, 0x18, 0x60, 0x83, 0x6, 0x1c, 0x30, 0x60, 0xc1, 0x80,

    /* U+0038 "8" */
    0x7c, 0xfe, 0xc6, 0xc6, 0x7c, 0x7e, 0xe7, 0xc3, 0xc3, 0xe7, 0x7e, 0x3c,

    /* U+0039 "9" */
    0x3c, 0x7e, 0xe7, 0xc3, 0xc3, 0xe7, 0x7f, 0x3b, 0x6, 0x6, 0x1c, 0x38, 0x20,

    /* U+0041 "A" */
    0xc, 0x7, 0x1, 0xc0, 0x58, 0x36, 0xc, 0x86, 0x31, 0xfc, 0x7f, 0x30, 0x6c,
    0x1b, 0x3,

    /* U+0042 "B" */
    0xfe, 0x7f, 0xb0, 0xd8, 0x6c, 0x37, 0xf3, 0xfd, 0x83, 0xc1, 0xe0, 0xff,
    0xdf, 0xc0,

    /* U+0043 "C" */
    0x1e, 0x3f, 0x98, 0xfc, 0xc, 0x6, 0x3, 0x1, 0x80, 0xe0, 0x31, 0xdf, 0xc3,
    0xc0,

    /* U+0044 "D" */
    0xfc, 0x7f, 0x30, 0xd8, 0x7c, 0x1e, 0xf, 0x7, 0x83, 0xc3, 0xe1, 0xbf, 0x9f,
    0x80,

    /* U+0045 "E" */
    0xff, 0xff, 0xc0, 0xc0, 0xc0, 0xf8, 0xf8, 0xc0, 0xc0, 0xc0, 0xff, 0xff,

    /* U+0046 "F" */
    0xff, 0xff, 0x6, 0xc, 0x1f, 0xbf, 0x60, 0xc1, 0x83, 0x6, 0x0,

    /* U+0047 "G" */
    0x1e, 0x3f, 0x98, 0xfc, 0xc, 0x6, 0x3, 0x1f, 0x8f, 0xe1, 0xb1, 0xdf, 0xc3,
    0xc0,

    /* U+0048 "H" */
    0xc1, 0xe0, 0xf0, 0x78, 0x3c, 0x1f, 0xff, 0xff, 0x83, 0xc1, 0xe0, 0xf0,
    0x78, 0x30,

    /* U+0049 "I" */
    0xff, 0xff, 0xff,

    /* U+004A "J" */
    0x6, 0xc, 0x18, 0x30, 0x60, 0xc1, 0x83, 0x7, 0x9f, 0xf3, 0xc0,

    /* U+004B "K" */
    0xc3, 0x63, 0x33, 0x99, 0x8d, 0x87, 0xe3, 0xf1, 0xcc, 0xc6, 0x61, 0xb0,
    0xd8, 0x30,

    /* U+004C "L" */
    0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xff, 0xff,

    /* U+004D "M" */
    0xc0, 0xf8, 0x3e, 0x1f, 0x87, 0xf3, 0xfc, 0xfd, 0x2f, 0x7b, 0xcc, 0xf3,
    0x3c, 0xcf, 0x23,

    /* U+004E "N" */
    0xc1, 0xf0, 0xf8, 0x7e, 0x3f, 0x9e, 0xcf, 0x37, 0x9f, 0xc7, 0xe1, 0xf0,
    0xf8, 0x30,

    /* U+004F "O" */
    0x1e, 0x1f, 0xe6, 0x1b, 0x87, 0xc0, 0xf0, 0x3c, 0xf, 0x3, 0xe1, 0xd8, 0x67,
    0xf8, 0x78,

    /* U+0050 "P" */
    0xfe, 0xfe, 0xc3, 0xc3, 0xc3, 0xfe, 0xfc, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,

    /* U+0051 "Q" */
    0x1e, 0x1f, 0xe6, 0x1b, 0x87, 0xc0, 0xf0, 0x3c, 0xf, 0x3, 0xe7, 0xd8, 0xe7,
    0xf0, 0x7e, 0x1, 0x80,

    /* U+0052 "R" */
    0xfe, 0x7f, 0xb0, 0x78, 0x3c, 0x1f, 0xfb, 0xf9, 0x8c, 0xc6, 0x61, 0xb0,
    0xd8, 0x70,

    /* U+0053 "S" */
    0x3c, 0xfe, 0xc6, 0xc0, 0xe0, 0x7c, 0xe, 0x3, 0x43, 0xc3, 0xfe, 0x3c,

    /* U+0054 "T" */
    0xff, 0xff, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,

    /* U+0055 "U" */
    0xc1, 0xe0, 0xf0, 0x78, 0x3c, 0x1e, 0xf, 0x7, 0x83, 0xc1, 0xf1, 0xdf, 0xc7,
    0xc0,

    /* U+0056 "V" */
    0xc1, 0xe0, 0xf0, 0xcc, 0x66, 0x33, 0x30, 0xd8, 0x6c, 0x34, 0xe, 0x7, 0x3,
    0x0,

    /* U+0057 "W" */
    0xc6, 0x3c, 0x63, 0xce, 0x6c, 0xe6, 0x4f, 0x66, 0xb6, 0x7b, 0x67, 0x94,
    0x79, 0xc7, 0x1c, 0x31, 0xc3, 0xc,

    /* U+0058 "X" */
    0xc3, 0x31, 0x99, 0x87, 0xc3, 0xc0, 0xc0, 0x70, 0x78, 0x36, 0x33, 0x38,
    0xd8, 0x70,

    /* U+0059 "Y" */
    0xc0, 0xd8, 0x66, 0x18, 0xcc, 0x3f, 0x7, 0x80, 0xc0, 0x30, 0xc, 0x3, 0x0,
    0xc0, 0x30,

    /* U+005A "Z" */
    0xff, 0xff, 0x6, 0x6, 0xc, 0x18, 0x18, 0x30, 0x60, 0x40, 0xff, 0xff,

    /* U+00B7 "·" */
    0xf0,

    /* U+2082 "₂" */
    0x69, 0x36, 0xcf,

    /* U+2093 "ₓ" */
    0xcd, 0x67, 0xc, 0x73, 0x6c, 0xc0};

/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0,
     .adv_w = 0,
     .box_w = 0,
     .box_h = 0,
     .ofs_x = 0,
     .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0,
     .adv_w = 63,
     .box_w = 1,
     .box_h = 1,
     .ofs_x = 0,
     .ofs_y = 0},
    {.bitmap_index = 1,
     .adv_w = 167,
     .box_w = 8,
     .box_h = 12,
     .ofs_x = 1,
     .ofs_y = 0},
    {.bitmap_index = 13,
     .adv_w = 100,
     .box_w = 4,
     .box_h = 12,
     .ofs_x = 1,
     .ofs_y = 0},
    {.bitmap_index = 19,
     .adv_w = 157,
     .box_w = 8,
     .box_h = 12,
     .ofs_x = 1,
     .ofs_y = 0},
    {.bitmap_index = 31,
     .adv_w = 154,
     .box_w = 7,
     .box_h = 12,
     .ofs_x = 1,
     .ofs_y = 0},
    {.bitmap_index = 42,
     .adv_w = 163,
     .box_w = 9,
     .box_h = 12,
     .ofs_x = 1,
     .ofs_y = 0},
    {.bitmap_index = 56,
     .adv_w = 157,
     .box_w = 8,
     .box_h = 12,
     .ofs_x = 1,
     .ofs_y = 0},
    {.bitmap_index = 68,
     .adv_w = 156,
     .box_w = 8,
     .box_h = 13,
     .ofs_x = 1,
     .ofs_y = 0},
    {.bitmap_index = 81,
     .adv_w = 136,
     .box_w = 7,
     .box_h = 12,
     .ofs_x = 1,
     .ofs_y = 0},
    {.bitmap_index = 92,
     .adv_w = 159,
     .box_w = 8,
     .box_h = 12,
     .ofs_x = 1,
     .ofs_y = 0},
    {.bitmap_index = 104,
     .adv_w = 156,
     .box_w = 8,
     .box_h = 13,
     .ofs_x = 1,
     .ofs_y = -1},
    {.bitmap_index = 117,
     .adv_w = 180,
     .box_w = 10,
     .box_h = 12,
     .ofs_x = 1,
     .ofs_y = 0},
    {.bitmap_index = 132,
     .adv_w = 174,
     .box_w = 9,
     .box_h = 12,
     .ofs_x = 1,
     .ofs_y = 0},
    {.bitmap_index = 146,
     .adv_w = 164,
     .box_w = 9,
     .box_h = 12,
     .ofs_x = 1,
     .ofs_y = 0},
    {.bitmap_index = 160,
     .adv_w = 179,
     .box_w = 9,
     .box_h = 12,
     .ofs_x = 1,
     .ofs_y = 0},
    {.bitmap_index = 174,
     .adv_w = 158,
     .box_w = 8,
     .box_h = 12,
     .ofs_x = 1,
     .ofs_y = 0},
    {.bitmap_index = 186,
     .adv_w = 146,
     .box_w = 7,
     .box_h = 12,
     .ofs_x = 1,
     .ofs_y = 0},
    {.bitmap_index = 197,
     .adv_w = 175,
     .box_w = 9,
     .box_h = 12,
     .ofs_x = 1,
     .ofs_y = 0},
    {.bitmap_index = 211,
     .adv_w = 183,
     .box_w = 9,
     .box_h = 12,
     .ofs_x = 1,
     .ofs_y = 0},
    {.bitmap_index = 225,
     .adv_w = 75,
     .box_w = 2,
     .box_h = 12,
     .ofs_x = 1,
     .ofs_y = 0},
    {.bitmap_index = 228,
     .adv_w = 142,
     .box_w = 7,
     .box_h = 12,
     .ofs_x = 1,
     .ofs_y = 0},
    {.bitmap_index = 239,
     .adv_w = 177,
     .box_w = 9,
     .box_h = 12,
     .ofs_x = 1,
     .ofs_y = 0},
    {.bitmap_index = 253,
     .adv_w = 144,
     .box_w = 8,
     .box_h = 12,
     .ofs_x = 1,
     .ofs_y = 0},
    {.bitmap_index = 265,
     .adv_w = 207,
     .box_w = 10,
     .box_h = 12,
     .ofs_x = 1,
     .ofs_y = 0},
    {.bitmap_index = 280,
     .adv_w = 183,
     .box_w = 9,
     .box_h = 12,
     .ofs_x = 1,
     .ofs_y = 0},
    {.bitmap_index = 294,
     .adv_w = 186,
     .box_w = 10,
     .box_h = 12,
     .ofs_x = 1,
     .ofs_y = 0},
    {.bitmap_index = 309,
     .adv_w = 163,
     .box_w = 8,
     .box_h = 12,
     .ofs_x = 1,
     .ofs_y = 0},
    {.bitmap_index = 321,
     .adv_w = 186,
     .box_w = 10,
     .box_h = 13,
     .ofs_x = 1,
     .ofs_y = -1},
    {.bitmap_index = 338,
     .adv_w = 174,
     .box_w = 9,
     .box_h = 12,
     .ofs_x = 1,
     .ofs_y = 0},
    {.bitmap_index = 352,
     .adv_w = 156,
     .box_w = 8,
     .box_h = 12,
     .ofs_x = 1,
     .ofs_y = 0},
    {.bitmap_index = 364,
     .adv_w = 153,
     .box_w = 8,
     .box_h = 12,
     .ofs_x = 1,
     .ofs_y = 0},
    {.bitmap_index = 376,
     .adv_w = 180,
     .box_w = 9,
     .box_h = 12,
     .ofs_x = 1,
     .ofs_y = 0},
    {.bitmap_index = 390,
     .adv_w = 172,
     .box_w = 9,
     .box_h = 12,
     .ofs_x = 1,
     .ofs_y = 0},
    {.bitmap_index = 404,
     .adv_w = 216,
     .box_w = 12,
     .box_h = 12,
     .ofs_x = 1,
     .ofs_y = 0},
    {.bitmap_index = 422,
     .adv_w = 167,
     .box_w = 9,
     .box_h = 12,
     .ofs_x = 1,
     .ofs_y = 0},
    {.bitmap_index = 436,
     .adv_w = 171,
     .box_w = 10,
     .box_h = 12,
     .ofs_x = 0,
     .ofs_y = 0},
    {.bitmap_index = 451,
     .adv_w = 165,
     .box_w = 8,
     .box_h = 12,
     .ofs_x = 1,
     .ofs_y = 0},
    {.bitmap_index = 463,
     .adv_w = 61,
     .box_w = 2,
     .box_h = 2,
     .ofs_x = 1,
     .ofs_y = 5},
    {.bitmap_index = 464,
     .adv_w = 94,
     .box_w = 4,
     .box_h = 6,
     .ofs_x = 1,
     .ofs_y = -2},
    {.bitmap_index = 467,
     .adv_w = 121,
     .box_w = 6,
     .box_h = 7,
     .ofs_x = 1,
     .ofs_y = -3}};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/

static const uint16_t unicode_list_3[] = {0x0, 0x1fcb, 0x1fdc};

/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] = {
    {.range_start = 32,
     .range_length = 1,
     .glyph_id_start = 1,
     .unicode_list = NULL,
     .glyph_id_ofs_list = NULL,
     .list_length = 0,
     .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY},
    {.range_start = 48,
     .range_length = 10,
     .glyph_id_start = 2,
     .unicode_list = NULL,
     .glyph_id_ofs_list = NULL,
     .list_length = 0,
     .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY},
    {.range_start = 65,
     .range_length = 26,
     .glyph_id_start = 12,
     .unicode_list = NULL,
     .glyph_id_ofs_list = NULL,
     .list_length = 0,
     .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY},
    {.range_start = 183,
     .range_length = 8157,
     .glyph_id_start = 38,
     .unicode_list = unicode_list_3,
     .glyph_id_ofs_list = NULL,
     .list_length = 3,
     .type = LV_FONT_FMT_TXT_CMAP_SPARSE_TINY}};

/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LVGL_VERSION_MAJOR == 8
/*Store all the custom data of the font*/
static lv_font_fmt_txt_glyph_cache_t cache;
#endif

#if LVGL_VERSION_MAJOR >= 8
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = NULL,
    .kern_scale = 0,
    .cmap_num = 4,
    .bpp = 1,
    .kern_classes = 0,
    .bitmap_format = 0,
#if LVGL_VERSION_MAJOR == 8
    .cache = &cache
#endif
};

/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LVGL_VERSION_MAJOR >= 8
const lv_font_t sublabel = {
#else
lv_font_t sublabel = {
#endif
    .get_glyph_dsc =
        lv_font_get_glyph_dsc_fmt_txt, /*Function pointer to get glyph's data*/
    .get_glyph_bitmap =
        lv_font_get_bitmap_fmt_txt, /*Function pointer to get glyph's bitmap*/
    .line_height = 16, /*The maximum line height required by the font*/
    .base_line = 3,    /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -2,
    .underline_thickness = 1,
#endif
    .dsc = &font_dsc, /*The custom font data. Will be accessed by
                         `get_glyph_bitmap/dsc` */
#if LV_VERSION_CHECK(8, 2, 0) || LVGL_VERSION_MAJOR >= 9
    .fallback = NULL,
#endif
    .user_data = NULL,
};

#endif /*#if SUBLABEL*/
