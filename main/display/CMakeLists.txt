idf_component_register(
    SRCS
        "display_test.c"
        "display_font.c"
        "display_page.c"
        "display_service.c"
    INCLUDE_DIRS "."
    REQUIRES
        EPD
        SHT31
        esp_driver_i2c
)
