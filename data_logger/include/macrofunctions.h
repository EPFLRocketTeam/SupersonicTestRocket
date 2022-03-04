#pragma once

/**
 * @brief Macro to declare a function argument not used in this function
 * 
 * Such a necessity may arise for inheritance compliance (implement virtual functions)
 */
#define unused(X) __attribute__((unused)) X

#define RESET_TERMINAL "\033[H\033[J"