/****************************************************************************/
/// @file    app-message-manager.cpp
/// @author  Julen Maneros
/// @author  Jerome Haerri (EURECOM)
/// @date
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright �� 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <typeinfo>

#include "app-message-manager.h"
#include "subscription.h"
#include "subs-return-cars-zone.h"
#include "subs-start-travel-time-calculation.h"
#include "subs-stop-travel-time-calculation.h"
#include "subs-get-received-cam-info.h"
#include "subs-calculate-travel-time.h"
#include "subs-set-cam-area.h"
#include "subs-get-facilities-info.h"
#include "subs-app-message-send.h"
#include "subs-app-message-receive.h"
#include "subs-app-result-traff-sim.h"
#include "subs-app-cmd-traff-sim.h"
#include "subs-x-application-data.h"
#include "../itetris-node.h"
#include "../vehicle-node.h"
#include "app-result-container.h"
#include "../sync-manager.h"
#include "../ics.h"
#include "../../utils/ics/log/ics-log.h"

#ifdef _WIN32
#include <windows.h> // needed for Sleep
#else
#include "../../config.h"
#include <unistd.h>
#define Sleep(x) usleep((x)*1000)
#endif


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;
using namespace tcpip;

namespace ics
{


AppMessageManager::AppMessageManager(SyncManager* syncManager)
{
    this->m_syncManager = syncManager;
}

// ===========================================================================
// member method definitions
// ===========================================================================
bool
AppMessageManager::Connect(string host, int port)
{
    m_socket = new Socket(host, port);
    m_socket->set_verbose(false);

    bool connected = false;
    for (int i=0; i<10; ++i) {
        try {
            cout << "iCS --> Trying " << i <<" to connect Application on port " << port << "..." << endl;
            m_socket->connect();
            return true;
        } catch (SocketException e) {
            cout << "iCS --> No connection to Application; waiting..." << endl;
            Sleep(3);
        }
    }

    return false;
}

bool
AppMessageManager::Close()
{
    if (m_socket != NULL) {
        m_socket->close();
    }

    return true;
}

int
AppMessageManager::CommandClose()
{
    Storage outMsg;
    Storage inMsg;
    std::stringstream msg;

    if (m_socket == NULL) {
        cout << "iCS --> #Error while sending command: no connection to server" << endl;
        return EXIT_FAILURE;
    }

    // command length
    outMsg.writeInt(4 + 4);
    // command id
    outMsg.writeUnsignedByte(CMD_APP_CLOSE);

    // send request message
    try {
        m_socket->sendExact(outMsg);
    } catch (SocketException e) {
        cout << "iCS --> Error while sending command: " << e.what()<< endl;
        return EXIT_FAILURE;
    }

    // receive answer message
    try {
        m_socket->receiveExact(inMsg);
    } catch (SocketException e) {
        cout << "iCS --> #Error while receiving command: " << e.what() << endl;
        return EXIT_FAILURE;
    }

    // validate result state
    if (!ReportResultState(inMsg, CMD_APP_CLOSE)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

bool
AppMessageManager::CommandGetNewSubscriptions(int nodeId, int appId, vector<Subscription*> *subscriptions, bool &noMoreSubs)
{
    tcpip::Storage outMsg;
    tcpip::Storage inMsg;

    if (m_socket == NULL) {
        cout << "iCS --> #Error while sending command: Socket is off" << endl;;
        return false;
    }

    // command length
    outMsg.writeInt(4 + 1 + 4 + 4);
    // command id
    outMsg.writeUnsignedByte(CMD_ASK_FOR_SUBSCRIPTION);
    // simulation timestep
    outMsg.writeInt(SyncManager::m_simStep);
    // node identifier
    outMsg.writeInt(nodeId);

    if (nodeId == 0) {
        cout << "Node id 0" << endl;
    }

    // send request message
    try {
        m_socket->sendExact(outMsg);
    } catch (SocketException e) {
        cout << "iCS --> #Error while sending command to Application: " << e.what() << endl;
        return false;
    }

    // receive answer message
    try {
        m_socket->receiveExact(inMsg);
    } catch (SocketException e) {
        cout << "iCS --> #Error while receiving response from Application: " << e.what() << endl;
        return false;
    }

    if (!ReportResultState(inMsg, CMD_ASK_FOR_SUBSCRIPTION))
        return false;

    int cmdLength;
    int cmdStart;
    int subscriptionCode;
    try {
        cmdStart = inMsg.position();
        cmdLength = inMsg.readUnsignedByte();
        if (inMsg.readUnsignedByte() != CMD_ASK_FOR_SUBSCRIPTION)
            return false;

        subscriptionCode = inMsg.readUnsignedByte();

#ifdef LOG_ON
        stringstream log;
        log <<  "[INFO] AppMessageManager::CommandGetNewSubscriptions() subscription code is " << subscriptionCode;
        IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif

        switch (subscriptionCode) {
        case SUB_RETURNS_CARS_IN_ZONE: {
            float baseX, baseY, radius;
            baseX = inMsg.readFloat();
            baseY = inMsg.readFloat();
            radius = inMsg.readFloat();

            cout << "[INFO] AppMessageManager::CommandGetNewSubscriptions() Subscribed to Return Cars In Zone " << baseX << " " << baseY << " " << radius << endl;
            stringstream log;
            log << "[INFO] AppMessageManager::CommandGetNewSubscriptions() Subscribed to Return Cars In Zone " << baseX << " " << baseY << " " << radius;
            IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);

            SubsReturnsCarInZone *subscription = new SubsReturnsCarInZone(appId, nodeId, baseX, baseY, radius);
            subscriptions->push_back(subscription);
            noMoreSubs = false;
            break;
        }
        case SUB_SET_CAM_AREA: {
            float baseX, baseY, radius, frequency;
            baseX = inMsg.readFloat();
            baseY = inMsg.readFloat();
            radius = inMsg.readFloat();
            frequency = inMsg.readFloat();
            int infoType = inMsg.readUnsignedByte();

            cout << "[INFO] AppMessageManager::CommandGetNewSubscriptions() Subscribed to Set CAM Area " << baseX << " " << baseY << " " << radius << endl;

            stringstream log;
            log << "[INFO] AppMessageManager::CommandGetNewSubscriptions() Subscribed to Set CAM Area " << baseX << " " << baseY << " " << radius;
            IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);

            SubsSetCamArea *subscription = new SubsSetCamArea(appId, nodeId, baseX, baseY, radius, frequency, infoType);
            subscriptions->push_back(subscription);
            noMoreSubs = false;
            break;
        }
        case SUB_TRAVEL_TIME_ESTIMATION_START: {
            inMsg.readUnsignedByte(); // status (to be removed)
            inMsg.readInt(); // id of rsu
            float x, y, frequency, msgRegenerationTime;
            vector<Point2D> vertex = vector<Point2D>();
            x = inMsg.readFloat(); // point 1
            y = inMsg.readFloat();
            Point2D point = Point2D(x,y);
            vertex.push_back(point);
            x = inMsg.readFloat(); // point 2
            y = inMsg.readFloat();
            point = Point2D(x,y);
            vertex.push_back(point);
            x = inMsg.readFloat(); // point 3
            y = inMsg.readFloat();
            point = Point2D(x,y);
            vertex.push_back(point);
            x = inMsg.readFloat(); // point 4
            y = inMsg.readFloat();
            point = Point2D(x,y);
            vertex.push_back(point);
            frequency = inMsg.readFloat();
            msgRegenerationTime = inMsg.readFloat(); // message regeneration time
            int msgLifeTime = inMsg.readInt(); // message life time in seconds
            SubsStartTravelTimeCalculation* subscription = new SubsStartTravelTimeCalculation(appId, nodeId, vertex, frequency, msgRegenerationTime, msgLifeTime);
            subscriptions->push_back(subscription);
            noMoreSubs = false;
            break;
        }
        case SUB_TRAVEL_TIME_ESTIMATION_END: {
            inMsg.readUnsignedByte(); // status (to be removed)
            inMsg.readInt(); // id of rsu
            float x, y, frequency, msgRegenerationTime;
            vector<Point2D> vertex = vector<Point2D>();
            x = inMsg.readFloat(); // point 1
            y = inMsg.readFloat();
            Point2D point = Point2D(x,y);
            vertex.push_back(point);
            x = inMsg.readFloat(); // point 2
            y = inMsg.readFloat();
            point = Point2D(x,y);
            vertex.push_back(point);
            x = inMsg.readFloat(); // point 3
            y = inMsg.readFloat();
            point = Point2D(x,y);
            vertex.push_back(point);
            x = inMsg.readFloat(); // point 4
            y = inMsg.readFloat();
            point = Point2D(x,y);
            vertex.push_back(point);
            frequency = inMsg.readFloat();
            msgRegenerationTime = inMsg.readFloat(); // message regeneration time
            int msgLifeTime = inMsg.readInt(); // message life time in seconds
            SubsStopTravelTimeCalculation* subscription = new SubsStopTravelTimeCalculation(appId, nodeId, vertex, frequency , msgRegenerationTime, msgLifeTime);
            subscriptions->push_back(subscription);
            noMoreSubs = false;
            break;
        }
        case SUB_TRAVEL_TIME_ESTIMATION: {
            //inMsg.readUnsignedByte(); // status (to be removed)
            inMsg.readInt(); // vehicle ID
            SubsCalculateTravelTime* subscription =  new SubsCalculateTravelTime(appId, nodeId);
            subscriptions->push_back(subscription);
            noMoreSubs = false;
            break;
        }
        case SUB_RECEIVED_CAM_INFO: {

            stringstream log;
            log << "CommandGetNewSubscriptions() Station " << nodeId << " subscribed to get information about the received CAM messages.";
            IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);

            SubsGetReceivedCamInfo *subscription = new SubsGetReceivedCamInfo(appId, nodeId);
            subscriptions->push_back(subscription);
            noMoreSubs = false;
            break;
        }
        case SUB_FACILITIES_INFORMATION: {

            stringstream log;
            log << "CommandGetNewSubscriptions() Station " << nodeId << " subscribed to get facilities information.";
            IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);

            // read the buffer
            //unsigned char* packet = new unsigned char[cmdLength-3];
            std::vector<unsigned char> packet;
            packet.reserve(cmdLength-3);
            for (unsigned int i = 3; i < cmdLength; i++) {
                //packet[i-3] = inMsg.readChar();
                packet.push_back(inMsg.readChar());
            }
            SubsGetFacilitiesInfo *subscription = new SubsGetFacilitiesInfo(appId, nodeId, packet);
            //delete [] packet;
            subscriptions->push_back(subscription);
            noMoreSubs = false;
            break;
        }
        case SUB_APP_MSG_SEND: {

#ifdef LOG_ON
            stringstream log;
            log << "CommandGetNewSubscriptions() Station " << nodeId << " subscribed to send an Application message.";
            IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif

            // read the buffer
            unsigned char* packet = new unsigned char[cmdLength-3];
            for (unsigned int i = 3; i < cmdLength; i++)
                packet[i-3] = inMsg.readChar();

            SubsAppMessageSend *subscription = new SubsAppMessageSend(appId, nodeId, m_syncManager, packet, cmdLength-3);
            //delete [] packet;
            subscriptions->push_back(subscription);
            noMoreSubs = false;
            break;
        }
        case SUB_APP_MSG_RECEIVE: {

#ifdef LOG_ON
            stringstream log;
            log << "CommandGetNewSubscriptions() Station " << nodeId << " subscribed to receive an Application message.";
            IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
            // read the buffer
            unsigned char* packet = new unsigned char[cmdLength-3];
            for (unsigned int i = 3; i < cmdLength; i++)
              packet[i-3] = inMsg.readChar();

            SubsAppMessageReceive *subscription = new SubsAppMessageReceive(appId, nodeId, m_syncManager, packet, cmdLength-3);
            //delete [] packet;
            subscriptions->push_back(subscription);
            noMoreSubs = false;
            break;
        }
        case SUB_APP_CMD_TRAFF_SIM: {
#ifdef LOG_ON
            stringstream log;
            log << "CommandGetNewSubscriptions() Station " << nodeId << " subscribed to send an Application CMD message to Traffic Simulator.";
            IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
            // read the buffer
            unsigned char* packet = new unsigned char[cmdLength-3];
            for (unsigned int i = 3; i < cmdLength; i++) {
                packet[i-3] = inMsg.readChar();
            }
            SubsAppCmdTraffSim *subscription = new SubsAppCmdTraffSim(appId, nodeId, m_syncManager,  packet, cmdLength-3);
            subscriptions->push_back(subscription);
            noMoreSubs = false;
            break;
        }
        case SUB_APP_RESULT_TRAFF_SIM: {
#ifdef LOG_ON
            stringstream log;
            log << "CommandGetNewSubscriptions() Station " << nodeId << " subscribed to send an Application Result Message to Traffic Simulator.";
            IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
            // read the buffer
            unsigned char* packet = new unsigned char[cmdLength-3];
            for (unsigned int i = 3; i < cmdLength; i++)
                packet[i-3] = inMsg.readChar();

            SubsAppResultTraffSim *subscription = new SubsAppResultTraffSim(appId, nodeId, packet, cmdLength-3);
            delete [] packet;
            subscriptions->push_back(subscription);
            noMoreSubs = false;
            break;
        }
        case SUB_X_APPLICATION_DATA: {
#ifdef LOG_ON
            stringstream log;
            log << "CommandGetNewSubscriptions() Station " << nodeId << " subscribed to get cross-application data.";
            IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);
#endif
            // read the buffer
            unsigned char* packet = new unsigned char[cmdLength-3];
            for (unsigned int i = 3; i < cmdLength; i++)
            	packet[i-3] = inMsg.readChar();

            SubsXApplicationData *subscription = new SubsXApplicationData(appId, nodeId, m_syncManager, packet, cmdLength-3);
            subscriptions->push_back(subscription);
            noMoreSubs = false;
            break;
        }
        case CMD_END_SUBSCRIPTION_REQUEST: {
            stringstream log;
            log << "CommandGetNewSubscriptions() No more requests from node " << nodeId;
            IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);

            noMoreSubs = true;
            break;
        }
        default: {
            cout << "App --> iCS #Error: No such subscription" << endl;
            noMoreSubs = true;
        }
        }
    } catch (std::invalid_argument e) {
        cout << "App --> iCS #Error: an exception was thrown while reading result state message. (Subscription code: " << subscriptionCode << ")" << endl;
        noMoreSubs = true;
        return false;
    }

    return true;
}

bool
AppMessageManager::CommandUnsubscribe(int nodeId, vector<Subscription*>* subscriptions, bool &noMoreUnSubs)
{
    tcpip::Storage outMsg;
    tcpip::Storage inMsg;

    if (m_socket == NULL) {
        cout << "iCS --> #Error while sending command: Socket is off" << endl;;
        return false;
    }

    if (subscriptions == NULL) {
        cout << "iCS --> #Error while sending command: subscriptions is NULL" << endl;;
        return false;
    }

    // command length
    outMsg.writeInt(4 + 1 + 4 + 4);
    // command id
    outMsg.writeUnsignedByte(CMD_END_SUBSCRIPTION);
    // simulation timestep
    outMsg.writeInt(SyncManager::m_simStep);
    // node identifier
    outMsg.writeInt(nodeId);

    if (nodeId == 0) {
        cout << "Node id 0" << endl;
    }

    // send request message
    try {
        m_socket->sendExact(outMsg);
    } catch (SocketException e) {
        cout << "iCS --> #Error while sending command to Application: " << e.what() << endl;
        return false;
    }

    // receive answer message
    try {
        m_socket->receiveExact(inMsg);
    } catch (SocketException e) {
        cout << "iCS --> #Error while receiving response from Application: " << e.what() << endl;
        noMoreUnSubs = true;
        return false;
    }

    if (!ReportResultState(inMsg, CMD_END_SUBSCRIPTION)) {
    	cout << "iCS --> #Error after ReportResultState" << endl;;
        noMoreUnSubs = true;
        return false;
    }
    if (!ValidateUnsubscriptions(inMsg, subscriptions, noMoreUnSubs)) {
        noMoreUnSubs = true;
        return false;
    }
    return true;
}

int
AppMessageManager::CommandUnsubscribe(int nodeId, Subscription* subscription)
{

    if (subscription == NULL) {
        cerr << "iCS --> [ERROR] CommandUnsubscribe() node " <<
                nodeId << " - subscription NULL!" << endl;
        return -1;
    }

    tcpip::Storage outMsg;
    tcpip::Storage inMsg;

    if (m_socket == NULL) {
        cout << "iCS --> #Error while sending command: Socket is off" << endl;;
        return -1;
    }

    if (subscription == NULL) {
        cout << "iCS --> #Error while sending command: subscriptions is NULL" << endl;;
        return -1;
    }

    // command length
    outMsg.writeInt(4 + 1 + 4 + 4 + 1);
    // command id
    outMsg.writeUnsignedByte(CMD_END_SUBSCRIPTION);
    // simulation timestep
    outMsg.writeInt(SyncManager::m_simStep);
    // node identifier
    outMsg.writeInt(nodeId);
    // subscription to be maintained or dropped
    const type_info& typeinfo = typeid(*subscription);
    if (typeinfo == typeid(SubsSetCamArea)) {
        outMsg.writeUnsignedByte(SUB_SET_CAM_AREA);
    }
    if (typeinfo == typeid(SubsReturnsCarInZone)) {
        outMsg.writeUnsignedByte(SUB_RETURNS_CARS_IN_ZONE);
    }
    if (typeinfo == typeid(SubsGetReceivedCamInfo)) {
        outMsg.writeUnsignedByte(SUB_RECEIVED_CAM_INFO);
    }
    if (typeinfo == typeid(SubsGetFacilitiesInfo)) {
            outMsg.writeUnsignedByte(SUB_FACILITIES_INFORMATION);
    }
    if (typeinfo == typeid(SubsXApplicationData)) {
            outMsg.writeUnsignedByte(SUB_X_APPLICATION_DATA);
    }
    if (typeinfo == typeid(SubsAppMessageSend)) {
            outMsg.writeUnsignedByte(SUB_APP_MSG_SEND);
    }
    if (typeinfo == typeid(SubsAppMessageReceive)) {
            outMsg.writeUnsignedByte(SUB_APP_MSG_RECEIVE);
    }
    if (typeinfo == typeid(SubsAppCmdTraffSim)) {
            outMsg.writeUnsignedByte(SUB_APP_CMD_TRAFF_SIM);
    }
    if (typeinfo == typeid(SubsAppResultTraffSim)) {
            outMsg.writeUnsignedByte(SUB_APP_RESULT_TRAFF_SIM);
    }

    // send request message
    try {
        m_socket->sendExact(outMsg);
    } catch (SocketException e) {
        cout << "iCS --> #Error while sending command to Application: " << e.what() << endl;
        return -1;
    }

    // receive answer message
    try {
        m_socket->receiveExact(inMsg);
    } catch (SocketException e) {
        cout << "iCS --> #Error while receiving response from Application: " << e.what() << endl;
        return -1;
    }

    // check out the status of the primitive
    if (!ReportResultState(inMsg, CMD_END_SUBSCRIPTION)) {
        return -1;
    }

    // check out the subscription status requested by the app
    return ValidateUnsubscriptions(inMsg);
}

bool
AppMessageManager::CommandSendSubscriptionCarsInZone(vector<VehicleNode*>* carsInZone, int nodeId)
{
    if (carsInZone == NULL) {
        return false;
    }

    tcpip::Storage outMsg;
    tcpip::Storage inMsg;

    if (m_socket == NULL) {
        cout << "iCS --> #Error while sending command: Socket is off" << endl;;
        return false;
    }

    int numberOfCars = (int)carsInZone->size();
    int numberOfCarsSize = numberOfCars * (4 + 4 + 4 + 4 +4);

    // command length
    outMsg.writeInt(4 + 1 + 4 + 4 + 4 + numberOfCarsSize);
    // command id
    outMsg.writeUnsignedByte(CMD_CARS_IN_ZONE);
    // simulation timestep
    outMsg.writeInt(SyncManager::m_simStep);
    // subscribed node id
    outMsg.writeInt(nodeId);
    // number of cars
    outMsg.writeInt((int)carsInZone->size());

    if (nodeId == 0) {
        cout << "Node id 0" << endl;
    }

    vector<VehicleNode*>::iterator nodeIt;
    for (nodeIt = carsInZone->begin() ; nodeIt < carsInZone->end() ; nodeIt++) {
        VehicleNode* node = *nodeIt;
        //car identifier
        outMsg.writeInt(node->m_icsId);
        //car position x
        outMsg.writeFloat(node->GetPositionX());
        //car position y
        outMsg.writeFloat(node->GetPositionY());
        ////////////////////////////////////////
        // ADDED by Florent KAISSER 02/21/2017
        //
        // write speed and heading to transmit them 
        // to the application
        //
        //car speed
        outMsg.writeFloat(node->GetSpeed());
        //car heading
        outMsg.writeFloat(node->GetHeading());
        //
        ////////////////////////////////////////

        stringstream log;
        log << "[INFO] CommandSendSubscriptionCarsInZone() Node Id " << node->m_icsId << " X: " << node->GetPositionX() << " Y: " << node->GetPositionY() << " Speed: " << node->GetSpeed() << " Direction: " << node->GetHeading();
        IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);

    }

    // send request message
    try {
        m_socket->sendExact(outMsg);
    } catch (SocketException e) {
        cout << "iCS --> #Error while sending command to Application: " << e.what() << endl;
        return false;
    }

    // receive answer message
    try {
        m_socket->receiveExact(inMsg);
    } catch (SocketException e) {
        cout << "iCS --> #Error while receiving response from Application: " << e.what() << endl;
        return false;
    }

    if (!ReportResultState(inMsg, CMD_CARS_IN_ZONE))
        return false;

    return true;
}

int
AppMessageManager::CommandSendSubcriptionCalculateTravelTimeFlags(int nodeId, int startStation, int stopStation)
{
    tcpip::Storage outMsg;
    tcpip::Storage inMsg;

    if (m_socket == NULL) {
        cout << "iCS --> #Error while sending command: Socket is off" << endl;
        return EXIT_FAILURE;
    }

    if (nodeId == 0) {
        cout << "Node id 0" << endl;
    }

    // command length
//    outMsg.writeUnsignedByte(1 + 1 + 4 + 4 + 4 + 4);
    outMsg.writeInt(1 + 1 + 4 + 4 + 4 + 4);
    // command id
    outMsg.writeUnsignedByte(CMD_TRAVEL_TIME_ESTIMATION);
    // simulation timestep
    outMsg.writeInt(SyncManager::m_simStep);
    // subscribed node id
    outMsg.writeInt(nodeId);
    // start station
    outMsg.writeInt(startStation);
    // stop station
    outMsg.writeInt(stopStation);

    // send request message
    try {
        m_socket->sendExact(outMsg);
    } catch (SocketException e) {
        cout << "iCS --> #Error while sending command to Application: " << e.what() << endl;
        return EXIT_FAILURE;
    }

    stringstream log;
    log << "Start and stop station info: " << startStation << " | " << stopStation;
    IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);

    // receive answer message
    try {
        m_socket->receiveExact(inMsg);
    } catch (SocketException e) {
        cout << "iCS --> #Error while receiving response from Application: " << e.what() << endl;
        return EXIT_FAILURE;
    }

    if (!ReportResultState(inMsg, CMD_TRAVEL_TIME_ESTIMATION))
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

bool
AppMessageManager::CommandSendSubscriptionReceivedCamInfo(vector<TCamInformation>* camInfo, int nodeId)
{
    if (camInfo == NULL) {
        return false;
    }

    if (nodeId == 0) {
        cout << "Node id 0" << endl;
        return false;
    }

    tcpip::Storage outMsg;
    tcpip::Storage inMsg;

    if (m_socket == NULL) {
        cout << "iCS --> #Error while sending command: Socket is off" << endl;;
        return false;
    }

    // Compute the information length to be sent
    int numberOfSendersSize = 0;
    vector<TCamInformation>::iterator it;
    for (it = camInfo->begin(); it < camInfo->end(); it++) {
        numberOfSendersSize += it->camInfoBuffSize;
    }

    // command length
    int totalLengthPacket = 4 /*length*/
                            + 1 /*commandId*/
                            + 4 /*timeStep*/
                            + 4 /*nodeId*/
                            + 4 /*nodeX*/
                            + 4 /*nodeY*/
                            + 4 /*NumCAMs*/
                            + numberOfSendersSize;

    outMsg.writeInt(totalLengthPacket);                 // bytes: 0-3
    // command id
    outMsg.writeUnsignedByte(CMD_RECEIVED_CAM_INFO);    // bytes: 4
    // simulation timestep
    outMsg.writeInt(SyncManager::m_simStep);            // bytes: 5-8
    // subscribed node id
    outMsg.writeInt(nodeId);                            // bytes: 9-12
    // node's position when receiving
    Point2D pos = SyncManager::m_facilitiesManager->getStationPosition(nodeId);
    outMsg.writeFloat(pos.x());                         // bytes: 13-16
    outMsg.writeFloat(pos.y());                         // bytes: 17-20
    // number of cams
    outMsg.writeInt((int)camInfo->size());              // bytes: 21-24

    for (it = camInfo->begin(); it < camInfo->end(); it++) {
        TCamInformation currCamInfo = *it;
        // General basic CAM profile
        outMsg.writeInt(currCamInfo.senderID);             // sender identifier
        outMsg.writeFloat(currCamInfo.senderPosition.x()); // sender x position
        outMsg.writeFloat(currCamInfo.senderPosition.y()); // sender y position
        outMsg.writeInt(currCamInfo.generationTime);       // message generationTime
        outMsg.writeInt((int) currCamInfo.staType);        // station Type
        // Vehicle CAM profile
        outMsg.writeFloat(currCamInfo.speed);              // speed
        outMsg.writeFloat(currCamInfo.angle);              // angle
        outMsg.writeFloat(currCamInfo.acceleration);       // acceleration
        outMsg.writeFloat(currCamInfo.length);             // vehicle length
        outMsg.writeFloat(currCamInfo.width);              // vehicle width
        outMsg.writeInt(currCamInfo.lights);               // vehicle exterior lights
        // Location Referencing information
        outMsg.writeString(currCamInfo.laneID);            // laneID
        outMsg.writeString(currCamInfo.edgeID);            // edgeID
        outMsg.writeString(currCamInfo.junctionID);        // junctionID
    }


    // send request message
    try {
        m_socket->sendExact(outMsg);
    } catch (SocketException e) {
        cout << "iCS --> #Error while sending command to Application: " << e.what() << endl;
        return false;
    }

    // receive answer message
    try {
        m_socket->receiveExact(inMsg);
    } catch (SocketException e) {
        cout << "iCS --> #Error while receiving response from Application: " << e.what() << endl;
        return false;
    }

    if (!ReportResultState(inMsg, CMD_RECEIVED_CAM_INFO))
        return false;

    return true;
}

bool
AppMessageManager::CommandSendSubscriptionFacilitiesInfo(vector<unsigned char>& facInfo, int nodeId)
{
    if (nodeId == 0) {
        cout << "Node id 0" << endl;
        return false;
    }

    tcpip::Storage outMsg;
    tcpip::Storage inMsg;

    if (m_socket == NULL) {
        cout << "iCS --> #Error while sending command: Socket is off" << endl;;
        return false;
    }

    // command length
    int totalLengthPacket = 4 /*length*/
                            + 1 /*commandId*/
                            + 4 /*timeStep*/
                            + 4 /*nodeId*/
                            + facInfo.size();

    outMsg.writeInt(totalLengthPacket);                                             // bytes: 0-3
    // command id
    outMsg.writeUnsignedByte(CMD_FACILITIES_INFORMATION);                          // bytes: 4
    // simulation timestep
    outMsg.writeInt(SyncManager::m_simStep);                                        // bytes: 5-8
    // subscribed node id
    outMsg.writeInt(nodeId);                                                        // bytes: 9-12
    // facilities information (expressed according to the Type-Length-Value syntax)
    outMsg.writePacket(facInfo); // bytes: 13-(13+facInfo.size())

    // send request message
    try {
        m_socket->sendExact(outMsg);
    } catch (SocketException e) {
        cout << "iCS --> #Error while sending command to Application: " << e.what() << endl;
        return false;
    }

    // receive answer message
    try {
        m_socket->receiveExact(inMsg);
    } catch (SocketException e) {
        cout << "iCS --> #Error while receiving response from Application: " << e.what() << endl;
        cout << "iCS --> #Error sent " << totalLengthPacket <<  " bytes to application" << endl;
        return false;
    }

    if (!ReportResultState(inMsg, CMD_FACILITIES_INFORMATION))
        return false;

    return true;
}

int
AppMessageManager::NotifyMessageStatus(int nodeId, vector<pair<int,stationID_t> >& receivedMessages)
{
    tcpip::Storage outMsg;
    tcpip::Storage inMsg;

    if (m_socket == NULL) {
        cout << "iCS --> #Error while sending command: Socket is off" << endl;
        return EXIT_FAILURE;
    }

    if (nodeId == 0) {
        cout << "Node id 0" << endl;
    }

    cout << "iCS --> AppMessageMananger: in NotifyMessageStatus..." << endl;

    int messagesSize = (receivedMessages.size() * 4) * 2;

    // command length
    outMsg.writeInt(4 + 1 + 4 + 4 + 4 + messagesSize);
    // command id
    outMsg.writeUnsignedByte(CMD_NOTIFY_APP_MESSAGE_STATUS);
    // simulation timestep
    outMsg.writeInt(SyncManager::m_simStep);
    // the node in which the applications is running
    outMsg.writeInt(nodeId);
    // number of received messages
    outMsg.writeInt(receivedMessages.size());
    int index = 0;
    for (vector<pair<int,stationID_t> >::iterator it=receivedMessages.begin() ; it != receivedMessages.end() ; ++it) {
        int messageId = (*it).first;
        int receiverId = (*it).second;
        outMsg.writeInt(messageId);
        outMsg.writeInt(receiverId);
        index++;

        stringstream log;
        log << "[INFO] NotifyMessageStatus() Receiver Id: " << receiverId << " Message Id: " << messageId << " sent as ARRIVED to the App";
        IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);

    }

    // send request message
    try {
        m_socket->sendExact(outMsg);
    } catch (SocketException e) {
        cout << "iCS --> #Error while sending command to Application: " << e.what() << endl;
        return EXIT_FAILURE;
    }

    // receive answer message
    try {
        m_socket->receiveExact(inMsg);
    } catch (SocketException e) {
        cout << "iCS --> #Error while receiving response from Application: " << e.what() << endl;
        return EXIT_FAILURE;
    }

    if (!ReportResultState(inMsg, CMD_NOTIFY_APP_MESSAGE_STATUS))
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

bool
AppMessageManager::CommandSendSubscriptionAppMessageSend(bool status, int nodeId)
{
    if (nodeId == 0) {
        cout << "Node id 0" << endl;
        return false;
    }

    tcpip::Storage outMsg;
    tcpip::Storage inMsg;

    if (m_socket == NULL) {
        cout << "iCS --> #Error while sending command: Socket is off" << endl;;
        return false;
    }

    // command length
    int totalLengthPacket = 4 /*length*/
                            + 1 /*commandId*/
                            + 4 /*timeStep*/
                            + 4 /*nodeId*/
                            + 1;/*scheduling status*/

    outMsg.writeInt(totalLengthPacket);                                             // bytes: 0-3
    // command id
    outMsg.writeUnsignedByte(CMD_APP_MSG_SEND);                                     // bytes: 4
    // simulation timestep
    outMsg.writeInt(SyncManager::m_simStep);                                        // bytes: 5-8
    // subscribed node id
    outMsg.writeInt(nodeId);                                                        // bytes: 9-12
    // scheduling status information (0x00 = NOT SCHEDULED, 0x01 = SCHEDULED)
    outMsg.writeUnsignedByte(status);                                               // bytes: 13

    // send request message
    try {
        m_socket->sendExact(outMsg);
    } catch (SocketException e) {
        cout << "iCS --> #Error while sending command to Application: " << e.what() << endl;
        return false;
    }

    // receive answer message
    try {
        m_socket->receiveExact(inMsg);
    } catch (SocketException e) {
        cout << "iCS --> #Error while receiving response from Application: " << e.what() << endl;
        return false;
    }

    if (!ReportResultState(inMsg, CMD_APP_MSG_SEND))
        return false;

    return true;
}

bool
AppMessageManager::CommandSendSubscriptionAppMessageReceive(std::vector<std::pair<ScheduledAPPMessageData,stationID_t> >& msgInfo)
{

	tcpip::Storage outMsg;
	tcpip::Storage inMsg;
	tcpip::Storage tmpMsg;

	int rcvMsg = 0;

	if (m_socket == NULL) {
	   cout << "iCS --> #Error while sending command: Socket is off" << endl;
	   return EXIT_FAILURE;
	}

	for (std::vector<std::pair<ScheduledAPPMessageData,stationID_t> >::iterator it = msgInfo.begin(); it != msgInfo.end(); ++it) {

      if ((*it).second == 0) {
        cout << "Node id 0" << endl;
        continue;
      }
      rcvMsg++;
      // subscribed node id
      tmpMsg.writeInt((*it).second);                                                        // bytes: 9-12
      // the received application message ID
      tmpMsg.writeInt((*it).first.appMessageId);
	}

	// command length
    int totalLengthPacket = 4 /*length*/
	                     + 1 /*commandId*/
	                     + 4 /*timeStep*/
	                     + 4 /* number of Rcv. Messages */
	                     + tmpMsg.size(); /*contains sequences of nodeId and appMsgId*/


	outMsg.writeInt(totalLengthPacket);                                             // bytes: 0-3
    // command id
	outMsg.writeUnsignedByte(CMD_APP_MSG_RECEIVE);                          // bytes: 4
    // simulation timestep
	outMsg.writeInt(SyncManager::m_simStep);                                        // bytes: 5-8

    // number of messages
    outMsg.writeInt(rcvMsg);

    // the tmp storage
    outMsg.writeStorage(tmpMsg);

    // send request message
      try {
          m_socket->sendExact(outMsg);
      } catch (SocketException e) {
          cout << "iCS --> #Error while sending command to Application: " << e.what() << endl;
          return EXIT_FAILURE;
      }

      // receive answer message
      try {
        m_socket->receiveExact(inMsg);
      } catch (SocketException e) {
        cout << "iCS --> #Error while receiving response from Application: " << e.what() << endl;
        return EXIT_FAILURE;
      }

      if (!ReportResultState(inMsg, CMD_APP_MSG_RECEIVE)) {
    	  cout << "iCS --> #Error while checking response state from Application: " << endl;
    	  //return false;
    	  return EXIT_FAILURE;
      }

    return EXIT_SUCCESS;
}

bool
AppMessageManager::CommandSendSubscriptionAppResultTraffSim(vector<unsigned char>& tsInfo, int nodeId)
{
    if (nodeId == 0) {
        cout << "Node id 0" << endl;
        return false;
    }

    tcpip::Storage outMsg;
    tcpip::Storage inMsg;

    if (m_socket == NULL) {
        cout << "iCS --> #Error while sending command: Socket is off" << endl;;
        return false;
    }

    // command length
    int totalLengthPacket = 4 /*length*/
                            + 1 /*commandId*/
                            + 4 /*timeStep*/
                            + 4 /*nodeId*/
                            + tsInfo.size();

    outMsg.writeInt(totalLengthPacket);                                             // bytes: 0-3
    // command id
    outMsg.writeUnsignedByte(CMD_APP_RESULT_TRAFF_SIM);                          // bytes: 4
    // simulation timestep
    outMsg.writeInt(SyncManager::m_simStep);                                        // bytes: 5-8
    // subscribed node id
    outMsg.writeInt(nodeId);                                                        // bytes: 9-12
    // facilities information (expressed according to the Type-Length-Value syntax)
    outMsg.writePacket(tsInfo); // bytes: 13-(13+tsInfo.size())

    // send request message
    try {
        m_socket->sendExact(outMsg);
    } catch (SocketException e) {
        cout << "iCS --> #Error while sending command to Application: " << e.what() << endl;
        return false;
    }

    // receive answer message
    try {
        m_socket->receiveExact(inMsg);
    } catch (SocketException e) {
        cout << "iCS --> #Error while receiving response from Application: " << e.what() << endl;
        return false;
    }

    if (!ReportResultState(inMsg, CMD_APP_RESULT_TRAFF_SIM))
        return false;

    return true;
}


bool
AppMessageManager::CommandSendSubscriptionXApplicationData(vector<unsigned char>& xAppData, int nodeId)
{
    if (nodeId == 0) {
        cout << "Node id 0" << endl;
        return false;
    }

    tcpip::Storage outMsg;
    tcpip::Storage inMsg;

    if (m_socket == NULL) {
        cout << "iCS --> #Error while sending command: Socket is off" << endl;;
        return false;
    }

    // command length
    int totalLengthPacket = 4 /*length*/
                            + 1 /*commandId*/
                            + 4 /*timeStep*/
                            + 4 /*nodeId*/
                            + xAppData.size();


    outMsg.writeInt(totalLengthPacket);                                             // bytes: 0-3
    // command id
    outMsg.writeUnsignedByte(CMD_X_APPLICATION_DATA);                          // bytes: 4
    // simulation timestep
    outMsg.writeInt(SyncManager::m_simStep);                                        // bytes: 5-8
    // subscribed node id
    outMsg.writeInt(nodeId);                                                        // bytes: 9-12
    // facilities information (expressed according to the Type-Length-Value syntax)
    outMsg.writePacket(xAppData); // bytes: 13-(13+tsInfo.size())

    // send request message
    try {
        m_socket->sendExact(outMsg);
    } catch (SocketException e) {
        cout << "iCS --> #Error while sending command to Application: " << e.what() << endl;
        return false;
    }

    // receive answer message
    try {
        m_socket->receiveExact(inMsg);
    } catch (SocketException e) {
        cout << "iCS --> #Error while receiving response from Application: " << e.what() << endl;
        return false;
    }

    if (!ReportResultState(inMsg, CMD_X_APPLICATION_DATA))
        return false;

    return true;
}

bool
AppMessageManager::CommandSendSubscriptionAppCmdTraffSim(bool status, int nodeId)
{
    if (nodeId == 0) {
        cout << "Node id 0" << endl;
        return false;
    }

    tcpip::Storage outMsg;
    tcpip::Storage inMsg;

    if (m_socket == NULL) {
        cout << "iCS --> #Error while sending command: Socket is off" << endl;;
        return false;
    }

    // command length
    int totalLengthPacket = 4 /*length*/
                            + 1 /*commandId*/
                            + 4 /*timeStep*/
                            + 4 /*nodeId*/
                            + 1;/*scheduling status*/

    outMsg.writeInt(totalLengthPacket);                                             // bytes: 0-3
    // command id
    outMsg.writeUnsignedByte(CMD_APP_CMD_TRAFF_SIM);                                     // bytes: 4
    // simulation timestep
    outMsg.writeInt(SyncManager::m_simStep);                                        // bytes: 5-8
    // subscribed node id
    outMsg.writeInt(nodeId);                                                        // bytes: 9-12
    // scheduling status information (0x00 = NOT SCHEDULED, 0x01 = SCHEDULED)
    outMsg.writeUnsignedByte(status);                                               // bytes: 13

    // send request message
    try {
        m_socket->sendExact(outMsg);
    } catch (SocketException e) {
        cout << "iCS --> #Error while sending command to Application: " << e.what() << endl;
        return false;
    }

    // receive answer message
    try {
        m_socket->receiveExact(inMsg);
    } catch (SocketException e) {
        cout << "iCS --> #Error while receiving response from Application: " << e.what() << endl;
        return false;
    }

    if (!ReportResultState(inMsg, CMD_APP_CMD_TRAFF_SIM))
        return false;

    return true;
}

bool
AppMessageManager::CommandApplicationToExecute(int nodeId, ResultContainer* resultContainer)
{
    tcpip::Storage outMsg;
    tcpip::Storage inMsg;

    // command length
    outMsg.writeInt(4 + 1 + 4 + 4); // Added by Ramon Bauza 29-09-10
    // command id
    outMsg.writeUnsignedByte(CMD_NOTIFY_APP_EXECUTE);
    // simulation timestep
    outMsg.writeInt(SyncManager::m_simStep);
    // the node in which the applications is running
    outMsg.writeInt(nodeId);

    if (nodeId == 0) {
        cout << "Node id 0" << endl;
    }

    // send request message
    try {
        m_socket->sendExact(outMsg);
    }
    catch (SocketException e) {
        cout << "iCS --> #Error while sending command to Application: " << e.what() << endl;
        return false;
    }

    // receive answer message
    try {
        m_socket->receiveExact(inMsg);

    }
    catch (SocketException e) {
        cout << "iCS --> #Error while receiving response from Application: " << e.what() << endl;
        return false;
    }

    if (!ReportResultState(inMsg, CMD_NOTIFY_APP_EXECUTE))
        return false;

    if (!resultContainer->ProcessResult(inMsg)) {
        cout << "iCS --> #Error processing the result." << endl;
        return false;
    }

    return true;
}

bool
AppMessageManager::ReportResultState(tcpip::Storage& inMsg, int command)
{
    int cmdLength;
    int cmdId;
    int resultType;
    int cmdStart;
    std::string msg;

    try {
        cmdStart = inMsg.position();
        cmdLength = inMsg.readUnsignedByte();
        cmdId = inMsg.readUnsignedByte();
        if (cmdId != command) {
            cout << "App --> iCS #Error: received status response to command: " << cmdId
                 << " but expected: " << command << endl;
            return false;
        }
        resultType = inMsg.readUnsignedByte();
        msg = inMsg.readString();
    } catch (std::invalid_argument e) {
        cout << "App --> iCS #Error: an exception was thrown while reading result state message. (command code: " << command << ")" << endl;
        return false;
    }

    switch (resultType) {
    case APP_RTYPE_ERR: {
        cout << ".. APP answered with error to command (" << cmdId << "), [description: " << msg << "]" << endl;
        return false;
    }
    case APP_RTYPE_NOTIMPLEMENTED:
        cout << ".. Sent command is not implemented (" << cmdId << "), [description: " << msg << "]" << endl;
        return false;
    case APP_RTYPE_OK:
#ifdef LOG_ON
    	cout << ".. Command acknowledged (" << cmdId << "), [description: " << msg << "]" << endl;
#endif
        break;
    default:
        cout << ".. Answered with unknown result code(" << resultType << ") to command(" << cmdId << "), [description: " << msg << "]" << endl;
        return false;
    }

    if ((cmdStart + cmdLength) != inMsg.position()) {
        cout << "App --> iCS #Error: command at position " << cmdStart << " has wrong length" << endl;
        return false;
    }
    return true;
}

bool
AppMessageManager::ValidateUnsubscriptions(tcpip::Storage& inMsg, vector<Subscription*>* subscriptions, bool &noMoreUnSubs)
{
    if (subscriptions == NULL) {
        cout << "iCS --> #Error while sending command: subsToDrop is NULL" << endl;
        return false;
    }

    int cmdId;
    int cmdLength;
    int cmdStart;

    while (inMsg.valid_pos()) {
        cmdStart = inMsg.position();
        cmdLength = inMsg.readUnsignedByte();
        cmdId = inMsg.readUnsignedByte();

        if (cmdId != CMD_END_SUBSCRIPTION) {
            cout << "iCS --> #Error: received response with command id: " << cmdId
                 << " but expected: " << (int)CMD_END_SUBSCRIPTION << endl;
            return false;
        }
        try {
            int subscription = inMsg.readUnsignedByte();

            switch (subscription) {
            case SUB_RETURNS_CARS_IN_ZONE: {
                float baseX = inMsg.readFloat();
                float baseY = inMsg.readFloat();
                float radius = inMsg.readFloat();
                SubsReturnsCarInZone::Delete(baseX, baseY, radius, subscriptions);
                cout << "iCS --> Unsubscribed to SUB_RETURNS_CARS_IN_ZONE with " << baseX << " " << baseY << " " << radius << endl;
                noMoreUnSubs = true;
                break;
            }
            case SUB_SET_CAM_AREA: {

                float baseX = inMsg.readFloat();
                float baseY = inMsg.readFloat();
                float radius = inMsg.readFloat();
                float frequency = inMsg.readFloat();
                SubsSetCamArea::Delete(baseX, baseY, radius, frequency, subscriptions);
                cout << "iCS --> Unsubscribed to SUB_SET_CAM_AREA with "<< baseX << " " << baseY << " " << radius << " " << frequency << endl;
                break;
            }
            case SUB_TRAVEL_TIME_ESTIMATION_START: {
                inMsg.readUnsignedByte(); //status (to be removed)
                break;
            }
            case SUB_TRAVEL_TIME_ESTIMATION_END: {
                inMsg.readUnsignedByte(); // status (to be removed)
                break;
            }
            case SUB_APP_MSG_RECEIVE: {
            	// SubsAppMessageReceive::Delete(stationID, subscriptions);
            	cout << "iCS --> Unsubscription to SUB_REQ_RECEIVED_MESSAGES_STATUS not yet implemented" << endl;
                break;
            }
            case SUB_FACILITIES_INFORMATION: {
                // TODO complete this method
                //SubsGetFacilitiesInfo::Delete(stationID, subscriptions);
                cout << "iCS --> Unsubscription to SUB_FACILITIES_INFORMATION not yet implemented" << endl;
                break;
            }
            case SUB_APP_MSG_SEND: {
                // TODO complete this method
                //SubsAppMessageSend::Delete(stationID, subscriptions);
                cout << "iCS --> Unsubscription to SUB_APP_MSG_SEND not yet implemented" << endl;
                break;
            }
            case SUB_APP_RESULT_TRAFF_SIM: {
                // TODO complete this method
                //SubsAppResultTraffSim::Delete(stationID, subscriptions);
                cout << "iCS --> Unsubscription to SUB_APP_RESULT_TRAFF_SIM not yet implemented" << endl;
                break;
            }
            case SUB_APP_CMD_TRAFF_SIM: {
                // TODO complete this method
                //SubsAppCmdTraffSim::Delete(stationID, subscriptions);
                cout << "iCS --> Unsubscription to SUB_APP_CMD_TRAFF_SIM not yet implemented" << endl;
                break;
            }
            case CMD_END_SUBSCRIPTION_REQUEST: {
                noMoreUnSubs = true;
                int stringLength = inMsg.readInt(); // !!! why?
                break;
            }
            default:
                cout << "iCS --> #Error: received unknown position format" << endl;
                return false;
            }
        } catch (std::invalid_argument e) {
            //answerLog << "#Error while reading message:" << e.what() << std::endl;
            return false;
        }
    }

    return true;
}

int
AppMessageManager::ValidateUnsubscriptions(Storage& inMsg)
{
    int cmdId;
    int cmdLength;
    int cmdStart;

    while (inMsg.valid_pos()) {
        cmdStart = inMsg.position();
        cmdLength = inMsg.readUnsignedByte();
        cmdId = inMsg.readUnsignedByte();
        if (cmdId != CMD_END_SUBSCRIPTION) {
            cout << "iCS --> #Error: received response with command id: " << cmdId << " but expected: " << (int)CMD_END_SUBSCRIPTION << endl;
            return -1;
        }
        try {
            int subscriptionStatus = inMsg.readUnsignedByte();

            if (subscriptionStatus == CMD_DROP_SUBSCRIPTION) {
                return 1;
            }

            if (subscriptionStatus == CMD_RENEW_SUBSCRIPTION) {
                return 0;
            }

            return -1;

        } catch (std::invalid_argument e) {
            //answerLog << "#Error while reading message:" << e.what() << std::endl;
            return -1;
        }
    }
}

}
