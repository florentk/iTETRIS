/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2006, 2009 INRIA
 * Copyright (c) 2009 MIRKO BANCHI
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
 * Author: Mirko Banchi <mk.banchi@gmail.com>
 */
#ifndef QAP_WIFI_MAC_H
#define QAP_WIFI_MAC_H

#include "ns3/mac48-address.h"
#include "ns3/callback.h"
#include "ns3/packet.h"
#include "ns3/nstime.h"
#include "ns3/event-id.h"

#include "supported-rates.h"
#include "wifi-remote-station-manager.h"
#include "wifi-mac.h"
#include "qos-utils.h"

#include <map>

namespace ns3 {

class DcaTxop;
class EdcaTxopN;
class WifiMacHeader;
class WifiPhy;
class MacLow;
class MacRxMiddle;
class MacTxMiddle;
class DcfManager;
class AmsduSubframeHeader;
class MsduAggregator;
class MgtAddBaRequestHeader;

class QapWifiMac : public WifiMac
{
public:
  static TypeId GetTypeId (void);
  QapWifiMac ();
  virtual ~QapWifiMac ();
  
  // inherited from WifiMac.
  virtual void SetSlot (Time slotTime);
  virtual void SetSifs (Time sifs);
  virtual void SetEifsNoDifs (Time eifsNoDifs);
  virtual void SetAckTimeout (Time ackTimeout);
  virtual void SetCtsTimeout (Time ctsTimeout);
  virtual void SetPifs (Time pifs);
  virtual Time GetSlot (void) const;
  virtual Time GetSifs (void) const;
  virtual Time GetEifsNoDifs (void) const;
  virtual Time GetAckTimeout (void) const;
  virtual Time GetCtsTimeout (void) const;
  virtual Time GetPifs (void) const;
  virtual void SetWifiPhy (Ptr<WifiPhy> phy);
  virtual void SetWifiRemoteStationManager (Ptr<WifiRemoteStationManager> stationManager);
  virtual void Enqueue (Ptr<const Packet> packet, Mac48Address to, Mac48Address from);
  virtual void Enqueue (Ptr<const Packet> packet, Mac48Address to);
  virtual bool SupportsSendFrom (void) const;
  virtual void SetForwardUpCallback (Callback<void,Ptr<Packet>, Mac48Address, Mac48Address> upCallback);
  virtual void SetLinkUpCallback (Callback<void> linkUp);
  virtual void SetLinkDownCallback (Callback<void> linkDown);
  virtual Mac48Address GetAddress (void) const;
  virtual Ssid GetSsid (void) const;
  virtual void SetAddress (Mac48Address address);
  virtual void SetSsid (Ssid ssid);
  virtual Mac48Address GetBssid (void) const;
  virtual void SetBasicBlockAckTimeout (Time blockAckTimeout);
  virtual void SetCompressedBlockAckTimeout (Time blockAckTimeout);
  virtual Time GetBasicBlockAckTimeout (void) const;
  virtual Time GetCompressedBlockAckTimeout (void) const;

  void SetBeaconInterval (Time interval);
  Time GetBeaconInterval (void) const;
  void StartBeaconing (void);

private:
  typedef std::map<AccessClass, Ptr<EdcaTxopN> > Queues;
  typedef std::list<std::pair<Ptr<Packet>, AmsduSubframeHeader> > DeaggregatedMsdus;
  typedef std::list<std::pair<Ptr<Packet>, AmsduSubframeHeader> >::const_iterator DeaggregatedMsdusCI;
  
  virtual void DoDispose (void);
  virtual void DoStart (void);
  void Receive (Ptr<Packet> packet, const WifiMacHeader* hdr);
  void ForwardUp (Ptr<Packet> packet, Mac48Address from, Mac48Address to);
  void ForwardDown (Ptr<const Packet> packet, Mac48Address from, Mac48Address to);
  /* Next function is invoked only when ap relies a frame. */
  void ForwardDown (Ptr<const Packet> packet, Mac48Address from, Mac48Address to,
                    const WifiMacHeader* oldHdr);
  void TxOk (const WifiMacHeader& hdr);
  void TxFailed (const WifiMacHeader& hdr);
  void SendProbeResp (Mac48Address to);
  void SendAssocResp (Mac48Address to, bool success);
  void SendAddBaResponse (const MgtAddBaRequestHeader *reqHdr, Mac48Address originator);
  void SendOneBeacon (void);
  SupportedRates GetSupportedRates (void) const;
  void SetBeaconGeneration (bool enable);
  bool GetBeaconGeneration (void) const;
  
  void DeaggregateAmsduAndForward (Ptr<Packet> aggregatedPacket, const WifiMacHeader* hdr);
  QapWifiMac &operator = (const QapWifiMac &);
  QapWifiMac (const QapWifiMac &);

  Ptr<EdcaTxopN> GetVOQueue (void) const;
  Ptr<EdcaTxopN> GetVIQueue (void) const;
  Ptr<EdcaTxopN> GetBEQueue (void) const;
  Ptr<EdcaTxopN> GetBKQueue (void) const;
  void SetQueue (enum AccessClass ac);

  virtual void FinishConfigureStandard (enum WifiPhyStandard standard);

  Queues m_queues;
  Ptr<DcaTxop> m_beaconDca;
  Ptr<MacLow> m_low;
  Ptr<WifiPhy> m_phy;
  Ptr<WifiRemoteStationManager> m_stationManager;
  MacRxMiddle *m_rxMiddle;
  MacTxMiddle *m_txMiddle;
  DcfManager *m_dcfManager;
  Ssid m_ssid;
  EventId m_beaconEvent;
  Time m_beaconInterval;
  bool m_enableBeaconGeneration;
  Callback<void,Ptr<Packet>, Mac48Address, Mac48Address> m_forwardUp;
};

}  //namespace ns3

#endif /* QAP_WIFI_MAC_H */
