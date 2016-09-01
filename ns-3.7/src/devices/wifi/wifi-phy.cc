/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2005,2006 INRIA
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

#include "wifi-phy.h"
#include "wifi-mode.h"
#include "wifi-channel.h"
#include "wifi-preamble.h"
#include "ns3/simulator.h"
#include "ns3/packet.h"
#include "ns3/random-variable.h"
#include "ns3/assert.h"
#include "ns3/log.h"
#include "ns3/double.h"
#include "ns3/uinteger.h"
#include "ns3/enum.h"
#include "ns3/trace-source-accessor.h"
#include <math.h>

NS_LOG_COMPONENT_DEFINE ("WifiPhy");

namespace ns3 {

/****************************************************************
 *       This destructor is needed.
 ****************************************************************/

WifiPhyListener::~WifiPhyListener ()
{}

/****************************************************************
 *       The actual WifiPhy class
 ****************************************************************/

NS_OBJECT_ENSURE_REGISTERED (WifiPhy);

TypeId 
WifiPhy::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::WifiPhy")
    .SetParent<Object> ()
    .AddTraceSource ("PhyTxBegin", 
                     "Trace source indicating a packet has begun transmitting over the channel medium",
                     MakeTraceSourceAccessor (&WifiPhy::m_phyTxBeginTrace))
    .AddTraceSource ("PhyTxEnd", 
                     "Trace source indicating a packet has been completely transmitted over the channel",
                     MakeTraceSourceAccessor (&WifiPhy::m_phyTxEndTrace))
    .AddTraceSource ("PhyTxDrop", 
                     "Trace source indicating a packet has been dropped by the device during transmission",
                     MakeTraceSourceAccessor (&WifiPhy::m_phyTxDropTrace))
    .AddTraceSource ("PhyRxBegin", 
                     "Trace source indicating a packet has begun being received from the channel medium by the device",
                     MakeTraceSourceAccessor (&WifiPhy::m_phyRxBeginTrace))
    .AddTraceSource ("PhyRxEnd", 
                     "Trace source indicating a packet has been completely received from the channel medium by the device",
                     MakeTraceSourceAccessor (&WifiPhy::m_phyRxEndTrace))
    .AddTraceSource ("PhyRxDrop", 
                     "Trace source indicating a packet has been dropped by the device during reception",
                     MakeTraceSourceAccessor (&WifiPhy::m_phyRxDropTrace))
    .AddTraceSource ("PromiscSnifferRx", 
                     "Trace source simulating a wifi device in monitor mode sniffing all received frames",
                     MakeTraceSourceAccessor (&WifiPhy::m_phyPromiscSniffRxTrace))
    .AddTraceSource ("PromiscSnifferTx", 
                     "Trace source simulating the capability of a wifi device in monitor mode to sniff all frames being transmitted",
                     MakeTraceSourceAccessor (&WifiPhy::m_phyPromiscSniffTxTrace))
    ;
  return tid;
}

WifiPhy::WifiPhy ()
  : m_isNodeActivated (false) // Added by Ramon Bauza
{
  NS_LOG_FUNCTION (this);
}

WifiPhy::~WifiPhy ()
{
  NS_LOG_FUNCTION (this);
}

WifiMode 
WifiPhy::Get6mba (void)
{
  static WifiMode mode = WifiModeFactory::CreateBpsk ("wifia-6mbs",
                                                      true,
                                                      20000000, 6000000, 12000000,
                                                      WIFI_PHY_STANDARD_80211a);
  return mode;
}
WifiMode 
WifiPhy::Get9mba (void)
{
  static WifiMode mode = WifiModeFactory::CreateBpsk ("wifia-9mbs",
                                                      false,
                                                      20000000, 9000000, 12000000,
                                                      WIFI_PHY_STANDARD_80211a);
  return mode;
}
WifiMode 
WifiPhy::Get12mba (void)
{
  static WifiMode mode = WifiModeFactory::CreateQpsk ("wifia-12mbs",
                                                      true,
                                                      20000000, 12000000, 24000000,
                                                      WIFI_PHY_STANDARD_80211a);
  return mode;
}
WifiMode 
WifiPhy::Get18mba (void)
{
  static WifiMode mode = WifiModeFactory::CreateQpsk ("wifia-18mbs",
                                                      false,
                                                      20000000, 18000000, 24000000,
                                                      WIFI_PHY_STANDARD_80211a);
  return mode;
}
WifiMode 
WifiPhy::Get24mba (void)
{
  static WifiMode mode = WifiModeFactory::CreateQam ("wifia-24mbs",
                                                     true,
                                                     20000000, 24000000, 48000000,
                                                     16,
                                                     WIFI_PHY_STANDARD_80211a);
  return mode;
}
WifiMode 
WifiPhy::Get36mba (void)
{
  static WifiMode mode = WifiModeFactory::CreateQam ("wifia-36mbs",
                                                     false,
                                                     20000000, 36000000, 48000000,
                                                     16,
                                                     WIFI_PHY_STANDARD_80211a);
  return mode;
}

WifiMode 
WifiPhy::Get48mba (void)
{
  static WifiMode mode = WifiModeFactory::CreateQam ("wifia-48mbs",
                                                     false,
                                                     20000000, 48000000, 72000000,
                                                     64,
                                                     WIFI_PHY_STANDARD_80211a);
  return mode;
}

WifiMode 
WifiPhy::Get54mba (void)
{
  static WifiMode mode = WifiModeFactory::CreateQam ("wifia-54mbs",
                                                     false,
                                                     20000000, 54000000, 72000000,
                                                     64,
                                                     WIFI_PHY_STANDARD_80211a);
  return mode;
}

void 
WifiPhy::NotifyTxBegin (Ptr<const Packet> packet)
{
  m_phyTxBeginTrace (packet);
}

void 
WifiPhy::NotifyTxEnd (Ptr<const Packet> packet)
{
  m_phyTxEndTrace (packet);
}

void 
WifiPhy::NotifyTxDrop (Ptr<const Packet> packet) 
{
  m_phyTxDropTrace (packet);
}

void 
WifiPhy::NotifyRxBegin (Ptr<const Packet> packet) 
{
  m_phyRxBeginTrace (packet);
}

void 
WifiPhy::NotifyRxEnd (Ptr<const Packet> packet) 
{
  m_phyRxEndTrace (packet);
}

void 
WifiPhy::NotifyRxDrop (Ptr<const Packet> packet) 
{
  m_phyRxDropTrace (packet);
}

void 
WifiPhy::NotifyPromiscSniffRx (Ptr<const Packet> packet, uint16_t channelFreqMhz, uint16_t channelNumber, uint32_t rate, bool isShortPreamble, double signalDbm, double noiseDbm)
{
  m_phyPromiscSniffRxTrace (packet, channelFreqMhz, channelNumber, rate, isShortPreamble, signalDbm, noiseDbm);
}

void 
WifiPhy::NotifyPromiscSniffTx (Ptr<const Packet> packet, uint16_t channelFreqMhz, uint16_t channelNumber, uint32_t rate, bool isShortPreamble)
{
  m_phyPromiscSniffTxTrace (packet, channelFreqMhz, channelNumber, rate, isShortPreamble);
}

WifiMode 
WifiPhy::Get1mbb (void)
{
  static WifiMode mode = WifiModeFactory::CreateDbpsk ("wifib-1mbs",
                                                      true,
                                                      22000000, 1000000, 1000000,
                                                      WIFI_PHY_STANDARD_80211b);
  return mode;
}

WifiMode 
WifiPhy::Get2mbb (void)
{
  static WifiMode mode = WifiModeFactory::CreateDqpsk ("wifib-2mbs",
                                                      true,
                                                      22000000, 2000000, 2000000,
                                                      WIFI_PHY_STANDARD_80211b);
  return mode;
}

WifiMode 
WifiPhy::Get5_5mbb (void)
{
  static WifiMode mode = WifiModeFactory::CreateDqpsk ("wifib-5.5mbs",
                                                      true,
                                                      22000000, 5500000, 5500000,
                                                      WIFI_PHY_STANDARD_80211b);
  return mode;
}

WifiMode 
WifiPhy::Get11mbb (void)
{
  static WifiMode mode = WifiModeFactory::CreateDqpsk ("wifib-11mbs",
                                                      true,
                                                      22000000, 11000000, 11000000,
                                                      WIFI_PHY_STANDARD_80211b);
  return mode;
}

WifiMode 
WifiPhy::Get3mb10Mhz (void)
{
  static WifiMode mode = WifiModeFactory::CreateBpsk ("wifi-3mbs-10Mhz",
                                                      true,
                                                      10000000, 3000000, 6000000,
                                                      WIFI_PHY_STANDARD_80211_10Mhz);
  return mode;
}

WifiMode 
WifiPhy::Get4_5mb10Mhz (void)
{
  static WifiMode mode = WifiModeFactory::CreateBpsk ("wifi-4.5mbs-10Mhz",
                                                      false,
                                                      10000000, 4500000, 6000000,
                                                      WIFI_PHY_STANDARD_80211_10Mhz);
  return mode;
}

WifiMode 
WifiPhy::Get6mb10Mhz (void)
{
  static WifiMode mode = WifiModeFactory::CreateBpsk ("wifi-6mbs-10Mhz",
                                                      true,
                                                      10000000, 6000000, 12000000,
                                                      WIFI_PHY_STANDARD_80211_10Mhz);
  return mode;
}

WifiMode 
WifiPhy::Get9mb10Mhz (void)
{
  static WifiMode mode = WifiModeFactory::CreateBpsk ("wifi-9mbs-10Mhz",
                                                      false,
                                                      10000000, 9000000, 12000000,
                                                      WIFI_PHY_STANDARD_80211_10Mhz);
  return mode;
}

WifiMode 
WifiPhy::Get12mb10Mhz (void)
{
  static WifiMode mode = WifiModeFactory::CreateBpsk ("wifi-12mbs-10Mhz",
                                                      true,
                                                      10000000, 12000000, 24000000,
                                                      WIFI_PHY_STANDARD_80211_10Mhz);
  return mode;
}

WifiMode 
WifiPhy::Get18mb10Mhz (void)
{
  static WifiMode mode = WifiModeFactory::CreateBpsk ("wifi-18mbs-10Mhz",
                                                      false,
                                                      10000000, 18000000, 24000000,
                                                      WIFI_PHY_STANDARD_80211_10Mhz);
  return mode;
}

WifiMode 
WifiPhy::Get24mb10Mhz (void)
{
  static WifiMode mode = WifiModeFactory::CreateBpsk ("wifi-24mbs-10Mhz",
                                                      false,
                                                      10000000, 24000000, 36000000,
                                                      WIFI_PHY_STANDARD_80211_10Mhz);
  return mode;
}

WifiMode 
WifiPhy::Get27mb10Mhz (void)
{
  static WifiMode mode = WifiModeFactory::CreateBpsk ("wifi-27mbs-10Mhz",
                                                      false,
                                                      10000000, 27000000, 36000000,
                                                      WIFI_PHY_STANDARD_80211_10Mhz);
  return mode;
}

WifiMode 
WifiPhy::Get1_5mb5Mhz (void)
{
  static WifiMode mode = WifiModeFactory::CreateBpsk ("wifi-1_5mbs-5Mhz",
                                                      true,
                                                      5000000, 1500000, 3000000,
                                                      WIFI_PHY_STANDARD_80211_5Mhz);
  return mode;
}

WifiMode 
WifiPhy::Get2_25mb5Mhz (void)
{
  static WifiMode mode = WifiModeFactory::CreateBpsk ("wifi-2.25mbs-5Mhz",
                                                      false,
                                                      5000000, 2250000, 3000000,
                                                      WIFI_PHY_STANDARD_80211_5Mhz);
  return mode;
}

WifiMode 
WifiPhy::Get3mb5Mhz (void)
{
  static WifiMode mode = WifiModeFactory::CreateBpsk ("wifi-3mbs-5Mhz",
                                                      true,
                                                      5000000, 3000000, 6000000,
                                                      WIFI_PHY_STANDARD_80211_5Mhz);
  return mode;
}

WifiMode 
WifiPhy::Get4_5mb5Mhz (void)
{
  static WifiMode mode = WifiModeFactory::CreateBpsk ("wifi-4.5mbs-5Mhz",
                                                      false,
                                                      5000000, 4500000, 6000000,
                                                      WIFI_PHY_STANDARD_80211_5Mhz);
  return mode;
}

WifiMode 
WifiPhy::Get6mb5Mhz (void)
{
  static WifiMode mode = WifiModeFactory::CreateBpsk ("wifi-6mbs-5Mhz",
                                                      true,
                                                      5000000, 6000000, 12000000,
                                                      WIFI_PHY_STANDARD_80211_5Mhz);
  return mode;
}

WifiMode 
WifiPhy::Get9mb5Mhz (void)
{
  static WifiMode mode = WifiModeFactory::CreateBpsk ("wifi-9mbs-5Mhz",
                                                      false,
                                                      5000000, 9000000, 12000000,
                                                      WIFI_PHY_STANDARD_80211_5Mhz);
  return mode;
}

WifiMode 
WifiPhy::Get12mb5Mhz (void)
{
  static WifiMode mode = WifiModeFactory::CreateBpsk ("wifi-12mbs-5Mhz",
                                                      false,
                                                      5000000, 12000000, 18000000,
                                                      WIFI_PHY_STANDARD_80211_5Mhz);
  return mode;
}

WifiMode 
WifiPhy::Get13_5mb5Mhz (void)
{
  static WifiMode mode = WifiModeFactory::CreateBpsk ("wifi-13.5mbs-5Mhz",
                                                      false,
                                                      5000000, 13500000, 18000000,
                                                      WIFI_PHY_STANDARD_80211_5Mhz);
  return mode;
}

// Added by Ramon Bauza
bool 
WifiPhy::IsNodeActivated (void)
{
  return (m_isNodeActivated);
}

// Added by Ramon Bauza
void 
WifiPhy::SetNodeStatus (bool activated)
{
  m_isNodeActivated = activated;
}

std::ostream& operator<< (std::ostream& os, enum WifiPhy::State state)
{
  switch (state) {
  case WifiPhy::IDLE:
    return (os << "IDLE");
  case WifiPhy::CCA_BUSY:
    return (os << "CCA_BUSY");
  case WifiPhy::TX:
    return (os << "TX");
  case WifiPhy::RX:
    return (os << "RX");
  case WifiPhy::SWITCHING: 
    return (os << "SWITCHING");
  default:
    NS_FATAL_ERROR ("Invalid WifiPhy state");
    return (os << "INVALID");
  }
}

} // namespace ns3

namespace {

static class Constructor
{
public:
  Constructor () {
    ns3::WifiPhy::Get6mba ();
    ns3::WifiPhy::Get9mba ();
    ns3::WifiPhy::Get12mba ();
    ns3::WifiPhy::Get18mba ();
    ns3::WifiPhy::Get24mba ();
    ns3::WifiPhy::Get36mba ();
    ns3::WifiPhy::Get48mba ();
    ns3::WifiPhy::Get54mba ();
    ns3::WifiPhy::Get1mbb ();
    ns3::WifiPhy::Get2mbb ();
    ns3::WifiPhy::Get5_5mbb ();
    ns3::WifiPhy::Get11mbb ();
    ns3::WifiPhy::Get3mb10Mhz ();
    ns3::WifiPhy::Get4_5mb10Mhz ();
    ns3::WifiPhy::Get6mb10Mhz ();
    ns3::WifiPhy::Get9mb10Mhz ();
    ns3::WifiPhy::Get12mb10Mhz ();
    ns3::WifiPhy::Get18mb10Mhz ();
    ns3::WifiPhy::Get24mb10Mhz ();
    ns3::WifiPhy::Get27mb10Mhz ();
    ns3::WifiPhy::Get1_5mb5Mhz ();
    ns3::WifiPhy::Get2_25mb5Mhz ();
    ns3::WifiPhy::Get3mb5Mhz ();
    ns3::WifiPhy::Get4_5mb5Mhz ();
    ns3::WifiPhy::Get6mb5Mhz ();
    ns3::WifiPhy::Get9mb5Mhz ();
    ns3::WifiPhy::Get12mb5Mhz ();
    ns3::WifiPhy::Get13_5mb5Mhz ();
  }
} g_constructor;
}
