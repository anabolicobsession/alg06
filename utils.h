#ifndef UTILS_H
#define UTILS_H

namespace utils {

/**
 * Read one non-negative integer from standard input, ignoring any non-digit characters.
 * Useful for fast reading.
 *
 * @return integer if one was in input, EOF otherwise.
 */
int get_integer();

char get_letter();

} // namespace stdin

#endif // UTILS_H
