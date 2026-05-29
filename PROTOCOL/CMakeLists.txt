idf_component_register(SRCS "protocol_test.c"
                        "terminal_protocol.c"
                    INCLUDE_DIRS "."
                     REQUIRES esp_driver_i2c
        
                    )
