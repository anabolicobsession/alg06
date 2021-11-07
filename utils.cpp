#include <iostream>
#include <cctype>

const int INT_MAX_LEN = 10;

namespace utils {

    int get_integer() {
        unsigned char str[INT_MAX_LEN];
        int c, i = 0;

        while ((c = std::getchar()) != EOF && !std::isdigit(c));
        while (c != EOF && std::isdigit(c)) {
            str[i++] = (unsigned char) c;
            c = std::getchar();
        }

        int num = 0, m = 1;
        while (i--) {
            num += (str[i] - '0') * m;
            m *= 10;
        }

        return m != 1 ? num : EOF;
    }

    char get_letter() {
        int c;
        do c = std::getchar(); while (!std::isalpha(c));
        return char(c);
    }

} // namespace utils
