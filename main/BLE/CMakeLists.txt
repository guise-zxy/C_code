idf_component_register(SRCS "ble_terminal_test.c" "ble_test.c"
                        "ble_terminal.c"
                    INCLUDE_DIRS "."
                     REQUIRES esp_driver_i2c
        
                    )
