npx lv_font_conv \
    --bpp 1 \
    --format lvgl \
    --font Overpass/Overpass-Medium.ttf \
    --symbols "-0123456789" \
    --range 0x00B0 \
    --size 40 \
    --output "small_number.c"
