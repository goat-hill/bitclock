npx lv_font_conv \
    --bpp 1 \
    --format lvgl \
    --font Overpass/Overpass-Bold.ttf \
    --symbols "-0123456789" \
    --range 0x00B0 \
    --size 60 \
    --output "medium_number.c"
