/****************************************************************************/
/// @file    MSPModel_Striping.h
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Mon, 13 Jan 2014
/// @version $Id: MSPModel_Striping.h 23150 2017-02-27 12:08:30Z behrisch $
///
// The pedestrian following model (prototype)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2014-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSPModel_Striping_h
#define MSPModel_Striping_h

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <limits>
#include <utils/common/SUMOTime.h>
#include <utils/common/Command.h>
#include <utils/options/OptionsCont.h>
#include <microsim/MSLane.h>
#include "MSPerson.h"
#include "MSPModel.h"

// ===========================================================================
// class declarations
// ===========================================================================
class MSNet;
class MSLink;
class MSJunction;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSPModel_Striping
 * @brief The pedestrian following model
 *
 */
class MSPModel_Striping : public MSPModel {

    friend class GUIPerson; // for debugging

public:

    /// @brief Constructor (it should not be necessary to construct more than one instance)
    MSPModel_Striping(const OptionsCont& oc, MSNet* net);

    ~MSPModel_Striping();

    /// @brief register the given person as a pedestrian
    PedestrianState* add(MSPerson* person, MSPerson::MSPersonStage_Walking* stage, SUMOTime now);

    /// @brief remove the specified person from the pedestrian simulation
    void remove(PedestrianState* state);

    /// @brief whether a pedestrian is blocking the crossing of lane at offset distToCrossing
    bool blockedAtDist(const MSLane* lane, double distToCrossing, std::vector<const MSPerson*>* collectBlockers);

    /// @brief remove state at simulation end
    void cleanupHelper();

    /// @brief model parameters
    ///@{

    // @brief the width of a pedstrian stripe
    static double stripeWidth;

    // @brief the factor for random slow-down
    static double dawdling;

    // @brief the time threshold before becoming jammed
    static SUMOTime jamTime;

    // @brief the distance (in seconds) to look ahead for changing stripes
    static const double LOOKAHEAD_SAMEDIR;
    // @brief the distance (in seconds) to look ahead for changing stripes (regarding oncoming pedestrians)
    static const double LOOKAHEAD_ONCOMING;

    // @brief the utility penalty for moving sideways (corresponds to meters)
    static const double LATERAL_PENALTY;

    // @brief the utility penalty for obstructed (physically blocking me) stripes (corresponds to meters)
    static const double OBSTRUCTED_PENALTY;

    // @brief the utility penalty for inappropriate (reserved for oncoming traffic or may violate my min gap) stripes (corresponds to meters)
    static const double INAPPROPRIATE_PENALTY;

    // @brief the utility penalty for oncoming conflicts on stripes (corresponds to meters)
    static const double ONCOMING_CONFLICT_PENALTY;

    // @brief the minimum utility that indicates obstruction
    static const double OBSTRUCTION_THRESHOLD;

    // @brief the factor by which pedestrian width is reduced when sqeezing past each other
    static const double SQUEEZE;

    // @brief the maximum distance (in meters) at which oncoming pedestrians block right turning traffic
    static const double BLOCKER_LOOKAHEAD;

    // @brief fraction of the leftmost lanes to reserve for oncoming traffic
    static const double RESERVE_FOR_ONCOMING_FACTOR;
    static const double RESERVE_FOR_ONCOMING_FACTOR_JUNCTIONS;

    // @brief the time pedestrians take to reach maximum impatience
    static const double MAX_WAIT_TOLERANCE;

    // @brief the fraction of forward speed to be used for lateral movemenk
    static const double LATERAL_SPEED_FACTOR;

    // @brief the minimum distance to the next obstacle in order to start walking after stopped
    static const double MIN_STARTUP_DIST;

    ///@}


protected:
    static const double DIST_FAR_AWAY;
    static const double DIST_BEHIND;
    static const double DIST_OVERLAP;

    class lane_by_numid_sorter {
    public:
        /// comparing operation
        bool operator()(const MSLane* l1, const MSLane* l2) const {
            return l1->getNumericalID() < l2->getNumericalID();
        }
    };

    struct Obstacle;
    struct WalkingAreaPath;
    class PState;
    typedef std::vector<PState*> Pedestrians;
    typedef std::map<const MSLane*, Pedestrians, lane_by_numid_sorter> ActiveLanes;
    typedef std::vector<Obstacle> Obstacles;
    typedef std::map<const MSLane*, Obstacles, lane_by_numid_sorter> NextLanesObstacles;
    typedef std::map<std::pair<const MSLane*, const MSLane*>, WalkingAreaPath> WalkingAreaPaths;
    typedef std::map<const MSLane*, double> MinNextLengths;

    struct NextLaneInfo {
        NextLaneInfo(const MSLane* _lane, const MSLink* _link, int _dir) :
            lane(_lane),
            link(_link),
            dir(_dir) {
        }

        NextLaneInfo() :
            lane(0),
            link(0),
            dir(UNDEFINED_DIRECTION) {
        }

        // @brief the next lane to be used
        const MSLane* lane;
        // @brief the link from the current lane to the next lane
        const MSLink* link;
        // @brief the direction on the next lane
        int dir;
    };

    /// @brief information regarding surround Pedestrians (and potentially other things)
    struct Obstacle {
        /// @brief create No-Obstacle
        Obstacle(int dir, double dist = DIST_FAR_AWAY);
        /// @brief create an obstacle from ped for ego moving in dir
        Obstacle(const PState& ped);
        /// @brief create an obstacle from explict values
        Obstacle(double _x, double _speed, const std::string& _description, const double width = 0., bool _border = false)
            : xFwd(_x + width / 2.), xBack(_x - width / 2.), speed(_speed), description(_description), border(_border) {};

        /// @brief maximal position on the current lane in forward direction
        double xFwd;
        /// @brief maximal position on the current lane in backward direction
        double xBack;
        /// @brief speed relative to lane direction (positive means in the same direction)
        double speed;
        /// @brief the id / description of the obstacle
        std::string description;
        /// @brief whether this obstacle denotes a border or a pedestrian
        bool border;
    };

    struct WalkingAreaPath {
        WalkingAreaPath(const MSLane* _from, const MSLane* _walkingArea, const MSLane* _to, const PositionVector& _shape) :
            from(_from),
            to(_to),
            lane(_walkingArea),
            shape(_shape),
            length(_shape.length()) {
        }

        WalkingAreaPath(): from(0), to(0), lane(0) {};

        const MSLane* from;
        const MSLane* to;
        const MSLane* lane; // the walkingArea;
        PositionVector shape; // actually const but needs to be copyable by some stl code
        double length;

    };

    class walkingarea_path_sorter {
    public:
        /// comparing operation
        bool operator()(const WalkingAreaPath* p1, const WalkingAreaPath* p2) const {
            if (p1->from->getNumericalID() < p2->from->getNumericalID()) {
                return true;
            }
            if (p1->from->getNumericalID() == p2->from->getNumericalID()) {
                if (p1->to->getNumericalID() < p2->to->getNumericalID()) {
                    return true;
                }
            }
            return false;
        }
    };


    /**
     * @class PState
     * @brief Container for pedestrian state and individual position update function
     */
    class PState : public PedestrianState {
    public:

        /// @brief abstract methods inherited from PedestrianState
        /// @{
        double getEdgePos(const MSPerson::MSPersonStage_Walking& stage, SUMOTime now) const;
        Position getPosition(const MSPerson::MSPersonStage_Walking& stage, SUMOTime now) const;
        double getAngle(const MSPerson::MSPersonStage_Walking& stage, SUMOTime now) const;
        SUMOTime getWaitingTime(const MSPerson::MSPersonStage_Walking& stage, SUMOTime now) const;
        double getSpeed(const MSPerson::MSPersonStage_Walking& stage) const;
        const MSEdge* getNextEdge(const MSPerson::MSPersonStage_Walking& stage) const;
        /// @}

        PState(MSPerson* person, MSPerson::MSPersonStage_Walking* stage, const MSLane* lane);

        ~PState() {};
        MSPerson* myPerson;
        MSPerson::MSPersonStage_Walking* myStage;
        /// @brief the current lane of this pedestrian
        const MSLane* myLane;
        /// @brief the advancement along the current lane
        double myRelX;
        /// @brief the orthogonal shift on the current lane
        double myRelY;
        /// @brief the walking direction on the current lane (1 forward, -1 backward)
        int myDir;
        /// @brief the current walking speed
        double mySpeed;
        /// @brief whether the pedestrian is waiting to start its walk
        bool myWaitingToEnter;
        /// @brief the consecutive time spent at speed 0
        SUMOTime myWaitingTime;
        /// @brief information about the upcoming lane
        NextLaneInfo myNLI;
        /// @brief the current walkingAreaPath or 0
        WalkingAreaPath* myWalkingAreaPath;
        /// @brief whether the person is jammed
        bool myAmJammed;

        /// @brief return the minimum position on the lane
        double getMinX(const bool includeMinGap = true) const;

        /// @brief return the maximum position on the lane
        double getMaxX(const bool includeMinGap = true) const;

        /// @brief return the length of the pedestrian
        double getLength() const;

        /// @brief return the minimum gap of the pedestrian
        double getMinGap() const;

        /// @brief the absolute distance to the end of the lane in walking direction (or to the arrivalPos)
        double distToLaneEnd() const;

        /// @brief return whether this pedestrian has passed the end of the current lane and update myRelX if so
        bool moveToNextLane(SUMOTime currentTime);

        /// @brief perform position update
        void walk(const Obstacles& obs, SUMOTime currentTime);

        /// @brief returns the impatience
        double getImpatience(SUMOTime now) const;

        int stripe() const;
        int otherStripe() const;

        int stripe(const double relY) const;
        int otherStripe(const double relY) const;

        /* @brief calculate distance to the given obstacle,
         * - non-negative values signify an obstacle in front of ego
         * the special values DIST_OVERLAP and DIST_BEHIND are used to signify
         * obstacles that overlap and obstacles behind ego respectively
         * the result is the same regardless of walking direction
         */
        double distanceTo(const Obstacle& obs, const bool includeMinGap = true) const;

        /// @brief replace obstacles in the first vector with obstacles from the second if they are closer to me
        void mergeObstacles(Obstacles& into, const Obstacles& obs2);

    };

    class MovePedestrians : public Command {
    public:
        MovePedestrians(MSPModel_Striping* model) : myModel(model) {};
        ~MovePedestrians() {};
        SUMOTime execute(SUMOTime currentTime);
    private:
        MSPModel_Striping* const myModel;
    private:
        /// @brief Invalidated assignment operator.
        MovePedestrians& operator=(const MovePedestrians&);
    };

    /// @brief sorts the persons by position on the lane. If dir is forward, higher x positions come first.
    class by_xpos_sorter {
    public:
        /// constructor
        by_xpos_sorter(int dir): myDir(dir) {}

    public:
        /// comparing operation
        bool operator()(const PState* p1, const PState* p2) const {
            if (p1->myRelX != p2->myRelX) {
                return myDir * p1->myRelX > myDir * p2->myRelX;
            }
            return p1->myPerson->getID() < p2->myPerson->getID();
        }

    private:
        const int myDir;

    private:
        /// @brief Invalidated assignment operator.
        by_xpos_sorter& operator=(const by_xpos_sorter&);
    };


    /// @brief move all pedestrians forward and advance to the next lane if applicable
    void moveInDirection(SUMOTime currentTime, std::set<MSPerson*>& changedLane, int dir);

    /// @brief move pedestrians forward on one lane
    void moveInDirectionOnLane(Pedestrians& pedestrians, const MSLane* lane, SUMOTime currentTime, std::set<MSPerson*>& changedLane, int dir);

    /// @brief handle arrivals and lane advancement
    void arriveAndAdvance(Pedestrians& pedestrians, SUMOTime currentTime, std::set<MSPerson*>& changedLane, int dir);

    const ActiveLanes& getActiveLanes() {
        return myActiveLanes;
    }

private:
    static void DEBUG_PRINT(const Obstacles& obs);

    /// @brief returns the direction in which these lanes are connectioned or 0 if they are not
    static int connectedDirection(const MSLane* from, const MSLane* to);

    /** @brief computes the successor lane for the given pedestrian and sets the
     * link as well as the direction to use on the succesor lane
     * @param[in] currentLane The lane the pedestrian is currently on
     * @param[in] ped The pedestrian for which to compute the next lane
     */
    static NextLaneInfo getNextLane(const PState& ped, const MSLane* currentLane, const MSLane* prevLane);

    /// @brief return the next walkingArea in the given direction
    static const MSLane* getNextWalkingArea(const MSLane* currentLane, const int dir, MSLink*& link);

    static void initWalkingAreaPaths(const MSNet* net);

    /// @brief return the maximum number of pedestrians walking side by side
    static int numStripes(const MSLane* lane);

    static Obstacles getNeighboringObstacles(const Pedestrians& pedestrians, int egoIndex, int stripes);

    const Obstacles& getNextLaneObstacles(NextLanesObstacles& nextLanesObs, const MSLane* lane, const MSLane* nextLane, int stripes,
                                          int nextDir, double currentLength, int currentDir);

    static void transformToCurrentLanePositions(Obstacles& o, int currentDir, int nextDir, double currentLength, double nextLength);

    static void addCloserObstacle(Obstacles& obs, double x, int stripe, int numStripes, const std::string& id, double width, int dir);

    /// @brief retrieves the pedestian vector for the given lane (may be empty)
    Pedestrians& getPedestrians(const MSLane* lane);

    /* @brief compute stripe-offset to transform relY values from a lane with origStripes into a lane wit destStrips
     * @note this is called once for transforming nextLane peds to into the current system as obstacles and another time
     * (in reverse) to transform the pedestrian coordinates into the nextLane-coordinates when changing lanes
     */
    static int getStripeOffset(int origStripes, int destStripes, bool addRemainder);


private:
    /// @brief the MovePedestrians command that is registered
    MovePedestrians* myCommand;

    /// @brief the total number of active pedestrians
    int myNumActivePedestrians;

    /// @brief store of all lanes which have pedestrians on them
    ActiveLanes myActiveLanes;

    /// @brief store for walkinArea elements
    static WalkingAreaPaths myWalkingAreaPaths;
    static MinNextLengths myMinNextLengths;

    /// @brief empty pedestrian vector
    static Pedestrians noPedestrians;

};


#endif /* MSPModel_Striping_h */

