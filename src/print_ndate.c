// vim:ts=4:sw=4:expandtab
#include <config.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <yajl/yajl_gen.h>
#include <yajl/yajl_version.h>

#include "i3status.h"

#include "newcal.h"

#define STRING_SIZE 50

void print_ndate(yajl_gen json_gen, char *buffer, const char *format, time_t t) {
    char *outwalk = buffer;
    struct newcal_tm tm;

    newcal_localtime(&t, &tm);

    outwalk += newcal_strftime(buffer, 4096, format, &tm);

    *outwalk = '\0';
    OUTPUT_FULL_TEXT(buffer);
}
