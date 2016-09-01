/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009 EU FP7 iTETRIS project
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
 * Author: Ramon Bauza <rbauza@umh.es>
 */
#ifndef MCS_TAG_WIFI_MANAGER_H
#define MCS_TAG_WIFI_MANAGER_H

#include <stdint.h>
#include "wifi-remote-station-manager.h"

namespace ns3 {

class McsTagWifiManager : public WifiRemoteStationManager
{
public:
  static TypeId GetTypeId (void);
  McsTagWifiManager ();
  virtual ~McsTagWifiManager ();

private:
  virtual class WifiRemoteStation *CreateStation (void);
};

class McsTagWifiRemoteStation : public WifiRemoteStation
{
public:
  McsTagWifiRemoteStation (Ptr<McsTagWifiManager> stations);
  virtual ~McsTagWifiRemoteStation ();
  WifiMode GetDataMode (Ptr<const Packet> packet, uint32_t fullPacketSize);
  WifiMode GetRtsMode (Ptr<const Packet> packet);

protected:
  virtual void DoReportRxOk (double rxSnr, WifiMode txMode);
  virtual void DoReportRtsFailed (void);
  virtual void DoReportDataFailed (void);
  virtual void DoReportRtsOk (double ctsSnr, WifiMode ctsMode, double rtsSnr);
  virtual void DoReportDataOk (double ackSnr, WifiMode ackMode, double dataSnr);
  virtual void DoReportFinalRtsFailed (void);
  virtual void DoReportFinalDataFailed (void);

private:
  virtual Ptr<WifiRemoteStationManager> GetManager (void) const;
  virtual WifiMode DoGetDataMode (uint32_t size);
  virtual WifiMode DoGetRtsMode (void);
  virtual WifiMode GetWifiMode (uint8_t mcs);
  Ptr<McsTagWifiManager> m_manager;
  Ptr<const Packet> m_packet;
};

} // namespace ns3

#endif /* MCS_TAG_WIFI_MANAGER_H */
