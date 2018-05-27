#include "provided.h"
#include "support.h"
#include "MyMap.h"
#include <string>
using namespace std;

class AttractionMapperImpl
{
public:
	AttractionMapperImpl();
	~AttractionMapperImpl();
	void init(const MapLoader& ml);
	bool getGeoCoord(string attraction, GeoCoord& gc) const;
private:
    // map of string keys to find GeoCoord values
    MyMap<string, GeoCoord> m_map;
};

AttractionMapperImpl::AttractionMapperImpl()
{
}

AttractionMapperImpl::~AttractionMapperImpl()
{
}

void AttractionMapperImpl::init(const MapLoader& ml)
{
    // building the map
    StreetSegment seg;
    // get each StreetSegment in MapLoader
    for(int i = 0; i < ml.getNumSegments(); i++){
        ml.getSegment(i, seg);
        // for each attraction in the StreetSegment, add the attraction to the map
        for(int j = 0; j < seg.attractions.size(); j++){
            // the key should be a case insensitive version of attraction name, designed so compare only lower case to lower case
            string name = seg.attractions[j].name;
            name = convertToLower(name);
            m_map.associate(name, seg.attractions[j].geocoordinates);
        }
    }
}

bool AttractionMapperImpl::getGeoCoord(string attraction, GeoCoord& gc) const
{
    // for case insensitive comparison, set entire attraction to lower case
    attraction = convertToLower(attraction);
    // check if attraction exists
    const GeoCoord* found = m_map.find(attraction);
    // attraction found
    if(found != nullptr){
        gc = *found;
        return true;
    }
    return false;
}

//******************** AttractionMapper functions *****************************

// These functions simply delegate to AttractionMapperImpl's functions.
// You probably don't want to change any of this code.

AttractionMapper::AttractionMapper()
{
	m_impl = new AttractionMapperImpl;
}

AttractionMapper::~AttractionMapper()
{
	delete m_impl;
}

void AttractionMapper::init(const MapLoader& ml)
{
	m_impl->init(ml);
}

bool AttractionMapper::getGeoCoord(string attraction, GeoCoord& gc) const
{
	return m_impl->getGeoCoord(attraction, gc);
}
