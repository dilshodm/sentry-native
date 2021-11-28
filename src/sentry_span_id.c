#include "sentry_boot.h"

#include "sentry_random.h"
#include <stdio.h>
#include <string.h>

sentry_span_id_t
sentry_span_id_nil(void)
{
    sentry_span_id_t rv;
    memset(rv.bytes, 0, 8);
    return rv;
}

sentry_span_id_t
sentry_span_id_new(void)
{
    char buf[8];
    if (sentry__getrandom(buf, sizeof(buf)) != 0) {
        return sentry_span_id_nil();
    }
    return sentry_span_id_from_bytes(buf);
}

sentry_span_id_t
sentry_span_id_from_string(const char *str)
{
    sentry_span_id_t rv;
    memset(&rv, 0, sizeof(rv));

    size_t i = 0;
    size_t len = strlen(str);
    size_t pos = 0;
    bool is_nibble = true;
    char nibble = 0;

    for (i = 0; i < len && pos < 8; i++) {
        char c = str[i];
        if (!c || c == '-') {
            continue;
        }

        char val = 0;
        if (c >= 'a' && c <= 'f') {
            val = 10 + (c - 'a');
        } else if (c >= 'A' && c <= 'F') {
            val = 10 + (c - 'A');
        } else if (c >= '0' && c <= '9') {
            val = c - '0';
        } else {
            return sentry_span_id_nil();
        }

        if (is_nibble) {
            nibble = val;
            is_nibble = false;
        } else {
            rv.bytes[pos++] = (nibble << 4) | val;
            is_nibble = true;
        }
    }

    return rv;
}

sentry_span_id_t
sentry_span_id_from_bytes(const char bytes[8])
{
    sentry_span_id_t rv;
    memcpy(rv.bytes, bytes, 8);
    return rv;
}

int
sentry_span_id_is_nil(const sentry_span_id_t *span_id)
{
    for (size_t i = 0; i < 8; i++) {
        if (span_id->bytes[i]) {
            return false;
        }
    }
    return true;
}

void
sentry_span_id_as_bytes(const sentry_span_id_t *span_id, char bytes[8])
{
    memcpy(bytes, span_id->bytes, 8);
}

void
sentry_span_id_as_string(const sentry_span_id_t *span_id, char str[17])
{
#define B(X) (unsigned char)span_id->bytes[X]
    snprintf(str, 17,
        "%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx",
        B(0), B(1), B(2), B(3), B(4), B(5), B(6), B(7));
#undef B
}
