/****************************************************************************/
/// @file    SUMOdigital-map.cpp
/// @author  Pasquale Cataldi (EURECOM)
/// @date    Jan 8, 2010
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright © 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/

/*! \file SUMOdigital-map.cpp
 \brief Parsing and loading of the DigitalMap information.
*/

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "SUMOdigital-map.h"
#include "../../../utils/common/TplConvert.h"

#ifdef SUMO_ON

namespace sumo_map {

#include <stdlib.h>
using namespace std;

void tokenize(const string&, vector<string>&, const string&);

/*
 *
 * SUMODigitalMap
 *
 */
SUMODigitalMap::SUMODigitalMap() {
    readLines = 0;
}

SUMODigitalMap::SUMODigitalMap(string filename) {
    NETfilename = filename;
    readLines = 0;
}

SUMODigitalMap::~SUMODigitalMap() {
}

/*! \fn bool SUMODigitalMap::loadMap(string filename)
 \brief Load the data stored in filename (that will be stored as NETfilename).
 It uses loadMap().
 \param[in] filename string with the name (and the path) of the file to be parsed.
 \param[out] true if the loading process succeeded, false otherwise (i.e. file not found).
 */
bool SUMODigitalMap::loadMap(string filename) {
    NETfilename = filename;
    return loadMap();
}

/*! \fn bool SUMODigitalMap::loadMap()
 \brief Load the data stored in the NETfilename variable.
 \param[out] true if the loading process succeeded, false otherwise (i.e. file not found).
 */
bool SUMODigitalMap::loadMap() {
    ifstream infile;
    infile.open(NETfilename.c_str());
    if (!infile.is_open()) {
        cerr << NETfilename << " not found. Map loading terminated." << endl;
        return false;
    }
    string line;
    while (getline(infile, line)) {
        readLines++;
        if (!line.empty()) {
            // Read the SUMOLocation
            if (line.find("<SUMOLocation") != string::npos)
                readSUMOLocation(line, &location);
            // Read the EDGE
            if (line.find("<edge") != string::npos)
                readSUMOEdge(line, infile);
//			// Read the ROW-LOGIC
//			if (line.find("<row-logic") != string::npos)
//				readRowLogic(line, infile);
            // Read the TRAFFICLIGHT
            if (line.find("<tl-logic") != string::npos)
                readSUMOTLlogic(line, infile);
            // Read the JUNCTION
            if (line.find("<junction") != string::npos)
                readSUMOJunction(line);
            // Read the SUCC
            if (line.find("<succ") != string::npos)
                readSUMOSucc(line, infile);
            // Read the END of NET
            if (line.find("</net>") != string::npos)
                break;
            // Read and skip the comments
            if (line.find("<!--") != string::npos)
                // Read and discard the following lines until the end-comment marker "-->" is found
                while (getline(infile, line))
                    if (line.find("-->") != string::npos)
                        break;
        } // end "if (!line.empty())"
    } // end "while (getline(infile, line))"

    connectSUMOLanes();
    createTrafficLightList();
    return true;
}

void SUMODigitalMap::readSUMOLocation(string line, SUMOLocation *loc) {
    vector<string> fields;

    // Separate fields
    tokenize(line, fields, " \",");
    // Find the fields and store the values
    for (unsigned int i = 1; i < fields.size(); i++) {
        if ((fields[i].find("netOffset=") != string::npos)
                && (fields[i + 1].find("=") == string::npos)
                && (fields[i + 1].find("/>") == string::npos)) {
            (*loc).netOffset.setx(TplConvert<char>::_2SUMOReal(fields[++i].c_str()));
            (*loc).netOffset.sety(TplConvert<char>::_2SUMOReal(fields[++i].c_str()));

        }

        if ((fields[i].find("convBoundary=") != string::npos)
                && (fields[i + 1].find("=") == string::npos)
                && (fields[i + 1].find("/>") == string::npos)) {
            (*loc).convBoundaryMin.setx(TplConvert<char>::_2SUMOReal(fields[++i].c_str()));
            (*loc).convBoundaryMin.sety(TplConvert<char>::_2SUMOReal(fields[++i].c_str()));



            (*loc).convBoundaryMax.setx(TplConvert<char>::_2SUMOReal(fields[++i].c_str()));
            (*loc).convBoundaryMax.sety(TplConvert<char>::_2SUMOReal(fields[++i].c_str()));
        }

        if ((fields[i].find("origBoundary=") != string::npos)
                && (fields[i + 1].find("=") == string::npos)
                && (fields[i + 1].find("/>") == string::npos)) {
            (*loc).origBoundaryMin.setx(TplConvert<char>::_2SUMOReal(fields[++i].c_str()));
            (*loc).origBoundaryMin.sety(TplConvert<char>::_2SUMOReal(fields[++i].c_str()));
            (*loc).origBoundaryMax.setx(TplConvert<char>::_2SUMOReal(fields[++i].c_str()));
            (*loc).origBoundaryMax.sety(TplConvert<char>::_2SUMOReal(fields[++i].c_str()));
        }

        if ((fields[i].find("projParameter=") != string::npos)
                && (fields[i + 1].find("=") == string::npos)
                && (fields[i + 1].find("/>") == string::npos)) {
            (*loc).projParameter = fields[++i].c_str()[0];
        }
    }
    return;
}

void SUMODigitalMap::readSUMOJunction(string line) {
    vector<string> fields;
    SUMOJunction currSUMOJunction;
    // Separate fields
    tokenize(line, fields, " \",");
    // Find the fields and store the values
    for (unsigned int i = 1; i < fields.size(); i++) {
        if ((fields[i].find("id=") != string::npos) && (fields[i + 1].find("=")
                == string::npos) && (fields[i + 1].find("/>") == string::npos)) {
            currSUMOJunction.id = fields[++i];
        }
        if ((fields[i].find("type=") != string::npos) && (fields[i + 1].find(
                    "=") == string::npos) && (fields[i + 1].find("/>")
                                              == string::npos)) {
            currSUMOJunction.type = fields[++i];
        }
        if ((fields[i].find("x=") != string::npos) && (fields[i + 1].find("=")
                == string::npos) && (fields[i + 1].find("/>") == string::npos)) {
            currSUMOJunction.center.setx(TplConvert<char>::_2SUMOReal(fields[++i].c_str()));
        }
        if ((fields[i].find("y=") != string::npos) && (fields[i + 1].find("=")
                == string::npos)) {
            currSUMOJunction.center.sety(TplConvert<char>::_2SUMOReal(fields[++i].c_str()));
        }
        if ((fields[i].find("incLanes=") != string::npos)
                && (fields[i + 1].find("=") == string::npos)
                && (fields[i + 1].find("/>") == string::npos)) {
            while ((fields[i + 1].c_str()[fields[i + 1].size() - 1] != '=')
                    || (fields[i + 1].find("/>") != string::npos)) {
                currSUMOJunction.stringIncSUMOLanes.push_back(fields[++i]);
            }
        }
        if ((fields[i].find("intLanes=") != string::npos)
                && (fields[i + 1].find("=") == string::npos)
                && (fields[i + 1].find("/>") == string::npos)) {
            while ((fields[i + 1].c_str()[fields[i + 1].size() - 1] != '=')
                    || (fields[i + 1].find("/>") != string::npos)) {
                currSUMOJunction.stringIntSUMOLanes.push_back(fields[++i]);
            }
        }
        if ((fields[i].find("shape=") != string::npos) && (fields[i + 1].find(
                    "=") == string::npos) && (fields[i + 1].find("/>")
                                              == string::npos)) {
            do {
                Point2D p;
                p.setx(TplConvert<char>::_2SUMOReal(fields[++i].c_str()));
                p.sety(TplConvert<char>::_2SUMOReal(fields[++i].c_str()));
                currSUMOJunction.shape.push_back(p);
            } while ((fields[i + 1].c_str()[fields[i + 1].size() - 1] != '=')
                     && (fields[i + 1].find("/>") == string::npos));
        }
    }
    // Add the currSUMOJunction obj to the map
    junctions.insert(pair<string, SUMOJunction> (currSUMOJunction.id, currSUMOJunction));
    return;
}

void SUMODigitalMap::readSUMOEdge(string line, ifstream& infile) {
    vector<string> fields;
    SUMOEdge currSUMOEdge;
    // Parse the line to get the id and the function of the currSUMOEdge

    // Separate fields
    tokenize(line, fields, " \",");
    // Find the fields and store the values
    for (unsigned int i = 1; i < fields.size(); i++) {
        if ((fields[i].find("id=") != string::npos) && (fields[i + 1].find("=")
                == string::npos) && (fields[i + 1].find(">") == string::npos)) {
            currSUMOEdge.id = fields[++i];
        }
        if ((fields[i].find("from=") != string::npos) && (fields[i + 1].find(
                    "=") == string::npos) && (fields[i + 1].find(">")
                                              == string::npos)) {
            currSUMOEdge.stringFrom = fields[++i];
        }
        if ((fields[i].find("to=") != string::npos) && (fields[i + 1].find("=")
                == string::npos) && (fields[i + 1].find(">") == string::npos)) {
            currSUMOEdge.stringTo = fields[++i];
        }
        if ((fields[i].find("function=") != string::npos)
                && (fields[i + 1].find("=") == string::npos)
                && (fields[i + 1].find(">") == string::npos)) {
            currSUMOEdge.funct = fields[++i];
        }
        if ((fields[i].find("inner=") != string::npos) && (fields[i + 1].find(
                    "=") == string::npos) && (fields[i + 1].find(">")
                                              == string::npos)) {
            currSUMOEdge.inner = fields[++i];
        }
    }
    // Parse the new lines containing the lanes
    string newline;
    while (getline(infile, newline)) {
        readLines++;
        if (!newline.empty()) {
            if (newline.find("<lane") != string::npos) {
                SUMOLane currSUMOLane;
                readSUMOLane(newline, currSUMOLane);
                currSUMOEdge.lanes.insert(pair<string, SUMOLane> (currSUMOLane.id, currSUMOLane));
            }

            // Read the end of the lanes
            if (newline.find("</lanes>") != string::npos)
                break;
        }
    }
    // Store the new edge in the map
    edges.insert(pair<string, SUMOEdge> (currSUMOEdge.id, currSUMOEdge));
    return;
}

void SUMODigitalMap::readSUMOLane(string line, SUMOLane& laneObj) {
    // Parse the current line with the information about the lane
    vector<string> fields;
    // Separate fields
    tokenize(line, fields, " \",");
    // Find the fields and store the values
    for (unsigned int i = 1; i < fields.size(); i++) {
        if ((fields[i].find("id=") != string::npos) && (fields[i + 1].find("=")
                == string::npos) && (fields[i + 1].find("/>") == string::npos)) {
            laneObj.id = fields[++i];
        }
        if ((fields[i].find("depart=") != string::npos) && (fields[i + 1].find(
                    "=") == string::npos) && (fields[i + 1].find("/>")
                                              == string::npos)) {
            laneObj.depart = TplConvert<char>::_2SUMOReal(fields[++i].c_str());
        }
        if ((fields[i].find("maxspeed=") != string::npos)
                && (fields[i + 1].find("=") == string::npos)
                && (fields[i + 1].find("/>") == string::npos)) {
            laneObj.maxspeed = TplConvert<char>::_2SUMOReal(fields[++i].c_str());
        }
        if ((fields[i].find("length=") != string::npos) && (fields[i + 1].find(
                    "=") == string::npos) && (fields[i + 1].find("/>")
                                              == string::npos)) {
            laneObj.length = TplConvert<char>::_2SUMOReal(fields[++i].c_str());
        }
        if ((fields[i].find("shape=") != string::npos) && (fields[i + 1].find(
                    "=") == string::npos) && (fields[i + 1].find("/>")
                                              == string::npos)) {
            do {
                Point2D p;
                p.setx(TplConvert<char>::_2SUMOReal(fields[++i].c_str()));
                p.sety(TplConvert<char>::_2SUMOReal(fields[++i].c_str()));
                laneObj.shape.push_back(p);
            } while ((fields[i + 1].c_str()[fields[i + 1].size() - 1] != '=')
                     && (fields[i + 1].find("/>") == string::npos));
        }
    }
    return;
}


void SUMODigitalMap::readSUMOLogicitem(string line, SUMOLogicitem *currSUMOLogicitem) {
    vector<string> fields;
    // Parse the current line with the information about the lane
    tokenize(line, fields, " \",");
    // Find the fields and store the values
    for (unsigned int i = 1; i < fields.size(); i++) {
        if ((fields[i].find("request=") != string::npos)
                && (fields[i + 1].find("=") == string::npos)
                && (fields[i + 1].find("/>") == string::npos)) {
            (*currSUMOLogicitem).request = (short) TplConvert<char>::_2int(fields[++i].c_str());
        }
        if ((fields[i].find("response=") != string::npos)
                && (fields[i + 1].find("=") == string::npos)
                && (fields[i + 1].find("/>") == string::npos)) {
            (*currSUMOLogicitem).response = fields[++i];
        }
        if ((fields[i].find("foes=") != string::npos) && (fields[i + 1].find(
                    "=") == string::npos) && (fields[i + 1].find("/>")
                                              == string::npos)) {
            (*currSUMOLogicitem).foes = fields[++i];
        }
        if ((fields[i].find("cont=") != string::npos) && (fields[i + 1].find(
                    "=") == string::npos) && (fields[i + 1].find("/>")
                                              == string::npos)) {
            (*currSUMOLogicitem).cont = (bool)(TplConvert<char>::_2int(fields[++i].c_str()) == 1);
        }
    }
    return;
}

void SUMODigitalMap::readSUMOTLlogic(string line, ifstream& infile) {
    vector<string> fields;
    SUMOTLlogic currTL;
    // Parse the line to get the id and the function of the currTL
    tokenize(line, fields, " \",");
    // Find the fields and store the values
    for (unsigned int i = 1; i < fields.size(); i++) {
        if ((fields[i].find("id=") != string::npos) && (fields[i + 1].find("=")
                == string::npos) && (fields[i + 1].find(">") == string::npos)) {
            currTL.id = fields[++i];
        }
        if ((fields[i].find("type=") != string::npos) && (fields[i + 1].find(
                    "=") == string::npos) && (fields[i + 1].find(">")
                                              == string::npos)) {
            currTL.type = fields[++i];
        }
        if ((fields[i].find("programID=") != string::npos)
                && (fields[i + 1].find("=") == string::npos)
                && (fields[i + 1].find(">") == string::npos)) {
            currTL.programID = fields[++i];
        }
    }
    // Parse the new lines containing the phases
    string newline;
    while (getline(infile, newline)) {
        readLines++;
        if (!newline.empty()) {
            if (newline.find("<phase") != string::npos) {
                SUMOPhase currSUMOPhase;
                readSUMOPhase(newline, &currSUMOPhase);
                currTL.phases.push_back(currSUMOPhase);
            }
            // Read the end of the phases
            if (newline.find("</tl-logic>") != string::npos)
                break;
        }
    }
    // Store the new TrafficLight in the map
    tllogics.insert(pair<string, SUMOTLlogic> (currTL.id, currTL));
    return;
}

void SUMODigitalMap::readSUMOPhase(string line, SUMOPhase *currSUMOPhase) {
    vector<string> fields;
    // Parse the current line with the information about the lane
    tokenize(line, fields, " \",");
    // Find the fields and store the values
    for (unsigned int i = 1; i < fields.size(); i++) {
        if ((fields[i].find("duration=") != string::npos)
                && (fields[i + 1].find("=") == string::npos)
                && (fields[i + 1].find("/>") == string::npos)) {
            (*currSUMOPhase).duration = (short) TplConvert<char>::_2int(fields[++i].c_str());
        }
        if ((fields[i].find("state=") != string::npos) && (fields[i + 1].find(
                    "=") == string::npos) && (fields[i + 1].find("/>")
                                              == string::npos)) {
            (*currSUMOPhase).state = fields[++i];
        }
    }
    return;
}

void SUMODigitalMap::readSUMOSucc(string line, ifstream& infile) {
    vector<string> fields;
    SUMOSucc currSUMOSucc;
    // Parse the line to get the info of currSUMOSucc
    tokenize(line, fields, " \",");
    // Find the fields and store the values
    for (unsigned int i = 1; i < fields.size(); i++) {
        if ((fields[i].find("edge=") != string::npos) && (fields[i + 1].find(
                    "=") == string::npos) && (fields[i + 1].find(">")
                                              == string::npos)) {
            (currSUMOSucc).edge = fields[++i];
        }
        if ((fields[i].find("lane=") != string::npos) && (fields[i + 1].find(
                    "=") == string::npos) && (fields[i + 1].find(">")
                                              == string::npos)) {
            (currSUMOSucc).lane = fields[++i];
        }
        if ((fields[i].find("junction=") != string::npos)
                && (fields[i + 1].find("=") == string::npos)
                && (fields[i + 1].find(">") == string::npos)) {
            (currSUMOSucc).junction = fields[++i];
        }
    }
    // Parse the new lines containing the SUMOSUMOSucclane
    string newline;
    while (getline(infile, newline)) {
        readLines++;
        if (!newline.empty()) {
            if (newline.find("<SUMOSUMOSucclane") != string::npos) {
                SUMOSUMOSucclane currSUMOSUMOSucclane;
                readSUMOSUMOSucclane(newline, &currSUMOSUMOSucclane);
                (currSUMOSucc).SUMOSUMOSucclanes.push_back(currSUMOSUMOSucclane);
            }
            // Read the end of the SUMOSUMOSucclanes
            if (newline.find("</succ>") != string::npos)
                break;
        }
    }
    // Store the new succ in the vector
    succs.push_back(currSUMOSucc);
    return;
}

void SUMODigitalMap::readSUMOSUMOSucclane(string line, SUMOSUMOSucclane *currSUMOSUMOSucclane) {
    vector<string> fields;
    // Parse the current line with the information about the SUMOSUMOSucclane
    tokenize(line, fields, " \",");
    // Find the fields and store the values
    for (unsigned int i = 1; i < fields.size(); i++) {
        if ((fields[i].find("lane=") != string::npos) && (fields[i + 1].find(
                    "=") == string::npos) && (fields[i + 1].find("/>")
                                              == string::npos)) {
            (*currSUMOSUMOSucclane).lane = fields[++i];
        }
        if ((fields[i].find("via=") != string::npos)
                && (fields[i + 1].find("=") == string::npos)
                && (fields[i + 1].find("/>") == string::npos)) {
            (*currSUMOSUMOSucclane).via = fields[++i];
        }
        if ((fields[i].find("tl=") != string::npos)
                && (fields[i + 1].find("=") == string::npos)
                && (fields[i + 1].find("/>") == string::npos)) {
            (*currSUMOSUMOSucclane).tl = fields[++i];
            ++i;
            (*currSUMOSUMOSucclane).linkIndex = (short)TplConvert<char>::_2int(fields[++i].c_str());
        }
        if ((fields[i].find("yield=") != string::npos) && (fields[i + 1].find(
                    "=") == string::npos) && (fields[i + 1].find("/>")
                                              == string::npos)) {
            (*currSUMOSUMOSucclane).yield = (bool)(TplConvert<char>::_2int(fields[++i].c_str()) == 1);
        }
        if ((fields[i].find("dir=") != string::npos)
                && (fields[i + 1].find("=") == string::npos)
                && (fields[i + 1].find("/>") == string::npos)) {
            (*currSUMOSUMOSucclane).dir = (char) fields[++i].c_str()[0];
        }
        if ((fields[i].find("state=") != string::npos) && (fields[i + 1].find(
                    "=") == string::npos) && (fields[i + 1].find("/>")
                                              == string::npos)) {
            (*currSUMOSUMOSucclane).state = (char) fields[++i].c_str()[0];
        }
        if ((fields[i].find("int_end=") != string::npos)
                && (fields[i + 1].find("=") == string::npos)
                && (fields[i + 1].find("/>") == string::npos)) {
            (*currSUMOSUMOSucclane).int_end = (char) fields[++i].c_str()[0];
        }
    }
    return;
}


/*
void SUMODigitalMap::readRowLogic(string line, ifstream& infile) {
	vector<string> fields;
	RowLogic currRowLogic;
	// Parse the line to get the id and the function of the currRowLogic
	tokenize(line, fields, " \",");
	// Find the fields and store the values
	for (unsigned int i = 1; i < fields.size(); i++) {
		if ((fields[i].find("id=") != string::npos) && (fields[i + 1].find("=")
				== string::npos) && (fields[i + 1].find(">") == string::npos)) {
			(currRowLogic).id = fields[++i];
		}
		if ((fields[i].find("requestSize=") != string::npos)
				&& (fields[i + 1].find("=") == string::npos)
				&& (fields[i + 1].find(">") == string::npos)) {
			(currRowLogic).requestSize = (short) TplConvert<char>::_2int(fields[++i].c_str());
		}
		if ((fields[i].find("laneNumber=") != string::npos)
				&& (fields[i + 1].find("=") == string::npos)
				&& (fields[i + 1].find(">") == string::npos)) {
			(currRowLogic).laneNumber = (short) TplConvert<char>::_2int(fields[++i].c_str());
		}
	}
	// Parse the new lines containing the logicitems
	string newline;
	while (getline(infile, newline)) {
		readLines++;
		if (!newline.empty()) {
			if (newline.find("<logicitem") != string::npos) {
				SUMOLogicitem currSUMOLogicitem;
				readSUMOLogicitem(newline, &currSUMOLogicitem);
				(currRowLogic).logics.push_back(currSUMOLogicitem);
			}
			// Read the end of the logicitems
			if (newline.find("</logic>") != string::npos)
				break;
		}
	}
	// Store the new rowLogic in the map
	rowLogics.insert(pair<string, RowLogic> (currRowLogic.id, currRowLogic));
	return;
}
*/


const string SUMODigitalMap::getNETfilename() const {
    return NETfilename;
}


void SUMODigitalMap::connectSUMOLanes() {
    string currSUMOLane;
    string currSUMOEdge;
    string currSUMOJunction;
    for (unsigned int i=0; i<succs.size(); i++) {
        currSUMOLane = succs[i].lane;
        currSUMOEdge = succs[i].edge;
        currSUMOJunction = succs[i].junction;
        for (unsigned int j=0; j<succs[i].SUMOSUMOSucclanes.size(); j++) {
            string nextSUMOLaneId = "";
            if (!succs[i].SUMOSUMOSucclanes[j].via.empty())
                nextSUMOLaneId = succs[i].SUMOSUMOSucclanes[j].via;
            else
                nextSUMOLaneId = succs[i].SUMOSUMOSucclanes[j].lane;

            // Find the lane called nextSUMOLaneId
            SUMOLane* nextSUMOLane = findSUMOLane(nextSUMOLaneId);
            if (nextSUMOLane != 0) {
                // Connect the currSUMOLane to nextSUMOLaneId by pointer
                edges[currSUMOEdge].lanes[currSUMOLane].nextSUMOLanes.push_back(nextSUMOLane);
                // Connect the nextSUMOLaneId to currSUMOLane by pointer
                (*nextSUMOLane).prevSUMOLanes.push_back(&(edges[currSUMOEdge].lanes[currSUMOLane]));
            }
        }
    }
    return;
}


void SUMODigitalMap::createTrafficLightList() {
    string currSUMOLaneStr;
    for (unsigned int i=0; i<succs.size(); i++) {
        currSUMOLaneStr = succs[i].lane;
        SUMOLane* currSUMOLane = findSUMOLane(currSUMOLaneStr);
        if (currSUMOLane == NULL) continue;
        Point2D currTLpos = (*currSUMOLane).shape[(*currSUMOLane).shape.size()-1];
        for (unsigned int j=0; j<succs[i].SUMOSUMOSucclanes.size(); j++) {
            // Get the programID
            if (succs[i].SUMOSUMOSucclanes[j].tl.empty()) continue;
            // Create object SUMOTrafficlight
            SUMOTrafficlight currTL;
            currTL.icsID = trafficlights.size();
            currTL.pos = currTLpos;
            currTL.tlID = succs[i].SUMOSUMOSucclanes[j].tl;
            currTL.linkIndex = succs[i].SUMOSUMOSucclanes[j].linkIndex;
            currTL.controlled = currSUMOLane;
            currTL.succ = findSUMOLane(succs[i].SUMOSUMOSucclanes[j].lane);
            currTL.via = findSUMOLane(succs[i].SUMOSUMOSucclanes[j].via);
            currTL.direction = succs[i].SUMOSUMOSucclanes[j].dir;
            // Add the TL to the vector
            trafficlights.push_back(currTL);
        }
    }
    return;
}


SUMOLane* SUMODigitalMap::findSUMOLane(string laneId) {
    SUMOLane *targetSUMOLane = NULL;
    map<string, SUMOEdge>::iterator it;
    for (it=edges.begin(); it != edges.end(); it++) {
        targetSUMOLane = (*it).second.getSUMOLane(laneId);
        if (targetSUMOLane != NULL)
            return targetSUMOLane;
    }
    return targetSUMOLane;
}


const string SUMODigitalMap::getSUMOJunctionFromSUMOEdge(string SUMOEdgeId) const {
    for (unsigned int i=0; i<succs.size(); i++) {
        if (succs[i].edge == SUMOEdgeId)
            return succs[i].junction;
    }
    return "";
}


SUMOLane* SUMOEdge::getSUMOLane(string laneId) {
    map<string, SUMOLane>::iterator it;
    it  = lanes.find(laneId);

    if (it != lanes.end())
        return &((*it).second);
    else
        return NULL;
}


bool SUMOJunction::isIncSUMOLane(string laneId) {
    for (unsigned int i=0; i<stringIncSUMOLanes.size(); i++) {
        if (laneId == stringIncSUMOLanes[i])
            return true;
    }
    return false;
}

bool SUMOJunction::isIntSUMOLane(string laneId) {
    for (unsigned int i=0; i<stringIntSUMOLanes.size(); i++) {
        if (laneId == stringIntSUMOLanes[i])
            return true;
    }
    return false;
}

/*! \fn void tokenize(const string& , vector<string>&, const string&)
 \brief Create a vector of strings from a single string.
 \param[in] str input string to split in chunks.
 \param[in] tokens vector of chuncks (that are strings).
 \param[in] delimiters string containing the characters of separation.
 */
void tokenize(const string& str, vector<string>& tokens,
              const string& delimiters) {
    // Skip delimiters at beginning.
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    string::size_type pos = str.find_first_of(delimiters, lastPos);

    while (string::npos != pos || string::npos != lastPos) {
        // Once found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
    return;
}

}

#endif /*SUMO_ON*/
