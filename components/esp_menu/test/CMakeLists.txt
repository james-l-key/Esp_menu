cmake_minimum_required(VERSION 3.16)

file(GLOB TEST_SRCS "test_*.c")
idf_component_register(SRCS ${TEST_SRCS}
					  INCLUDE_DIRS "."
					  REQUIRES esp_menu
					  PRIV_REQUIRES unity)
