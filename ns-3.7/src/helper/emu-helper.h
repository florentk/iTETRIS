/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2008 University of Washington
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
 */

#ifndef EMU_HELPER_H
#define EMU_HELPER_H

#include <string>
#include <ostream>
#include "ns3/attribute.h"
#include "ns3/object-factory.h"
#include "ns3/net-device-container.h"
#include "ns3/node-container.h"
#include "ns3/emu-net-device.h"

namespace ns3 {

class Packet;
class PcapWriter;
class AsciiWriter;

/**
 * \brief build a set of EmuNetDevice objects
 */
class EmuHelper
{
public:
  /*
   * Construct an EmuHelper() which is used to make installing and configuring 
   * Emulated Net Devices easier.
   */
  EmuHelper ();

  /**
   * \param type the type of queue
   * \param n1 the name of the attribute to set on the queue
   * \param v1 the value of the attribute to set on the queue
   * \param n2 the name of the attribute to set on the queue
   * \param v2 the value of the attribute to set on the queue
   * \param n3 the name of the attribute to set on the queue
   * \param v3 the value of the attribute to set on the queue
   * \param n4 the name of the attribute to set on the queue
   * \param v4 the value of the attribute to set on the queue
   *
   * Set the type of queue to create and associated to each
   * EmuNetDevice created through EmuHelper::Install.
   */
  void SetQueue (std::string type,
    std::string n1 = "", const AttributeValue &v1 = EmptyAttributeValue (),
    std::string n2 = "", const AttributeValue &v2 = EmptyAttributeValue (),
    std::string n3 = "", const AttributeValue &v3 = EmptyAttributeValue (),
    std::string n4 = "", const AttributeValue &v4 = EmptyAttributeValue ());

  /**
   * \param n1 the name of the attribute to set
   * \param v1 the value of the attribute to set
   *
   * Set these attributes on each ns3::EmuNetDevice created
   * by EmuHelper::Install
   */
  void SetAttribute (std::string n1, const AttributeValue &v1);

  /**
   * \param filename filename prefix to use for pcap files.
   * \param nodeid the id of the node to generate pcap output for.
   * \param deviceid the id of the device to generate pcap output for.
   * \param promiscuous If true capture all possible packets available at the device.
   *
   * Generate a pcap file which contains the link-level data observed
   * by the specified deviceid within the specified nodeid. The pcap
   * data is stored in the file prefix-nodeid-deviceid.pcap.
   *
   * This method should be invoked after the network topology has 
   * been fully constructed.
   */
  static void EnablePcap (std::string filename, uint32_t nodeid, uint32_t deviceid, bool promiscuous);

  /**
   * \param filename filename prefix to use for pcap files.
   * \param nd Indicates net device on which you want to enable tracing.
   * \param promiscuous If true capture all possible packets available at the device.
   *
   * Enable pcap output on each input device which is of the
   * ns3::EmuNetDevice type.
   */
  static void EnablePcap (std::string filename, Ptr<NetDevice> nd, bool promiscuous);

  /**
   * \param filename filename prefix to use for pcap files.
   * \param ndName Name of net device on which you want to enable tracing.
   * \param promiscuous If true capture all possible packets available at the device.
   *
   * Enable pcap output on each input device which is of the
   * ns3::EmuNetDevice type.
   */
  static void EnablePcap (std::string filename, std::string ndName, bool promiscuous);

  /**
   * \param filename filename prefix to use for pcap files.
   * \param d container of devices of type ns3::EmuNetDevice
   * \param promiscuous If true capture all possible packets available at the device.
   *
   * Enable pcap output on each input device which is of the
   * ns3::EmuNetDevice type.
   */
  static void EnablePcap (std::string filename, NetDeviceContainer d, bool promiscuous);

  /**
   * \param filename filename prefix to use for pcap files.
   * \param n container of nodes.
   * \param promiscuous If true capture all possible packets available at the device.
   *
   * Enable pcap output on each device which is of the
   * ns3::EmuNetDevice type and which is located in one of the 
   * input nodes.
   */
  static void EnablePcap (std::string filename, NodeContainer n, bool promiscuous);

  /**
   * \param filename filename prefix to use for pcap files.
   * \param promiscuous If true capture all possible packets available at the device.
   *
   * Enable pcap output on each device which is of the
   * ns3::EmuNetDevice type
   */
  static void EnablePcapAll (std::string filename, bool promiscuous);

  /**
   * \param os output stream
   * \param nodeid the id of the node to generate ascii output for.
   * \param deviceid the id of the device to generate ascii output for.
   *
   * Enable ascii output on the specified deviceid within the
   * specified nodeid if it is of type ns3::EmuNetDevice and dump 
   * that to the specified stdc++ output stream.
   */
  static void EnableAscii (std::ostream &os, uint32_t nodeid, uint32_t deviceid);

  /**
   * \param os output stream
   * \param d device container
   *
   * Enable ascii output on each device which is of the
   * ns3::EmuNetDevice type and which is located in the input
   * device container and dump that to the specified
   * stdc++ output stream.
   */
  static void EnableAscii (std::ostream &os, NetDeviceContainer d);

  /**
   * \param os output stream
   * \param n node container
   *
   * Enable ascii output on each device which is of the
   * ns3::EmuNetDevice type and which is located in one
   * of the input node and dump that to the specified
   * stdc++ output stream.
   */
  static void EnableAscii (std::ostream &os, NodeContainer n);

  /**
   * \param os output stream
   *
   * Enable ascii output on each device which is of the
   * ns3::EmuNetDevice type and dump that to the specified
   * stdc++ output stream.
   */
  static void EnableAsciiAll (std::ostream &os);

  /**
   * This method creates an ns3::EmuNetDevice with the attributes configured by 
   * EmuHelper::SetDeviceAttribute and then adds the device to the node.
   *
   * \param node The node to install the device in
   * \returns A containter holding the added net device.
   */
  NetDeviceContainer Install (Ptr<Node> node) const;

  /**
   * This method creates an ns3::EmuNetDevice with the attributes configured by 
   * EmuHelper::SetDeviceAttribute and then adds the device to the node.
   *
   * \param nodeName The name of the node to install the device in
   * \returns A containter holding the added net device.
   */
  NetDeviceContainer Install (std::string nodeName) const;

  /**
   * For each Ptr<node> in the provided container this method creates an 
   * ns3::EmuNetDevice (with the attributes configured by 
   * EmuHelper::SetDeviceAttribute); adds the device to the node.
   *
   * \param c The NodeContainer holding the nodes to be changed.
   * \returns A containter holding the added net devices.
   */
  NetDeviceContainer Install (const NodeContainer &c) const;

private:
  /*
   * \internal
   */
  Ptr<NetDevice> InstallPriv (Ptr<Node> node) const;

  /*
   * \internal
   */
  static void SniffEvent (Ptr<PcapWriter> writer, Ptr<const Packet> packet);

  /*
   * \internal
   */
  static void AsciiRxEvent (Ptr<AsciiWriter> writer, std::string path, Ptr<const Packet> packet);

  /*
   * \internal
   */
  static void AsciiEnqueueEvent (Ptr<AsciiWriter> writer, std::string path, Ptr<const Packet> packet);

  /*
   * \internal
   */
  static void AsciiDequeueEvent (Ptr<AsciiWriter> writer, std::string path, Ptr<const Packet> packet);

  /*
   * \internal
   */
  static void AsciiDropEvent (Ptr<AsciiWriter> writer, std::string path, Ptr<const Packet> packet);

  ObjectFactory m_queueFactory;
  ObjectFactory m_deviceFactory;
};


} // namespace ns3

#endif /* EMU_HELPER_H */
