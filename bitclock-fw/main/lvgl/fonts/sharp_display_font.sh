npx lv_font_conv \
    --bpp 1 \
    --format lvgl \
    --font Overpass/Overpass-SemiBold.ttf \
    --symbols "0123456789ABCDEFGHIJGKLMNOPQRSTUVWXYZabcdefghijgklmnopqrstuvwxyz· ₂ₓ:°-%" \
    --size 42 \
    --output "sharp_display_font.c"
