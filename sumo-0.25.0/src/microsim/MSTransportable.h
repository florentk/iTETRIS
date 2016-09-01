/****************************************************************************/
/// @file    MSTransportable.h
/// @author  Michael Behrisch
/// @date    Tue, 21 Apr 2015
/// @version $Id: MSTransportable.h 19388 2015-11-19 21:33:01Z behrisch $
///
// The common superclass for modelling transportable objects like persons and containers
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2015 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSTransportable_h
#define MSTransportable_h

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif


// ===========================================================================
// class declarations
// ===========================================================================
class MSEdge;
class MSLane;
class MSNet;
class MSStoppingPlace;
class MSVehicleType;
class OutputDevice;
class Position;
class SUMOVehicleParameter;
class SUMOVehicle;


// ===========================================================================
// class definitions
// ===========================================================================
/**
  * @class MSTransportable
  *
  * The class holds a simulated moveable object
  */
class MSTransportable {
public:
    enum StageType {
        DRIVING = 0,
        WAITING = 1,
        MOVING_WITHOUT_VEHICLE = 2 // walking for persons, tranship for containers
    };

    /**
    * The "abstract" class for a single stage of a movement
    * Contains the destination of the current movement step
    */
    class Stage {
    public:
        /// constructor
        Stage(const MSEdge& destination, MSStoppingPlace* toStop, const SUMOReal arrivalPos, StageType type);

        /// destructor
        virtual ~Stage();

        /// returns the destination edge
        const MSEdge& getDestination() const;

        /// returns the destination stop (if any)
        const MSStoppingPlace* getDestinationStop() const {
            return myDestinationStop;
        }

        SUMOReal getArrivalPos() const {
            return myArrivalPos;
        }

        /// Returns the current edge
        virtual const MSEdge* getEdge() const = 0;
        virtual const MSEdge* getFromEdge() const = 0;
        virtual SUMOReal getEdgePos(SUMOTime now) const = 0;

        /// returns the position of the transportable
        virtual Position getPosition(SUMOTime now) const = 0;

        /// returns the angle of the transportable
        virtual SUMOReal getAngle(SUMOTime now) const = 0;

        ///
        StageType getStageType() const {
            return myType;
        }

        /// @brief return string representation of the current stage
        virtual std::string getStageDescription() const = 0;

        /// proceeds to the next step
        virtual void proceed(MSNet* net, MSTransportable* transportable, SUMOTime now, Stage* previous) = 0;

        /// logs end of the step
        void setDeparted(SUMOTime now);

        /// logs end of the step
        void setArrived(SUMOTime now);

        /// Whether the transportable waits for a vehicle of the line specified.
        virtual bool isWaitingFor(const std::string& line) const;

        /// @brief Whether the transportable waits for a vehicle
        virtual bool isWaiting4Vehicle() const {
            return false;
        }

        /// @brief Whether the transportable waits for a vehicle
        virtual SUMOVehicle* getVehicle() const {
            return 0;
        }

        /// @brief the time this transportable spent waiting
        virtual SUMOTime getWaitingTime(SUMOTime now) const = 0;

        /// @brief the speed of the transportable
        virtual SUMOReal getSpeed() const = 0;

        /// @brief get position on edge e at length at with orthogonal offset
        Position getEdgePosition(const MSEdge* e, SUMOReal at, SUMOReal offset) const;

        /// @brief get position on lane at length at with orthogonal offset
        Position getLanePosition(const MSLane* lane, SUMOReal at, SUMOReal offset) const;

        /// @brief get angle of the edge at a certain position
        SUMOReal getEdgeAngle(const MSEdge* e, SUMOReal at) const;

        /** @brief Called on writing tripinfo output
         * @param[in] os The stream to write the information into
         * @exception IOError not yet implemented
         */
        virtual void tripInfoOutput(OutputDevice& os) const = 0;

        /** @brief Called on writing vehroute output
         * @param[in] os The stream to write the information into
         * @exception IOError not yet implemented
         */
        virtual void routeOutput(OutputDevice& os) const = 0;

        /** @brief Called for writing the events output (begin of an action)
         * @param[in] os The stream to write the information into
         * @exception IOError not yet implemented
         */
        virtual void beginEventOutput(const MSTransportable& transportable, SUMOTime t, OutputDevice& os) const = 0;

        /** @brief Called for writing the events output (end of an action)
         * @param[in] os The stream to write the information into
         * @exception IOError not yet implemented
         */
        virtual void endEventOutput(const MSTransportable& transportable, SUMOTime t, OutputDevice& os) const = 0;

    protected:
        /// the next edge to reach by getting transported
        const MSEdge& myDestination;

        /// the stop to reach by getting transported (if any)
        MSStoppingPlace* const myDestinationStop;

        /// the position at which we want to arrive
        SUMOReal myArrivalPos;

        /// the time at which this stage started
        SUMOTime myDeparted;

        /// the time at which this stage ended
        SUMOTime myArrived;

        /// The type of this stage
        StageType myType;

    private:
        /// @brief Invalidated copy constructor.
        Stage(const Stage&);

        /// @brief Invalidated assignment operator.
        Stage& operator=(const Stage&);

    };

    /// the structure holding the plan of a transportable
    typedef std::vector<MSTransportable::Stage*> MSTransportablePlan;

    /// constructor
    MSTransportable(const SUMOVehicleParameter* pars, const MSVehicleType* vtype, MSTransportablePlan* plan);

    /// destructor
    virtual ~MSTransportable();

    /* @brief proceeds to the next step of the route,
     * @return Whether the transportables plan continues  */
    virtual bool proceed(MSNet* net, SUMOTime time) = 0;

    /// returns the id of the transportable
    const std::string& getID() const;

    inline const SUMOVehicleParameter& getParameter() const {
        return *myParameter;
    }

    inline const MSVehicleType& getVehicleType() const {
        return *myVType;
    }

    /// Returns the desired departure time.
    SUMOTime getDesiredDepart() const;

    /// logs depart time of the current stage
    void setDeparted(SUMOTime now);

    /// Returns the current destination.
    const MSEdge& getDestination() const {
        return (*myStep)->getDestination();
    }

    /// Returns the destination after the current destination.
    const MSEdge& getNextDestination() const {
        return (*(myStep + 1))->getDestination();
    }

    /// @brief Returns the current edge
    const MSEdge* getEdge() const {
        return (*myStep)->getEdge();
    }

    /// @brief Returns the departure edge
    const MSEdge* getFromEdge() const {
        return (*myStep)->getFromEdge();
    }

    /// @brief Return the position on the edge
    virtual SUMOReal getEdgePos() const;

    /// @brief Return the Network coordinate of the transportable
    virtual Position getPosition() const;

    /// @brief return the current angle of the transportable
    virtual SUMOReal getAngle() const;

    /// @brief the time this transportable spent waiting in seconds
    virtual SUMOReal getWaitingSeconds() const;

    /// @brief the current speed of the transportable
    virtual SUMOReal getSpeed() const;

    /// @brief the current stage type of the transportable
    StageType getCurrentStageType() const {
        return (*myStep)->getStageType();
    }

    /// Returns the current stage description as a string
    std::string getCurrentStageDescription() const {
        return (*myStep)->getStageDescription();
    }

    /// @brief Return the current stage
    MSTransportable::Stage* getCurrentStage() const {
        return *myStep;
    }

    /** @brief Called on writing tripinfo output
     *
     * @param[in] os The stream to write the information into
     * @exception IOError not yet implemented
     */
    void tripInfoOutput(OutputDevice& os) const;

    /** @brief Called on writing vehroute output
     *
     * @param[in] os The stream to write the information into
     * @exception IOError not yet implemented
     */
    virtual void routeOutput(OutputDevice& os) const = 0;

    /// Whether the transportable waits for a vehicle of the line specified.
    bool isWaitingFor(const std::string& line) const {
        return (*myStep)->isWaitingFor(line);
    }

    /// Whether the transportable waits for a vehicle
    bool isWaiting4Vehicle() const {
        return (*myStep)->isWaiting4Vehicle();
    }

    /// The vehicle associated with this transportable
    SUMOVehicle* getVehicle() const {
        return (*myStep)->getVehicle();
    }

protected:
    /// the plan of the transportable
    const SUMOVehicleParameter* myParameter;

    /// @brief This transportable's type. (mainly used for drawing related information
    /// Note sure if it is really necessary
    const MSVehicleType* myVType;

    /// @brief Whether events shall be written
    bool myWriteEvents;

    /// the plan of the transportable
    MSTransportablePlan* myPlan;

    /// the iterator over the route
    MSTransportablePlan::iterator myStep;

private:
    /// @brief Invalidated copy constructor.
    MSTransportable(const MSTransportable&);

    /// @brief Invalidated assignment operator.
    MSTransportable& operator=(const MSTransportable&);

};


#endif

/****************************************************************************/
