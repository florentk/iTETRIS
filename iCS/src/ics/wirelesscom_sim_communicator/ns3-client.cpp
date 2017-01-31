/****************************************************************************/
/// @file    ns3-client.cpp
/// @author  Julen Maneros
/// @date
/// @version $Id:
///
/****************************************************************************/
// iTETRIS, see http://www.ict-itetris.eu
// Copyright © 2008 iTetris Project Consortium - All rights reserved
/****************************************************************************/
// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <ctime>
#include <cstdlib>

#include "ns3-client.h"
#include "../utilities.h"
#include "../ics.h"
#include "../../utils/ics/iCStypes.h"
#include "../../utils/ics/log/ics-log.h"

#ifdef _WIN32
#include <windows.h> // needed for Sleep
#else
#include <unistd.h>
#define Sleep(x) usleep((x)*1000)
#endif

using namespace std;
using namespace ics_types;

namespace ics
{

// ===========================================================================
// static member definitions
// ===========================================================================
string Ns3Client::CAM_TYPE = "0";
string Ns3Client::DENM_TYPE = "1";

// ===========================================================================
// member method definitions
// ===========================================================================
Ns3Client::Ns3Client() {}

Ns3Client::~Ns3Client()
{
    delete m_socket;
}

bool
Ns3Client::Connect()
{
    m_socket = new SocketNs3(m_host, m_port);

    for (int i=0; i<10; ++i) {
        try {
            cout << "iCS --> Trying " << i << " to connect ns-3 on port " << m_port << " and Host "<<m_host<<"..." << endl;
            m_socket->connect();
            return true;
        } catch (SocketException e) {
            cout << "iCS --> No connection to ns-3; waiting..." << endl;
            Sleep(3000);
        }
    }

    return false;
}

int
Ns3Client::Close()
{
    if (m_socket != 0) {
        m_socket->close();
        delete m_socket;
        m_socket = 0;
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

bool
Ns3Client::CommandSimulationStep(int time)
{
    StorageNs3 outMsg;
    StorageNs3 inMsg;

    if (m_socket == NULL) {
        cout << "iCS -->  #Error while sending command: no connection to server ns-3";
        return false;
    }

    // command length
    outMsg.writeInt(4 + 1 + 4);
    // command id
    outMsg.writeUnsignedByte(CMD_SIMSTEP);
    // simulation time
    outMsg.writeInt(time);

    // send request message
    try {
        m_socket->sendExact(outMsg);
    } catch (SocketException e) {
        cout << "iCS --> #Error while sending command to ns-3: " << e.what();
        return false;
    }

    // receive answer message
    try {
        m_socket->receiveExact(inMsg);
    } catch (SocketException e) {
        cout << "iCS --> #Error while receiving command: " << e.what();
        return false;
    }

    // validate result state
    if (!ReportResultState(inMsg, CMD_SIMSTEP)) {
        return false;
    }

    return true;

}

int
Ns3Client::CommandUpdateNodePosition(int nodeId, float x, float y)
{
    StorageNs3 outMsg;
    StorageNs3 inMsg;

    if (m_socket == NULL) {
        cout << "iCS --> #Error while sending command: no connection to server";
        return EXIT_FAILURE;
    }

    // command length
    outMsg.writeInt(4 + 1 + 4 + 4 + 4);
    // command id
    outMsg.writeUnsignedByte(CMD_UPDATENODE);
    // node id
    outMsg.writeInt(nodeId);
    // position x
    outMsg.writeFloat(x);
    // position y
    outMsg.writeFloat(y);

    // send request message
    try {
        m_socket->sendExact(outMsg);
    } catch (SocketException e) {
        cout << "iCS --> #Error while sending command: " << e.what();
        return EXIT_FAILURE;
    }

    // receive answer message
    try {
        m_socket->receiveExact(inMsg);
    } catch (SocketException e) {
        cout << "iCS --> #Error while receiving command: " << e.what();
        return EXIT_FAILURE;
    }

    // validate result state
    if (!ReportResultState(inMsg, CMD_UPDATENODE)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int
Ns3Client::CommandUpdateNodePosition2(int nodeId, float x, float y, float speed, float heading, std::string laneId)
{
    StorageNs3 outMsg;
    StorageNs3 inMsg;

    if (m_socket == NULL) {
        cout << "iCS --> #Error while sending command: no connection to server";
        return EXIT_FAILURE;
    }

    // command length
    outMsg.writeInt(4 + 1 + 4 + 4 + 4 + 4 + 4 + 4 + laneId.length());
    // command id
    outMsg.writeUnsignedByte(CMD_UPDATENODE2);
    // node id
    outMsg.writeInt(nodeId);
    // position x
    outMsg.writeFloat(x);
    // position y
    outMsg.writeFloat(y);
    // speed
    outMsg.writeFloat(speed);
    // heading
    outMsg.writeFloat(heading);
    // laneId
    outMsg.writeString(laneId);

    // send request message
    try {
        m_socket->sendExact(outMsg);
    } catch (SocketException e) {
        cout << "iCS --> #Error while sending command: " << e.what();
        return EXIT_FAILURE;
    }

    // receive answer message
    try {
        m_socket->receiveExact(inMsg);
    } catch (SocketException e) {
        cout << "iCS --> #Error while receiving command: " << e.what();
        return EXIT_FAILURE;
    }

    // validate result state
    if (!ReportResultState(inMsg, CMD_UPDATENODE2)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int
Ns3Client::CommandCreateNode(float x, float y, std::vector<std::string> techList)
{
    StorageNs3 outMsg;
    StorageNs3 inMsg;

    if (m_socket == NULL) {
        cout << "iCS --> #Error while sending command: no connection to server" ;
        return -1;
    }

    // 4 bytes for the length of the string list
    // 4 bytes for the length each member of the list
    int stringSize = 4 + (techList.size() * 4);

    int totalAmountOfCharacters = 0;
    vector<string>::iterator it;
    for (it = techList.begin() ; it < techList.end() ; it++) {
        string chain = *it;
        size_t chainSize = chain.length();
        totalAmountOfCharacters += chainSize;
    }

    // Finally one byte per character
    stringSize = stringSize + totalAmountOfCharacters;

    // command length
    outMsg.writeInt(4 + 1 + 4 + 4 + stringSize);
    // command id
    outMsg.writeUnsignedByte(CMD_CREATENODE);
    // position x
    outMsg.writeFloat(x);
    // position y
    outMsg.writeFloat(y);
    // rat type
    outMsg.writeStringList(techList);

    // send request message
    try {
        m_socket->sendExact(outMsg);
    } catch (SocketException e) {
        cout << "iCS --> Error while sending command: " << e.what();
        return -1;
    }

    // receive answer message
    try {
        m_socket->receiveExact(inMsg);
    } catch (SocketException e) {
        cout << "iCS --> #Error while receiving command: " << e.what();
        return -1;
    }

    // validate result state
    if (!ReportResultState(inMsg, CMD_CREATENODE)) {
        return -1;
    }

    // length
    inMsg.readUnsignedByte();
    // command id
    inMsg.readUnsignedByte();
    int32_t nodeId = inMsg.readInt();

    return nodeId;
}

int
Ns3Client::CommandCreateNode2(float x, float y,  float speed, float heading, std::string laneId, std::vector<std::string> techList)
{
    StorageNs3 outMsg;
    StorageNs3 inMsg;

    if (m_socket == NULL) {
        cout << "iCS --> #Error while sending command: no connection to server" ;
        return -1;
    }

    // 4 bytes for the length of the string list
    // 4 bytes for the length each member of the list
    int stringSize = 4 + (techList.size() * 4);

    int totalAmountOfCharacters = 0;
    vector<string>::iterator it;
    for (it = techList.begin() ; it < techList.end() ; it++) {
        string chain = *it;
        size_t chainSize = chain.length();
        totalAmountOfCharacters += chainSize;
    }

    // Finally one byte per character
    stringSize = stringSize + totalAmountOfCharacters;

    // command length
    outMsg.writeInt(4 + 1 + 4 + 4 + 4 + 4 + 4 + laneId.length() + stringSize);
    // command id
    outMsg.writeUnsignedByte(CMD_CREATENODE2);
    // position x
    outMsg.writeFloat(x);
    // position y
    outMsg.writeFloat(y);
    // speed
    outMsg.writeFloat(speed);
    // heading
    outMsg.writeFloat(heading);
    // laneId
    outMsg.writeString(laneId);
    // rat type
    outMsg.writeStringList(techList);

    // send request message
    try {
        m_socket->sendExact(outMsg);
    } catch (SocketException e) {
        cout << "iCS --> Error while sending command: " << e.what();
        return -1;
    }

    // receive answer message
    try {
        m_socket->receiveExact(inMsg);
    } catch (SocketException e) {
        cout << "iCS --> #Error while receiving command: " << e.what();
        return -1;
    }

    // validate result state
    if (!ReportResultState(inMsg, CMD_CREATENODE2)) {
        return -1;
    }

    // length
    inMsg.readUnsignedByte();
    // command id
    inMsg.readUnsignedByte();
    int32_t nodeId = inMsg.readInt();

    return nodeId;
}

bool
Ns3Client::CommandStartSendingCam(vector<string> sendersId, unsigned int payloadLength, float frequency)
{
    StorageNs3 outMsg;
    StorageNs3 inMsg;

    if (m_socket == NULL) {
        cout << "iCS --> #Error while sending command: no connection to server" ;
        return false;
    }

    // 4 bytes for the length of the string list
    // 4 bytes for the length each member of the list
    int stringSize = 4 + (sendersId.size() * 4);

    int totalAmountOfCharacters = 0;
    vector<string>::iterator it;
    for (it = sendersId.begin() ; it < sendersId.end() ; it++) {
        string chain = *it;
        size_t chainSize = chain.length();
        totalAmountOfCharacters += chainSize;
    }

    // Finally one byte per character
    stringSize = stringSize + totalAmountOfCharacters;

    outMsg.writeInt(4 + 1 + stringSize + 4 + 4);
    // command id
    outMsg.writeUnsignedByte(CMD_START_CAM);
    // ns-3 ids of the nodes that has to start sending CAM
    outMsg.writeStringList(sendersId);
    // the length of information the CAM message transmits  --> TODO: the value shall not be hard coded!
    outMsg.writeInt(20);
    // the frequency in which the message will be sent
    outMsg.writeFloat(frequency);

    // send request message
    try {
        m_socket->sendExact(outMsg);
    } catch (SocketException e) {
        cout << "iCS --> Error while sending command: " << e.what();
        return false;
    }

    // receive answer message
    try {
        m_socket->receiveExact(inMsg);
    } catch (SocketException e) {
        cout << "iCS --> #Error while receiving command: " << e.what();
        return false;
    }

    // validate result state
    if (!ReportResultState(inMsg, CMD_START_CAM)) {
        return false;
    }

    return true;
}

bool
Ns3Client::CommandStopSendingCam(vector<string> sendersId)
{
    StorageNs3 outMsg;
    StorageNs3 inMsg;

    if (m_socket == NULL) {
        cout << "iCS --> #Error while sending command: no connection to server" ;
        return false;
    }

    // 4 bytes for the length of the string list
    // 4 bytes for the length each member of the list
    int stringSize = 4 + (sendersId.size() * 4);

    int totalAmountOfCharacters = 0;
    vector<string>::iterator it;
    for (it = sendersId.begin() ; it < sendersId.end() ; it++) {
        string chain = *it;
        size_t chainSize = chain.length();
        totalAmountOfCharacters += chainSize;
    }

    // Finally one byte per character
    stringSize = stringSize + totalAmountOfCharacters;

    // command length
    outMsg.writeInt(4 + 1 + stringSize);
    // command id
    outMsg.writeUnsignedByte(CMD_STOP_CAM);
    // ns-3 ids of the nodes that has to start sending CAM
    outMsg.writeStringList(sendersId);


    // send request message
    try {
        m_socket->sendExact(outMsg);
    } catch (SocketException e) {
        cout << "iCS --> Error while sending command: " << e.what();
        return false;
    }

    // receive answer message
    try {
        m_socket->receiveExact(inMsg);
    } catch (SocketException e) {
        cout << "iCS --> #Error while receiving command: " << e.what();
        return false;
    }

    // validate result state
    if (!ReportResultState(inMsg, CMD_STOP_CAM)) {
        return false;
    }

    return true;
}

bool
Ns3Client::CommandGetReceivedMessages(int nodeId, vector<ReceivedMessage>* receivedMessages)
{
    StorageNs3 outMsg;
    StorageNs3 inMsg;


    if (m_socket == NULL) {
        cout << "iCS --> #Error while sending command: no connection to server" ;
        return false;
    }

    // command length
    outMsg.writeInt(4 + 1 + 4);
    // command id
    outMsg.writeUnsignedByte(CMD_GET_RECEIVED_MESSAGES);
    // node id
    outMsg.writeInt(nodeId);


    // send request message
    try {
        m_socket->sendExact(outMsg);
    } catch (SocketException e) {
        cout << "iCS --> Error while sending command: " << e.what();
        return false;
    }

    // receive answer message
    try {
        m_socket->receiveExact(inMsg);
    } catch (SocketException e) {
        cout << "iCS --> #Error while receiving command: " << e.what();
        return false;
    }

    // validate result state
    if (!ReportResultState(inMsg, CMD_GET_RECEIVED_MESSAGES)) {
        return false;
    }

    // length
    inMsg.readInt();
    // command id
    inMsg.readUnsignedByte();

    vector<string> receivedNodes = inMsg.readStringList();
    vector<string> types = inMsg.readStringList();
    vector<string> sentTimesteps = inMsg.readStringList();
    vector<string> seqNumb = inMsg.readStringList();

    if (!(receivedNodes.size() == types.size()) || !(sentTimesteps.size() == types.size()) || !(seqNumb.size() == sentTimesteps.size())) {
        cout << "iCS --> #Error, the returned values size do not match" << endl;
        return false;
    }

    // If the simulator did not return any message do not process
    if (receivedNodes.size() == 0) {

        stringstream log;
        log << "iCS --> [CommandGetReceivedMessages] 0 messages were received";
        IcsLog::LogLevel((log.str()).c_str(), kLogLevelInfo);

    } else {
        vector<string>::iterator resultIt;
        int index = 0;
        for (resultIt = receivedNodes.begin() ; resultIt != receivedNodes.end() ; resultIt++) {
            string senderId = *resultIt;
            string type = types.at(index);
            string timestep = sentTimesteps.at(index);
            string seqNum = seqNumb.at(index);

            ReceivedMessage receivedMessage;
            receivedMessage.senderId = utils::Conversion::string2Int(senderId);
            if (type =="CAM" /*CAM_TYPE*/) {
                //receivedMessage.messageType = 0;
                receivedMessage.messageType = CAM;
            } else {
                if (type == DENM_TYPE) {
                    //receivedMessage.messageType = 1;
                    receivedMessage.messageType = DENM;
                } else {
                    if (type == "serviceIdUnicast") {
                        receivedMessage.messageType = UNICAST;
                    } else {
                        if (type == "serviceIdGeobroadcast") {
                            receivedMessage.messageType = GEOBROADCAST;
                        } else {
                            if (type == "serviceIdTopobroadcast") {
                                receivedMessage.messageType = TOPOBROADCAST;
                            }
                        }
                    }
                }
            }

            receivedMessage.timeStep =  utils::Conversion::string2Int(timestep);
            receivedMessage.sequenceNumber = utils::Conversion::string2Int(seqNum);
            receivedMessages->push_back(receivedMessage);

            index++;
        }
    }

    return true;
}

bool
Ns3Client::CommandStartTopoTxon(std::vector<std::string> sendersId, std::string serviceId, unsigned char commProfile, std::vector<std::string> technologyList, float frequency, unsigned int payloadLength, float msgRegenerationTime, unsigned int msgLifetime, unsigned int numHops)
{

    StorageNs3 outMsg;
    StorageNs3 inMsg;

    if (m_socket == NULL) {
        cout << "iCS --> #Error while sending command: no connection to server" ;
        return false;
    }

    int stringSize = CalculateStringListByteSize(sendersId);
    int stringSize2 = CalculateStringListByteSize(technologyList);

    ////////////////////////////////////////////
    // MODIFIED BY Florent KAISSER 01/10/2017
    // twice stringSize : seconde stringSize -> stringSize2
    outMsg.writeInt(4 + 1 + stringSize + 4 + serviceId.length() + 1 + stringSize2 + 4 + 4 + 4 + 4 + 4); // Not sure if this works // FK : WTF !!
    ////////////////////////////////////////////
    outMsg.writeUnsignedByte(CMD_START_TOPO_TXON);
    outMsg.writeStringList(sendersId);
    outMsg.writeString(serviceId);
    outMsg.writeUnsignedByte(commProfile);
    outMsg.writeStringList(technologyList);
    outMsg.writeFloat(frequency);
    outMsg.writeInt(payloadLength);
    outMsg.writeFloat(msgRegenerationTime);
    outMsg.writeInt(msgLifetime);
    outMsg.writeInt(numHops);

    // send request message
    try {
        m_socket->sendExact(outMsg);
    } catch (SocketException e) {
        cout << "iCS --> Error while sending command: " << e.what();
        return false;
    }

    // receive answer message
    try {
        m_socket->receiveExact(inMsg);
    } catch (SocketException e) {
        cout << "iCS --> #Error while receiving command: " << e.what();
        return false;
    }

    // validate result state
    if (!ReportResultState(inMsg, CMD_START_TOPO_TXON)) {
        return false;
    }

    return true;
}

int
Ns3Client::CommandClose()
{
    StorageNs3 outMsg;
    StorageNs3 inMsg;

    if (m_socket == NULL) {
        cout << "iCS --> #Error while sending command: no connection to server" ;
        return EXIT_FAILURE;
    }

    // command length
    outMsg.writeInt(4 + 1);
    // command id
    outMsg.writeUnsignedByte(CMD_CLOSE);

    // send request message
    try {
        m_socket->sendExact(outMsg);
    } catch (SocketException e) {
        cout << "iCS --> Error while sending command: " << e.what();
        return EXIT_FAILURE;
    }

    // receive answer message
    try {
        m_socket->receiveExact(inMsg);
    } catch (SocketException e) {
        cout << "iCS --> #Error while receiving command: " << e.what();
        return EXIT_FAILURE;
    }

    // validate result state
    if (!ReportResultState(inMsg, CMD_CLOSE)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

bool
Ns3Client::ReportResultState(StorageNs3& inMsg, int command)
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
            cout << "ns-3 --> iCS #Error: received status response to command: " << cmdId
                 << " but expected: " << command << endl;
            return false;
        }
        resultType = inMsg.readUnsignedByte();
        msg = inMsg.readString();
    } catch (std::invalid_argument e) {
        cout << "ns-3 --> iCS #Error: an exception was thrown while reading result state message" << endl;
        return false;
    }

    switch (resultType) {
    case NS3_RTYPE_ERR: {
        cout << ".. ns-3 answered with error to command (" << cmdId << "), [description: " << msg << "]" << endl;
        return false;
    }
    case NS3_RTYPE_NOTIMPLEMENTED:
        cout << ".. Sent command is not implemented (" << cmdId << "), [description: " << msg << "]" << endl;
        return false;
    case NS3_RTYPE_OK:
#ifdef LOG_ON
        cout << ".. Command acknowledged (" << cmdId << "), [description: " << msg << "]" << endl;
#endif
    	break;
    default:
        cout << ".. Answered with unknown result code(" << resultType << ") to command(" << cmdId
             << "), [description: " << msg << "]" << endl;
        return false;
    }


    if ((cmdStart + cmdLength) != inMsg.position()) {
        cout << "ns-3 --> iCS #Error: command at position " << cmdStart << " has wrong length "<<cmdLength<< "H" << endl;
        return false;
    }

    return true;
}

int
Ns3Client::CalculateStringListByteSize(vector<string> list)
{
    ////////////////////////////////////////////
    // MODIFIED BY Florent KAISSER 01/10/2017
    // migth return 4 (to write size = 0)
    if (list.size() == 0)
        return 4;
    ////////////////////////////////////////////        

    // 4 bytes for the length of the string list
    // 4 bytes for the length each member of the list
    int stringSize = 4 + (list.size() * 4);

    int totalAmountOfCharacters = 0;
    vector<string>::iterator it;
    for (it = list.begin() ; it < list.end() ; it++) {
        string chain = *it;
        size_t chainSize = chain.length();
        totalAmountOfCharacters += chainSize;
    }

    // Finally one byte per character
    stringSize = stringSize + totalAmountOfCharacters;

    return stringSize;
}

bool
Ns3Client::CommandStartIpCiuTxon(std::vector<std::string> sendersId, std::string serviceId, float frequency, unsigned int payloadLength, unsigned int destination, float msgRegenerationTime)
{

    StorageNs3 outMsg;
    StorageNs3 inMsg;

    if (m_socket == NULL) {
        cout << "iCS --> #Error while sending command: no connection to server" ;
        return false;
    }

    int stringSize = CalculateStringListByteSize(sendersId);

    outMsg.writeInt(4 + 1 + stringSize + 4 + serviceId.length() + 4 + 4 + 4 + 4);
    outMsg.writeUnsignedByte(CMD_START_IPCIU_TXON);
    outMsg.writeStringList(sendersId);
    outMsg.writeString(serviceId);
    outMsg.writeFloat(frequency);
    outMsg.writeInt(payloadLength);
    outMsg.writeInt(destination);
    outMsg.writeFloat(msgRegenerationTime);

    // send request message
    try {
        m_socket->sendExact(outMsg);
    } catch (SocketException e) {
        cout << "iCS --> Error while sending command: " << e.what();
        return false;
    }

    // receive answer message
    try {
        m_socket->receiveExact(inMsg);
    } catch (SocketException e) {
        cout << "iCS --> #Error while receiving command: " << e.what();
        return false;
    }

    // validate result state
    if (!ReportResultState(inMsg, CMD_START_IPCIU_TXON)) {
        return false;
    }

    return true;
}

bool
Ns3Client::CommandStartIdBasedTxon(std::vector<std::string> sendersId, std::string serviceId, unsigned char commProfile, std::vector<std::string> technologyList, float frequency, unsigned int payloadLength, unsigned int destination, float msgRegenerationTime, unsigned int msgLifetime)
{
    StorageNs3 outMsg;
    StorageNs3 inMsg;

    if (m_socket == NULL) {
        cout << "iCS --> #Error while sending command: no connection to server" ;
        return false;
    }

    int stringSize = CalculateStringListByteSize(sendersId);
    int stringSize2 = CalculateStringListByteSize(technologyList);

    outMsg.writeInt(4 + 1 + stringSize + 4 + serviceId.length() + 1 + stringSize2 + 4 + 4 + 4  + 4 + 4);
    outMsg.writeUnsignedByte(CMD_START_ID_BASED_TXON);
    outMsg.writeStringList(sendersId);
    outMsg.writeString(serviceId);
    outMsg.writeUnsignedByte(commProfile);
    outMsg.writeStringList(technologyList);
    outMsg.writeFloat(frequency);
    outMsg.writeInt(payloadLength);
    outMsg.writeInt(destination);
    outMsg.writeFloat(msgRegenerationTime);
    outMsg.writeInt(msgLifetime);

    // send request message
    try {
        m_socket->sendExact(outMsg);
    } catch (SocketException e) {
        cout << "iCS --> Error while sending command: " << e.what();
        return false;
    }

    // receive answer message
    try {
        m_socket->receiveExact(inMsg);
    } catch (SocketException e) {
        cout << "iCS --> #Error while receiving command: " << e.what();
        return false;
    }

    // validate result state
    if (!ReportResultState(inMsg, CMD_START_ID_BASED_TXON)) {
        return false;
    }

    return true;
}

bool
Ns3Client::CommandStartMWTxon (std::vector<std::string> sendersId, std::string serviceId, unsigned char commProfile, std::vector<std::string> technologyList, CircularGeoAddress destination, float frequency, uint32_t payloadLength,  double msgRegenerationTime, uint8_t msgLifetime)
{
    StorageNs3 outMsg;
    StorageNs3 inMsg;

    if (m_socket == NULL) {
        cout << "iCS --> #Error while sending command: no connection to server" ;
        return false;
    }

    int stringSize = CalculateStringListByteSize(sendersId);
    int stringSize2 = CalculateStringListByteSize(technologyList);

    outMsg.writeInt(4 + 1 + stringSize  + 4 + serviceId.length() + 1 + stringSize2 + 4 + 4 + 4 + 4 + 4 + 4 + 4);
    outMsg.writeUnsignedByte(CMD_START_MW_TXON);
    outMsg.writeStringList(sendersId);
    outMsg.writeString(serviceId);
    outMsg.writeUnsignedByte(commProfile);
    outMsg.writeStringList(technologyList);
    outMsg.writeInt(destination.lat);
    outMsg.writeInt(destination.lon);
    outMsg.writeInt(destination.areaSize);
    outMsg.writeFloat(frequency);
    outMsg.writeInt(payloadLength);
    outMsg.writeFloat(msgRegenerationTime);
    outMsg.writeInt(msgLifetime);

    // send request message
    try {
        m_socket->sendExact(outMsg);
    } catch (SocketException e) {
        cout << "iCS --> Error while sending command: " << e.what();
        return false;
    }

    // receive answer message
    try {
        m_socket->receiveExact(inMsg);
    } catch (SocketException e) {
        cout << "iCS --> #Error while receiving command: " << e.what();
        return false;
    }

    // validate result state
    if (!ReportResultState(inMsg, CMD_START_MW_TXON)) {
        return false;
    }

    return true;
}

bool
Ns3Client::CommandStopServiceTxon(std::vector<std::string> sendersId, std::string serviceId)
{
    StorageNs3 outMsg;
    StorageNs3 inMsg;

    if (m_socket == NULL) {
        cout << "iCS --> #Error while sending command: no connection to server" ;
        return false;
    }

    int stringSize = CalculateStringListByteSize(sendersId);

    outMsg.writeInt(4 + 1 + stringSize + 4 + serviceId.length());
    outMsg.writeUnsignedByte(CMD_STOP_SERVICE_TXON);
    outMsg.writeStringList(sendersId);
    outMsg.writeString(serviceId);

    // send request message
    try {
        m_socket->sendExact(outMsg);
    } catch (SocketException e) {
        cout << "iCS --> Error while sending command: " << e.what();
        return false;
    }

    // receive answer message
    try {
        m_socket->receiveExact(inMsg);
    } catch (SocketException e) {
        cout << "iCS --> #Error while receiving command: " << e.what();
        return false;
    }

    // validate result state
    if (!ReportResultState(inMsg, CMD_STOP_SERVICE_TXON)) {
        return false;
    }

    return true;
}

bool
Ns3Client::CommandStopIpCiuServiceTxon(std::vector<std::string> sendersId, std::string serviceId)
{
    StorageNs3 outMsg;
    StorageNs3 inMsg;

    if (m_socket == NULL) {
        cout << "iCS --> #Error while sending command: no connection to server" ;
        return false;
    }

    int stringSize = CalculateStringListByteSize(sendersId);

    outMsg.writeInt(4 + 1 + stringSize + 4 + serviceId.length());
    outMsg.writeUnsignedByte(CMD_STOP_IPCIU_SERVICE_TXON);
    outMsg.writeStringList(sendersId);
    outMsg.writeString(serviceId);

    // send request message
    try {
        m_socket->sendExact(outMsg);
    } catch (SocketException e) {
        cout << "iCS --> Error while sending command: " << e.what();
        return false;
    }

    // receive answer message
    try {
        m_socket->receiveExact(inMsg);
    } catch (SocketException e) {
        cout << "iCS --> #Error while receiving command: " << e.what();
        return false;
    }

    // validate result state
    if (!ReportResultState(inMsg, CMD_STOP_IPCIU_SERVICE_TXON)) {
        return false;
    }

    return true;
}

bool
Ns3Client::CommandStopMWServiceTxon(std::vector<std::string> sendersId, std::string serviceId)
{
    StorageNs3 outMsg;
    StorageNs3 inMsg;

    if (m_socket == NULL) {
        cout << "iCS --> #Error while sending command: no connection to server" ;
        return false;
    }

    int stringSize = CalculateStringListByteSize(sendersId);

    outMsg.writeInt(4 + 1 + stringSize + 4 + serviceId.length());
    outMsg.writeUnsignedByte(CMD_STOP_MW_SERVICE_TXON);
    outMsg.writeStringList(sendersId);
    outMsg.writeString(serviceId);

    // send request message
    try {
        m_socket->sendExact(outMsg);
    } catch (SocketException e) {
        cout << "iCS --> Error while sending command: " << e.what();
        return false;
    }

    // receive answer message
    try {
        m_socket->receiveExact(inMsg);
    } catch (SocketException e) {
        cout << "iCS --> #Error while receiving command: " << e.what();
        return false;
    }

    // validate result state
    if (!ReportResultState(inMsg, CMD_STOP_MW_SERVICE_TXON)) {
        return false;
    }

    return true;
}

bool
Ns3Client::StartGeobroadcastTxon(std::vector<std::string> sendersId, std::string serviceId, unsigned char commProfile, std::vector<std::string> technList, CircularGeoAddress destination, float frequency, unsigned int payloadLength, float msgRegenerationTime, unsigned int msgLifetime)
{
    StorageNs3 outMsg;
    StorageNs3 inMsg;

    if (m_socket == NULL) {
        cout << "iCS --> #Error while sending command: no connection to server" ;
        return false;
    }

    int stringSize = CalculateStringListByteSize(sendersId);
    int stringSize2 = CalculateStringListByteSize(technList);

    outMsg.writeInt(4 + 1 + stringSize + 4 + serviceId.length() + 1 + stringSize2 + 4 + 4 + 4 + 4 + 4 + 4 + 4);
    outMsg.writeUnsignedByte(CMD_START_GEO_BROAD_TXON);
    outMsg.writeStringList(sendersId);
    outMsg.writeString(serviceId);
    outMsg.writeUnsignedByte(commProfile);
    outMsg.writeStringList(technList);
    outMsg.writeInt(destination.lat);
    outMsg.writeInt(destination.lon);
    outMsg.writeInt(destination.areaSize);
    outMsg.writeFloat(frequency);
    outMsg.writeInt(payloadLength);
    outMsg.writeFloat(msgRegenerationTime);
    outMsg.writeInt(msgLifetime);

    // send request message
    try {
        m_socket->sendExact(outMsg);
        cout << "msg sended" << endl ;
    } catch (SocketException e) {
        cout << "iCS --> Error while sending command: " << e.what();
        return false;
    }
cout << "try to receive" << endl ;
    // receive answer message
    try {
        m_socket->receiveExact(inMsg);
        cout << "msg received" << endl ;
    } catch (SocketException e) {
        cout << "iCS --> #Error while receiving !!! command: " << e.what();
        return false;
    }

    // validate result state
    if (!ReportResultState(inMsg, CMD_START_GEO_BROAD_TXON)) {
        return false;
    }

    return true;
}

bool
Ns3Client::StartGeoanycastTxon(std::vector<std::string> sendersId, std::string serviceId, unsigned char commProfile, std::vector<std::string> technList, CircularGeoAddress destination, float frequency, unsigned int payloadLength, float msgRegenerationTime, unsigned int msgLifetime)
{
    StorageNs3 outMsg;
    StorageNs3 inMsg;

    if (m_socket == NULL) {
        cout << "iCS --> #Error while sending command: no connection to server" ;
        return false;
    }

    int stringSize = CalculateStringListByteSize(sendersId);
    int stringSize2 = CalculateStringListByteSize(technList);

    outMsg.writeInt(4 + 1 + stringSize + 4 + serviceId.length() + 1 + stringSize2 + 4 + 4 + 4 + 4 + 4 + 4 + 4);
    outMsg.writeUnsignedByte(CMD_START_GEO_ANY_TXON);
    outMsg.writeStringList(sendersId);
    outMsg.writeString(serviceId);
    outMsg.writeUnsignedByte(commProfile);
    outMsg.writeStringList(technList);
    outMsg.writeInt(destination.lat);
    outMsg.writeInt(destination.lon);
    outMsg.writeInt(destination.areaSize);
    outMsg.writeFloat(frequency);
    outMsg.writeInt(payloadLength);
    outMsg.writeFloat(msgRegenerationTime);
    outMsg.writeInt(msgLifetime);

    // send request message
    try {
        m_socket->sendExact(outMsg);
    } catch (SocketException e) {
        cout << "iCS --> Error while sending command: " << e.what();
        return false;
    }

    // receive answer message
    try {
        m_socket->receiveExact(inMsg);
    } catch (SocketException e) {
        cout << "iCS --> #Error while receiving command: " << e.what();
        return false;
    }

    // validate result state
    if (!ReportResultState(inMsg, CMD_START_GEO_ANY_TXON)) {
        return false;
    }

    return true;
}

bool
Ns3Client::CommandActivateNode(vector<string> sendersId)
{
    StorageNs3 outMsg;
    StorageNs3 inMsg;

    if (m_socket == NULL) {
        cout << "iCS --> #Error while sending command: no connection to server" ;
        return false;
    }

    // 4 bytes for the length of the string list
    // 4 bytes for the length each member of the list
    int stringSize = 4 + (sendersId.size() * 4);

    int totalAmountOfCharacters = 0;
    vector<string>::iterator it;
    for (it = sendersId.begin() ; it < sendersId.end() ; it++) {
        string chain = *it;
        size_t chainSize = chain.length();
        totalAmountOfCharacters += chainSize;
    }

    // Finally one byte per character
    stringSize = stringSize + totalAmountOfCharacters;

    // command length
//     outMsg.writeUnsignedByte(1 + 1 + stringSize);
    outMsg.writeInt(4 + 1 + stringSize);
    // command id
    outMsg.writeUnsignedByte(CMD_ACTIVATE_NODE);
    // ns-3 ids of the nodes to be activated
    outMsg.writeStringList(sendersId);


    // send request message
    try {
        m_socket->sendExact(outMsg);
    } catch (SocketException e) {
        cout << "iCS --> Error while sending command: " << e.what();
        return false;
    }

    // receive answer message
    try {
        m_socket->receiveExact(inMsg);
    } catch (SocketException e) {
        cout << "iCS --> #Error while receiving command: " << e.what();
        return false;
    }

    // validate result state
    if (!ReportResultState(inMsg, CMD_ACTIVATE_NODE)) {
        return false;
    }

    return true;
}

bool
Ns3Client::CommandDeactivateNode(vector<string> sendersId)
{
    StorageNs3 outMsg;
    StorageNs3 inMsg;

    if (m_socket == NULL) {
        cout << "iCS --> #Error while sending command: no connection to server" ;
        return false;
    }

    // 4 bytes for the length of the string list
    // 4 bytes for the length each member of the list
    int stringSize = 4 + (sendersId.size() * 4);

    int totalAmountOfCharacters = 0;
    vector<string>::iterator it;
    for (it = sendersId.begin() ; it < sendersId.end() ; it++) {
        string chain = *it;
        size_t chainSize = chain.length();
        totalAmountOfCharacters += chainSize;
    }

    // Finally one byte per character
    stringSize = stringSize + totalAmountOfCharacters;

    // command length
    outMsg.writeInt(4 + 1 + stringSize);
    // command id
    outMsg.writeUnsignedByte(CMD_DEACTIVATE_NODE);
    // ns-3 ids of the nodes to be activated
    outMsg.writeStringList(sendersId);


    // send request message
    try {
        m_socket->sendExact(outMsg);
    } catch (SocketException e) {
        cout << "iCS --> Error while sending command: " << e.what();
        return false;
    }

    // receive answer message
    try {
        m_socket->receiveExact(inMsg);
    } catch (SocketException e) {
        cout << "iCS --> #Error while receiving command: " << e.what();
        return false;
    }

    // validate result state
    if (!ReportResultState(inMsg, CMD_DEACTIVATE_NODE)) {
        return false;
    }

    return true;
}

}
