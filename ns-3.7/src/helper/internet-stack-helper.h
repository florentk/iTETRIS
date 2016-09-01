/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2008 INRIA
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
 * Author: Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
 */

#ifndef INTERNET_STACK_HELPER_H
#define INTERNET_STACK_HELPER_H

#include "node-container.h"
#include "net-device-container.h"
#include "ns3/packet.h"
#include "ns3/ptr.h"
#include "ns3/object-factory.h"
#include "ns3/pcap-writer.h"
#include "ns3/ascii-writer.h"
#include "ns3/ipv4-l3-protocol.h"
#include "ns3/ipv6-l3-protocol.h"
#include "ns3/c2c-l3-protocol.h"                 //iTETRIS
#include "ns3/c2c-common-header.h"

namespace ns3 {

class Node;
class Ipv4RoutingHelper;
class Ipv6RoutingHelper;
class c2cRoutingHelper;                      //iTETRIS

/**
 * \brief iTETRIS [WP600] :[modified] - Modified to include c2c/c2cTransport functionality.
 * \brief by Vineet Kumar <Vineet.Kumar@hitachi-eu.com>
 *
 * aggregate IP/TCP/UDP functionality to existing Nodes.
 *
 */
class InternetStackHelper
{
public:
  /**
   * Create a new InternetStackHelper which uses a mix of static routing
   * and global routing by default. The static routing protocol 
   * (ns3::Ipv4StaticRouting) and the global routing protocol are
   * stored in an ns3::Ipv4ListRouting protocol with priorities 0, and -10
   * by default. If you wish to use different priorites and different
   * routing protocols, you need to use an adhoc ns3::Ipv4RoutingHelper, 
   * such as ns3::OlsrHelper
   */
  InternetStackHelper(void);

  /**
   * Destroy the InternetStackHelper
   */
  virtual ~InternetStackHelper(void);
  InternetStackHelper (const InternetStackHelper &);
  InternetStackHelper &operator = (const InternetStackHelper &o);

  /**
   * Return helper internal state to that of a newly constructed one
   */
  void Reset (void);

  /**
   * \param routing a new routing helper
   *
   * Set the routing helper to use during Install. The routing
   * helper is really an object factory which is used to create 
   * an object of type ns3::Ipv4RoutingProtocol per node. This routing
   * object is then associated to a single ns3::Ipv4 object through its 
   * ns3::Ipv4::SetRoutingProtocol.
   */
  void SetRoutingHelper (const Ipv4RoutingHelper &routing);
  
  /**
   * \brief Set IPv6 routing helper.
   * \param routing IPv6 routing helper
   */
  void SetRoutingHelper (const Ipv6RoutingHelper &routing);

  /**
   * \param routing a new routing helper
   *
   * Set the routing helper to use during Install. The routing
   * helper is really an object factory which is used to create 
   * an object of type ns3::c2cRoutingHelper per node. This routing
   * object is then associated to a single ns3::c2c object through its 
   * ns3::c2c::SetRoutingProtocol.
   */
  void SetRoutingHelper (const c2cRoutingHelper &routing);

  /**
   * Aggregate implementations of the ns3::Ipv4, ns3::Ipv6, ns3::Udp, and ns3::Tcp classes
   * onto the provided node.  This method will assert if called on a node that 
   * already has an Ipv4 object aggregated to it.
   * 
   * \param nodeName The name of the node on which to install the stack.
   */
  void Install (std::string nodeName) const;

  /**
   * Aggregate implementations of the ns3::Ipv4, ns3::Ipv6, ns3::Udp, and ns3::Tcp classes
   * onto the provided node.  This method will assert if called on a node that 
   * already has an Ipv4 object aggregated to it.
   * 
   * \param node The node on which to install the stack.
   */
  void Install (Ptr<Node> node) const;

  /**
   * For each node in the input container, aggregate implementations of the 
   * ns3::Ipv4, ns3::Ipv6, ns3::Udp, and, ns3::Tcp classes.  The program will assert 
   * if this method is called on a container with a node that already has
   * an Ipv4 object aggregated to it.
   * 
   * \param c NodeContainer that holds the set of nodes on which to install the
   * new stacks.
   */
  void Install (NodeContainer c) const;

  /**
   * Aggregate IPv4, IPv6, UDP, and TCP stacks to all nodes in the simulation
   */
  void InstallAll (void) const;

  /**
   * \brief set the Tcp stack which will not need any other parameter.  
   *
   * This function sets up the tcp stack to the given TypeId. It should not be 
   * used for NSC stack setup because the nsc stack needs the Library attribute
   * to be setup, please use instead the version that requires an attribute
   * and a value. If you choose to use this function anyways to set nsc stack
   * the default value for the linux library will be used: "liblinux2.6.26.so".
   *
   * \param tid the type id, typically it is set to  "ns3::TcpL4Protocol"
   */
  void SetTcp(std::string tid);
   
  /**
   * \brief This function is used to setup the Network Simulation Cradle stack with library value.
   * 
   * Give the NSC stack a shared library file name to use when creating the 
   * stack implementation.  The attr string is actually the attribute name to 
   * be setup and val is its value. The attribute is the stack implementation 
   * to be used and the value is the shared library name.
   * 
   * \param tid The type id, for the case of nsc it would be "ns3::NscTcpL4Protocol" 
   * \param attr The attribute name that must be setup, for example "Library"
   * \param val The attribute value, which will be in fact the shared library name (example:"liblinux2.6.26.so")
   */
  void SetTcp (std::string tid, std::string attr, const AttributeValue &val); 

  /**
   * \param os output stream
   * \param n node container
   *
   * Enable ascii output on these drop traces, for each node in the NodeContainer..
   * /NodeList/[i]/$ns3ArpL3Protocol/Drop 
   * /NodeList/[i]/$ns3Ipv4L3Protocol/Drop 
   * /NodeList/[i]/$ns3Ipv6L3Protocol/Drop 
   */
  static void EnableAscii (std::ostream &os, NodeContainer n);

  /**
   * \param os output stream
   *
   * Enable ascii output on these drop traces, for all nodes.
   * /NodeList/[i]/$ns3ArpL3Protocol/Drop 
   * /NodeList/[i]/$ns3Ipv4L3Protocol/Drop 
   * /NodeList/[i]/$ns3Ipv6L3Protocol/Drop 
   */
  static void EnableAsciiAll (std::ostream &os);

  /**
   * Enable pcap output on each protocol instance which is of the
   * ns3::Ipv4L3Protocol or ns3::Ipv6L3Protocol type.  Both Tx and 
   * Rx events will be logged.
   *
   * \param filename filename prefix to use for pcap files.
   *
   * \warning If you perform multiple simulations in a single script,
   * each iteration of the simulation will result in the trace files
   * being overwritten.  We don't attempt to anticipate what a user
   * might actually want to do, so we leave it up to them.  If you want
   * to save any particular data, do so manually at inter-simulation 
   * time.
   */
  static void EnablePcapAll (std::string filename);

  /**
   * \brief Enable/disable IPv4 stack install.
   * \param enable enable state
   */
  void SetIpv4StackInstall (bool enable);

  /**
   * \brief Enable/disable IPv6 stack install.
   * \param enable enable state
   */
  void SetIpv6StackInstall (bool enable);

  /**
   * \brief Enable/disable c2c stack install.
   * \param enable enable state
   */
  void Setc2cStackInstall (bool enable);                 //iTETRIS

private:
  void Initialize (void);
  ObjectFactory m_tcpFactory;
  const Ipv4RoutingHelper *m_routing;
  
  /**
   * \internal
   * \brief IPv6 routing helper.
   */
  const Ipv6RoutingHelper *m_routingv6;

  /**
   * \internal
   * \brief c2c routing helper.
   */
  const c2cRoutingHelper *m_routingc2c;                 //iTETRIS

  /**
   * \internal
   */
  static void CreateAndAggregateObjectFromTypeId (Ptr<Node> node, const std::string typeId);

  /**
   * \internal
   */
  static void Cleanup (void);

  /**
   * \internal
   */
  static void LogRxIp (std::string context, Ptr<const Packet> packet, uint32_t deviceId);

  /**
   * \internal
   */
  static void LogTxIp (std::string context, Ptr<const Packet> packet, uint32_t deviceId);

  /**
   * \internal
   */
  static void LogRxc2c (std::string context, Ptr<const Packet> packet, uint32_t deviceId);

  /**
   * \internal
   */
  static void LogTxc2c (std::string context, Ptr<const Packet> packet, uint32_t deviceId);

  /**
   * \internal
   */
  static Ptr<PcapWriter> GetStream (uint32_t nodeId, uint32_t interfaceId);

  struct Trace {
    uint32_t nodeId;
    uint32_t interfaceId;
    Ptr<PcapWriter> writer;
  };

  /**
   * \internal
   */
  static void AsciiDropEventIpv4 (Ptr<AsciiWriter> writer, std::string path,
                                  Ipv4Header const &header, Ptr<const Packet> packet,
                                  Ipv4L3Protocol::DropReason reason, uint32_t interface);
  /**
   * \internal
   */
  static void AsciiDropEventArp (Ptr<AsciiWriter> writer, std::string path, Ptr<const Packet> packet);

  /**
   * \internal
   */
  static void AsciiDropEventIpv6 (Ptr<AsciiWriter> writer, std::string path,
                                  Ipv6Header const &header, Ptr<const Packet> packet,
                                  Ipv6L3Protocol::DropReason reason, uint32_t interface);
  /**
   * \internal
   */
  static void AsciiDropEventc2c (Ptr<AsciiWriter> writer, std::string path,
                                  c2cCommonHeader const &header, Ptr<const Packet> packet,
                                  c2cL3Protocol::DropReason reason, uint32_t interface);

  static std::string m_pcapBaseFilename;

  /**
   * \internal
   */
  static uint32_t GetNodeIndex (std::string context);

  static std::vector<Trace> m_traces;

  /**
   * \brief IPv4 install state (enabled/disabled) ?
   */
  bool m_ipv4Enabled;

  /**
   * \brief IPv6 install state (enabled/disabled) ?
   */
  bool m_ipv6Enabled;

  /**
   * \brief c2c install state (enabled/disabled) ?
   */
  bool m_c2cEnabled;                                    //iTETRIS
};

} // namespace ns3

#endif /* INTERNET_STACK_HELPER_H */
