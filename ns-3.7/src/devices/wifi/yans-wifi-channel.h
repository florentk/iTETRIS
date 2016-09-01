/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2006,2007 INRIA,
 *            2009-2010, Uwicore Laboratory (www.uwicore.umh.es),
 *            University Miguel Hernandez, EU FP7 iTETRIS project
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
 * Author: Mathieu Lacage, <mathieu.lacage@sophia.inria.fr>, Ramon Bauza <rbauza@umh.es>
 */
#ifndef YANS_WIFI_CHANNEL_H
#define YANS_WIFI_CHANNEL_H

#include <vector>
#include <stdint.h>
#include "ns3/packet.h"
#include "wifi-channel.h"
#include "wifi-mode.h"
#include "wifi-preamble.h"

namespace ns3 {

class NetDevice;
class PropagationLossModel;
class PropagationDelayModel;
class YansWifiPhy;

/**
 * \brief A Yans wifi channel
 *
 * This wifi channel implements the propagation model described in
 * "Yet Another Network Simulator", (http://cutebugs.net/files/wns2-yans.pdf).
 *
 * This class is expected to be used in tandem with the ns3::YansWifiPhy 
 * class and contains a ns3::PropagationLossModel and a ns3::PropagationDelayModel.
 * By default, no propagation models are set so, it is the caller's responsability
 * to set them before using the channel.
 * This class was modified in the iTETRIS project in order to support an interference range
 * calculation for V2V and V2I transmissions aimed at save simulation time.
 * The interference ranges for V2V and V2I transmissions can be set in this class by the wave-installer 
 * class based on the instructions contained in the iTETRIS ns-3 configuration files for WAVE-equipped vehicles.
 * The interference range is the maximum distance separating transmitting and receiving nodes that 
 * the simulator considers for interference and packet reception computation. these values have to 
 * be set considering that fixing a given propagation channel model and a transmission power, the probability
 * of packet receptions beyond a given distance is negligible. This solution permits to higly reduce 
 * the simulation time given that the computations for packet receptions are made only 
 * for nodes within the interference range.
 */
class YansWifiChannel : public WifiChannel
{
public:
  static TypeId GetTypeId (void);

  YansWifiChannel ();
  virtual ~YansWifiChannel ();

  // inherited from Channel.
  virtual uint32_t GetNDevices (void) const;
  virtual Ptr<NetDevice> GetDevice (uint32_t i) const;

  void Add (Ptr<YansWifiPhy> phy);

  /**
   * \param loss the new propagation loss model.
   */
  void SetPropagationLossModel (Ptr<PropagationLossModel> loss);
  /**
   * \param delay the new propagation delay model.
   */
  void SetPropagationDelayModel (Ptr<PropagationDelayModel> delay);

  /**
   * \param sender the device from which the packet is originating.
   * \param packet the packet to send
   * \param txPowerDbm the tx power associated to the packet
   * \param wifiMode the tx mode associated to the packet
   * \param preamble the preamble associated to the packet
   *
   * This method should not be invoked by normal users. It is 
   * currently invoked only from WifiPhy::Send. YansWifiChannel 
   * delivers packets only between PHYs with the same m_channelNumber,
   * e.g. PHYs that are operating on the same channel.
   */
  void Send (Ptr<YansWifiPhy> sender, Ptr<const Packet> packet, double txPowerDbm,
             WifiMode wifiMode, WifiPreamble preamble) const;
  void SetInterferenceRangeVehicle (float range);
  void SetInterferenceRangeCiu (float range);

private:
  typedef std::vector<Ptr<YansWifiPhy> > PhyList;
  void Receive (uint32_t i, Ptr<Packet> packet, double rxPowerDbm,
                WifiMode txMode, WifiPreamble preamble) const;


  PhyList m_phyList;
  Ptr<PropagationLossModel> m_loss;
  Ptr<PropagationDelayModel> m_delay;
  float m_interferenceRangeVehicle;
  float m_interferenceRangeCiu;
};

} // namespace ns3


#endif /* YANS_WIFI_CHANNEL_H */
