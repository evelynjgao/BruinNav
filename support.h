#ifndef SUPPORT_INCLUDED
#define SUPPORT_INCLUDED

#include "provided.h"
#include <string>

bool operator == (const GeoCoord& a, const GeoCoord& b);
bool operator != (const GeoCoord& a, const GeoCoord& b);
bool operator < (const GeoCoord& a, const GeoCoord& b);
bool operator > (const GeoCoord& a, const GeoCoord& b);

std::string convertToLower(std::string name);

std::string directionOfLine(const GeoSegment& gs);

#endif
