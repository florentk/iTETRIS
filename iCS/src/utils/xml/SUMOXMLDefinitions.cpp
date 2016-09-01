/****************************************************************************/
/// @file    SUMOXMLDefinitions.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id: SUMOXMLDefinitions.cpp 11359 2011-10-07 12:24:50Z namdre $
///
// Definitions of elements and attributes known by SUMO
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "SUMOXMLDefinitions.h"
#include <utils/common/StringBijection.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// definitions
// ===========================================================================

StringBijection<int>::Entry SUMOXMLDefinitions::tags[] = {
    { "edge",             SUMO_TAG_EDGE },
    { "lane",             SUMO_TAG_LANE },
    { "poi",              SUMO_TAG_POI },
    { "poly",             SUMO_TAG_POLY },
    { "junction",         SUMO_TAG_JUNCTION },
    { "e1-detector",      SUMO_TAG_E1DETECTOR__DEPRECATED },
    { "e1Detector",       SUMO_TAG_E1DETECTOR },
    { "inductionLoop",    SUMO_TAG_INDUCTION_LOOP },
    { "e2-detector",      SUMO_TAG_E2DETECTOR__DEPRECATED },
    { "e2Detector",       SUMO_TAG_E2DETECTOR },
    { "laneAreaDetector", SUMO_TAG_LANE_AREA_DETECTOR },
    { "e3-detector",      SUMO_TAG_E3DETECTOR__DEPRECATED },
    { "e3Detector",       SUMO_TAG_E3DETECTOR },
    { "entryExitDetector",SUMO_TAG_ENTRY_EXIT_DETECTOR },
    { "meandata-edge",    SUMO_TAG_MEANDATA_EDGE__DEPRECATED },
    { "edgeData",         SUMO_TAG_MEANDATA_EDGE },
    { "meandata-lane",    SUMO_TAG_MEANDATA_LANE__DEPRECATED },
    { "laneData",         SUMO_TAG_MEANDATA_LANE },

#ifdef _MESSAGES
    { "msgemitter",		  SUMO_TAG_MSG_EMITTER },
#endif
    { "det_entry",        SUMO_TAG_DET_ENTRY__DEPRECATED },
    { "detEntry",         SUMO_TAG_DET_ENTRY },
    { "det_exit",         SUMO_TAG_DET_EXIT__DEPRECATED },
    { "detExit",          SUMO_TAG_DET_EXIT },
    { "edgeFollowDetector", SUMO_TAG_EDGEFOLLOWDETECTOR },
    { "instantInductionLoop", SUMO_TAG_INSTANT_INDUCTION_LOOP },
    { "variableSpeedSign",SUMO_TAG_VSS },
    { "calibrator",       SUMO_TAG_CALIBRATOR },
    { "rerouter",         SUMO_TAG_REROUTER },
    { "busStop",          SUMO_TAG_BUS_STOP },
    { "vtypeprobe",       SUMO_TAG_VTYPEPROBE__DEPRECATED },
    { "vTypeProbe",       SUMO_TAG_VTYPEPROBE },
    { "routeprobe",       SUMO_TAG_ROUTEPROBE__DEPRECATED },
    { "routeProbe",       SUMO_TAG_ROUTEPROBE },
    { "vehicle",          SUMO_TAG_VEHICLE },
    { "vtype",            SUMO_TAG_VTYPE__DEPRECATED },
    { "vType",            SUMO_TAG_VTYPE },
    { "route",            SUMO_TAG_ROUTE },
    { "succ",             SUMO_TAG_SUCC },
    { "succlane",         SUMO_TAG_SUCCLANE },
    { "logicitem",        SUMO_TAG_LOGICITEM }, // deprecated
    { "row-logic",        SUMO_TAG_ROWLOGIC__DEPRECATED }, // deprecated
    { "request",          SUMO_TAG_REQUEST },
    { "source",           SUMO_TAG_SOURCE },
    { "district",         SUMO_TAG_DISTRICT__DEPRECATED },
    { "taz",              SUMO_TAG_TAZ },
    { "dsource",          SUMO_TAG_DSOURCE__DEPRECATED },
    { "tazSource",        SUMO_TAG_TAZSOURCE },
    { "dsink",            SUMO_TAG_DSINK__DEPRECATED },
    { "tazSink",          SUMO_TAG_TAZSINK },
    { "tl-logic",         SUMO_TAG_TLLOGIC__DEPRECATED },
    { "tlLogic",          SUMO_TAG_TLLOGIC },
    { "phase",            SUMO_TAG_PHASE },
    { "tripdef",          SUMO_TAG_TRIP__DEPRECATED },
    { "trip",             SUMO_TAG_TRIP },
    { "flow",             SUMO_TAG_FLOW },
    { "step",             SUMO_TAG_STEP },
    { "interval",         SUMO_TAG_INTERVAL },
    { "timed_event",      SUMO_TAG_TIMEDEVENT__DEPRECATED },
    { "timedEvent",       SUMO_TAG_TIMEDEVENT },
    { "fromedge",         SUMO_TAG_FROMEDGE__DEPRECATED },
    { "fromEdge",         SUMO_TAG_FROMEDGE },
    { "toedge",           SUMO_TAG_TOEDGE__DEPRECATED },
    { "toEdge",           SUMO_TAG_TOEDGE },
    { "sink",             SUMO_TAG_SINK },
    { "param",	          SUMO_TAG_PARAM },
    { "WAUT",	          SUMO_TAG_WAUT },
    { "wautSwitch",       SUMO_TAG_WAUT_SWITCH },
    { "wautJunction",     SUMO_TAG_WAUT_JUNCTION },
    { "reset",        	  SUMO_TAG_RESET },
    { "stop",        	  SUMO_TAG_STOP },
    { "dest_prob_reroute", SUMO_TAG_DEST_PROB_REROUTE__DEPRECATED },
    { "destProbReroute",  SUMO_TAG_DEST_PROB_REROUTE },
    { "closing_reroute",  SUMO_TAG_CLOSING_REROUTE__DEPRECATED },
    { "closingReroute",   SUMO_TAG_CLOSING_REROUTE },
    { "route_prob_reroute", SUMO_TAG_ROUTE_PROB_REROUTE__DEPRECATED },
    { "routeProbReroute", SUMO_TAG_ROUTE_PROB_REROUTE },
    { "polytype",         SUMO_TAG_POLYTYPE__DEPRECATED },
    { "polygonType",      SUMO_TAG_POLYTYPE },
    { "connection",       SUMO_TAG_CONNECTION },
    { "prohibition",      SUMO_TAG_PROHIBITION },
    { "split",            SUMO_TAG_SPLIT },
    { "node",             SUMO_TAG_NODE },
    { "type",             SUMO_TAG_TYPE },
    { "detector_definition", SUMO_TAG_DETECTOR_DEFINITION__DEPRECATED },
    { "detectorDefinition",  SUMO_TAG_DETECTOR_DEFINITION },
    { "routeDistribution",   SUMO_TAG_ROUTE_DISTRIBUTION },
    { "vtypeDistribution",   SUMO_TAG_VTYPE_DISTRIBUTION__DEPRECATED },
    { "vTypeDistribution",   SUMO_TAG_VTYPE_DISTRIBUTION },
    { "vaporizer",        SUMO_TAG_VAPORIZER },
    { "roundabout",       SUMO_TAG_ROUNDABOUT },
    { "join",             SUMO_TAG_JOIN },
    { "joinExclude",      SUMO_TAG_JOINEXCLUDE },

    { "way",              SUMO_TAG_WAY },
    { "nd",               SUMO_TAG_ND },
    { "tag",              SUMO_TAG_TAG },

    { "decal",            SUMO_TAG_VIEWSETTINGS_DECAL },
    { "scheme",           SUMO_TAG_VIEWSETTINGS_SCHEME },
    { "opengl",           SUMO_TAG_VIEWSETTINGS_OPENGL },
    { "background",       SUMO_TAG_VIEWSETTINGS_BACKGROUND },
    { "edges",            SUMO_TAG_VIEWSETTINGS_EDGES },
    { "nlcC",             SUMO_TAG_VIEWSETTINGS_EDGE_COLOR_ITEM },
    { "vehicles",         SUMO_TAG_VIEWSETTINGS_VEHICLES },
    { "nvcC",             SUMO_TAG_VIEWSETTINGS_VEHICLE_COLOR_ITEM },
    { "junctions",        SUMO_TAG_VIEWSETTINGS_JUNCTIONS },
    { "additionals",      SUMO_TAG_VIEWSETTINGS_ADDITIONALS },
    { "pois",             SUMO_TAG_VIEWSETTINGS_POIS },
    { "legend",           SUMO_TAG_VIEWSETTINGS_LEGEND },
    { "include",          SUMO_TAG_INCLUDE },
    { "xi:include",       SUMO_TAG_INCLUDE },
    { "delay",            SUMO_TAG_DELAY },
    { "viewport",         SUMO_TAG_VIEWPORT },
    { "snapshot",         SUMO_TAG_SNAPSHOT },
    { "location",         SUMO_TAG_LOCATION },
    { "colorScheme",      SUMO_TAG_COLORSCHEME },
    { "entry",            SUMO_TAG_ENTRY },

    { "carFollowing-IDM",         SUMO_TAG_CF_IDM },
    { "carFollowing-IDMM",        SUMO_TAG_CF_IDMM },
    { "carFollowing-Krauss",      SUMO_TAG_CF_KRAUSS },
    { "carFollowing-KraussOrig1", SUMO_TAG_CF_KRAUSS_ORIG1 },
    { "carFollowing-PWagner2009", SUMO_TAG_CF_PWAGNER2009 },
    { "carFollowing-BKerner",     SUMO_TAG_CF_BKERNER },
    { "carFollowing-Wiedemann",   SUMO_TAG_CF_WIEDEMANN },

    { "person",           SUMO_TAG_PERSON },
    { "ride",             SUMO_TAG_RIDE },
    { "walk",             SUMO_TAG_WALK },

    // ActivityGen statistics file
    { "general",          AGEN_TAG_GENERAL },
    { "street",           AGEN_TAG_STREET },
    { "workHours",        AGEN_TAG_WORKHOURS },
    { "opening",          AGEN_TAG_OPENING },
    { "closing",          AGEN_TAG_CLOSING },
    { "schools",          AGEN_TAG_SCHOOLS },
    { "school",           AGEN_TAG_SCHOOL },
    { "busStation",       AGEN_TAG_BUSSTATION },
    { "busLine",          AGEN_TAG_BUSLINE },
    { "stations",         AGEN_TAG_STATIONS },
    { "revStations",      AGEN_TAG_REV_STATIONS },
    { "station",          AGEN_TAG_STATION },
    { "frequency",        AGEN_TAG_FREQUENCY },
    { "population",       AGEN_TAG_POPULATION },
    { "bracket",          AGEN_TAG_BRACKET },
    { "cityGates",        AGEN_TAG_CITYGATES },
    { "entrance",         AGEN_TAG_ENTRANCE },
    { "parameters",       AGEN_TAG_PARAM },

    { "",                 SUMO_TAG_NOTHING }
};


StringBijection<int>::Entry SUMOXMLDefinitions::attrs[] = {
    { "id",             SUMO_ATTR_ID },
    { "refid",          SUMO_ATTR_REFID__DEPRECATED },
    { "refId",          SUMO_ATTR_REFID },
    { "name",           SUMO_ATTR_NAME },
    { "type",           SUMO_ATTR_TYPE },
    { "priority",       SUMO_ATTR_PRIORITY },
    { "nolanes",        SUMO_ATTR_NOLANES__DEPRECATED },
    { "numLanes",       SUMO_ATTR_NUMLANES },
    { "speed",          SUMO_ATTR_SPEED },
    { "oneway",         SUMO_ATTR_ONEWAY },
    { "width",          SUMO_ATTR_WIDTH },
    { "remove",         SUMO_ATTR_REMOVE },

    { "length",         SUMO_ATTR_LENGTH },
    { "fromnode",       SUMO_ATTR_FROMNODE },
    { "tonode",         SUMO_ATTR_TONODE },
    { "xfrom",          SUMO_ATTR_XFROM },
    { "yfrom",          SUMO_ATTR_YFROM },
    { "xto",            SUMO_ATTR_XTO },
    { "yto",            SUMO_ATTR_YTO },

    { "x",              SUMO_ATTR_X },
    { "y",              SUMO_ATTR_Y },
    { "z",              SUMO_ATTR_Z },

    { "key",            SUMO_ATTR_KEY },
    { "requestSize",    SUMO_ATTR_REQUESTSIZE },
    { "request",        SUMO_ATTR_REQUEST },
    { "response",       SUMO_ATTR_RESPONSE },
    { "programID",      SUMO_ATTR_PROGRAMID },
    { "offset",         SUMO_ATTR_OFFSET },
    { "endOffset",      SUMO_ATTR_ENDOFFSET },
    { "incLanes",       SUMO_ATTR_INCLANES },
    { "intLanes",       SUMO_ATTR_INTLANES },

    { "weight",         SUMO_ATTR_WEIGHT },
    { "edge",           SUMO_ATTR_EDGE },
    { "edges",          SUMO_ATTR_EDGES },

    { "depart",         SUMO_ATTR_DEPART },
    { "departlane",     SUMO_ATTR_DEPARTLANE__DEPRECATED },
    { "departLane",     SUMO_ATTR_DEPARTLANE },
    { "departpos",      SUMO_ATTR_DEPARTPOS__DEPRECATED },
    { "departPos",      SUMO_ATTR_DEPARTPOS },
    { "departspeed",    SUMO_ATTR_DEPARTSPEED__DEPRECATED },
    { "departSpeed",    SUMO_ATTR_DEPARTSPEED },
    { "arrivallane",    SUMO_ATTR_ARRIVALLANE__DEPRECATED },
    { "arrivalLane",    SUMO_ATTR_ARRIVALLANE },
    { "arrivalpos",     SUMO_ATTR_ARRIVALPOS__DEPRECATED },
    { "arrivalPos",     SUMO_ATTR_ARRIVALPOS },
    { "arrivalspeed",   SUMO_ATTR_ARRIVALSPEED__DEPRECATED },
    { "arrivalSpeed",   SUMO_ATTR_ARRIVALSPEED },
    { "route",          SUMO_ATTR_ROUTE },
    { "maxspeed",       SUMO_ATTR_MAXSPEED__DEPRECATED },
    { "maxSpeed",       SUMO_ATTR_MAXSPEED },
    { "accel",          SUMO_ATTR_ACCEL },
    { "decel",          SUMO_ATTR_DECEL },
    { "vclass",         SUMO_ATTR_VCLASS__DEPRECATED },
    { "vClass",         SUMO_ATTR_VCLASS },
    { "repno",          SUMO_ATTR_REPNUMBER },
    { "speedFactor",    SUMO_ATTR_SPEEDFACTOR },
    { "speedDev",       SUMO_ATTR_SPEEDDEV },
    { "laneChangeModel", SUMO_ATTR_LANE_CHANGE_MODEL },
    { "minGap",         SUMO_ATTR_MINGAP },

    { "sigma",          SUMO_ATTR_SIGMA },
    { "tau",            SUMO_ATTR_TAU },

    { "last",           SUMO_ATTR_LAST },
    { "cost",           SUMO_ATTR_COST },
    { "probability",    SUMO_ATTR_PROB },
    { "routes",         SUMO_ATTR_ROUTES },
    { "vtypes",         SUMO_ATTR_VTYPES__DEPRECATED },
    { "vTypes",         SUMO_ATTR_VTYPES },

    { "lane",           SUMO_ATTR_LANE },
    { "lanes",          SUMO_ATTR_LANES },
    { "from",           SUMO_ATTR_FROM },
    { "to",             SUMO_ATTR_TO },
    { "period",         SUMO_ATTR_PERIOD },
    { "fromtaz",        SUMO_ATTR_FROM_TAZ__DEPRECATED },
    { "fromTaz",        SUMO_ATTR_FROM_TAZ },
    { "totaz",          SUMO_ATTR_TO_TAZ__DEPRECATED },
    { "toTaz",          SUMO_ATTR_TO_TAZ },
    { "reroute",        SUMO_ATTR_REROUTE },

    { "function",       SUMO_ATTR_FUNCTION },
    { "pos",            SUMO_ATTR_POSITION },
    { "freq",           SUMO_ATTR_FREQUENCY },
    { "style",          SUMO_ATTR_STYLE },
    { "file",           SUMO_ATTR_FILE },
    { "junction",       SUMO_ATTR_JUNCTION },
    { "no",             SUMO_ATTR_NO__DEPRECATED },
    { "number",         SUMO_ATTR_NUMBER },
    { "duration",       SUMO_ATTR_DURATION },
    { "until",          SUMO_ATTR_UNTIL },

    { "time",           SUMO_ATTR_TIME },
    { "begin",          SUMO_ATTR_BEGIN },
    { "end",            SUMO_ATTR_END },
    { "tl",             SUMO_ATTR_TLID },
    { "linkno",         SUMO_ATTR_TLLINKNO__DEPRECATED },
    { "linkIndex",      SUMO_ATTR_TLLINKINDEX },
    { "shape",          SUMO_ATTR_SHAPE },
    { "spread_type",    SUMO_ATTR_SPREADFUNC__DEPRECATED },
    { "spreadType",     SUMO_ATTR_SPREADTYPE },
    { "color",          SUMO_ATTR_COLOR },
    { "dir",            SUMO_ATTR_DIR },
    { "state",          SUMO_ATTR_STATE },
    { "layer",          SUMO_ATTR_LAYER },
    { "fill",           SUMO_ATTR_FILL },
    { "prefix",         SUMO_ATTR_PREFIX },
    { "discard",        SUMO_ATTR_DISCARD },

    { "fromLane",       SUMO_ATTR_FROM_LANE },
    { "toLane",         SUMO_ATTR_TO_LANE },
    { "dest",           SUMO_ATTR_DEST },
    { "source",         SUMO_ATTR_SOURCE },
    { "via",            SUMO_ATTR_VIA },
    { "nodes",          SUMO_ATTR_NODES },

#ifdef _MESSAGES
    { "msg",			SUMO_ATTR_MSG },
    { "emit_msg",		SUMO_ATTR_EVENTS },
    { "reverse",		SUMO_ATTR_REVERSE },
    { "table",			SUMO_ATTR_TABLE },
    { "xy",				SUMO_ATTR_XY },
    { "step",			SUMO_ATTR_STEP },
#endif
    { "min_dur",        SUMO_ATTR_MINDURATION__DEPRECATED },
    { "minDur",         SUMO_ATTR_MINDURATION },
    { "max_dur",        SUMO_ATTR_MAXDURATION__DEPRECATED },
    { "maxDur",         SUMO_ATTR_MAXDURATION },
    { "foes",           SUMO_ATTR_FOES },

    { "cont",           SUMO_ATTR_CONT },
    { "time_treshold",  SUMO_ATTR_HALTING_TIME_THRESHOLD__DEPRECATED },
    { "timeThreshold",  SUMO_ATTR_HALTING_TIME_THRESHOLD },
    { "speed_treshold", SUMO_ATTR_HALTING_SPEED_THRESHOLD__DEPRECATED },
    { "speedThreshold", SUMO_ATTR_HALTING_SPEED_THRESHOLD },
    { "jam_treshold",   SUMO_ATTR_JAM_DIST_THRESHOLD__DEPRECATED },
    { "jamThreshold",   SUMO_ATTR_JAM_DIST_THRESHOLD },

    { "wautID",		    SUMO_ATTR_WAUT_ID },
    { "junctionID",     SUMO_ATTR_JUNCTION_ID },
    { "procedure",	    SUMO_ATTR_PROCEDURE },
    { "synchron",	    SUMO_ATTR_SYNCHRON },
    { "refTime",	    SUMO_ATTR_REF_TIME },
    { "startProg",	    SUMO_ATTR_START_PROG },
    { "off",            SUMO_ATTR_OFF },
    { "friendly_pos",   SUMO_ATTR_FRIENDLY_POS__DEPRECATED },
    { "friendlyPos",    SUMO_ATTR_FRIENDLY_POS },
    { "splitByType",    SUMO_ATTR_SPLIT_VTYPE },
    { "uncontrolled",   SUMO_ATTR_UNCONTROLLED },
    { "pass",           SUMO_ATTR_PASS },
    { "bus_stop",       SUMO_ATTR_BUS_STOP__DEPRECATED },
    { "busStop",        SUMO_ATTR_BUS_STOP },
    { "line",           SUMO_ATTR_LINE },
    { "lines",          SUMO_ATTR_LINES },
    { "value",          SUMO_ATTR_VALUE },
    { "prohibitor",     SUMO_ATTR_PROHIBITOR },
    { "prohibited",     SUMO_ATTR_PROHIBITED },
    { "allow",          SUMO_ATTR_ALLOW },
    { "disallow",       SUMO_ATTR_DISALLOW },
    { "prefer",         SUMO_ATTR_PREFER },
    { "controlled_inner", SUMO_ATTR_CONTROLLED_INNER__DEPRECATED },
    { "controlledInner", SUMO_ATTR_CONTROLLED_INNER },
    { "vehsPerHour",    SUMO_ATTR_VEHSPERHOUR },
    { "output",         SUMO_ATTR_OUTPUT },
    { "guiWidth",       SUMO_ATTR_GUIWIDTH },
    { "guiOffset",      SUMO_ATTR_GUIOFFSET },
    { "guiShape",       SUMO_ATTR_GUISHAPE },
    { "emissionClass",  SUMO_ATTR_EMISSIONCLASS },
    { "startPos",       SUMO_ATTR_STARTPOS },
    { "endPos",         SUMO_ATTR_ENDPOS },
    { "triggered",      SUMO_ATTR_TRIGGERED },
    { "parking",        SUMO_ATTR_PARKING },
    { "index",          SUMO_ATTR_INDEX },

    { "entering",       SUMO_ATTR_ENTERING },
    { "excludeEmpty",   SUMO_ATTR_EXCLUDE_EMPTY },
    { "withInternal",   SUMO_ATTR_WITH_INTERNAL },
    { "trackVehicles",  SUMO_ATTR_TRACK_VEHICLES },
    { "maxTraveltime",  SUMO_ATTR_MAX_TRAVELTIME },
    { "minSamples",     SUMO_ATTR_MIN_SAMPLES },

    { "lon",            SUMO_ATTR_LON },
    { "lat",            SUMO_ATTR_LAT },
    { "k",              SUMO_ATTR_K },
    { "v",              SUMO_ATTR_V },
    { "ref",            SUMO_ATTR_REF },
    { "href",           SUMO_ATTR_HREF },
    { "zoom",           SUMO_ATTR_ZOOM },
    { "interpolated",   SUMO_ATTR_INTERPOLATED },
    { "threshold",      SUMO_ATTR_THRESHOLD },

    { "netOffset",      SUMO_ATTR_NET_OFFSET },
    { "convBoundary",   SUMO_ATTR_CONV_BOUNDARY },
    { "origBoundary",   SUMO_ATTR_ORIG_BOUNDARY },
    { "projParameter",  SUMO_ATTR_ORIG_PROJ },

    { "tauLast",        SUMO_ATTR_CF_PWAGNER2009_TAULAST },
    { "apProb",         SUMO_ATTR_CF_PWAGNER2009_APPROB },
    { "delta",          SUMO_ATTR_CF_IDM_DELTA },
    { "stepping",       SUMO_ATTR_CF_IDM_STEPPING },
    { "adaptFactor",    SUMO_ATTR_CF_IDMM_ADAPT_FACTOR },
    { "adaptTime",      SUMO_ATTR_CF_IDMM_ADAPT_TIME },
    { "phi",            SUMO_ATTR_CF_KERNER_PHI },

    { "security",       SUMO_ATTR_CF_WIEDEMANN_SECURITY },
    { "estimation",     SUMO_ATTR_CF_WIEDEMANN_ESTIMATION },

    { "generateWalks",  SUMO_ATTR_GENERATE_WALKS },

    // ActivityGen statistics file
    { "inhabitants",    AGEN_ATTR_INHABITANTS },
    { "households",     AGEN_ATTR_HOUSEHOLDS },
    { "childrenAgeLimit", AGEN_ATTR_CHILDREN },
    { "retirementAgeLimit", AGEN_ATTR_RETIREMENT },
    { "carRate",        AGEN_ATTR_CARS },
    { "unemploymentRate", AGEN_ATTR_UNEMPLOYEMENT },
    { "footDistanceLimit", AGEN_ATTR_MAX_FOOT_DIST },
    { "incomingTraffic", AGEN_ATTR_IN_TRAFFIC },
    { "incoming", AGEN_ATTR_INCOMING },
    { "outgoingTraffic", AGEN_ATTR_OUT_TRAFFIC },
    { "outgoing", AGEN_ATTR_OUTGOING },
    { "population",     AGEN_ATTR_POPULATION },
    { "workPosition",   AGEN_ATTR_OUT_WORKPOSITION },
    { "hour",           AGEN_ATTR_HOUR },
    { "proportion",     AGEN_ATTR_PROP },
    { "capacity",       AGEN_ATTR_CAPACITY },
    { "opening",        AGEN_ATTR_OPENING },
    { "closing",        AGEN_ATTR_CLOSING },
    { "maxTripDuration", AGEN_ATTR_MAX_TRIP_DURATION },
    { "rate",           AGEN_ATTR_RATE },
    { "beginAge",       AGEN_ATTR_BEGINAGE },
    { "endAge",         AGEN_ATTR_ENDAGE },
    { "peopleNbr",      AGEN_ATTR_PEOPLENBR },
    { "carPreference",  AGEN_ATTR_CARPREF },
    { "meanTimePerKmInCity", AGEN_ATTR_CITYSPEED },
    { "freeTimeActivityRate", AGEN_ATTR_FREETIMERATE },
    { "uniformRandomTraffic", AGEN_ATTR_UNI_RAND_TRAFFIC},
    { "departureVariation", AGEN_ATTR_DEP_VARIATION},

    { "modificationStatusNotForPrinting", GNE_ATTR_MODIFICATION_STATUS},
    { "shapeStartNotForPrinting", GNE_ATTR_SHAPE_START},
    { "shapeEndNotForPrinting", GNE_ATTR_SHAPE_END},

    { "",               SUMO_ATTR_NOTHING }
};


StringBijection<SumoXMLNodeType>::Entry SUMOXMLDefinitions::sumoNodeTypeValues[] = {
    {"traffic_light",       NODETYPE_TRAFFIC_LIGHT},
    {"priority",            NODETYPE_PRIORITY_JUNCTION},
    {"right_before_left",   NODETYPE_RIGHT_BEFORE_LEFT},
    {"district",            NODETYPE_DISTRICT},
    {"unregulated",         NODETYPE_NOJUNCTION},
    {"internal",            NODETYPE_INTERNAL},
    {"dead_end",            NODETYPE_DEAD_END},
    {"DEAD_END",            NODETYPE_DEAD_END_DEPRECATED},

    {"unknown",             NODETYPE_UNKNOWN}
};


StringBijection<SumoXMLEdgeFunc>::Entry SUMOXMLDefinitions::sumoEdgeFuncValues[] = {
    {"normal",       EDGEFUNC_NORMAL},
    {"connector",    EDGEFUNC_CONNECTOR},
    {"sink",         EDGEFUNC_SINK},
    {"source",       EDGEFUNC_SOURCE},

    {"internal",     EDGEFUNC_INTERNAL}
};


StringBijection<LaneSpreadFunction>::Entry SUMOXMLDefinitions::laneSpreadFunctionValues[] = {
    {"right",             LANESPREAD_RIGHT}, // default
    {"center",            LANESPREAD_CENTER}
};


StringBijection<LinkState>::Entry SUMOXMLDefinitions::linkStateValues[] = {
    { "G", LINKSTATE_TL_GREEN_MAJOR },
    { "g", LINKSTATE_TL_GREEN_MINOR },
    { "r", LINKSTATE_TL_RED },
    { "Y", LINKSTATE_TL_YELLOW_MAJOR },
    { "y", LINKSTATE_TL_YELLOW_MINOR },
    { "o", LINKSTATE_TL_OFF_BLINKING },
    { "O", LINKSTATE_TL_OFF_NOSIGNAL },
    { "M", LINKSTATE_MAJOR },
    { "m", LINKSTATE_MINOR },
    { "=", LINKSTATE_EQUAL },
    { "-", LINKSTATE_DEADEND }
};


StringBijection<LinkDirection>::Entry SUMOXMLDefinitions::linkDirectionValues[] = {
    { "s", LINKDIR_STRAIGHT },
    { "t", LINKDIR_TURN },
    { "l", LINKDIR_LEFT },
    { "r", LINKDIR_RIGHT },
    { "L", LINKDIR_PARTLEFT },
    { "R", LINKDIR_PARTRIGHT },
    { "invalid", LINKDIR_NODIR }
};


StringBijection<TrafficLightType>::Entry SUMOXMLDefinitions::trafficLightTypesVales[] = {
    { "static", TLTYPE_STATIC },
    { "actuated", TLTYPE_ACTUATED },
    { "agentbased", TLTYPE_AGENT }
};

StringBijection<int> SUMOXMLDefinitions::Tags(
    SUMOXMLDefinitions::tags, SUMO_TAG_NOTHING);

StringBijection<int> SUMOXMLDefinitions::Attrs(
    SUMOXMLDefinitions::attrs, SUMO_ATTR_NOTHING);

StringBijection<SumoXMLNodeType> SUMOXMLDefinitions::NodeTypes(
    SUMOXMLDefinitions::sumoNodeTypeValues, NODETYPE_UNKNOWN);

StringBijection<SumoXMLEdgeFunc> SUMOXMLDefinitions::EdgeFunctions(
    SUMOXMLDefinitions::sumoEdgeFuncValues, EDGEFUNC_INTERNAL);

StringBijection<LaneSpreadFunction> SUMOXMLDefinitions::LaneSpreadFunctions(
    SUMOXMLDefinitions::laneSpreadFunctionValues, LANESPREAD_CENTER);

StringBijection<LinkState> SUMOXMLDefinitions::LinkStates(
    SUMOXMLDefinitions::linkStateValues, LINKSTATE_DEADEND);

StringBijection<LinkDirection> SUMOXMLDefinitions::LinkDirections(
    SUMOXMLDefinitions::linkDirectionValues, LINKDIR_NODIR);

StringBijection<TrafficLightType> SUMOXMLDefinitions::TrafficLightTypes(
    SUMOXMLDefinitions::trafficLightTypesVales, TLTYPE_AGENT);

/****************************************************************************/

