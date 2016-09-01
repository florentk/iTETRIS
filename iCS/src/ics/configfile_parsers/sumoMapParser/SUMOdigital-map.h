/****************************************************************************/
/// @file    SUMOdigital-map.h
/// @author  Pasquale Cataldi (EURECOM)
/// @date    Jan 8, 2010
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/

/*! \file SUMOdigital-map.h
 \brief Data Structure and classes for the iTetris DigitalMap block.
*/

#ifndef SUMODIGITALMAP_H_
#define SUMODIGITALMAP_H_

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif


#ifdef SUMO_ON

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <map>
using namespace std;

#include "../../../utils/ics/geometric/Point2D.h"

#define LANEWIDTH 3.2                                /**< width of a lane [m]. */

/*! \namespace sumo_map
 \brief Namespace for the DigitalMap block

 The namespace "sumo_map" contains the data structure of the DigitalMap block and the methods to load it from a SUMO net.xml file.
 */
namespace sumo_map {

using namespace ics_types;

/*
 * ****************
 *    STRUCTURES
 * ****************
 */

/*! \struct SUMOLocation
 \brief General information about the digital map (size and offset).

 The structure reflects the SUMO's one.
*/
struct SUMOLocation {
    Point2D netOffset;                              /**< Offset of the map */
    Point2D convBoundaryMin;                        /**< Farther South-West point on the map */
    Point2D convBoundaryMax;                        /**< Farther North-East point on the map */
    Point2D origBoundaryMin;                        /**< Farther South-West point on the original map */
    Point2D origBoundaryMax;                        /**< Farther North-East point on the original map */
    char projParameter;                             /**< optional parameter for proj. */

    SUMOLocation():                                 /**< Initializer */
            projParameter(' ') {};
};

/*! \struct SUMOSUMOSucclane
 \brief Structure that gives information about the lanes that follow a lane that ends in a junction.

It also gives information about the turning direction, the traffic light program and if the yield rule applies. The structure reflects the SUMO one.
*/
struct SUMOSUMOSucclane {
    string lane;                                    /**< ID of the lane that is connected after the junction. */
    string via;                                     /**< ID of the lane (internal of the junction) that connects the reference lane to the one with ID 'lane' */
    string tl;                                      /**< Traffic light program used to manage the reference lane. */
    short linkIndex;                                /**< Index of the signal within this traffic light. */
    bool yield;                                     /**< Application of the yield rule. */
    char dir;                                       /**< direction: {'s' = STRAIGHT, 'l' = LEFT, 'r' = RIGHT, 't' = TURN, 'L' = PARTLEFT, 'R' = PARTRIGHT}. */
    char state;                                     /**< connection: {'M' = MAYOR, 'm' = MINOR}. */
    char int_end;                                   /**<  */

    SUMOSUMOSucclane():                             /**< Initializer */
            lane(""), via(""), yield(true),
            dir(' '), state(' '),  int_end(' ') {};
};

/*! \struct SUMOPhase
 \brief Structure that gives information about the SUMOPhase of the reference traffic light program.
*/
struct SUMOPhase {
    short duration;                                    /**< Duration of the traffic light program's SUMOPhase. */
    string state;                                    /**< Traffic light program's state. */

    SUMOPhase():                                         /**< Initializer */
            duration(0), state("") {};
};

/*! \struct SUMOLogicitem
 \brief Structure that gives information about the traffic light logic.
*/
struct SUMOLogicitem {
    short request;                                    /**<  */
    string response;                                /**<  */
    string foes;                                    /**<  */
    bool cont;                                        /**<  */

    SUMOLogicitem():                                    /**< Initializer. */
            request(0), response(""), foes(""), cont(false) {};
};



/*
 * ****************
 *  COMMON CLASSES
 * ****************
 */

/*! \class SUMOLane
 \brief Class that contains the information about a lane and its previous and next ones.
*/
class SUMOLane {
public:
    SUMOLane():                                            /**< Constructor. */
            id(""), depart(0), maxspeed(0), length(0) {};
    virtual ~SUMOLane()    {};                                /**< Destructor. */

    // Variables
    string id;                                        /**< ID of the lane. */
    float depart;                                    /**< Starting time for the lane to be active [s]. */
    float maxspeed;                                    /**< Max speed in [m/s]. */
    float length;                                    /**< Total length of the lane. */
    vector<Point2D> shape;                            /**< Shape of the lane, defined by a set of points. */
    vector<SUMOLane *> nextSUMOLanes;                        /**< Vector of pointers to the next lanes. */
    vector<SUMOLane *> prevSUMOLanes;                        /**< Vector of pointers to the previous lanes. */
};

/*! \class SUMOEdge
 \brief Class that contains the information about an edge (i.e. road).
*/
class SUMOEdge {
public:
    SUMOEdge():                                            /**< Constructor. */
            id(""), stringFrom(""), stringTo(""),  funct(""), inner("") {};
    virtual ~SUMOEdge() {};                                /**< Destructor. */

    // Variables
    string id;                                        /**< ID of the edge. */
    string stringFrom;                                /**< ID of the previous junction. */
    string stringTo;                                /**< ID of the next junction. */
    string funct;                                    /**< function of the edge. */
    string inner;                                    /**< If defined, it indicates that the edge is inside of a junction. */
    map<string, SUMOLane> lanes;                        /**< Set of lanes contained by the edge.*/

    // Methods
    SUMOLane *getSUMOLane(string);                            /**< Get the pointer to the lane with the id given as input (NULL otherwise). */
};

/*! \class SUMOJunction
 \brief Class that contains the information about a junction.
*/
class SUMOJunction {
public:
    SUMOJunction() {};                                    /**< Constructor. */
    virtual ~SUMOJunction() {};                            /**< Destructor. */

    // Variables
    string id;
    string type;                                    /**< Type of the junction {priority, internal, DEAD_END}. */
    Point2D center;                                    /**< Geometric center of the junction. */
    vector<string> stringIncSUMOLanes;                    /**< IDs of the incoming lanes. */
    vector<string> stringIntSUMOLanes;                    /**< IDs of the internal lanes. */
    vector<Point2D> shape;                            /**< Shape of the junction, defined by a set of points. */

    // Methods
    bool isIncSUMOLane(string);                            /**< search if the input lane (id) is among the incoming lanes. */
    bool isIntSUMOLane(string);                            /**< search if the input lane (id) is among the internal lanes. */
};

/*! \class SUMOTLlogic
 \brief Class that contains the information about a traffic light logic.
*/
class SUMOTLlogic {
public:
    SUMOTLlogic() {};                                    /**< Constructor */
    virtual ~SUMOTLlogic() {};                            /**< Destructor */

    // Variables
    string id;                                        /**< ID of traffic light logic. */
    string type;                                    /**<  */
    string programID;                                /**< ID of the traffic light program. */
    vector<SUMOPhase> phases;                            /**< Definition of the SUMOPhases (sequence of red and green). */
};

///*! \class RowLogic
// \brief This class reflects the SUMO's one.
//*/
//class RowLogic {
//public:
//    RowLogic() {};                                    /**< Constructor */
//    virtual ~RowLogic() {};                            /**< Destructor */
//
//    // Variables
//    string id;                                        /**<  */
//    short requestSize;                                /**<  */
//    short laneNumber;                                /**<  */
//    vector<SUMOLogicitem> logics;                        /**<  */
//};

/*! \class SUMOSucc
 \brief Class that contains the information about how edges, lanes and junctions are connected.
*/
class SUMOSucc {
public:
    SUMOSucc() {};                                        /**< Constructor */
    virtual ~SUMOSucc() {};                                /**< Destructor */

    // Variables
    string edge;                                    /**< SUMOEdge ID. */
    string lane;                                    /**< SUMOLane ID. */
    string junction;                                /**< SUMOJunction ID. */
    vector<SUMOSUMOSucclane> SUMOSUMOSucclanes;                        /**< Vector of the next connected lanes. */
};


/*
 * ****************
 *   EXTRAS
 * ****************
 */

/*! \struct SUMOTrafficlight
 \brief Structure that contains all the information about a traffic light.

 This structure does not exist in SUMO.
*/
struct SUMOTrafficlight {
    short icsID;                   /**< ID of the traffic light in the map. */
    string tlID;                   /**< Traffic light ID for this signal. */
    short linkIndex;               /**< ID of the traffic light in the map. */
    Point2D pos;                   /**< 2D-position of the traffic light. */
    SUMOLane *controlled;          /**< Pointer to the controlled lane. */
    SUMOLane *via;                 /**< Pointer to the lane of the junction that connects the controlled lane to the next one (succ). */
    SUMOLane *succ;                /**< Pointer to the next lane. */
    char direction;                /**< direction indicated by the traffic light. */

    SUMOTrafficlight():                                 /**< Initializer. */
            icsID(-1), direction(' ') {};
};



/*
 * ****************
 *    DIGITAL MAP
 * ****************
 */

/*! \class SUMODigitalMap
 \brief This class contains all the information about a digital map.
*/
class SUMODigitalMap {
public:
    // Constructors/Destructor
    SUMODigitalMap();                              /**< Void constructor. */
    SUMODigitalMap(string filename);               /**< Constructor with the name of the file to be parsed. */
    ~SUMODigitalMap();                             /**< Destructor. */

    // Public variables
    SUMOLocation location;                         /**< general parameters of the map. */
    vector<SUMOSucc> succs;                        /**< vector that contains the connections between lanes, edges and junctions. */
    map<string, SUMOEdge> edges;                   /**< edges (roads or links) of the topology map. */
    map<string, SUMOJunction> junctions;           /**< junctions (usually intersections) of the topology map. */
    map<string, SUMOTLlogic> tllogics;             /**< traffic light logics. */
    vector<SUMOTrafficlight> trafficlights;        /**< vector containing the traffic lights positions and lanes. */
//    map<string, RowLogic> rowLogics;

    // Public functions
    bool loadMap();                                /**< load the topology map. */
    bool loadMap(string filename);                 /**< load the topology map contained in the file 'filename'. */
    const string getNETfilename() const;           /**< get the file name of the net file. */
    SUMOLane *findSUMOLane(string);                /**< returns the pointer to the SUMOLane object given the lane id. */
    const string getSUMOJunctionFromSUMOEdge(string) const; /**< get the id of the junction that contains the internal edge (an empty string otherwise). */

private:
    // Private variables
    string NETfilename;
    unsigned int readLines;                        /**< number of lines of the topology file that have been read. */
    ifstream infile;

    // Private methods
    void readSUMOLocation(string, SUMOLocation*);  /**< read the "Location" line. */
    void readSUMOEdge(string, ifstream&);          /**< read the "edge" line. */
    void readSUMOLane(string, SUMOLane&);          /**< read the "lane" line. */
    void readSUMOLogicitem(string, SUMOLogicitem*);/**< read the "SUMOLogicitem" line. */
    void readSUMOTLlogic(string, ifstream&);       /**< read the "traffic-logic" line. */
    void readSUMOPhase(string, SUMOPhase*);        /**< read the "SUMOPhase" line. */
    void readSUMOJunction(string);                 /**< read the "junction" line. */
    void readSUMOSucc(string, ifstream&);          /**< read the "succ" line. */
    void readSUMOSUMOSucclane(string, SUMOSUMOSucclane*);  /**< read the "SUMOSUMOSucclane" line. */

    void connectSUMOLanes();
    void createTrafficLightList();
};

}

#endif /*SUMO_ON*/

#endif /*SUMODIGITALMAP_H_*/
