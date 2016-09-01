/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, EU FP7 iTETRIS project
 *                          CBT
 *                          Uwicore Laboratory (www.uwicore.umh.es), University Miguel Hernandez 
 *                          EURECOM (www.eurecom.fr), EU FP7 iTETRIS project
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Julen Maneros <jmaneros@cbt.es>, Ramon Bauza <rbauza@umh.es>, Fatma Hrizi <Fatma.Hrizi@eurecom.fr>, Jerome Haerri <Jerome.Haerri@eurecom.fr>
 */

#include "ns3-server.h"
#include "ns3-commands.h"
#include "ns3-comm-constants.h"
#include <iostream>

using namespace std;
using namespace tcpip;

namespace ns3
{

Ns3Server* Ns3Server::instance_ = 0;
bool Ns3Server::closeConnection_ = false;
int Ns3Server::targetTime_ = 0;
ofstream Ns3Server::myfile;
string Ns3Server::CAM_TYPE = "0";
string Ns3Server::DNEM_TYPE = "1";
bool Ns3Server::logActive_ = false;

Ns3Server::Ns3Server(int port, iTETRISNodeManager *node_manager, PacketManager *packetManager)
{  
	myDoingSimStep = false;
	closeConnection_ = false;
	my_nodeManagerPtr= node_manager;
	my_packetManagerPtr= packetManager;

	try
	{
		socket_ = new ServerSocket(port);
		socket_->accept();
	} catch(SocketException e) {
		cout << "ns-3 server --> #Error while creating socket: " << e.what() << endl;
	}
} 

void
Ns3Server::processCommandsUntilSimStep(int port, string logfile, iTETRISNodeManager *node_manager, PacketManager *packetManager) {

	if (logfile.empty()) {
		cout << "[ns3] WARNING - Log file name not specified!" << endl;
		logActive_ = false;
	}
	else {
		myfile.open(logfile.c_str());
		logActive_ = true;
	}

	try {
		if (instance_ == 0) {
			if (!closeConnection_) {
				instance_ = new ns3::Ns3Server(port, node_manager, packetManager);
			} else {
				return;
			}
		}

		// Simulation should run until
		// 1. end time reached or
		// 2. got CMD_CLOSE or
		// 3. Client closes socket connection
		if (instance_->myDoingSimStep) {
			instance_->myDoingSimStep = false;
		}

		while (!closeConnection_) {

			if (!instance_->myInputStorage.valid_pos())
			{
				if (instance_->myOutputStorage.size() > 0) {
					// send out all answers as one storage
					instance_->socket_->sendExact(instance_->myOutputStorage);
				}
				instance_->myInputStorage.reset();
				instance_->myOutputStorage.reset();
				// Read a message
				instance_->socket_->receiveExact(instance_->myInputStorage);
			}

			while (instance_->myInputStorage.valid_pos() && !closeConnection_) {
				// dispatch each command
				if (instance_->dispatchCommand() == CMD_SIMSTEP) {
					instance_->myDoingSimStep = true;
				}
			}
		}
		if (closeConnection_ && instance_->myOutputStorage.size() > 0) {
			// send out all answers as one storage
			instance_->socket_->sendExact(instance_->myOutputStorage);
		}
	} catch (std::invalid_argument e) {
		cout << "ns-3 server --> Invalid argument in ns3-server.cc, processCommandsUntilSimStep() " << e.what() << endl;

		stringstream log;
		log << "[processCommandsUntilSimStep] [ERROR] Invalid argument: " << e.what();
		Log((log.str()).c_str());


	} catch (SocketException e) {
		cout << "ns-3 server --> Socket exception in ns3-server.cc, processCommandsUntilSimStep() " << e.what() << endl;

		stringstream log;
		log << "[processCommandsUntilSimStep] [ERROR] Socket exception: " << e.what();
		Log((log.str()).c_str());

	}

	cout << "ns-3 server --> Finishing server.... " << endl;

	if (instance_ != NULL) {
		delete instance_;
		instance_ = 0;
		closeConnection_ = true;
	}

	if (logActive_)
		myfile.close();
}

int
Ns3Server::dispatchCommand()
{
	int commandStart = myInputStorage.position();
	int commandLength = myInputStorage.readInt();
	int commandId = myInputStorage.readUnsignedByte();

	bool success = false;

	stringstream log;

	// dispatch commands
	switch (commandId) {
	case CMD_SIMSTEP:
#ifdef _DEBUG
		log <<"ns-3 server --> CMD_SIMSTEP received" << endl; ;
		Log((log.str()).c_str());

#endif
		success = RunSimStep (myInputStorage.readInt());
		return commandId;
	case CMD_UPDATENODE:
#ifdef _DEBUG
		log << "ns-3 server --> CMD_UPDATENODE received" << endl;
		Log((log.str()).c_str());
#endif
		success = UpdateNodePosition();
		break;
	case CMD_UPDATENODE2:
#ifdef _DEBUG
		log << "ns-3 server --> CMD_UPDATENODE2 received" << endl;
		Log((log.str()).c_str());
#endif
		success = UpdateNodePosition2 ();
		break;
	case CMD_CREATENODE:
#ifdef _DEBUG
		log<< "ns-3 server --> CMD_CREATENODE received" << endl;
		Log((log.str()).c_str());
#endif
		success = CreateNode();
		break;
	case CMD_CREATENODE2:
#ifdef _DEBUG
		log<< "ns-3 server --> CMD_CREATENODE2 received" << endl;
		Log((log.str()).c_str());
#endif
		success = CreateNode2 ();
		break;
	case CMD_START_CAM:
#ifdef _DEBUG
		log<< "ns-3 server --> CMD_START_CAM received" << endl;
		Log((log.str()).c_str());
#endif
		success = StartSendingCam();
		break;
	case CMD_STOP_CAM:
#ifdef _DEBUG
		log<< "ns-3 server --> CMD_STOP_CAM received" << endl;
		Log((log.str()).c_str());
#endif
		success = StopSendingCam();
		break;
	case CMD_GET_RECEIVED_MESSAGES:
#ifdef _DEBUG
		log<< "ns-3 server --> CMD_GET_RECEIVED_MESSAGES received" << endl;
		Log((log.str()).c_str());
#endif
		success = GetReceivedMessages();
		break;
	case CMD_CLOSE:
#ifdef _DEBUG
		log<< "ns-3 server --> CMD_CLOSE received" << endl;
		Log((log.str()).c_str());
#endif
		success = Close();
		break;
	case CMD_START_TOPO_TXON:
#ifdef _DEBUG
		log<< "ns-3 server --> CMD_START_TOPO_TXON received" << endl;
		Log((log.str()).c_str());
#endif
		success = StartTopoTxon();
		break;
	case CMD_START_ID_BASED_TXON:
#ifdef _DEBUG
		log<< "ns-3 server --> CMD_START_ID_BASED_TXON received" << endl;
		Log((log.str()).c_str());
#endif
		success = StartIdBasedTxon();
		break;
	case CMD_START_MW_TXON:
#ifdef _DEBUG
		log<< "ns-3 server --> CMD_START_MW_TXON received" << endl;
		Log((log.str()).c_str());
#endif
		success = StartMWTxon();
		break;
	case CMD_START_IPCIU_TXON:
#ifdef _DEBUG
		log<< "ns-3 server --> CMD_START_IPCIU_TXON received" << endl;
		Log((log.str()).c_str());
#endif
		success = StartIpCiuTxon();
		break;
	case CMD_STOP_SERVICE_TXON:
#ifdef _DEBUG
		log<< "ns-3 server --> CMD_STOP_SERVICE_TXON received" << endl;
		Log((log.str()).c_str());
#endif
		success = StopServiceTxon();
		break;
	case CMD_STOP_MW_SERVICE_TXON:
#ifdef _DEBUG
		log<< "ns-3 server --> CMD_STOP_MW_SERVICE_TXON received" << endl;
		Log((log.str()).c_str());
#endif
		success = StopMWServiceTxon();
		break;
	case CMD_STOP_IPCIU_SERVICE_TXON:
#ifdef _DEBUG
		log<< "ns-3 server --> CMD_STOP_IPCIU_SERVICE_TXON received" << endl;
		Log((log.str()).c_str());
#endif
		success = StopIpCiuServiceTxon();
		break;
	case CMD_START_GEO_BROAD_TXON:
#ifdef _DEBUG
		log<< "ns-3 server --> CMD_START_GEO_BROAD_TXON received" << endl;
		Log((log.str()).c_str());
#endif
		success = StartGeobroadcastTxon ();
		break;
	case CMD_START_GEO_ANY_TXON:
#ifdef _DEBUG
		log<< "ns-3 server --> CMD_START_GEO_ANY_TXON received" << endl;
		Log((log.str()).c_str());
#endif
		success = StartGeoanycastTxon ();
		break;
	case CMD_ACTIVATE_NODE:
#ifdef _DEBUG
		log<< "ns-3 server --> CMD_ACTIVATE_NODE received" << endl;
		Log((log.str()).c_str());
#endif
		success = ActivateNode ();
		break;
	case CMD_DEACTIVATE_NODE:
#ifdef _DEBUG
		log<< "ns-3 server --> CMD_DEACTIVATE_NODE received" << endl;
		Log((log.str()).c_str());
#endif
		success = DeactivateNode ();
		break;
	default:
		writeStatusCmd(commandId, RTYPE_NOTIMPLEMENTED, "Command not implemented in ns3");
	}

	if (!success) {
		while (myInputStorage.valid_pos() && myInputStorage.position() < (unsigned int)(commandStart + commandLength)) {
			myInputStorage.readChar();
		}
	}

	if (myInputStorage.position() != (unsigned int)(commandStart + commandLength)) {
		ostringstream msg;
		msg << "Wrong position in requestMessage after dispatching command.";
		msg << " Expected command length was " << commandLength;
		msg << " but " << myInputStorage.position() - commandStart << " Bytes were read.";
		writeStatusCmd(commandId, RTYPE_ERR, msg.str());
		closeConnection_ = true;
	}

	return commandId;
}

void
Ns3Server::writeStatusCmd(int commandId, int status, std::string description) {
	if (status == RTYPE_ERR) {
		cout << "ns-3 server --> Answered with error to command " << commandId << ": " << description;
	} else if (status == RTYPE_NOTIMPLEMENTED) {
		cout << "ns-3 server --> Requested command not implemented (" << commandId << "): " << description;
	}

        cout << "ns-3 server --> Requested command (" << commandId << "): " << description;
	myOutputStorage.writeUnsignedByte(1 + 1 + 1 + 4 + static_cast<int>(description.length()));

	// command type
	myOutputStorage.writeUnsignedByte(commandId);
	// status
	myOutputStorage.writeUnsignedByte(status);
	// description
	myOutputStorage.writeString(description);

	return;
}

bool
Ns3Server::RunSimStep(int time)
{

	targetTime_++;

#ifdef _DEBUG
	stringstream log;
	log << "**********************************************************************************";
	Log((log.str()).c_str());
	log.str("");
	log << "[RunSimStep] Run events in ns-3 to target time " << targetTime_;
	Log((log.str()).c_str());
#endif

	if ( !(Simulator::IsFinished()) )  // IsFinished == true if no event to be scheduled anymore || Simulator::Stop reached
	{
		int eventCounter=0;
		while ( !(Simulator::IsFinished()) && (Seconds(time) > Simulator::Next ()) )
		{
			Simulator::RunOneEvent ();
			eventCounter++;
		}

		writeStatusCmd(CMD_SIMSTEP, RTYPE_OK, "RunSimStep()");
		return true;
	}
	else
	{
		writeStatusCmd(CMD_SIMSTEP, RTYPE_OK, "RunSimStep(), simulation finishes");
		return false;
	}
}

bool
Ns3Server::CreateNode (void)
{
	bool success = false;
	float x = myInputStorage.readFloat();
	float y = myInputStorage.readFloat();
	Vector pos = Vector(x,y,0);
	vector<string> listOfCommModules = myInputStorage.readStringList();
	vector<string>::iterator moduleIt;

	int32_t nodeId=my_nodeManagerPtr->CreateItetrisNode (pos);

	stringstream log;

	for (moduleIt = listOfCommModules.begin() ; moduleIt < listOfCommModules.end() ; moduleIt++)
	{
		string cadena = *moduleIt;

#ifdef _DEBUG
		log<< "ns-3 server --> Nodes IDs: " << cadena << endl;
		Log((log.str()).c_str());
#endif

		my_nodeManagerPtr->InstallCommunicationModule (*moduleIt);
		success = true;
	}

#ifdef _DEBUG
	log<< "ns-3 server --> Node with ID "<<nodeId<<" created successfully in Position "<<x<<" "<<y<< endl;
	Log((log.str()).c_str());
#endif

	writeStatusCmd(CMD_CREATENODE, RTYPE_OK, "CreateNode()");

	myOutputStorage.writeUnsignedByte(1 + 1 + 4);
	myOutputStorage.writeUnsignedByte(CMD_CREATENODE);
	myOutputStorage.writeInt(nodeId);

	return success;

}

bool
Ns3Server::CreateNode2 (void)
{
	bool success = false;
	float x = myInputStorage.readFloat();
	float y = myInputStorage.readFloat();
	float speed = myInputStorage.readFloat();
	float heading = myInputStorage.readFloat();
	string laneId = myInputStorage.readString ();
	Vector pos = Vector(x,y,0);
	vector<string> listOfCommModules = myInputStorage.readStringList();
	vector<string>::iterator moduleIt;

	int32_t nodeId=my_nodeManagerPtr->CreateItetrisNode (pos, speed, heading, laneId);

#ifdef _DEBUG
	stringstream log;
#endif

	for (moduleIt = listOfCommModules.begin() ; moduleIt < listOfCommModules.end() ; moduleIt++)
	{
		string cadena = *moduleIt;

#ifdef _DEBUG
		log<< "ns-3 server --> Nodes IDs: " << cadena << endl;
		Log((log.str()).c_str());
#endif

		my_nodeManagerPtr->InstallCommunicationModule (*moduleIt);
		success = true;
	}

#ifdef _DEBUG
	log<< "ns-3 server --> Node with ID "<< nodeId<<" created successfully in Position "<<x<<" "<<y<< endl;
	Log((log.str()).c_str());
#endif

	writeStatusCmd(CMD_CREATENODE2, RTYPE_OK, "CreateNode2()");

	myOutputStorage.writeUnsignedByte(1 + 1 + 4);
	myOutputStorage.writeUnsignedByte(CMD_CREATENODE2);
	myOutputStorage.writeInt(nodeId);

	return success;
}

bool
Ns3Server::UpdateNodePosition()
{ 
	uint32_t nodeId = myInputStorage.readInt();
	float x = myInputStorage.readFloat();
	float y = myInputStorage.readFloat();

	Vector pos = Vector(x,y,0);

	my_nodeManagerPtr->UpdateNodePosition(nodeId, pos);  // call the iTETRISNodeManager function to update the node's position

#ifdef _DEBUG
	stringstream log;
	log<< "ns-3 server --> Node with ID "<<nodeId<<" has updated its position x="<<x<<" y="<<y<< endl;
	Log((log.str()).c_str());
#endif

	writeStatusCmd(CMD_UPDATENODE, RTYPE_OK, "UpdateNodePosition()");

	return true;
}

bool
Ns3Server::UpdateNodePosition2 ()
{ 
	uint32_t nodeId = myInputStorage.readInt();
	float x = myInputStorage.readFloat();
	float y = myInputStorage.readFloat();
	float speed = myInputStorage.readFloat();
	float heading = myInputStorage.readFloat();
	string laneId = myInputStorage.readString ();
	Vector pos = Vector(x,y,0);

	my_nodeManagerPtr->UpdateNodePosition(nodeId, pos, speed, heading, laneId);  // call the iTETRISNodeManager function to update the node position

#ifdef _DEBUG
	stringstream log;
	log<< "ns-3 server --> Node with ID "<<nodeId<<" has updated its position x="<<x<<" y="<<y<< endl;
	Log((log.str()).c_str());
#endif

	writeStatusCmd(CMD_UPDATENODE2, RTYPE_OK, "UpdateNodePosition2()");

	return true;
}

bool 
Ns3Server::ActivateNode (void)
{
	vector<string> idCollection = myInputStorage.readStringList();
	vector<string>::iterator nodeIt;
	for (nodeIt = idCollection.begin() ; nodeIt < idCollection.end() ; nodeIt++)
	{
		string cadena = *nodeIt;
		stringstream temp;
		uint32_t nodeId;
		temp << cadena;
		temp >> nodeId;

#ifdef _DEBUG
		stringstream logNode;
		logNode << "[CMD_ACTIVATE_NODE] Activating node [ns3-ID] [" << nodeId << "]";
		Log((logNode.str()).c_str());
		logNode.str("");
#endif

		my_nodeManagerPtr->ActivateNode(nodeId);
	}
	writeStatusCmd(CMD_ACTIVATE_NODE, RTYPE_OK, "ActivateNode()");
	return true;
}

bool 
Ns3Server::DeactivateNode (void)
{
	vector<string> idCollection = myInputStorage.readStringList();
	vector<string>::iterator nodeIt;
	for (nodeIt = idCollection.begin() ; nodeIt < idCollection.end() ; nodeIt++)
	{
		string cadena = *nodeIt;
		stringstream temp;
		uint32_t nodeId;
		temp << cadena;
		temp >> nodeId;

#ifdef _DEBUG
		stringstream logNode;
		logNode << "[CMD_DEACTIVATE_NODE] Deactivating node [ns3-ID] [" << nodeId << "]";
		Log((logNode.str()).c_str());
		logNode.str("");
#endif

		my_nodeManagerPtr->DeactivateNode(nodeId);
	}
	writeStatusCmd(CMD_DEACTIVATE_NODE, RTYPE_OK, "DeactivateNode()");
	return true;
}

bool
Ns3Server::StartSendingCam()
{
	vector<string> senderIdCollection = myInputStorage.readStringList();
	int payloadLength = myInputStorage.readInt();
	float frequency = myInputStorage.readFloat();

	string ids;
	vector<string>::iterator senderIt;
	for (senderIt = senderIdCollection.begin() ; senderIt < senderIdCollection.end() ; senderIt++)
	{
		string cadena = *senderIt;

#ifdef _DEBUG
		stringstream log;
		log << "[StartSendingCam] Node Starts Sending CAM [ns3-ID|Frequency|Payload Length] [" ;
		log << cadena << "|" << frequency << "|" << payloadLength << "]";
		Log((log.str()).c_str());
#endif

		stringstream temp;
		uint32_t nodeId;
		temp << cadena;
		temp >> nodeId;
		my_packetManagerPtr->ActivateCamTxon (nodeId, frequency, payloadLength);
	}

	if (senderIdCollection.size() == 0) {
		stringstream log;
		log << "[StartSendingCam] There are not nodes to start sending CAM";
		Log((log.str()).c_str());
	}

	writeStatusCmd(CMD_START_CAM ,RTYPE_OK, "StartSendingCam()");

	return true;
}

bool
Ns3Server::StopSendingCam()
{
	vector<string> senderIdCollection = myInputStorage.readStringList();

	vector<string>::iterator senderIt;
	for (senderIt = senderIdCollection.begin() ; senderIt < senderIdCollection.end() ; senderIt++)
	{
		string cadena = *senderIt;

#ifdef _DEBUG
		stringstream log;
		log<< "ns-3 server --> Nodes IDs: " << cadena << endl;
		Log((log.str()).c_str());
#endif

		stringstream temp;
		uint32_t nodeId;
		temp << cadena;
		temp >> nodeId;
		stringstream logNode;

#ifdef _DEBUG
		logNode << "[STOPSENDINGCAM] Stopping CAM of Node [ns3-ID] [" << nodeId << "]";
		Log((logNode.str()).c_str());
		logNode.str("");
#endif

		my_packetManagerPtr->DeactivateCamTxon (nodeId);
	}

	writeStatusCmd(CMD_STOP_CAM, RTYPE_OK, "StopSendingCam()");

	return true;
}

bool
Ns3Server::GetReceivedMessages ()
{
	vector<string> sender;
	vector<string> type;
	vector<string> sentTimestep;
	vector<string> sequenceNumber;

	int nodeId = myInputStorage.readInt();

#ifdef _DEBUG
	stringstream logNode;
	logNode << "[GetReceivedMessages] Retrieve Messages of Node [ns3-ID] [" << nodeId << "]";
	Log((logNode.str()).c_str());
	logNode.str("");
#endif

	/**
	 * The struct InciPacket::ReceivedInciPacket contains the data fields of the CAM
	 *
	 *   uint64_t senderId
	 *   std::string msgType
	 *   uint32_t ts
	 *   uint32_t tsSeqNo
	 *
	 * The function PacketManager::GetReceivedPacket (uint32_t recvNodeId, struct InciPacket::ReceivedInciPacket &inciPacket)
	 * accepts as parameters the id of the node ('recvNodeId') from which we want to retrive the received packets and a reference
	 * struct (InciPacket::ReceivedInciPacket) with the data fields of the last CAM received. The function returns a bool
	 * indicating if more packets have been received by the node.
	 *
	 */

	struct InciPacket::ReceivedInciPacket inciPacket;

	while ( my_packetManagerPtr->GetReceivedPacket(nodeId,inciPacket) )
	{
		sender.push_back(Int2String(inciPacket.senderId));
		type.push_back(inciPacket.msgType);
		sentTimestep.push_back(Int2String(inciPacket.ts));
		sequenceNumber.push_back(Int2String(inciPacket.tsSeqNo));

#ifdef _DEBUG
		stringstream log;
		log << "[GetReceivedMessages] Message Info of node [ns3-ID|Sender ns3-ID|MessageType|Sent Timestep|SeqNum] ["
				<< nodeId << "|" << inciPacket.senderId << "|" << inciPacket.msgType << "|" << inciPacket.ts << "|" << inciPacket.tsSeqNo <<"]";
		Log((log.str()).c_str());
#endif
	}

	writeStatusCmd(CMD_GET_RECEIVED_MESSAGES, RTYPE_OK, "GetReceivedMessages()");

	myOutputStorage.writeInt(4 + 1 + CalculateStringListByteSize(sender) + CalculateStringListByteSize(type) +
			CalculateStringListByteSize(sentTimestep) + CalculateStringListByteSize(sequenceNumber));
	myOutputStorage.writeUnsignedByte(CMD_GET_RECEIVED_MESSAGES);
	myOutputStorage.writeStringList(sender);
	myOutputStorage.writeStringList(type);
	myOutputStorage.writeStringList(sentTimestep);
	myOutputStorage.writeStringList(sequenceNumber);

	return true;
}

std::string 
Ns3Server::Int2String (int n)
{
	std::stringstream res;
	res << n;
	return(res.str());
}

bool
Ns3Server::Close()
{
	closeConnection_ = true;
	writeStatusCmd(CMD_CLOSE, RTYPE_OK, "Close()");
	return true;
}

int 
Ns3Server::CalculateStringListByteSize(vector<string> list)
{

	if (list.size() == 0)
		return 0;

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
Ns3Server::Log(const char* message)
{
	if (logActive_) {

		time_t rawtime;
		struct tm * timeinfo;
		time(&rawtime);
		timeinfo = localtime(&rawtime);

		if (timeinfo == NULL) {
			cout << "[WARNING] Impossible to get time from system." << endl;
			return false;
		}

		// Removes \0 character
		stringstream auxi;
		auxi.exceptions(ifstream::eofbit | ifstream::failbit | ifstream::badbit);
		auxi << asctime(timeinfo);
		string mytime;
		try {
			getline(auxi, mytime);
		} catch (stringstream::failure e) {
			cout << "[WARNING] Impossible to format system time." << endl;
			return false;
		}


		myfile << "[" << mytime << "] " << "[" << targetTime_ << "] " << message << endl;

		return true;
	}
	return false;
}

bool 
Ns3Server::StartTopoTxon (void)
{
	vector<string> senderIdCollection = myInputStorage.readStringList ();
	string serviceId = myInputStorage.readString ();
	int commProfile = myInputStorage.readUnsignedByte (); // commProfile is coded as a byte by the iCS
	vector<string> technologies = myInputStorage.readStringList ();
	float frequency = myInputStorage.readFloat();
	int payloadLength = myInputStorage.readInt();
	float msgRegenerationTime = myInputStorage.readFloat();
	int msgLifetime = myInputStorage.readInt();
	int numHops = myInputStorage.readInt();

	vector<string>::iterator senderIt;
	for (senderIt = senderIdCollection.begin() ; senderIt < senderIdCollection.end() ; senderIt++)
	{
		string cadena = *senderIt;

#ifdef _DEBUG
		stringstream log;
		log << "[ns-3][Ns3Server::StartTopoTxon] Node starts topobroadcast transmission [ns3-ID|ServiceId|Frequency|PayloadLength|MsgRegenerationTime|MsgLifetime|NumHops] [" ;
		log << cadena << "|" << serviceId << "|" << frequency << "|" << payloadLength << "|" << msgRegenerationTime << "|" << msgLifetime << "|" << numHops << "]";
		Log((log.str()).c_str());
#endif

		stringstream temp;
		uint32_t nodeId;
		temp << cadena;
		temp >> nodeId;
		my_packetManagerPtr->ActivateTopoBroadcastTxon (nodeId, serviceId, commProfile, technologies, frequency, payloadLength, msgRegenerationTime, msgLifetime, numHops);
	}

	if (senderIdCollection.size() == 0) {
		stringstream log;
		log << "[ns-3][Ns3Server::StartTopoTxon] There are not nodes to start topobroadcast transmission";
		Log((log.str()).c_str());
	}

	writeStatusCmd(CMD_START_TOPO_TXON, RTYPE_OK, "StartTopoTxon ()");
	return true;
}

bool 
Ns3Server::StartIdBasedTxon (void)
{
	vector<string> senderIdCollection = myInputStorage.readStringList ();
	string serviceId = myInputStorage.readString ();
	int commProfile = myInputStorage.readUnsignedByte(); // commProfile is coded as a byte by the iCS
	vector<string> technologies = myInputStorage.readStringList ();
	float frequency = myInputStorage.readFloat();
	int payloadLength = myInputStorage.readInt();
	int destination = myInputStorage.readInt();
	float msgRegenerationTime = myInputStorage.readFloat();
	int msgLifetime = myInputStorage.readInt();

	vector<string>::iterator senderIt;
	for (senderIt = senderIdCollection.begin() ; senderIt < senderIdCollection.end() ; senderIt++)
	{
		string cadena = *senderIt;

#ifdef _DEBUG
		stringstream log;
		log << "[ns-3][Ns3Server::StartIdBasedTxon] Node starts ID-based transmission [ns3-ID|ServiceId|Frequency|PayloadLength|Destination|TechnologyList|MsgRegenerationTime|MsgLifetime] [" ;
		log << cadena << "|" << serviceId << "|" << frequency << "|" << payloadLength << "|" << destination << "|";
		for (vector<string>::iterator techsIt = technologies.begin () ; techsIt < technologies.end () ; techsIt++)
		{
			string techTmp = *techsIt;
			if (techsIt != technologies.begin ())
			{
				log << "-";
			}
			log << techTmp;
		}
		log << "|" << msgRegenerationTime << "|" << msgLifetime << "]";
		Log((log.str()).c_str());
#endif

		stringstream temp;
		uint32_t nodeId;
		temp << cadena;
		temp >> nodeId;
		my_packetManagerPtr->InitiateIdBasedTxon (nodeId, serviceId, commProfile, technologies, frequency, payloadLength, destination, msgRegenerationTime, msgLifetime);
                stringstream log1;
		log1 << "[ns-3][Ns3Server::StartIdBasedTxon] Finished IdBasedTxon";
		Log((log1.str()).c_str());
	}

	if (senderIdCollection.size() == 0) {
		stringstream log;
		log << "[ns-3][Ns3Server::StartIdBasedTxon] There are not nodes to start ID-based transmission";
		Log((log.str()).c_str());
	}

	writeStatusCmd(CMD_START_ID_BASED_TXON ,RTYPE_OK, "StartIdBasedTxon ()");
	return true;
}

bool 
Ns3Server::StartMWTxon (void)
{
	vector<string> senderIdCollection = myInputStorage.readStringList ();
	string serviceId = myInputStorage.readString ();
	int commProfile = myInputStorage.readUnsignedByte (); // commProfile is coded as a byte by the iCS
	vector<string> technologies = myInputStorage.readStringList ();
	CircularGeoAddress destination;
	destination.lat = myInputStorage.readInt();
	destination.lon = myInputStorage.readInt();
	destination.areaSize = myInputStorage.readInt();
	float frequency = myInputStorage.readFloat();
	int payloadLength = myInputStorage.readInt();
	float msgRegenerationTime = myInputStorage.readFloat();
	int msgLifetime = myInputStorage.readInt();

	vector<string>::iterator senderIt;
	for (senderIt = senderIdCollection.begin() ; senderIt < senderIdCollection.end() ; senderIt++)
	{
		string cadena = *senderIt;
		// TODO add a check on the node ID: It must be a TMC...need a method: isTMC(cadena)

#ifdef _DEBUG
		stringstream log;
		log << "[ns-3][Ns3Server::StartMWTxon] Node (TMC) starts MW transmission [ns3-ID|ServiceId|TechnologyList|CenterPointLat|CenterPointLon|AreaSize|Frequency|PayloadLength|MsgRegenerationTime|MsgLifetime] [" ;
		log << cadena << "|" << serviceId  << "|";
		for (vector<string>::iterator techsIt = technologies.begin () ; techsIt < technologies.end () ; techsIt++)
		{
			string techTmp = *techsIt;
			if (techsIt != technologies.begin ())
			{
				log << "-";
			}
			log << techTmp;
		}
		log << "|" << destination.lat << "|" << destination.lon << "|" << destination.areaSize << "|" << frequency << "|" << payloadLength << "|" << msgRegenerationTime << "|" << msgLifetime << "]";
		Log((log.str()).c_str());
#endif

		stringstream temp;
		uint32_t nodeId;
		temp << cadena;
		temp >> nodeId;
		my_packetManagerPtr->InitiateMWTxon (nodeId, serviceId, commProfile, technologies, destination, frequency, payloadLength, msgRegenerationTime, msgLifetime);
	}

	if (senderIdCollection.size() == 0) {

		stringstream log;
		log << "[ns-3][Ns3Server::StartMWTxon] There are not TMC node to start MW transmission";
		Log((log.str()).c_str());

		writeStatusCmd(CMD_START_ID_BASED_TXON ,RTYPE_ERR, "StartMWTxon ()");
	}

	writeStatusCmd(CMD_START_ID_BASED_TXON ,RTYPE_OK, "StartMWTxon ()");
	return true;
}

bool
Ns3Server::StartIpCiuTxon (void)
{
	vector<string> senderIdCollection = myInputStorage.readStringList ();
	string serviceId = myInputStorage.readString ();
	float frequency = myInputStorage.readFloat();
	int payloadLength = myInputStorage.readInt();
	int destination = myInputStorage.readInt();
	float msgRegenerationTime = myInputStorage.readFloat();

	vector<string>::iterator senderIt;
	for (senderIt = senderIdCollection.begin() ; senderIt < senderIdCollection.end() ; senderIt++)
	{
		string cadena = *senderIt;

#ifdef _DEBUG
		stringstream log;
		log << "[ns-3][Ns3Server::StartIpCiuTxon] CIU node starts IP-based transmission [ns3-ID|ServiceId|Frequency|PayloadLength|Destination|MsgRegenerationTime] [" ;
		log << cadena << "|" << serviceId << "|" << frequency << "|" << payloadLength << "|" << destination << "|" << msgRegenerationTime << "]";
		Log((log.str()).c_str());
#endif

		stringstream temp;
		uint32_t nodeId;
		temp << cadena;
		temp >> nodeId;
		my_packetManagerPtr->InitiateIPCIUTxon (nodeId, serviceId, frequency, payloadLength, destination, msgRegenerationTime);

	}

	if (senderIdCollection.size() == 0) {
		stringstream log;
		log << "[ns-3][Ns3Server::StartIpCiuTxon] There are not CIU nodes to start IP-based transmission";
		Log((log.str()).c_str());
	}

	writeStatusCmd(CMD_START_IPCIU_TXON ,RTYPE_OK, "StartIpCiuTxon ()");
	return true;
}

bool 
Ns3Server::StopServiceTxon (void)
{
	vector<string> senderIdCollection = myInputStorage.readStringList();
	vector<string>::iterator senderIt;
	string serviceId = myInputStorage.readString ();
	for (senderIt = senderIdCollection.begin() ; senderIt < senderIdCollection.end() ; senderIt++)
	{
		string cadena = *senderIt;

#ifdef _DEBUG
		stringstream log;
		log << "[ns-3][Ns3Server::StopServiceTxon] Nodes IDs: " << cadena << endl;
		Log((log.str()).c_str());
#endif

		stringstream temp;
		uint32_t nodeId;
		temp << cadena;
		temp >> nodeId;
		my_packetManagerPtr->DeactivateServiceTxon (nodeId, serviceId);
	}
	writeStatusCmd(CMD_STOP_SERVICE_TXON, RTYPE_OK, "StopServiceTxon()");
	return true; ;
}

bool 
Ns3Server::StopMWServiceTxon (void)
{
	vector<string> senderIdCollection = myInputStorage.readStringList();
	vector<string>::iterator senderIt;
	string serviceId = myInputStorage.readString ();
	for (senderIt = senderIdCollection.begin() ; senderIt < senderIdCollection.end() ; senderIt++)
	{
		string cadena = *senderIt;
		// TODO add a check that nodeID (cadena) is a TMC - method isTMC(cadena) required

#ifdef _DEBUG
		stringstream log;
		log << "[ns-3][Ns3Server::StopMWServiceTxon] Nodes IDs: " << cadena << endl;
		Log((log.str()).c_str());
#endif

		stringstream temp;
		uint32_t nodeId;
		temp << cadena;
		temp >> nodeId;
		my_packetManagerPtr->DeactivateMWServiceTxon (nodeId, serviceId);
	}

	if (senderIdCollection.size() == 0) {

		stringstream log;
		log << "[ns-3][Ns3Server::StopMWServiceTxon] There are not TMC node to stop MW Services";
		Log((log.str()).c_str());

		writeStatusCmd(CMD_STOP_MW_SERVICE_TXON, RTYPE_ERR, "StopMWServiceTxon()");
	}

	writeStatusCmd(CMD_STOP_MW_SERVICE_TXON, RTYPE_OK, "StopMWServiceTxon()");
	return true; ;
}

bool
Ns3Server::StopIpCiuServiceTxon (void)
{
	vector<string> senderIdCollection = myInputStorage.readStringList();
	vector<string>::iterator senderIt;
	string serviceId = myInputStorage.readString ();
	for (senderIt = senderIdCollection.begin() ; senderIt < senderIdCollection.end() ; senderIt++)
	{
		string cadena = *senderIt;

#ifdef _DEBUG
		stringstream log;
		log<< "[ns-3][Ns3Server::StopIpCiuServiceTxon] Nodes IDs: " << cadena << endl;
		Log((log.str()).c_str());
#endif

		stringstream temp;
		uint32_t nodeId;
		temp << cadena;
		temp >> nodeId;
		my_packetManagerPtr->DeactivateIPCIUServiceTxon (nodeId, serviceId);
	}
	writeStatusCmd(CMD_STOP_IPCIU_SERVICE_TXON, RTYPE_OK, "StopIpCiuServiceTxon()");
	return true; ;
}

bool 
Ns3Server::StartGeobroadcastTxon (void)
{
	vector<string> senderIdCollection = myInputStorage.readStringList ();
	string serviceId = myInputStorage.readString ();
	int commProfile = myInputStorage.readUnsignedByte (); // commProfile is coded as a byte by the iCS
	vector<string> technologies = myInputStorage.readStringList ();
	CircularGeoAddress destination;
	destination.lat = myInputStorage.readInt();
	destination.lon = myInputStorage.readInt();
	destination.areaSize = myInputStorage.readInt();
	float frequency = myInputStorage.readFloat();
	int payloadLength = myInputStorage.readInt();
	float msgRegenerationTime = myInputStorage.readFloat();
	int msgLifetime = myInputStorage.readInt();

	vector<string>::iterator senderIt;
	for (senderIt = senderIdCollection.begin() ; senderIt < senderIdCollection.end() ; senderIt++)
	{
		string cadena = *senderIt;

#ifdef _DEBUG
		stringstream log;
		log << "[ns-3][Ns3Server::StartGeobroadcastTxon] Node starts geobroadcast transmission "
				"[ns3-ID|ServiceId|CenterPointLat|CenterPointLon|AreaSize|Frequency|PayloadLength|MsgRegenerationTime|MsgLifetime] ["
				<< cadena << "|" << serviceId << "|" << destination.lat << "|" << destination.lat << "|" << destination.lon
				<< "|" << destination.areaSize << "|" << frequency << "|" << payloadLength << "|" << msgRegenerationTime
				<< "|" << msgLifetime << "]";
		Log((log.str()).c_str());
#endif

		stringstream temp;
		uint32_t nodeId;
		temp << cadena;
		temp >> nodeId;
		my_packetManagerPtr->InitiateGeoBroadcastTxon (nodeId, serviceId, commProfile, technologies, destination, frequency, payloadLength, msgRegenerationTime, msgLifetime);
	}

	if (senderIdCollection.size() == 0) {
		stringstream log;
		log << "[ns-3][Ns3Server::StartGeobroadcastTxon] There are not nodes to start geobroadcast transmission";
		Log((log.str()).c_str());
	}

	writeStatusCmd(CMD_START_GEO_BROAD_TXON ,RTYPE_OK, "StartGeobroadcastTxon ()");
	return true;
}

bool 
Ns3Server::StartGeoanycastTxon (void)
{
	vector<string> senderIdCollection = myInputStorage.readStringList ();
	string serviceId = myInputStorage.readString ();
	int commProfile = myInputStorage.readUnsignedByte(); // commProfile is coded as a byte by the iCS
	vector<string> technologies = myInputStorage.readStringList ();
	CircularGeoAddress destination;
	destination.lat = myInputStorage.readInt();
	destination.lon = myInputStorage.readInt();
	destination.areaSize = myInputStorage.readInt();
	float frequency = myInputStorage.readFloat();
	int payloadLength = myInputStorage.readInt();
	float msgRegenerationTime = myInputStorage.readFloat();
	int msgLifetime = myInputStorage.readInt();

	vector<string>::iterator senderIt;
	for (senderIt = senderIdCollection.begin() ; senderIt < senderIdCollection.end() ; senderIt++)
	{
		string cadena = *senderIt;

#ifdef _DEBUG
		stringstream log;
		log << "[ns-3][Ns3Server::StartGeoanycastTxon] Node starts geoanycast transmission [ns3-ID|ServiceId|CenterPointLat|CenterPointLon|AreaSize|Frequency|PayloadLength|MsgRegenerationTime|MsgLifetime] [" ;
		log << cadena << "|" << serviceId << "|" << destination.lat << "|" << destination.lat << "|" << destination.lon << "|" << destination.areaSize << "|" << frequency << "|" << payloadLength << "|" << msgRegenerationTime << "|" << msgLifetime << "]";
		Log((log.str()).c_str());
#endif

		stringstream temp;
		uint32_t nodeId;
		temp << cadena;
		temp >> nodeId;
		my_packetManagerPtr->InitiateGeoAnycastTxon (nodeId, serviceId, commProfile, technologies, destination, frequency, payloadLength, msgRegenerationTime, msgLifetime);
	}

	if (senderIdCollection.size() == 0) {
		stringstream log;
		log << "[ns-3][Ns3Server::StartGeoanycastTxon] There are not nodes to start geoanycast transmission";
		Log((log.str()).c_str());
	}

	writeStatusCmd(CMD_START_GEO_ANY_TXON ,RTYPE_OK, "StartGeoanycastTxon ()");
	return true;
}

}
