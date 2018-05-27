#include "provided.h"
#include "MyMap.h"
#include <vector>
using namespace std;

class SegmentMapperImpl
{
public:
	SegmentMapperImpl();
	~SegmentMapperImpl();
	void init(const MapLoader& ml);
	vector<StreetSegment> getSegments(const GeoCoord& gc) const;
private:
    MyMap<GeoCoord, vector<StreetSegment>> coordMap;
};

SegmentMapperImpl::SegmentMapperImpl()
{
}

SegmentMapperImpl::~SegmentMapperImpl()
{
}

void SegmentMapperImpl::init(const MapLoader& ml)
{
    StreetSegment street;
    // for all StreetSegments in MapLoader,
    for(int i = 0; i < ml.getNumSegments(); i++){
        ml.getSegment(i, street);
        // associate the starting geocoordinate, ending geocoordinate, and attractions' geocoordinates as keys to the StreetSegment into the map
        // get the vector holding associated segments
        vector<StreetSegment> streetSegments = getSegments(street.segment.start);
        // add Segment to the vector
        streetSegments.push_back(street);
        // reassociate the new vector with the coordinate
        coordMap.associate(street.segment.start, streetSegments);
        // do the same for the end coordinate
        streetSegments = getSegments(street.segment.end);
        streetSegments.push_back(street);
        coordMap.associate(street.segment.end, streetSegments);
        for(int j = 0; j < street.attractions.size(); j++){
            // prevent duplicate StreetSegments from being associated with coord (if attraction is at start/end point of a street)
            if(street.attractions[j].geocoordinates != street.segment.start && street.attractions[j].geocoordinates != street.segment.end){
                // add the StreetSegment into the associated vector and reassociate
                streetSegments = getSegments(street.attractions[j].geocoordinates);
                streetSegments.push_back(street);
                coordMap.associate(street.attractions[j].geocoordinates, streetSegments);
            }
        }
    }
}

vector<StreetSegment> SegmentMapperImpl::getSegments(const GeoCoord& gc) const
{
    vector<StreetSegment> segments;
    // see if GeoCoordinate exists in map
    if(coordMap.find(gc)){
        // return associated vector
        return *coordMap.find(gc);
    }
    // return empty vector if GeoCoordinate doesn't exist
    return segments;
}

//******************** SegmentMapper functions ********************************

// These functions simply delegate to SegmentMapperImpl's functions.
// You probably don't want to change any of this code.

SegmentMapper::SegmentMapper()
{
	m_impl = new SegmentMapperImpl;
}

SegmentMapper::~SegmentMapper()
{
	delete m_impl;
}

void SegmentMapper::init(const MapLoader& ml)
{
	m_impl->init(ml);
}

vector<StreetSegment> SegmentMapper::getSegments(const GeoCoord& gc) const
{
	return m_impl->getSegments(gc);
}
