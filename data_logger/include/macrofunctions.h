#pragma once

/**
 * @brief Macro to declare a function argument not used in this function
 *
 * Such a necessity may arise for inheritance compliance (implement virtual functions)
 */
#define unused(X) __attribute__((unused)) X

/**
 * @brief Macro to write \p data byte by byte into \p buffer while increasing the pointer
 *
 */
#define BIG_ENDIAN_WRITE(data, buffer)               \
    for (int i = sizeof((data)) - 1; i >= 0; i--) \
    {                                                \
        *((buffer)++) = ((uint8_t *)&(data))[i];     \
    }

// Macro for clear screen terminal command
#define RESET_TERMINAL "\033[H\033[J"