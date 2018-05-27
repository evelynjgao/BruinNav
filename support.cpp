#include "support.h"
#include "provided.h"

using namespace std;

bool operator == (const GeoCoord& a, const GeoCoord& b)
{
    return a.latitude == b.latitude && a.longitude == b.longitude;
}

bool operator != (const GeoCoord& a, const GeoCoord& b)
{
    return a.latitude != b.latitude || a.longitude != b.longitude;
}

bool operator < (const GeoCoord& a, const GeoCoord& b)
{
    if(a.latitude == b.latitude){
        return a.longitude < b.longitude;
    }
    return a.latitude < b.latitude;
}

bool operator > (const GeoCoord& a, const GeoCoord& b)
{
    if(a.latitude == b.latitude){
        return a.longitude > b.longitude;
    }
    return a.latitude > b.latitude;
}

string convertToLower(string name)
{
    for(int i = 0; i < name.size(); i++){
        name[i] = tolower(name[i]);
    }
    return name;
}

string directionOfLine(const GeoSegment& gs){
    string direction;
    double angle = angleOfLine(gs);
    if(angle >= 0 && angle <= 22.5){
        direction = "east";
    }else if(angle > 22.5 && angle <= 67.5){
        direction = "northeast";
    }else if(angle > 67.5 && angle <= 112.5){
        direction = "north";
    }else if(angle > 112.5 && angle <= 157.5){
        direction = "northwest";
    }else if(angle > 157.5 && angle <= 202.5){
        direction = "west";
    }else if(angle > 202.5 && angle <= 247.5){
        direction = "southwest";
    }else if(angle > 247.5 && angle <= 292.5){
        direction = "south";
    }else if(angle > 292.5 && angle <= 337.5){
        direction = "southeast";
    }else{
        direction = "east";
    }
    return direction;
}
