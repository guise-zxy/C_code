idf_component_register(SRCS "audio_test.c"
                        "audio_input.c"
                    INCLUDE_DIRS "."
                     REQUIRES esp_driver_spi
               
                    )
