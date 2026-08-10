#ifndef OPENVELA_UI_CITY_DATA_H
#define OPENVELA_UI_CITY_DATA_H

#include <stddef.h>

/*
 * Read-only province/city/district data used by the weather city picker.
 *
 * The source's synthetic `国外` chain is deliberately excluded, leaving the
 * same valid domestic hierarchy as the reference Quick App: 34 provinces,
 * 392 cities and 3209 districts.  City indexes are local to a province and
 * district indexes are local to a city.  District counts do not include an
 * artificial "不选择" option; the UI may prepend that itself.
 *
 * Every accessor is bounds-safe: invalid indexes return either zero or an
 * empty string, and string accessors never return NULL.  Returned strings
 * remain valid for the lifetime of the process.
 */

size_t openvela_ui_city_province_count(void);

const char *openvela_ui_city_province_name(size_t province_index);

size_t openvela_ui_city_city_count(size_t province_index);

const char *openvela_ui_city_city_name(size_t province_index,
                                       size_t city_index);

const char *openvela_ui_city_city_short_name(size_t province_index,
                                             size_t city_index);

/* Returns a QWeather location ID when one is known, otherwise "". */
const char *openvela_ui_city_city_location_id(size_t province_index,
                                              size_t city_index);

size_t openvela_ui_city_district_count(size_t province_index,
                                       size_t city_index);

const char *openvela_ui_city_district_name(size_t province_index,
                                           size_t city_index,
                                           size_t district_index);

#endif
