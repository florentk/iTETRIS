/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2006 INRIA
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
#ifndef IDEAL_MAC_STATIONS_H
#define IDEAL_MAC_STATIONS_H

#include <stdint.h>
#include <vector>
#include "wifi-mode.h"
#include "wifi-remote-station-manager.h"

namespace ns3 {

/**
 * \brief Ideal rate control algorithm
 *
 * This class implements an 'ideal' rate control algorithm
 * similar to RBAR in spirit (see <i>A rate-adaptive MAC
 * protocol for multihop wireless networks</i> by G. Holland,
 * N. Vaidya, and P. Bahl.): every station keeps track of the
 * snr of every packet received and sends back this snr to the
 * original transmitter by an out-of-band mechanism. Each 
 * transmitter keeps track of the last snr sent back by a receiver
 * and uses it to pick a transmission mode based on a set
 * of snr thresholds built from a target ber and transmission 
 * mode-specific snr/ber curves.
 */
class IdealWifiManager : public WifiRemoteStationManager 
{
public:
  static TypeId GetTypeId (void);
  IdealWifiManager ();
  virtual ~IdealWifiManager ();

  virtual void SetupPhy (Ptr<WifiPhy> phy);

  // return the min snr needed to successfully transmit
  // data with this mode at the specified ber.
  double GetSnrThreshold (WifiMode mode) const;
  void AddModeSnrThreshold (WifiMode mode, double ber);
private:
  virtual class WifiRemoteStation *CreateStation (void);

  typedef std::vector<std::pair<double,WifiMode> > Thresholds;

  double m_ber;
  Thresholds m_thresholds;
  double m_minSnr;
  double m_maxSnr;
};

class IdealWifiRemoteStation : public WifiRemoteStation
{
public:
  IdealWifiRemoteStation (Ptr<IdealWifiManager> stations);

  virtual ~IdealWifiRemoteStation ();

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

  Ptr<IdealWifiManager> m_manager;
  double m_lastSnr;
};

} // namespace ns3

#endif /* MAC_STA_H */
