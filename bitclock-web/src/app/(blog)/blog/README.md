# Converting videos

```sh
ffmpeg \
    -i input.mp4 \
    -c:v libx264 \
    -vf "scale=1280:-2" \
    -crf 23 \
    -preset veryslow \
    -an \
    -movflags +faststart \
    output.mp4
```

- H.264 compression
- 1280 width, recalculate height
- 23 constant rate factor (higher is higher quality)
- -preset medium (tradeoff between speed and compression efficiency)
- -an (no audio)
- -movflags +faststart (optimizes for fast playback start when streamed)
