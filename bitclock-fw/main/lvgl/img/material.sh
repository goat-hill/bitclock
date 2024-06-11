#!/bin/sh
python3 ../../../components/lvgl/scripts/LVGLImage.py \
    material_thunderstorm.png \
    --ofmt C \
    --cf I2 \
    --output .
python3 ../../../components/lvgl/scripts/LVGLImage.py \
    material_sunny.png \
    --ofmt C \
    --cf I1 \
    --output .
python3 ../../../components/lvgl/scripts/LVGLImage.py \
    material_snowy.png \
    --ofmt C \
    --cf I2 \
    --output .
python3 ../../../components/lvgl/scripts/LVGLImage.py \
    material_rainy.png \
    --ofmt C \
    --cf I1 \
    --output .
python3 ../../../components/lvgl/scripts/LVGLImage.py \
    material_partly_cloudy_night.png \
    --ofmt C \
    --cf I2 \
    --output .
python3 ../../../components/lvgl/scripts/LVGLImage.py \
    material_partly_cloudy_day.png \
    --ofmt C \
    --cf I2 \
    --output .
python3 ../../../components/lvgl/scripts/LVGLImage.py \
    material_foggy.png \
    --ofmt C \
    --cf I1 \
    --output .
python3 ../../../components/lvgl/scripts/LVGLImage.py \
    material_cloud.png \
    --ofmt C \
    --cf I1 \
    --output .
python3 ../../../components/lvgl/scripts/LVGLImage.py \
    material_clear_night.png \
    --ofmt C \
    --cf I1 \
    --output .
