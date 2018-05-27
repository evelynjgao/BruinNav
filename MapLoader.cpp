#include "provided.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
using namespace std;

class MapLoaderImpl
{
public:
	MapLoaderImpl();
	~MapLoaderImpl();
	bool load(string mapFile);
	size_t getNumSegments() const;
	bool getSegment(size_t segNum, StreetSegment& seg) const;
private:
    vector<StreetSegment*> m_streetSegments;
};

MapLoaderImpl::MapLoaderImpl()
{
}

MapLoaderImpl::~MapLoaderImpl()
{
    for(int i = 0; i < m_streetSegments.size(); i++){
        delete m_streetSegments[i];
    }
}

bool MapLoaderImpl::load(string mapFile)
{
    ifstream infile(mapFile);
    if(!infile){ // failure in trying to load data
        return false;
    }
    // able to load data, assume data is given in correct format
    string streetName, geoSegment;
    int countAttractions;
    // read file line by line
    while(getline(infile, streetName)){
        string startingLat, startingLong, endingLat, endingLong;
        // reading in and parsing latitude/longitude line
        char c;
        // starting latitude and longitude are separated by a comma and a space
        while(infile.get(c) && c != ','){
            startingLat += c;
        }
        // find first digit or negative sign
        while(infile.get(c) && !isdigit(c) && c != '-'){}
        infile.unget();
        while(infile.get(c) && c != ' '){
            startingLong += c;
        }
        // starting and ending coordinates are separated by a space
        while(infile.get(c) && !isdigit(c) && c != '-'){}
        infile.unget();
        // ending latitude and longitude are separated by a comma
        while(infile.get(c) && c != ','){
            endingLat += c;
        }
        while(infile.get(c) && !isdigit(c) && c != '-'){}
        infile.unget();
        while(infile.get(c) && c != '\n'){
            endingLong += c;
        }
        infile >> countAttractions;
        infile.ignore(10000, '\n');
        // create new StreetSegment object
        StreetSegment* s = new StreetSegment;
        // each attraction will have another line associated with it
        for(int i = 0; i < countAttractions; i++){
            // attraction format is its name, followed by a pipe character, then its coordinates, separated by a comma
            string attraction, attractionName, attractionCoordLat, attractionCoordLong;
            while(infile.get(c) && c != '|'){
                attractionName += c;
            }
            while(infile.get(c) && c != '-' && !isdigit(c)){}
            infile.unget();
            while(infile.get(c) && c != ','){
                attractionCoordLat += c;
            }
            while(infile.get(c) && c != '-' && !isdigit(c)){}
            infile.unget();
            while(infile.get(c) && c != '\n'){
                attractionCoordLong += c;
            }
            GeoCoord attractionCoord(attractionCoordLat, attractionCoordLong);
            Attraction a;
            a.name = attractionName;
            a.geocoordinates = attractionCoord;
            s->attractions.push_back(a);
        }
        // GeoCoord objects constructed with coordinate values
        GeoCoord start(startingLat, startingLong);
        GeoCoord end(endingLat, endingLong);
        // GeoSegment of the GeoCoords
        GeoSegment seg(start, end);
        // assign streetname and segment
        s->streetName = streetName;
        s->segment = seg;
        // add the StreetSegment object to the vector
        m_streetSegments.push_back(s);
    }
	return true;
}

size_t MapLoaderImpl::getNumSegments() const
{
    // the number of StreetSegments is equal to the size of the vector
    return m_streetSegments.size();
}

bool MapLoaderImpl::getSegment(size_t segNum, StreetSegment &seg) const
{
    // checking bounds: segNum must be between 0 and getNumSegments() - 1 (size_t always > 0)
    if(segNum > getNumSegments() - 1){
        // out of bounds, return false
        return false;
    }
    // set seg equal to StreetSegment found at appropriate location and return true
    seg = *m_streetSegments[segNum];
    return true;
}

//******************** MapLoader functions ************************************

// These functions simply delegate to MapLoaderImpl's functions.
// You probably don't want to change any of this code.

MapLoader::MapLoader()
{
	m_impl = new MapLoaderImpl;
}

MapLoader::~MapLoader()
{
	delete m_impl;
}

bool MapLoader::load(string mapFile)
{
	return m_impl->load(mapFile);
}

size_t MapLoader::getNumSegments() const
{
	return m_impl->getNumSegments();
}

bool MapLoader::getSegment(size_t segNum, StreetSegment& seg) const
{
   return m_impl->getSegment(segNum, seg);
}
