#ifndef _NEWCAL_H
#define _NEWCAL_H

#include <stdint.h>
#include <time.h>

struct newcal_tm {
    int sec;  /* Seconds (0-59) */
    int min;  /* Minutes (0-59) */
    int hour; /* Hours (0-23) */

    int wday; /* Day of week (Mon = 0, Sun = 6) */
    int week; /* Week of season, starting from 1 */
    int seas; /* Season of year (0-3) */

    int year; /* Number of years since 1900 */
};

const char *newcal_snames[4]  = { "South", "East", "North", "West" };
const char *newcal_dnames[7]  = { "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday" };
const char *newcal_wnames[13] = { "1st", "2nd", "3rd", "4th", "5th", "6th", "7th", "8th", "9th", "10th", "11th", "12th", "13th" };

struct newcal_tm *newcal_gmtime(const time_t *timep, struct newcal_tm *result)
{
    result->sec  = *timep % 60;
    result->min  = *timep / 60 % 60;
    result->hour = *timep / 3600 % 24;

    const int days    = *timep / 3600 / 24 + 135152,
              weeks   = days / 7,
              gcycles = weeks / 20871,
              gweeks  = weeks % 20871,
              ccycles = gweeks / 2348,
              cweeks  = gweeks % 2348,
              lcycles = cweeks / 261,
              lweeks  = cweeks % 261,
              year    = gcycles*400 + ccycles*45 + lcycles*5 + lweeks/52,
              isleap  = (lweeks != 260) && (year % 5 == 0) && (year % 400 % 45 != 0),
              yweeks  = weeks - gcycles*20871 - ccycles*2348 - lcycles*261 - lweeks/52*52 + isleap;

    result->wday = days % 7;
    result->week = yweeks % 13 + 1;
    result->seas = yweeks / 13;

    result->year = year - 300;

    return result;
}

struct newcal_tm *newcal_localtime(const time_t *timep, struct newcal_tm *result)
{
    struct tm localtm;
    localtime_r(timep, &localtm);
    time_t local_time = timegm(&localtm);
    return newcal_gmtime(&local_time, result);
}

size_t newcal_strftime(char *s, size_t max, const char *format, const struct newcal_tm *tm)
{
    if (!max) return 0;

    int i = 0;
    char c;
    while ((c = *format++)) {
        if (c != '%') {
            s[i++] = c;
            if (i >= max) return i;
            continue;
        }

        switch (*format++) {
        case 'Y':
            i += snprintf(&s[i], max - i, "%d", tm->year + 1900);
            if (i >= max) return i;
            break;
        case 'm':
            i += snprintf(&s[i], max - i, "%d", tm->seas + 1);
            if (i >= max) return i;
            break;
        case 'w':
            i += snprintf(&s[i], max - i, "%d", tm->week);
            if (i >= max) return i;
        case 'W':
            i += snprintf(&s[i], max - i, "%s", newcal_wnames[tm->week - 1]);
            if (i >= max) return i;
            break;
        case 'u':
            i += snprintf(&s[i], max - i, "%d", tm->wday + 1);
            if (i >= max) return i;
            break;

        case 'B':
            i += snprintf(&s[i], max - i, "%s", newcal_snames[tm->seas]);
            if (i >= max) return i;
            break;
        case 'A':
            i += snprintf(&s[i], max - i, "%s", newcal_dnames[tm->wday]);
            if (i >= max) return i;
            break;

        case 'H':
            i += snprintf(&s[i], max - i, "%d", tm->hour);
            if (i >= max) return i;
            break;
        case 'M':
            i += snprintf(&s[i], max - i, "%d", tm->min);
            if (i >= max) return i;
            break;
        case 'S':
            i += snprintf(&s[i], max - i, "%d", tm->sec);
            if (i >= max) return i;
            break;
        }
    }

    s[i++] = 0;
    return i;
}

#endif
