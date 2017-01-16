#ifndef GUERILLAMAIL_DEBUG_H
#define GUERILLAMAIL_DEBUG_H

#define DEBUG 1

#define DEBUG_PRINT(fmt, ...) \
            do { if (DEBUG) fprintf(stderr, fmt, __VA_ARGS__); } while (0)

#endif
