/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2005,2006,2007 INRIA
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
#ifndef WIFI_REMOTE_STATION_MANAGER_H
#define WIFI_REMOTE_STATION_MANAGER_H

#include <vector>
#include <utility>
#include <map>
#include "ns3/mac48-address.h"
#include "ns3/packet.h"
#include "ns3/object.h"
#include "ns3/traced-value.h"
#include "ns3/wifi-mac-header.h"
#include "wifi-mode.h"
#include "qos-utils.h"

namespace ns3 {

class WifiRemoteStation;
class NonUnicastWifiRemoteStation;
class WifiPhy;

/**
 * \brief hold a list of per-remote-station state.
 *
 * \sa ns3::WifiRemoteStation.
 */
class WifiRemoteStationManager : public Object
{
private:
  typedef std::vector<WifiMode> BasicModes;
public:
  typedef BasicModes::const_iterator BasicModesIterator;

  static TypeId GetTypeId (void);

  WifiRemoteStationManager ();
  virtual ~WifiRemoteStationManager ();

  virtual void SetupPhy (Ptr<WifiPhy> phy);

  uint32_t GetMaxSsrc (void) const;
  uint32_t GetMaxSlrc (void) const;
  uint32_t GetRtsCtsThreshold (void) const;
  uint32_t GetFragmentationThreshold (void) const;
  void SetMaxSsrc (uint32_t maxSsrc);
  void SetMaxSlrc (uint32_t maxSlrc);
  void SetRtsCtsThreshold (uint32_t threshold);
  void SetFragmentationThreshold (uint32_t threshold);
  
  // Invoked in a STA upon dis-association
  // or in an AP upon reboot
  void Reset (void);
  // Invoked in a STA upon association to store
  // the set of rates which belong to the 
  // BSSBasicRateSet of the associated AP
  // and which are supported locally.
  // Invoked in an AP to configure the BSSBasicRateSet
  void AddBasicMode (WifiMode mode);

  WifiMode GetDefaultMode (void) const;
  uint32_t GetNBasicModes (void) const;
  WifiMode GetBasicMode (uint32_t i) const;
  BasicModesIterator BeginBasicModes (void) const;
  BasicModesIterator EndBasicModes (void) const;

  bool IsLowLatency (void) const;

  WifiMode GetNonUnicastMode (void) const;

  WifiRemoteStation *Lookup (Mac48Address address);
  WifiRemoteStation *LookupNonUnicast (void);
protected:
  friend class WifiRemoteStation;
  virtual void DoDispose (void);
  WifiRemoteStation *m_nonUnicast;
private:
  typedef std::vector <WifiRemoteStation *> Stations;
  virtual class WifiRemoteStation *CreateStation (void) = 0;
  Stations m_stations;
  WifiMode m_defaultTxMode;
  BasicModes m_basicModes;
  bool m_isLowLatency;
  uint32_t m_maxSsrc;
  uint32_t m_maxSlrc;
  uint32_t m_rtsCtsThreshold;
  uint32_t m_fragmentationThreshold;
  WifiMode m_nonUnicastMode;


  /**
   * Public method used to fire a MacTxRtsFailed trace.
   * Implemented for encapsulation purposes.
   */
  void NotifyTxRtsFailed (Mac48Address address);  

  /**
   * Public method used to fire a MacTxDataFailed trace.
   * Implemented for encapsulation purposes. 
   */
  void NotifyTxDataFailed (Mac48Address address);  

  /**
   * Public method used to fire a MacTxFinalRtsFailed trace.
   * Implemented for encapsulation purposes.
   */
  void NotifyTxFinalRtsFailed (Mac48Address address);  

  /**
   * Public method used to fire a MacTxFinalDataFailed trace.
   * Implemented for encapsulation purposes. 
   */
  void NotifyTxFinalDataFailed (Mac48Address address);    


  /**
   * The trace source fired when the transmission of a RTS has failed
   *
   * \see class CallBackTraceSource
   */
  TracedCallback<Mac48Address> m_macTxRtsFailed;

  /**
   * The trace source fired when the transmission of a data packet has failed 
   *
   * \see class CallBackTraceSource
   */
  TracedCallback<Mac48Address> m_macTxDataFailed;

  /**
   * The trace source fired when the transmission of a RTS has
   * exceeded the maximum number of attempts
   *
   * \see class CallBackTraceSource
   */
  TracedCallback<Mac48Address> m_macTxFinalRtsFailed;

  /**
   * The trace source fired when the transmission of a data packet has
   * exceeded the maximum number of attempts
   *
   * \see class CallBackTraceSource
   */
  TracedCallback<Mac48Address> m_macTxFinalDataFailed;

};

} // namespace ns3

namespace ns3 {

/**
 * \brief hold per-remote-station state.
 *
 * The state in this class is used to keep track
 * of association status if we are in an infrastructure
 * network and to perform the selection of tx parameters
 * on a per-packet basis.
 */
class WifiRemoteStation {
public:
  
  static TypeId GetTypeId (void);
  
  WifiRemoteStation ();
  virtual ~WifiRemoteStation ();

  /**
   * Invoked in an AP upon disassociation of a
   * specific STA.
   */
  void Reset (void);
  /**
   * Invoked in a STA or AP to store the set of 
   * modes supported by a destination which is
   * also supported locally.
   * The set of supported modes includes
   * the BSSBasicRateSet.
   */
  void AddSupportedMode (WifiMode mode);

  bool IsBrandNew (void) const;
  bool IsAssociated (void) const;
  bool IsWaitAssocTxOk (void) const;
  void RecordWaitAssocTxOk (void);
  void RecordGotAssocTxOk (void);
  void RecordGotAssocTxFailed (void);
  void RecordDisassociated (void);

  /**
   * \param packet the packet to queue
   * \param fullPacketSize the size of the packet after its 802.11 MAC header has been added.
   *
   * This method is typically invoked just before queuing a packet for transmission.
   * It is a no-op unless the IsLowLatency attribute of the attached ns3::WifiRemoteStationManager
   * is set to false, in which case, the tx parameters of the packet are calculated and stored in
   * the packet as a tag. These tx parameters are later retrieved from GetDadaMode and GetRtsMode.
   */
  void PrepareForQueue (Ptr<const Packet> packet, uint32_t fullPacketSize);
  /**
   * \param packet the packet to send
   * \param fullPacketSize the size of the packet after its 802.11 MAC header has been added.
   * \returns the transmission mode to use to send this packet
   */
  virtual WifiMode GetDataMode (Ptr<const Packet> packet, uint32_t fullPacketSize);
  /**
   * \param packet the packet to send
   * \returns the transmission mode to use to send the RTS prior to the
   *          transmission of the data packet itself.
   */
  virtual WifiMode GetRtsMode (Ptr<const Packet> packet);
  /**
   * \param ac Represents access class of the packet for which a rts was transmitted.
   *
   * Should be invoked whenever the RtsTimeout associated to a transmission
   * attempt expires.
   */
  void ReportRtsFailed (enum AccessClass ac = AC_BE_NQOS);
  /**
   * \param ac Access class which failed data belongs.
   *
   * Should be invoked whenever the AckTimeout associated to a transmission
   * attempt expires.
   */
  void ReportDataFailed (enum AccessClass ac = AC_BE_NQOS);
  /**
   * \param ac Represents access class of the packet for which the rts was transmitted.
   *
   * Should be invoked whenever we receive the Cts associated to an RTS 
   * we just sent.
   */
  void ReportRtsOk (double ctsSnr, WifiMode ctsMode, double rtsSnr, enum AccessClass ac = AC_BE_NQOS);
  /**
   * \param ac Access class which correct received data belongs.
   *
   * Should be invoked whenever we receive the Ack associated to a data packet
   * we just sent.
   */
  void ReportDataOk (double ackSnr, WifiMode ackMode, double dataSnr, enum AccessClass ac = AC_BE_NQOS);
  /**
   * \param ac Represents access class of the packet for which a rts was transmitted.
   *
   * Should be invoked after calling ReportRtsFailed if 
   * NeedRtsRetransmission returns false
   */
  void ReportFinalRtsFailed (enum AccessClass ac = AC_BE_NQOS);
  /**
   * \param ac Access class which failed data belongs.
   *
   * Should be invoked after calling ReportDataFailed if 
   * NeedDataRetransmission returns false
   */
  void ReportFinalDataFailed (enum AccessClass ac = AC_BE_NQOS);

  /**
   * \param rxSnr the snr of the packet received
   * \param txMode the transmission mode used for the packet received.
   *
   * Should be invoked whenever a packet is successfully received.
   */
  void ReportRxOk (double rxSnr, WifiMode txMode, enum AccessClass ac = AC_BE_NQOS);

  /**
   * \param packet the packet to send
   * \returns true if we want to use an RTS/CTS handshake for this
   *          packet before sending it, false otherwise.
   */
  virtual bool NeedRts (Ptr<const Packet> packet);
  /**
   * \param hdr the header of the packet to send
   * \returns true if we want to restart a failed RTS/CTS 
   *          handshake, false otherwise.
   */
  virtual bool NeedRtsRetransmission (const WifiMacHeader *hdr);
  /**
   * \param hdr the header of the packet to send
   * \returns true if we want to resend a packet 
   *          after a failed transmission attempt, false otherwise.
   */
  virtual bool NeedDataRetransmission (const WifiMacHeader *hdr);

  /**
   * \param packet the packet to send
   * \returns true if this packet should be fragmented, false otherwise.
   */
  virtual bool NeedFragmentation (Ptr<const Packet> packet);
  /**
   * \param packet the packet to send
   * \param fragmentNumber the fragment index of the next fragment to send (starts at zero).
   * \returns the size of the corresponding fragment.
   */
  virtual uint32_t GetFragmentSize (Ptr<const Packet> packet, uint32_t fragmentNumber);
  /**
   * \param packet the packet to send
   * \param fragmentNumber the fragment index of the next fragment to send (starts at zero).
   * \returns the offset within the original packet where this fragment starts.
   */
  virtual uint32_t GetFragmentOffset (Ptr<const Packet> packet, uint32_t fragmentNumber);
  /**
   * \param packet the packet to send
   * \param fragmentNumber the fragment index of the next fragment to send (starts at zero).
   * \returns true if this is the last fragment, false otherwise.
   */
  virtual bool IsLastFragment (Ptr<const Packet> packet, uint32_t fragmentNumber);

  /**
   * \param rtsMode the transmission mode used to send an RTS we just received
   * \returns the transmission mode to use for the CTS to complete the RTS/CTS
   *          handshake.
   */
  WifiMode GetCtsMode (WifiMode rtsMode);
  /**
   * \param dataMode the transmission mode used to send an ACK we just received
   * \returns the transmission mode to use for the ACK to complete the data/ACK
   *          handshake.
   */
  WifiMode GetAckMode (WifiMode dataMode);
  /**
   * \return exponentially weighted average SLRC, this is used by Airtime link metric of 802.11s
   */
  double GetAvgSlrc () const;
  /** 
   * set the address of the remote stationt represented by this instance of WifiRemoteStation
   * 
   * @param address the MAC address of the remote station
   */
  void SetAddress(Mac48Address address);
  /** 
   * get the address of the remote stationt represented by this instance of WifiRemoteStation
   * 
   * @return the MAC address of the remote station
   */
  Mac48Address GetAddress();
private:
  virtual Ptr<WifiRemoteStationManager> GetManager (void) const = 0;
  virtual WifiMode DoGetDataMode (uint32_t size) = 0;
  virtual WifiMode DoGetRtsMode (void) = 0;
  virtual void DoReportRtsFailed (void) = 0;
  virtual void DoReportDataFailed (void) = 0;
  virtual void DoReportRtsOk (double ctsSnr, WifiMode ctsMode, double rtsSnr) = 0;
  virtual void DoReportDataOk (double ackSnr, WifiMode ackMode, double dataSnr) = 0;
  virtual void DoReportFinalRtsFailed (void) = 0;
  virtual void DoReportFinalDataFailed (void) = 0;
  virtual void DoReportRxOk (double rxSnr, WifiMode txMode) = 0;
protected:
  uint32_t GetNSupportedModes (void) const;
  WifiMode GetSupportedMode (uint32_t i) const;
private:
  typedef std::vector<WifiMode> SupportedModes;
  uint32_t GetNFragments (Ptr<const Packet> packet);
  bool IsIn (WifiMode mode) const;
  WifiMode GetControlAnswerMode (WifiMode reqMode);

  void UpdateSsrcCounter (enum AccessClass ac, bool reset);
  void UpdateSlrcCounter (enum AccessClass ac, bool reset);
  

  enum {
    BRAND_NEW,
    DISASSOC,
    WAIT_ASSOC_TX_OK,
    GOT_ASSOC_TX_OK
  } m_state;
  SupportedModes m_modes;
  TracedValue<uint32_t> m_ssrc;
  TracedValue<uint32_t> m_slrc;
  /* voice access class retry counters */
  TracedValue<uint32_t> m_voSsrc;
  TracedValue<uint32_t> m_voSlrc;
  /* video access class retry counters */
  TracedValue<uint32_t> m_viSsrc;
  TracedValue<uint32_t> m_viSlrc;
  /* best-effort access class retry counters */
  TracedValue<uint32_t> m_beSsrc;
  TracedValue<uint32_t> m_beSlrc;
  /* background access class retry counters */
  TracedValue<uint32_t> m_bkSsrc;
  TracedValue<uint32_t> m_bkSlrc;
  /* next two maps are defined for an easy access to qos retry counters */
  std::map<AccessClass, TracedValue<uint32_t>* > m_qosSsrc;
  std::map<AccessClass, TracedValue<uint32_t>* > m_qosSlrc;
  double m_avgSlrcCoefficient;
  double m_avgSlrc;
  Mac48Address m_address;
};

} // namespace ns3 

#endif /* MAC_STATIONS_H */
/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2005,2006,2007 INRIA
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
#ifndef WIFI_REMOTE_STATION_MANAGER_H
#define WIFI_REMOTE_STATION_MANAGER_H

#include <vector>
#include <utility>
#include <map>
#include "ns3/mac48-address.h"
#include "ns3/packet.h"
#include "ns3/object.h"
#include "ns3/traced-value.h"
#include "ns3/wifi-mac-header.h"
#include "wifi-mode.h"
#include "qos-utils.h"

namespace ns3 {

class WifiRemoteStation;
class NonUnicastWifiRemoteStation;
class WifiPhy;

/**
 * \brief hold a list of per-remote-station state.
 *
 * \sa ns3::WifiRemoteStation.
 */
class WifiRemoteStationManager : public Object
{
private:
  typedef std::vector<WifiMode> BasicModes;
public:
  typedef BasicModes::const_iterator BasicModesIterator;

  static TypeId GetTypeId (void);

  WifiRemoteStationManager ();
  virtual ~WifiRemoteStationManager ();

  virtual void SetupPhy (Ptr<WifiPhy> phy);

  uint32_t GetMaxSsrc (void) const;
  uint32_t GetMaxSlrc (void) const;
  uint32_t GetRtsCtsThreshold (void) const;
  uint32_t GetFragmentationThreshold (void) const;
  void SetMaxSsrc (uint32_t maxSsrc);
  void SetMaxSlrc (uint32_t maxSlrc);
  void SetRtsCtsThreshold (uint32_t threshold);
  void SetFragmentationThreshold (uint32_t threshold);
  
  // Invoked in a STA upon dis-association
  // or in an AP upon reboot
  void Reset (void);
  // Invoked in a STA upon association to store
  // the set of rates which belong to the 
  // BSSBasicRateSet of the associated AP
  // and which are supported locally.
  // Invoked in an AP to configure the BSSBasicRateSet
  void AddBasicMode (WifiMode mode);

  WifiMode GetDefaultMode (void) const;
  uint32_t GetNBasicModes (void) const;
  WifiMode GetBasicMode (uint32_t i) const;
  BasicModesIterator BeginBasicModes (void) const;
  BasicModesIterator EndBasicModes (void) const;

  bool IsLowLatency (void) const;

  WifiMode GetNonUnicastMode (void) const;

  WifiRemoteStation *Lookup (Mac48Address address);
  WifiRemoteStation *LookupNonUnicast (void);
protected:
  friend class WifiRemoteStation;
  virtual void DoDispose (void);
  WifiRemoteStation *m_nonUnicast; 
private:
  typedef std::vector <WifiRemoteStation *> Stations;
  virtual class WifiRemoteStation *CreateStation (void) = 0;
  Stations m_stations;
  WifiMode m_defaultTxMode;
  BasicModes m_basicModes;
  bool m_isLowLatency;
  uint32_t m_maxSsrc;
  uint32_t m_maxSlrc;
  uint32_t m_rtsCtsThreshold;
  uint32_t m_fragmentationThreshold;
  WifiMode m_nonUnicastMode;


  /**
   * Public method used to fire a MacTxRtsFailed trace.
   * Implemented for encapsulation purposes.
   */
  void NotifyTxRtsFailed (Mac48Address address);  

  /**
   * Public method used to fire a MacTxDataFailed trace.
   * Implemented for encapsulation purposes. 
   */
  void NotifyTxDataFailed (Mac48Address address);  

  /**
   * Public method used to fire a MacTxFinalRtsFailed trace.
   * Implemented for encapsulation purposes.
   */
  void NotifyTxFinalRtsFailed (Mac48Address address);  

  /**
   * Public method used to fire a MacTxFinalDataFailed trace.
   * Implemented for encapsulation purposes. 
   */
  void NotifyTxFinalDataFailed (Mac48Address address);    


  /**
   * The trace source fired when the transmission of a RTS has failed
   *
   * \see class CallBackTraceSource
   */
  TracedCallback<Mac48Address> m_macTxRtsFailed;

  /**
   * The trace source fired when the transmission of a data packet has failed 
   *
   * \see class CallBackTraceSource
   */
  TracedCallback<Mac48Address> m_macTxDataFailed;

  /**
   * The trace source fired when the transmission of a RTS has
   * exceeded the maximum number of attempts
   *
   * \see class CallBackTraceSource
   */
  TracedCallback<Mac48Address> m_macTxFinalRtsFailed;

  /**
   * The trace source fired when the transmission of a data packet has
   * exceeded the maximum number of attempts
   *
   * \see class CallBackTraceSource
   */
  TracedCallback<Mac48Address> m_macTxFinalDataFailed;

};

} // namespace ns3

namespace ns3 {

/**
 * \brief hold per-remote-station state.
 *
 * The state in this class is used to keep track
 * of association status if we are in an infrastructure
 * network and to perform the selection of tx parameters
 * on a per-packet basis.
 */
class WifiRemoteStation {
public:
  
  static TypeId GetTypeId (void);
  
  WifiRemoteStation ();
  virtual ~WifiRemoteStation ();

  /**
   * Invoked in an AP upon disassociation of a
   * specific STA.
   */
  void Reset (void);
  /**
   * Invoked in a STA or AP to store the set of 
   * modes supported by a destination which is
   * also supported locally.
   * The set of supported modes includes
   * the BSSBasicRateSet.
   */
  void AddSupportedMode (WifiMode mode);

  bool IsBrandNew (void) const;
  bool IsAssociated (void) const;
  bool IsWaitAssocTxOk (void) const;
  void RecordWaitAssocTxOk (void);
  void RecordGotAssocTxOk (void);
  void RecordGotAssocTxFailed (void);
  void RecordDisassociated (void);

  /**
   * \param packet the packet to queue
   * \param fullPacketSize the size of the packet after its 802.11 MAC header has been added.
   *
   * This method is typically invoked just before queuing a packet for transmission.
   * It is a no-op unless the IsLowLatency attribute of the attached ns3::WifiRemoteStationManager
   * is set to false, in which case, the tx parameters of the packet are calculated and stored in
   * the packet as a tag. These tx parameters are later retrieved from GetDadaMode and GetRtsMode.
   */
  void PrepareForQueue (Ptr<const Packet> packet, uint32_t fullPacketSize);
  /**
   * \param packet the packet to send
   * \param fullPacketSize the size of the packet after its 802.11 MAC header has been added.
   * \returns the transmission mode to use to send this packet
   */
  WifiMode GetDataMode (Ptr<const Packet> packet, uint32_t fullPacketSize);
  /**
   * \param packet the packet to send
   * \returns the transmission mode to use to send the RTS prior to the
   *          transmission of the data packet itself.
   */
  WifiMode GetRtsMode (Ptr<const Packet> packet);
  /**
   * \param ac Represents access class of the packet for which a rts was transmitted.
   *
   * Should be invoked whenever the RtsTimeout associated to a transmission
   * attempt expires.
   */
  void ReportRtsFailed (enum AccessClass ac = AC_BE_NQOS);
  /**
   * \param ac Access class which failed data belongs.
   *
   * Should be invoked whenever the AckTimeout associated to a transmission
   * attempt expires.
   */
  void ReportDataFailed (enum AccessClass ac = AC_BE_NQOS);
  /**
   * \param ac Represents access class of the packet for which the rts was transmitted.
   *
   * Should be invoked whenever we receive the Cts associated to an RTS 
   * we just sent.
   */
  void ReportRtsOk (double ctsSnr, WifiMode ctsMode, double rtsSnr, enum AccessClass ac = AC_BE_NQOS);
  /**
   * \param ac Access class which correct received data belongs.
   *
   * Should be invoked whenever we receive the Ack associated to a data packet
   * we just sent.
   */
  void ReportDataOk (double ackSnr, WifiMode ackMode, double dataSnr, enum AccessClass ac = AC_BE_NQOS);
  /**
   * \param ac Represents access class of the packet for which a rts was transmitted.
   *
   * Should be invoked after calling ReportRtsFailed if 
   * NeedRtsRetransmission returns false
   */
  void ReportFinalRtsFailed (enum AccessClass ac = AC_BE_NQOS);
  /**
   * \param ac Access class which failed data belongs.
   *
   * Should be invoked after calling ReportDataFailed if 
   * NeedDataRetransmission returns false
   */
  void ReportFinalDataFailed (enum AccessClass ac = AC_BE_NQOS);

  /**
   * \param rxSnr the snr of the packet received
   * \param txMode the transmission mode used for the packet received.
   *
   * Should be invoked whenever a packet is successfully received.
   */
  void ReportRxOk (double rxSnr, WifiMode txMode, enum AccessClass ac = AC_BE_NQOS);

  /**
   * \param packet the packet to send
   * \returns true if we want to use an RTS/CTS handshake for this
   *          packet before sending it, false otherwise.
   */
  virtual bool NeedRts (Ptr<const Packet> packet);
  /**
   * \param hdr the header of the packet to send
   * \returns true if we want to restart a failed RTS/CTS 
   *          handshake, false otherwise.
   */
  virtual bool NeedRtsRetransmission (const WifiMacHeader *hdr);
  /**
   * \param hdr the header of the packet to send
   * \returns true if we want to resend a packet 
   *          after a failed transmission attempt, false otherwise.
   */
  virtual bool NeedDataRetransmission (const WifiMacHeader *hdr);

  /**
   * \param packet the packet to send
   * \returns true if this packet should be fragmented, false otherwise.
   */
  virtual bool NeedFragmentation (Ptr<const Packet> packet);
  /**
   * \param packet the packet to send
   * \param fragmentNumber the fragment index of the next fragment to send (starts at zero).
   * \returns the size of the corresponding fragment.
   */
  virtual uint32_t GetFragmentSize (Ptr<const Packet> packet, uint32_t fragmentNumber);
  /**
   * \param packet the packet to send
   * \param fragmentNumber the fragment index of the next fragment to send (starts at zero).
   * \returns the offset within the original packet where this fragment starts.
   */
  virtual uint32_t GetFragmentOffset (Ptr<const Packet> packet, uint32_t fragmentNumber);
  /**
   * \param packet the packet to send
   * \param fragmentNumber the fragment index of the next fragment to send (starts at zero).
   * \returns true if this is the last fragment, false otherwise.
   */
  virtual bool IsLastFragment (Ptr<const Packet> packet, uint32_t fragmentNumber);

  /**
   * \param rtsMode the transmission mode used to send an RTS we just received
   * \returns the transmission mode to use for the CTS to complete the RTS/CTS
   *          handshake.
   */
  WifiMode GetCtsMode (WifiMode rtsMode);
  /**
   * \param dataMode the transmission mode used to send an ACK we just received
   * \returns the transmission mode to use for the ACK to complete the data/ACK
   *          handshake.
   */
  WifiMode GetAckMode (WifiMode dataMode);
  /**
   * \return exponentially weighted average SLRC, this is used by Airtime link metric of 802.11s
   */
  double GetAvgSlrc () const;
  /** 
   * set the address of the remote stationt represented by this instance of WifiRemoteStation
   * 
   * @param address the MAC address of the remote station
   */
  void SetAddress(Mac48Address address);
  /** 
   * get the address of the remote stationt represented by this instance of WifiRemoteStation
   * 
   * @return the MAC address of the remote station
   */
  Mac48Address GetAddress();
private:
  virtual Ptr<WifiRemoteStationManager> GetManager (void) const = 0;
  virtual WifiMode DoGetDataMode (uint32_t size) = 0;
  virtual WifiMode DoGetRtsMode (void) = 0;
  virtual void DoReportRtsFailed (void) = 0;
  virtual void DoReportDataFailed (void) = 0;
  virtual void DoReportRtsOk (double ctsSnr, WifiMode ctsMode, double rtsSnr) = 0;
  virtual void DoReportDataOk (double ackSnr, WifiMode ackMode, double dataSnr) = 0;
  virtual void DoReportFinalRtsFailed (void) = 0;
  virtual void DoReportFinalDataFailed (void) = 0;
  virtual void DoReportRxOk (double rxSnr, WifiMode txMode) = 0;
protected:
  uint32_t GetNSupportedModes (void) const;
  WifiMode GetSupportedMode (uint32_t i) const;
private:
  typedef std::vector<WifiMode> SupportedModes;
  uint32_t GetNFragments (Ptr<const Packet> packet);
  bool IsIn (WifiMode mode) const;
  WifiMode GetControlAnswerMode (WifiMode reqMode);

  void UpdateSsrcCounter (enum AccessClass ac, bool reset);
  void UpdateSlrcCounter (enum AccessClass ac, bool reset);

  enum {
    BRAND_NEW,
    DISASSOC,
    WAIT_ASSOC_TX_OK,
    GOT_ASSOC_TX_OK
  } m_state;
  SupportedModes m_modes;
  TracedValue<uint32_t> m_ssrc;
  TracedValue<uint32_t> m_slrc;
  /* voice access class retry counters */
  TracedValue<uint32_t> m_voSsrc;
  TracedValue<uint32_t> m_voSlrc;
  /* video access class retry counters */
  TracedValue<uint32_t> m_viSsrc;
  TracedValue<uint32_t> m_viSlrc;
  /* best-effort access class retry counters */
  TracedValue<uint32_t> m_beSsrc;
  TracedValue<uint32_t> m_beSlrc;
  /* background access class retry counters */
  TracedValue<uint32_t> m_bkSsrc;
  TracedValue<uint32_t> m_bkSlrc;
  /* next two maps are defined for an easy access to qos retry counters */
  std::map<AccessClass, TracedValue<uint32_t>* > m_qosSsrc;
  std::map<AccessClass, TracedValue<uint32_t>* > m_qosSlrc;
  double m_avgSlrcCoefficient;
  double m_avgSlrc;
  Mac48Address m_address;
};

} // namespace ns3 

#endif /* MAC_STATIONS_H */
