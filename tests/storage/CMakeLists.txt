include_directories(..)

add_executable(
    test_parser2
    test_parser2.cpp
)

target_link_libraries(
    test_parser2
    libzhuyin
)

add_executable(
    test_chewing_table
    test_chewing_table.cpp
)

target_link_libraries(
    test_chewing_table
    libzhuyin
)

add_executable(
    test_phrase_index
    test_phrase_index.cpp
)

target_link_libraries(
    test_phrase_index
    libzhuyin
)

add_executable(
    test_phrase_index_logger
    test_phrase_index_logger.cpp
)

target_link_libraries(
    test_phrase_index_logger
    libzhuyin
)

add_executable(
    test_phrase_table
    test_phrase_table.cpp
)

target_link_libraries(
    test_phrase_table
    libzhuyin
)

add_executable(
    test_ngram
    test_ngram.cpp
)

target_link_libraries(
    test_ngram
    libzhuyin
)

add_executable(
    test_flexible_ngram
    test_flexible_ngram.cpp
)

target_link_libraries(
    test_flexible_ngram
    libzhuyin
)
