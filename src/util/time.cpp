#include "time.h"

#include "locale/locale.h"
#include "types/string.hpp"

String Time::Format(const unsigned int seconds) {
    /* Get hours and minutes */
    const unsigned int minutes = seconds / 60;
    const unsigned int hours = minutes / 60;

    /* If the duration more than an hour */
    if (hours > 0)
        return String::Format(_("%uh%um%us"),
                              hours,
                              minutes % 60,
                              seconds % 60);

    /* If the duration less than an hour and more than a minute */
    if (minutes > 0)
        return String::Format(_("%um%us"),
                              minutes,
                              seconds % 60);

    /* If the duration less than a minute */
    return String::Format(_("%us"),
                          seconds);
}
