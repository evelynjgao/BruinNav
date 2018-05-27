#include "provided.h"
#include "MyMap.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <stack>
using namespace std;

class NavigatorImpl
{
public:
    NavigatorImpl();
    ~NavigatorImpl();
    bool loadMapData(string mapFile);
    NavResult navigate(string start, string end, vector<NavSegment>& directions) const;
private:
    AttractionMapper m_attractionMap;
    SegmentMapper m_segmentMap;
    // a node object is used within NavigatorImpl for the A* algorithm closed and open lists
    struct Node{
        // a node consists of the geocoords of its parents, its own geocoords, the distance from the start point, the heuristic distance to the end, the sum of its total distance estimated, and its street name
        //constructs a Node object
        Node(GeoCoord parent, GeoCoord geocoord, double distG, double distH, string street){
            parentCoord = parent;
            gc = geocoord;
            g = distG;
            h = distH;
            f = g+h;
            streetName = street;
        }
        GeoCoord parentCoord;
        GeoCoord gc;
        double f, g, h;
        string streetName;
        // overloads less than operator to order priority queue of nodes
        bool operator < (const Node& other) const{
            if(f < other.f){
                return false;
            }
            return true;
        }
    };
};

NavigatorImpl::NavigatorImpl()
{
}

NavigatorImpl::~NavigatorImpl()
{
}

bool NavigatorImpl::loadMapData(string mapFile)
{
    // use maploader to load map's data
    MapLoader mapLoader;
    // unable to load
    if(!mapLoader.load(mapFile)){
        return false;
    }
    // able to load, construct attraction and segment mappers
    m_attractionMap.init(mapLoader);
    m_segmentMap.init(mapLoader);
    return true;
}

NavResult NavigatorImpl::navigate(string start, string end, vector<NavSegment> &directions) const
{
    // clears direction vectors if not an empty vector
    directions.clear();
    GeoCoord startingCoord, endingCoord;
    // not a valid source attraction, cannot route from starting point
    if(!m_attractionMap.getGeoCoord(start, startingCoord)){
        return NAV_BAD_SOURCE;
    }
    // not a valid source attraction, cannot route to ending point
    if(!m_attractionMap.getGeoCoord(end, endingCoord)){
        return NAV_BAD_DESTINATION;
    }
    bool foundDestination = false;
    // we use an STL priority queue of nodes to represent the fringe/open list of the A* algorithm
    priority_queue<Node> open;
    // we use a map of geocoords and nodes to represent the closed list of the A* algorithm
    MyMap<GeoCoord, Node> closed;
    // starting node associated with starting coord
    open.push(Node(startingCoord, startingCoord, 0, distanceEarthMiles(startingCoord, endingCoord), ""));
    // while we haven't found our destination and we still have geocoordinates to traverse
    while(!foundDestination && !open.empty()){
        // get the first node in the stack
        Node cur = open.top();
        open.pop();
        // if our node doesn't already exist in the closed list, add it
        if(closed.find(cur.gc) == nullptr){
            closed.associate(cur.gc, cur);
        }
        // retrieve all segments associated to that coordinate
        vector<StreetSegment> associatedSegments = m_segmentMap.getSegments(cur.gc);
        for(int i = 0; i < associatedSegments.size(); i++){
            // check if our destination is one of the attractions on this segment
            for(int j = 0; j < associatedSegments[i].attractions.size(); j++){
                GeoCoord attractionGeoCoord = associatedSegments[i].attractions[j].geocoordinates;
                // found our end destination! add to closed list
                if(attractionGeoCoord == endingCoord){
                    double distanceFromCurrent = distanceEarthMiles(attractionGeoCoord, cur.gc);
                    Node toAdd(cur.gc, attractionGeoCoord, cur.g + distanceFromCurrent, 0, associatedSegments[i].streetName);
                    closed.associate(attractionGeoCoord, toAdd);
                    foundDestination = true;
                }
            }
            // if the destination was not found on this segment,
            if(!foundDestination){
                GeoCoord streetStart = associatedSegments[i].segment.start;
                GeoCoord streetEnd = associatedSegments[i].segment.end;
                string streetName = associatedSegments[i].streetName;
                // we need to add the other end of the segment to the queue
                // (or both if it's the starting attraction)
                if(cur.gc != streetStart && closed.find(streetStart) == nullptr){
                    double distanceFromCurrent = distanceEarthMiles(streetStart, cur.gc);
                    double distanceToDestination = distanceEarthMiles(streetStart, endingCoord);
                    Node toAdd(cur.gc, streetStart, cur.g + distanceFromCurrent, distanceToDestination, streetName);
                    open.push(toAdd);
                }
                if(cur.gc != streetEnd && closed.find(streetEnd) == nullptr){
                    double distanceFromCurrent = distanceEarthMiles(streetEnd, cur.gc);
                    double distanceToDestination = distanceEarthMiles(streetEnd, endingCoord);
                    Node toAdd(cur.gc, streetEnd, cur.g + distanceFromCurrent, distanceToDestination, streetName);
                    open.push(toAdd);
                }
            }
        }
    }
    // if we exited the while loop and we haven't found our destination, there is no route (no more geocoords)
    if(!foundDestination){
        closed.clear();
        return NAV_NO_ROUTE;
    }
    // to get our directions, we need to backtrack our nodes into a stack
    stack<Node> backtrack;
    // starting with the ending coordinate,
    Node cur = *closed.find(endingCoord);
    string streetName;
    // while not our starting coordinate
    while(cur.gc != cur.parentCoord){
        // we add the node to the stack
        backtrack.push(cur);
        // find the last streetname before the starting coordinate
        streetName = cur.streetName;
        // and trace backwards by searching for the parent coordinate
        cur = *closed.find(cur.parentCoord);
    }
    // our starting coordinate's street name should be the same as the second coordinate
    cur.streetName = streetName;
    // add our starting coordinate
    backtrack.push(cur);
    // while our stack is not empty,
    while(!backtrack.empty()){
        // we start to process from the starting coordinate
        Node process = backtrack.top();
        backtrack.pop();
        // if there is another node,
        if(!backtrack.empty()){
            // get the value of the node
            Node next = backtrack.top();
            // construct the segment between the two nodes
            GeoSegment seg(process.gc, next.gc);
            if(process.streetName != next.streetName){
                // turn-style nav segment
                // construct the two street segments, from the street we're on and the next street
                GeoSegment seg1(process.parentCoord, process.gc);
                GeoSegment seg2(process.gc, next.gc);
                // find the angle between the segments
                double angle = angleBetween2Lines(seg1, seg2);
                string direction;
                // if the angle of the turn is less than 180, we are turning left
                if(angle < 180){
                    direction = "left";
                }else{ //otherwise, we turn right
                    direction = "right";
                }
                // construct the NavSegment to push back into direction vector
                NavSegment nav(direction, next.streetName);
                directions.push_back(nav);
            }
            // proceed-style nav segment
            // call support function to get the direction we travel
            string direction = directionOfLine(seg);
            // construct the NavSegment to push back into direction vector
            NavSegment nav(direction, next.streetName, next.g - process.g, seg);
            directions.push_back(nav);
        }
    }
	return NAV_SUCCESS;
}

//******************** Navigator functions ************************************

// These functions simply delegate to NavigatorImpl's functions.
// You probably don't want to change any of this code.

Navigator::Navigator()
{
    m_impl = new NavigatorImpl;
}

Navigator::~Navigator()
{
    delete m_impl;
}

bool Navigator::loadMapData(string mapFile)
{
    return m_impl->loadMapData(mapFile);
}

NavResult Navigator::navigate(string start, string end, vector<NavSegment>& directions) const
{
    return m_impl->navigate(start, end, directions);
}
