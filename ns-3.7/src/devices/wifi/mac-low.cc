/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2005,2006 INRIA
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

#include "ns3/assert.h"
#include "ns3/packet.h"
#include "ns3/simulator.h"
#include "ns3/tag.h"
#include "ns3/log.h"
#include "ns3/node.h"

#include "mac-low.h"
#include "wifi-phy.h"
#include "wifi-mac-trailer.h"
#include "qos-utils.h"
#include "edca-txop-n.h"

#include "ns3/tx-power-tag.h"

NS_LOG_COMPONENT_DEFINE ("MacLow");

#undef NS_LOG_APPEND_CONTEXT
#define NS_LOG_APPEND_CONTEXT std::clog << "[mac=" << m_self << "] "


namespace ns3 {

class SnrTag : public Tag
{
public:

  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;

  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (TagBuffer i) const;
  virtual void Deserialize (TagBuffer i);
  virtual void Print (std::ostream &os) const;

  void Set (double snr);
  double Get (void) const;
private:
  double m_snr;
};

TypeId 
SnrTag::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::SnrTag")
    .SetParent<Tag> ()
    .AddConstructor<SnrTag> ()
    .AddAttribute ("Snr", "The snr of the last packet received",
                   DoubleValue (0.0),
                   MakeDoubleAccessor (&SnrTag::Get),
                   MakeDoubleChecker<double> ())
    ;
  return tid;
}
TypeId 
SnrTag::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t 
SnrTag::GetSerializedSize (void) const
{
  return sizeof (double);
}
void 
SnrTag::Serialize (TagBuffer i) const
{
  i.WriteDouble (m_snr);
}
void 
SnrTag::Deserialize (TagBuffer i)
{
  m_snr = i.ReadDouble ();
}
void 
SnrTag::Print (std::ostream &os) const
{
  os << "Snr=" << m_snr;
}
void 
SnrTag::Set (double snr)
{
  m_snr = snr;
}
double 
SnrTag::Get (void) const
{
  return m_snr;
}


MacLowTransmissionListener::MacLowTransmissionListener ()
{}
MacLowTransmissionListener::~MacLowTransmissionListener ()
{}
void
MacLowTransmissionListener::GotBlockAck (const CtrlBAckResponseHeader *blockAck,
                                         Mac48Address source)
{}
void
MacLowTransmissionListener::MissedBlockAck (void)
{}
MacLowDcfListener::MacLowDcfListener ()
{}
MacLowDcfListener::~MacLowDcfListener ()
{}

MacLowBlockAckEventListener::MacLowBlockAckEventListener ()
{}
MacLowBlockAckEventListener::~MacLowBlockAckEventListener ()
{}

MacLowTransmissionParameters::MacLowTransmissionParameters ()
  : m_nextSize (0),
    m_waitAck (ACK_NONE),
    m_sendRts (false),
    m_overrideDurationId (Seconds (0))
{}
void 
MacLowTransmissionParameters::EnableNextData (uint32_t size)
{
  m_nextSize = size;
}
void 
MacLowTransmissionParameters::DisableNextData (void)
{
  m_nextSize = 0;
}
void 
MacLowTransmissionParameters::EnableOverrideDurationId (Time durationId)
{
  m_overrideDurationId = durationId;
}
void 
MacLowTransmissionParameters::DisableOverrideDurationId (void)
{
  m_overrideDurationId = Seconds (0);
}
void 
MacLowTransmissionParameters::EnableSuperFastAck (void)
{
  m_waitAck = ACK_SUPER_FAST;
}
void
MacLowTransmissionParameters::EnableBasicBlockAck (void)
{
  m_waitAck = BLOCK_ACK_BASIC;
}
void
MacLowTransmissionParameters::EnableCompressedBlockAck (void)
{
  m_waitAck = BLOCK_ACK_COMPRESSED;
}
void
MacLowTransmissionParameters::EnableMultiTidBlockAck (void)
{
  m_waitAck = BLOCK_ACK_MULTI_TID;
}
void 
MacLowTransmissionParameters::EnableFastAck (void)
{
  m_waitAck = ACK_FAST;
}
void 
MacLowTransmissionParameters::EnableAck (void)
{
  m_waitAck = ACK_NORMAL;
}
void 
MacLowTransmissionParameters::DisableAck (void)
{
  m_waitAck = ACK_NONE;
}
void 
MacLowTransmissionParameters::EnableRts (void)
{
  m_sendRts = true;
}
void 
MacLowTransmissionParameters::DisableRts (void)
{
  m_sendRts = false;
}
bool 
MacLowTransmissionParameters::MustWaitAck (void) const
{
  return (m_waitAck != ACK_NONE);
}
bool 
MacLowTransmissionParameters::MustWaitNormalAck (void) const
{
  return (m_waitAck == ACK_NORMAL);
}
bool 
MacLowTransmissionParameters::MustWaitFastAck (void) const
{
  return (m_waitAck == ACK_FAST);
}
bool 
MacLowTransmissionParameters::MustWaitSuperFastAck (void) const
{
  return (m_waitAck == ACK_SUPER_FAST);
}
bool
MacLowTransmissionParameters::MustWaitBasicBlockAck (void) const
{
  return (m_waitAck == BLOCK_ACK_BASIC)?true:false;
}
bool
MacLowTransmissionParameters::MustWaitCompressedBlockAck (void) const
{
  return (m_waitAck == BLOCK_ACK_COMPRESSED)?true:false;
}
bool
MacLowTransmissionParameters::MustWaitMultiTidBlockAck (void) const
{
  return (m_waitAck == BLOCK_ACK_MULTI_TID)?true:false;
}
bool 
MacLowTransmissionParameters::MustSendRts (void) const
{
  return m_sendRts;
}
bool 
MacLowTransmissionParameters::HasDurationId (void) const
{
  return (m_overrideDurationId != Seconds (0));
}
Time
MacLowTransmissionParameters::GetDurationId (void) const
{
  NS_ASSERT (m_overrideDurationId != Seconds (0));
  return m_overrideDurationId;
}
bool 
MacLowTransmissionParameters::HasNextPacket (void) const
{
  return (m_nextSize != 0);
}
uint32_t 
MacLowTransmissionParameters::GetNextPacketSize (void) const
{
  NS_ASSERT (HasNextPacket ());
  return m_nextSize;
}

std::ostream &operator << (std::ostream &os, const MacLowTransmissionParameters &params)
{
  os << "[" 
     << "send rts=" << params.m_sendRts << ", "
     << "next size=" << params.m_nextSize << ", "
     << "dur=" << params.m_overrideDurationId << ", "
     << "ack=";
  switch (params.m_waitAck) {
  case MacLowTransmissionParameters::ACK_NONE:
    os << "none";
    break;
  case MacLowTransmissionParameters::ACK_NORMAL:
    os << "normal";
    break;
  case MacLowTransmissionParameters::ACK_FAST:
    os << "fast";
    break;
  case MacLowTransmissionParameters::ACK_SUPER_FAST:
    os << "super-fast";
    break;
  case MacLowTransmissionParameters::BLOCK_ACK_BASIC:
    os << "basic-block-ack";
    break;
  case MacLowTransmissionParameters::BLOCK_ACK_COMPRESSED:
    os << "compressed-block-ack";
    break;
  case MacLowTransmissionParameters::BLOCK_ACK_MULTI_TID:
    os << "multi-tid-block-ack";
    break;
  }
  os << "]";
  return os;
}


/***************************************************************
 *         Listener for PHY events. Forwards to MacLow
 ***************************************************************/


class PhyMacLowListener : public ns3::WifiPhyListener {
public:
  PhyMacLowListener (ns3::MacLow *macLow)
    : m_macLow (macLow) {}
  virtual ~PhyMacLowListener () {}
  virtual void NotifyRxStart (Time duration) {}
  virtual void NotifyRxEndOk (void) {}
  virtual void NotifyRxEndError (void) {}
  virtual void NotifyTxStart (Time duration) {}
  virtual void NotifyMaybeCcaBusyStart (Time duration) {}
  virtual void NotifySwitchingStart (Time duration) { 
    m_macLow->NotifySwitchingStartNow (duration);
  }
private:
  ns3::MacLow *m_macLow;
};


MacLow::MacLow ()
  : m_normalAckTimeoutEvent (),
    m_fastAckTimeoutEvent (),
    m_superFastAckTimeoutEvent (),
    m_fastAckFailedTimeoutEvent (),
    m_blockAckTimeoutEvent (),
    m_ctsTimeoutEvent (),
    m_sendCtsEvent (),
    m_sendAckEvent (),
    m_sendDataEvent (),
    m_waitSifsEvent (),
    m_currentPacket (0),
    m_listener (0)
{
  NS_LOG_FUNCTION (this);
  m_lastNavDuration = Seconds (0);
  m_lastNavStart = Seconds (0);
}

MacLow::~MacLow ()
{
  NS_LOG_FUNCTION (this);
}

void 
MacLow::SetupPhyMacLowListener (Ptr<WifiPhy> phy)
{
  m_phyMacLowListener = new PhyMacLowListener (this); 
  phy->RegisterListener (m_phyMacLowListener);
}

void
MacLow::ReportDataFailed (void) const
{
  WifiRemoteStation *station = GetStation (m_currentHdr.GetAddr1 ());
  if (m_currentHdr.IsQosData ())
    {
      uint8_t tid = m_currentHdr.GetQosTid ();
      station->ReportDataFailed (QosUtilsMapTidToAc (tid));
    }
  else
    {
      station->ReportDataFailed ();
    }
}

void
MacLow::ReportDataOk (double rxSnr, WifiMode txMode, SnrTag tagSnr) const
{
  WifiRemoteStation *station = GetStation (m_currentHdr.GetAddr1 ());
  if (m_currentHdr.IsQosData ())
    {
      uint8_t tid = m_currentHdr.GetQosTid ();
      station->ReportDataOk (rxSnr, txMode, tagSnr.Get (), QosUtilsMapTidToAc (tid));
    }
  else
    {
      station->ReportDataOk (rxSnr, txMode, tagSnr.Get ());
    }
}

void 
MacLow::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  m_normalAckTimeoutEvent.Cancel ();
  m_fastAckTimeoutEvent.Cancel ();
  m_superFastAckTimeoutEvent.Cancel ();
  m_fastAckFailedTimeoutEvent.Cancel ();
  m_blockAckTimeoutEvent.Cancel ();
  m_ctsTimeoutEvent.Cancel ();
  m_sendCtsEvent.Cancel ();
  m_sendAckEvent.Cancel ();
  m_sendDataEvent.Cancel ();
  m_waitSifsEvent.Cancel ();
  m_phy = 0;
  m_stationManager = 0;
  delete m_phyMacLowListener;
  m_phyMacLowListener = 0;
}

void
MacLow::CancelAllEvents (void)
{
  NS_LOG_FUNCTION (this);
  bool oneRunning = false;
  if (m_normalAckTimeoutEvent.IsRunning ()) 
    {
      m_normalAckTimeoutEvent.Cancel ();
      oneRunning = true;
    }
  if (m_fastAckTimeoutEvent.IsRunning ()) 
    {
      m_fastAckTimeoutEvent.Cancel ();
      oneRunning = true;
    }
  if (m_superFastAckTimeoutEvent.IsRunning ()) 
    {
      m_superFastAckTimeoutEvent.Cancel ();
      oneRunning = true;
    }
  if (m_fastAckFailedTimeoutEvent.IsRunning ()) 
    {
      m_fastAckFailedTimeoutEvent.Cancel ();
      oneRunning = true;
    }
  if (m_blockAckTimeoutEvent.IsRunning ())
    {
      m_blockAckTimeoutEvent.Cancel ();
      oneRunning = true;
    }
  if (m_ctsTimeoutEvent.IsRunning ()) 
    {
      m_ctsTimeoutEvent.Cancel ();
      oneRunning = true;
    }
  if (m_sendCtsEvent.IsRunning ()) 
    {
      m_sendCtsEvent.Cancel ();
      oneRunning = true;
    }
  if (m_sendAckEvent.IsRunning ()) 
    {
      m_sendAckEvent.Cancel ();
      oneRunning = true;
    }
  if (m_sendDataEvent.IsRunning ()) 
    {
      m_sendDataEvent.Cancel ();
      oneRunning = true;
    }
  if (m_waitSifsEvent.IsRunning ()) 
    {
      m_waitSifsEvent.Cancel ();
      oneRunning = true;
    }
  if (oneRunning && m_listener != 0) 
    {
      m_listener->Cancel ();
      m_listener = 0;
    }
}

void
MacLow::SetPhy (Ptr<WifiPhy> phy)
{
  m_phy = phy;
  m_phy->SetReceiveOkCallback (MakeCallback (&MacLow::ReceiveOk, this));
  m_phy->SetReceiveErrorCallback (MakeCallback (&MacLow::ReceiveError, this));
  SetupPhyMacLowListener(phy); 
}
void 
MacLow::SetWifiRemoteStationManager (Ptr<WifiRemoteStationManager> manager)
{
  m_stationManager = manager;
}

void 
MacLow::SetAddress (Mac48Address ad)
{
  m_self = ad;
}
void 
MacLow::SetAckTimeout (Time ackTimeout)
{
  m_ackTimeout = ackTimeout;
}
void
MacLow::SetBasicBlockAckTimeout (Time blockAckTimeout)
{
  m_basicBlockAckTimeout = blockAckTimeout;
}
void
MacLow::SetCompressedBlockAckTimeout (Time blockAckTimeout)
{
  m_compressedBlockAckTimeout = blockAckTimeout;
}
void 
MacLow::SetCtsTimeout (Time ctsTimeout)
{
  m_ctsTimeout = ctsTimeout;
}
void
MacLow::SetSifs (Time sifs)
{
  m_sifs = sifs;
}
void 
MacLow::SetSlotTime (Time slotTime)
{
  m_slotTime = slotTime;
}
void 
MacLow::SetPifs (Time pifs)
{
  m_pifs = pifs;
}
void
MacLow::SetBssid (Mac48Address bssid)
{
  m_bssid = bssid;
}
Mac48Address 
MacLow::GetAddress (void) const
{
  return m_self;
}
Time 
MacLow::GetAckTimeout (void) const
{
  return m_ackTimeout;
}
Time
MacLow::GetBasicBlockAckTimeout () const
{
  return m_basicBlockAckTimeout;
}
Time
MacLow::GetCompressedBlockAckTimeout () const
{
  return m_compressedBlockAckTimeout;
}
Time 
MacLow::GetCtsTimeout (void) const
{
  return m_ctsTimeout;
}
Time
MacLow::GetSifs (void) const
{
  return m_sifs;
}
Time 
MacLow::GetSlotTime (void) const
{
  return m_slotTime;
}
Time 
MacLow::GetPifs (void) const
{
  return m_pifs;
}
Mac48Address 
MacLow::GetBssid (void) const
{
  return m_bssid;
}

void 
MacLow::SetRxCallback (Callback<void,Ptr<Packet>,const WifiMacHeader *> callback)
{
  m_rxCallback = callback;
}
void 
MacLow::RegisterDcfListener (MacLowDcfListener *listener)
{
  m_dcfListeners.push_back (listener);
}


void 
MacLow::StartTransmission (Ptr<const Packet> packet, 
                           const WifiMacHeader* hdr, 
                           MacLowTransmissionParameters params,
                           MacLowTransmissionListener *listener)
{
  NS_LOG_FUNCTION (this << packet << hdr << params << listener);
  /* m_currentPacket is not NULL because someone started
   * a transmission and was interrupted before one of:
   *   - ctsTimeout
   *   - sendDataAfterCTS
   * expired. This means that one of these timers is still
   * running. They are all cancelled below anyway by the 
   * call to CancelAllEvents (because of at least one
   * of these two timer) which will trigger a call to the
   * previous listener's cancel method.
   *
   * This typically happens because the high-priority 
   * QapScheduler has taken access to the channel from
   * one of the Edca of the QAP.
   */
  m_currentPacket = packet->Copy ();
  m_currentHdr = *hdr;
  CancelAllEvents ();
  m_listener = listener;
  m_txParams = params;

  //NS_ASSERT (m_phy->IsStateIdle ());

  NS_LOG_DEBUG ("startTx size="<< GetSize (m_currentPacket, &m_currentHdr) << 
            ", to=" << m_currentHdr.GetAddr1()<<", listener="<<m_listener);

  if (m_txParams.MustSendRts ()) 
    {
      SendRtsForPacket ();
    } 
  else 
    {
      SendDataPacket ();
    }

  /* When this method completes, we have taken ownership of the medium. */
  NS_ASSERT (m_phy->IsStateTx ());  
}

void
MacLow::ReceiveError (Ptr<const Packet> packet, double rxSnr)
{
  NS_LOG_FUNCTION (this << packet << rxSnr);
  NS_LOG_DEBUG ("rx failed ");
  if (m_txParams.MustWaitFastAck ()) 
    {
      NS_ASSERT (m_fastAckFailedTimeoutEvent.IsExpired ());
      m_fastAckFailedTimeoutEvent = Simulator::Schedule (GetSifs (), 
                                                         &MacLow::FastAckFailedTimeout, this);
    }
  return;
}

void 
MacLow::NotifySwitchingStartNow (Time duration)
{
  NS_LOG_DEBUG ("switching channel. Cancelling MAC pending events"); 
  m_stationManager->Reset();
  CancelAllEvents(); 
  if (m_navCounterResetCtsMissed.IsRunning ())
    {
      m_navCounterResetCtsMissed.Cancel();
    }
  m_lastNavStart = Simulator::Now (); 
  m_lastNavDuration = Seconds (0);
  m_currentPacket = 0;
  m_listener = 0;
}

void 
MacLow::ReceiveOk (Ptr<Packet> packet, double rxSnr, WifiMode txMode, WifiPreamble preamble)
{
  NS_LOG_FUNCTION (this << packet << rxSnr << txMode << preamble);
  /* A packet is received from the PHY.
   * When we have handled this packet,
   * we handle any packet present in the
   * packet queue.
   */
  WifiMacHeader hdr;
  packet->RemoveHeader (hdr);
  
  bool isPrevNavZero = IsNavZero ();
  NS_LOG_DEBUG ("duration/id=" << hdr.GetDuration ());
  NotifyNav (hdr, txMode, preamble);
  if (hdr.IsRts ()) 
    {
      /* see section 9.2.5.7 802.11-1999
       * A STA that is addressed by an RTS frame shall transmit a CTS frame after a SIFS 
       * period if the NAV at the STA receiving the RTS frame indicates that the medium is 
       * idle. If the NAV at the STA receiving the RTS indicates the medium is not idle, 
       * that STA shall not respond to the RTS frame.
       */
      if (isPrevNavZero &&
          hdr.GetAddr1 () == m_self) 
        {
          NS_LOG_DEBUG ("rx RTS from=" << hdr.GetAddr2 () << ", schedule CTS");
          NS_ASSERT (m_sendCtsEvent.IsExpired ());
          WifiRemoteStation *station = GetStation (hdr.GetAddr2 ());
          station->ReportRxOk (rxSnr, txMode);
          m_sendCtsEvent = Simulator::Schedule (GetSifs (),
                                                &MacLow::SendCtsAfterRts, this,
                                                hdr.GetAddr2 (), 
                                                hdr.GetDuration (),
                                                txMode,
                                                rxSnr);
        } 
      else 
        {
          NS_LOG_DEBUG ("rx RTS from=" << hdr.GetAddr2 () << ", cannot schedule CTS");
        }
    } 
  else if (hdr.IsCts () &&
           hdr.GetAddr1 () == m_self &&
           m_ctsTimeoutEvent.IsRunning () &&
           m_currentPacket != 0) 
    {
      NS_LOG_DEBUG ("receive cts from="<<m_currentHdr.GetAddr1 ());
      SnrTag tag;
      packet->RemovePacketTag (tag);
      WifiRemoteStation *station = GetStation (m_currentHdr.GetAddr1 ());
      station->ReportRxOk (rxSnr, txMode);
      if (m_currentHdr.IsQosData ())
        {
          uint8_t tid = m_currentHdr.GetQosTid ();
          station->ReportRtsOk (rxSnr, txMode, tag.Get (), QosUtilsMapTidToAc (tid));
        }
      else
        {
          station->ReportRtsOk (rxSnr, txMode, tag.Get ());
        }
      m_ctsTimeoutEvent.Cancel ();
      NotifyCtsTimeoutResetNow ();
      m_listener->GotCts (rxSnr, txMode);
      NS_ASSERT (m_sendDataEvent.IsExpired ());
      m_sendDataEvent = Simulator::Schedule (GetSifs (), 
                                             &MacLow::SendDataAfterCts, this, 
                                             hdr.GetAddr1 (),
                                             hdr.GetDuration (),
                                             txMode);
    } 
  else if (hdr.IsAck () &&
           hdr.GetAddr1 () == m_self &&
           (m_normalAckTimeoutEvent.IsRunning () || 
            m_fastAckTimeoutEvent.IsRunning () ||
            m_superFastAckTimeoutEvent.IsRunning ()) &&
           m_txParams.MustWaitAck ()) 
    {
      NS_LOG_DEBUG ("receive ack from="<<m_currentHdr.GetAddr1 ());
      SnrTag tag;
      packet->RemovePacketTag (tag);
      WifiRemoteStation *station = GetStation (m_currentHdr.GetAddr1 ());
      station->ReportRxOk (rxSnr, txMode);
      ReportDataOk (rxSnr, txMode, tag);
      bool gotAck = false;
      if (m_txParams.MustWaitNormalAck () &&
          m_normalAckTimeoutEvent.IsRunning ()) 
        {
          m_normalAckTimeoutEvent.Cancel ();
          NotifyAckTimeoutResetNow ();
          gotAck = true;
        }
      if (m_txParams.MustWaitFastAck () &&
          m_fastAckTimeoutEvent.IsRunning ()) 
        {
          m_fastAckTimeoutEvent.Cancel ();
          NotifyAckTimeoutResetNow ();
          gotAck = true;
        }
      if (gotAck) 
        {
          m_listener->GotAck (rxSnr, txMode);
        }
      if (m_txParams.HasNextPacket ()) 
        {
          m_waitSifsEvent = Simulator::Schedule (GetSifs (), 
                                                 &MacLow::WaitSifsAfterEndTx, this);
        }
    } 
  else if (hdr.IsBlockAck () && hdr.GetAddr1 () == m_self &&
          (m_txParams.MustWaitBasicBlockAck () || m_txParams.MustWaitCompressedBlockAck ()) && 
           m_blockAckTimeoutEvent.IsRunning ())
    {
      NS_LOG_DEBUG ("got block ack from "<<hdr.GetAddr2 ());
      CtrlBAckResponseHeader blockAck;
      packet->RemoveHeader (blockAck);
      m_blockAckTimeoutEvent.Cancel ();
      m_listener->GotBlockAck (&blockAck, hdr.GetAddr2 ());
    }
  else if (hdr.IsBlockAckReq () && hdr.GetAddr1 () == m_self)
    {
      CtrlBAckRequestHeader blockAckReq;
      packet->RemoveHeader (blockAckReq);
      if (!blockAckReq.IsMultiTid ())
        {
          AgreementsI it = m_bAckAgreements.find (std::make_pair (hdr.GetAddr2 (), blockAckReq.GetTidInfo ()));
          if (it != m_bAckAgreements.end ())
            {
              NS_ASSERT (m_sendAckEvent.IsExpired ());
              /* See section 11.5.3 in IEEE802.11 for mean of this timer */
              ResetBlockAckInactivityTimerIfNeeded (it->second.first);
              if ((*it).second.first.IsImmediateBlockAck ())
                {
                  NS_LOG_DEBUG ("rx blockAckRequest/sendImmediateBlockAck from="<< hdr.GetAddr2 ());
                  m_sendAckEvent = Simulator::Schedule (GetSifs (),
                                                        &MacLow::SendBlockAckAfterBlockAckRequest, this,
                                                        blockAckReq,
                                                        hdr.GetAddr2 (), 
                                                        hdr.GetDuration (),
                                                        txMode);
                }
              else
                {
                  NS_FATAL_ERROR ("Delayed block ack not supported.");
                }
            }
          else
            {
              NS_LOG_DEBUG ("There's not a valid agreement for this block ack request.");
            }
        }
      else
        {
          NS_FATAL_ERROR ("Multi-tid block ack is not supported.");
        }
    }
  else if (hdr.IsCtl ()) 
    {
      NS_LOG_DEBUG ("rx drop " << hdr.GetTypeString ());
    } 
  else if (hdr.GetAddr1 () == m_self) 
    {
      WifiRemoteStation *station = GetStation (hdr.GetAddr2 ());
      station->ReportRxOk (rxSnr, txMode);
      
      if (hdr.IsQosData () && StoreMpduIfNeeded (packet, hdr)) 
        {
          /* From section 9.10.4 in IEEE802.11:
             Upon the receipt of a QoS data frame from the originator for which
             the Block Ack agreement exists, the recipient shall buffer the MSDU
             regardless of the value of the Ack Policy subfield within the 
             QoS Control field of the QoS data frame. */
          if (hdr.IsQosAck ())
            {
              AgreementsI it = m_bAckAgreements.find (std::make_pair (hdr.GetAddr2 (), hdr.GetQosTid ()));
              RxCompleteBufferedPacketsWithSmallerSequence (it->second.first.GetStartingSequence (),
                                                            hdr.GetAddr2 (), hdr.GetQosTid ());
              RxCompleteBufferedPackets (hdr.GetAddr2 (), hdr.GetQosTid ());
              NS_ASSERT (m_sendAckEvent.IsExpired ());
              m_sendAckEvent = Simulator::Schedule (GetSifs (),
                                                    &MacLow::SendAckAfterData, this,
                                                    hdr.GetAddr2 (), 
                                                    hdr.GetDuration (),
                                                    txMode,
                                                    rxSnr);
            }
          else if (hdr.IsQosBlockAck ())
            {
              AgreementsI it = m_bAckAgreements.find (std::make_pair (hdr.GetAddr2 (), hdr.GetQosTid ()));
              /* See section 11.5.3 in IEEE802.11 for mean of this timer */
              ResetBlockAckInactivityTimerIfNeeded (it->second.first);
            }
          return;  
        }
      else if (hdr.IsQosData () && hdr.IsQosBlockAck ())
        {
          /* This happens if a packet with ack policy Block Ack is received and a block ack
             agreement for that packet doesn't exist.

             From section 11.5.3 in IEEE802.11e:
             When a recipient does not have an active Block ack for a TID, but receives
             data MPDUs with the Ack Policy subfield set to Block Ack, it shall discard
             them and shall send a DELBA frame using the normal access 
             mechanisms. */
          AccessClass ac = QosUtilsMapTidToAc (hdr.GetQosTid ());
          m_edcaListeners[ac]->BlockAckInactivityTimeout (hdr.GetAddr2 (), hdr.GetQosTid ());
          return;
        }
      else if (hdr.IsQosData () && hdr.IsQosNoAck ()) 
        {
          NS_LOG_DEBUG ("rx unicast/noAck from="<<hdr.GetAddr2 ());
        } 
      else if (hdr.IsData () || hdr.IsMgt ()) 
        {
          NS_LOG_DEBUG ("rx unicast/sendAck from=" << hdr.GetAddr2 ());
          NS_ASSERT (m_sendAckEvent.IsExpired ());
          m_sendAckEvent = Simulator::Schedule (GetSifs (),
                                                &MacLow::SendAckAfterData, this,
                                                hdr.GetAddr2 (), 
                                                hdr.GetDuration (),
                                                txMode,
                                                rxSnr);
        }
      goto rxPacket;
    } 
  else if (hdr.GetAddr1 ().IsGroup ())
    {
      if (hdr.IsData () || hdr.IsMgt ())
        {
          NS_LOG_DEBUG ("rx group from=" << hdr.GetAddr2 ());
          goto rxPacket;
        }
      else
        {
          // DROP
        }
    }
  else 
    {
      //NS_LOG_DEBUG_VERBOSE ("rx not-for-me from %d", GetSource (packet));
    }
  return;
 rxPacket:
  WifiMacTrailer fcs;
  packet->RemoveTrailer (fcs);
  m_rxCallback (packet, &hdr);
  return;
}

uint32_t 
MacLow::GetAckSize (void) const
{
  WifiMacHeader ack;
  ack.SetType (WIFI_MAC_CTL_ACK);
  return ack.GetSize () + 4;
}
uint32_t
MacLow::GetBlockAckSize (enum BlockAckType type) const
{
  WifiMacHeader hdr;
  hdr.SetType (WIFI_MAC_CTL_BACKRESP);
  CtrlBAckResponseHeader blockAck;
  if (type == BASIC_BLOCK_ACK)
    {
      blockAck.SetType (BASIC_BLOCK_ACK);
    }
  else if (type == COMPRESSED_BLOCK_ACK)
    {
      blockAck.SetType (COMPRESSED_BLOCK_ACK);
    }
  else if (type == MULTI_TID_BLOCK_ACK)
    {
      //Not implemented
      NS_ASSERT (false);  
    }
  return hdr.GetSize () + blockAck.GetSerializedSize () + 4; 
}
uint32_t 
MacLow::GetRtsSize (void) const
{
  WifiMacHeader rts;
  rts.SetType (WIFI_MAC_CTL_RTS);
  return rts.GetSize () + 4;
}
Time
MacLow::GetAckDuration (Mac48Address to, WifiMode dataTxMode) const
{
  WifiMode ackMode = GetAckTxModeForData (to, dataTxMode);
  return m_phy->CalculateTxDuration (GetAckSize (), ackMode, WIFI_PREAMBLE_LONG);
}
Time
MacLow::GetBlockAckDuration (Mac48Address to, WifiMode blockAckReqTxMode, enum BlockAckType type) const
{
  /* 
   * For immediate BlockAck we should transmit the frame with the same WifiMode
   * as the BlockAckReq.
   *
   * from section 9.6 in IEEE802.11e:
   * The BlockAck control frame shall be sent at the same rate and modulation class as
   * the BlockAckReq frame if it is sent in response to a BlockAckReq frame.
   */
  return m_phy->CalculateTxDuration (GetBlockAckSize (type), blockAckReqTxMode, WIFI_PREAMBLE_LONG);
}
Time
MacLow::GetCtsDuration (Mac48Address to, WifiMode rtsTxMode) const
{
  WifiMode ctsMode = GetCtsTxModeForRts (to, rtsTxMode);
  return m_phy->CalculateTxDuration (GetCtsSize (), ctsMode, WIFI_PREAMBLE_LONG);
}
uint32_t 
MacLow::GetCtsSize (void) const
{
  WifiMacHeader cts;
  cts.SetType (WIFI_MAC_CTL_CTS);
  return cts.GetSize () + 4;
}
uint32_t 
MacLow::GetSize (Ptr<const Packet> packet, const WifiMacHeader *hdr) const
{
  WifiMacTrailer fcs;
  return packet->GetSize () + hdr->GetSize () + fcs.GetSerializedSize ();
}

WifiMode
MacLow::GetRtsTxMode (Ptr<const Packet> packet, const WifiMacHeader *hdr) const
{
  Mac48Address to = hdr->GetAddr1 ();
  return GetStation (to)->GetRtsMode (packet);
}
WifiMode
MacLow::GetDataTxMode (Ptr<const Packet> packet, const WifiMacHeader *hdr) const
{
  Mac48Address to = hdr->GetAddr1 ();
  WifiMacTrailer fcs;
  uint32_t size =  packet->GetSize () + hdr->GetSize () + fcs.GetSerializedSize ();
  return GetStation (to)->GetDataMode (packet, size);
}

WifiMode
MacLow::GetCtsTxModeForRts (Mac48Address to, WifiMode rtsTxMode) const
{
  return GetStation (to)->GetCtsMode (rtsTxMode);
}
WifiMode
MacLow::GetAckTxModeForData (Mac48Address to, WifiMode dataTxMode) const
{
  return GetStation (to)->GetAckMode (dataTxMode);
}


Time
MacLow::CalculateOverallTxTime (Ptr<const Packet> packet,
                                const WifiMacHeader* hdr, 
                                const MacLowTransmissionParameters& params) const
{
  Time txTime = Seconds (0);
  WifiMode rtsMode = GetRtsTxMode (packet, hdr);
  WifiMode dataMode = GetDataTxMode (packet, hdr);
  if (params.MustSendRts ()) 
    {
      txTime += m_phy->CalculateTxDuration (GetRtsSize (), rtsMode, WIFI_PREAMBLE_LONG);
      txTime += GetCtsDuration (hdr->GetAddr1 (), rtsMode);
      txTime += GetSifs () * Scalar (2);
    }
  uint32_t dataSize = GetSize (packet, hdr);
  txTime += m_phy->CalculateTxDuration (dataSize, dataMode, WIFI_PREAMBLE_LONG);
  if (params.MustWaitAck ())
    {
      txTime += GetSifs ();
      txTime += GetAckDuration (hdr->GetAddr1 (), dataMode);
    }
  return txTime;
}

Time
MacLow::CalculateTransmissionTime (Ptr<const Packet> packet,
                                   const WifiMacHeader* hdr, 
                                   const MacLowTransmissionParameters& params) const
{
  Time txTime = CalculateOverallTxTime (packet, hdr, params);
  if (params.HasNextPacket ()) 
    {
      WifiMode dataMode = GetDataTxMode (packet, hdr);
      txTime += GetSifs ();
      txTime += m_phy->CalculateTxDuration (params.GetNextPacketSize (), dataMode, WIFI_PREAMBLE_LONG);
    }
  return txTime;
}

void
MacLow::NotifyNav (const WifiMacHeader &hdr, WifiMode txMode, WifiPreamble preamble)
{
  NS_ASSERT (m_lastNavStart <= Simulator::Now ());
  Time duration = hdr.GetDuration ();

  if (hdr.IsCfpoll () &&
      hdr.GetAddr2 () == m_bssid) 
    {
      // see section 9.3.2.2 802.11-1999
      DoNavResetNow (duration);
      return;
    }
  // XXX Note that we should also handle CF_END specially here
  // but we don't for now because we do not generate them.
  else if (hdr.GetAddr1 () != m_self)
    {
      // see section 9.2.5.4 802.11-1999
      bool navUpdated = DoNavStartNow (duration);
      if (hdr.IsRts () && navUpdated)
        {
          /**
           * A STA that used information from an RTS frame as the most recent basis to update its NAV setting 
           * is permitted to reset its NAV if no PHY-RXSTART.indication is detected from the PHY during a 
           * period with a duration of (2 * aSIFSTime) + (CTS_Time) + (2 * aSlotTime) starting at the 
           * PHY-RXEND.indication corresponding to the detection of the RTS frame. The “CTS_Time” shall 
           * be calculated using the length of the CTS frame and the data rate at which the RTS frame 
           * used for the most recent NAV update was received.
           */
          WifiMacHeader cts;
          cts.SetType (WIFI_MAC_CTL_CTS);
          Time navCounterResetCtsMissedDelay = 
            m_phy->CalculateTxDuration (cts.GetSerializedSize (), txMode, preamble) +
            Scalar (2) * GetSifs () + Scalar (2) * GetSlotTime ();
          m_navCounterResetCtsMissed = Simulator::Schedule (navCounterResetCtsMissedDelay,
                                                            &MacLow::NavCounterResetCtsMissed, this,
                                                            Simulator::Now ());
        }
    }
}

void
MacLow::NavCounterResetCtsMissed (Time rtsEndRxTime)
{
  if (m_phy->GetLastRxStartTime () > rtsEndRxTime)
    {
      DoNavResetNow (Seconds (0.0));
    }
}

void
MacLow::DoNavResetNow (Time duration)
{
  for (DcfListenersCI i = m_dcfListeners.begin (); i != m_dcfListeners.end (); i++) 
    {
      (*i)->NavReset (duration);
    }
  m_lastNavStart = Simulator::Now ();
  m_lastNavStart = duration;
}
bool
MacLow::DoNavStartNow (Time duration)
{
  for (DcfListenersCI i = m_dcfListeners.begin (); i != m_dcfListeners.end (); i++) 
    {
      (*i)->NavStart (duration);
    }
  Time newNavEnd = Simulator::Now () + duration;
  Time oldNavEnd = m_lastNavStart + m_lastNavDuration;
  if (newNavEnd > oldNavEnd)
    {
      m_lastNavStart = Simulator::Now ();
      m_lastNavDuration = duration;
      return true;
    }
  return false;
}
void
MacLow::NotifyAckTimeoutStartNow (Time duration)
{
  for (DcfListenersCI i = m_dcfListeners.begin (); i != m_dcfListeners.end (); i++) 
    {
      (*i)->AckTimeoutStart (duration);
    }
}
void
MacLow::NotifyAckTimeoutResetNow ()
{
  for (DcfListenersCI i = m_dcfListeners.begin (); i != m_dcfListeners.end (); i++) 
    {
      (*i)->AckTimeoutReset ();
    }
}
void
MacLow::NotifyCtsTimeoutStartNow (Time duration)
{
  for (DcfListenersCI i = m_dcfListeners.begin (); i != m_dcfListeners.end (); i++) 
    {
      (*i)->CtsTimeoutStart (duration);
    }
}
void
MacLow::NotifyCtsTimeoutResetNow ()
{
  for (DcfListenersCI i = m_dcfListeners.begin (); i != m_dcfListeners.end (); i++) 
    {
      (*i)->CtsTimeoutReset ();
    }
}

void
MacLow::ForwardDown (Ptr<const Packet> packet, const WifiMacHeader* hdr, 
                     WifiMode txMode)
{
  NS_LOG_FUNCTION (this << packet << hdr << txMode);
  NS_LOG_DEBUG ("send " << hdr->GetTypeString () <<
            ", to=" << hdr->GetAddr1 () <<
            ", size=" << packet->GetSize () <<
            ", mode=" << txMode <<
            ", duration=" << hdr->GetDuration () <<
            ", seq=0x"<< std::hex << m_currentHdr.GetSequenceControl () << std::dec);
  //m_phy->SendPacket (packet, txMode, WIFI_PREAMBLE_LONG, 0);
  m_phy->SendPacket (packet, txMode, WIFI_PREAMBLE_LONG, GetTxPower(packet));
}

uint8_t
MacLow::GetTxPower(Ptr<const Packet> packet)
{
  TxPowerTag txPower;
  uint8_t power = 0;
  if (packet->PeekPacketTag (txPower))
     {
       if (txPower.Get () <= m_phy->GetNTxPower ())
         {
           power = txPower.Get ();
         }
     }
  return power;
}

void
MacLow::CtsTimeout (void)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_DEBUG ("cts timeout");
  // XXX: should check that there was no rx start before now.
  // we should restart a new cts timeout now until the expected
  // end of rx if there was a rx start before now.
  WifiRemoteStation *station = GetStation (m_currentHdr.GetAddr1 ());
  if (m_currentHdr.IsQosData ())
    {
      uint8_t tid = m_currentHdr.GetQosTid ();
      station->ReportRtsFailed (QosUtilsMapTidToAc (tid));
    }
  else
    {
      station->ReportRtsFailed ();
    }
  m_currentPacket = 0;
  MacLowTransmissionListener *listener = m_listener;
  m_listener = 0;
  listener->MissedCts ();
}
void
MacLow::NormalAckTimeout (void)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_DEBUG ("normal ack timeout");
  // XXX: should check that there was no rx start before now.
  // we should restart a new ack timeout now until the expected
  // end of rx if there was a rx start before now.
  ReportDataFailed ();
  MacLowTransmissionListener *listener = m_listener;
  m_listener = 0;
  listener->MissedAck ();
}
void
MacLow::FastAckTimeout (void)
{
  NS_LOG_FUNCTION (this);
  ReportDataFailed ();
  MacLowTransmissionListener *listener = m_listener;
  m_listener = 0;
  if (m_phy->IsStateIdle ()) 
    {
      NS_LOG_DEBUG ("fast Ack idle missed");
      listener->MissedAck ();
    }
  else
    {
      NS_LOG_DEBUG ("fast Ack ok");
    }
}
void
MacLow::BlockAckTimeout (void)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_DEBUG ("block ack timeout");
  
  WifiRemoteStation *station = GetStation (m_currentHdr.GetAddr1 ());
  station->ReportDataFailed ();
  MacLowTransmissionListener *listener = m_listener;
  m_listener = 0;
  listener->MissedBlockAck ();
}
void
MacLow::SuperFastAckTimeout ()
{
  NS_LOG_FUNCTION (this);
  ReportDataFailed ();
  MacLowTransmissionListener *listener = m_listener;
  m_listener = 0;
  if (m_phy->IsStateIdle ()) 
    {
      NS_LOG_DEBUG ("super fast Ack failed");
      listener->MissedAck ();
    } 
  else 
    {
      NS_LOG_DEBUG ("super fast Ack ok");
      listener->GotAck (0.0, WifiMode ());
    }
}

void
MacLow::SendRtsForPacket (void)
{
  NS_LOG_FUNCTION (this);
  /* send an RTS for this packet. */
  WifiMacHeader rts;
  rts.SetType (WIFI_MAC_CTL_RTS);
  rts.SetDsNotFrom ();
  rts.SetDsNotTo ();
  rts.SetNoRetry ();
  rts.SetNoMoreFragments ();
  rts.SetAddr1 (m_currentHdr.GetAddr1 ());
  rts.SetAddr2 (m_self);
  WifiMode rtsTxMode = GetRtsTxMode (m_currentPacket, &m_currentHdr);
  Time duration = Seconds (0);
  if (m_txParams.HasDurationId ()) 
    {
      duration += m_txParams.GetDurationId ();
    } 
  else 
    {
      WifiMode dataTxMode = GetDataTxMode (m_currentPacket, &m_currentHdr);
      duration += GetSifs ();
      duration += GetCtsDuration (m_currentHdr.GetAddr1 (), rtsTxMode);
      duration += GetSifs ();
      duration += m_phy->CalculateTxDuration (GetSize (m_currentPacket, &m_currentHdr), 
                                              dataTxMode, WIFI_PREAMBLE_LONG);
      duration += GetSifs ();
      duration += GetAckDuration (m_currentHdr.GetAddr1 (), dataTxMode);
    }
  rts.SetDuration (duration);

  Time txDuration = m_phy->CalculateTxDuration (GetRtsSize (), rtsTxMode, WIFI_PREAMBLE_LONG);
  Time timerDelay = txDuration + GetCtsTimeout ();

  NS_ASSERT (m_ctsTimeoutEvent.IsExpired ());
  NotifyCtsTimeoutStartNow (timerDelay);
  m_ctsTimeoutEvent = Simulator::Schedule (timerDelay, &MacLow::CtsTimeout, this);

  Ptr<Packet> packet = Create<Packet> ();
  packet->AddHeader (rts);
  WifiMacTrailer fcs;
  packet->AddTrailer (fcs);

  ForwardDown (packet, &rts, rtsTxMode);
}

void
MacLow::StartDataTxTimers (void)
{
  WifiMode dataTxMode = GetDataTxMode (m_currentPacket, &m_currentHdr);
  Time txDuration = m_phy->CalculateTxDuration (GetSize (m_currentPacket, &m_currentHdr), dataTxMode, WIFI_PREAMBLE_LONG);
  if (m_txParams.MustWaitNormalAck ()) 
    {
      Time timerDelay = txDuration + GetAckTimeout ();
      NS_ASSERT (m_normalAckTimeoutEvent.IsExpired ());
      NotifyAckTimeoutStartNow (timerDelay);
      m_normalAckTimeoutEvent = Simulator::Schedule (timerDelay, &MacLow::NormalAckTimeout, this);
    } 
  else if (m_txParams.MustWaitFastAck ()) 
    {
      Time timerDelay = txDuration + GetPifs ();
      NS_ASSERT (m_fastAckTimeoutEvent.IsExpired ());
      NotifyAckTimeoutStartNow (timerDelay);
      m_fastAckTimeoutEvent = Simulator::Schedule (timerDelay, &MacLow::FastAckTimeout, this);
    } 
  else if (m_txParams.MustWaitSuperFastAck ()) 
    {
      Time timerDelay = txDuration + GetPifs ();
      NS_ASSERT (m_superFastAckTimeoutEvent.IsExpired ());
      NotifyAckTimeoutStartNow (timerDelay);
      m_superFastAckTimeoutEvent = Simulator::Schedule (timerDelay, 
                                                        &MacLow::SuperFastAckTimeout, this);
    }
  else if (m_txParams.MustWaitBasicBlockAck ())
    {
      Time timerDelay = txDuration + GetBasicBlockAckTimeout ();
      NS_ASSERT (m_blockAckTimeoutEvent.IsExpired ());
      m_blockAckTimeoutEvent = Simulator::Schedule (timerDelay, &MacLow::BlockAckTimeout, this);
    }
  else if (m_txParams.MustWaitCompressedBlockAck ())
    {
      Time timerDelay = txDuration + GetCompressedBlockAckTimeout ();
      NS_ASSERT (m_blockAckTimeoutEvent.IsExpired ());
      m_blockAckTimeoutEvent = Simulator::Schedule (timerDelay, &MacLow::BlockAckTimeout, this);
    }
  else if (m_txParams.HasNextPacket ()) 
    {
      Time delay = txDuration + GetSifs ();
      NS_ASSERT (m_waitSifsEvent.IsExpired ());
      m_waitSifsEvent = Simulator::Schedule (delay, &MacLow::WaitSifsAfterEndTx, this);
    } 
  else 
    {
      // since we do not expect any timer to be triggered.
      m_listener = 0;
    }
}

void
MacLow::SendDataPacket (void)
{
  NS_LOG_FUNCTION (this);
  /* send this packet directly. No RTS is needed. */
  StartDataTxTimers ();

  WifiMode dataTxMode = GetDataTxMode (m_currentPacket, &m_currentHdr);
  Time duration = Seconds (0.0);
  if (m_txParams.HasDurationId ()) 
    {
      duration += m_txParams.GetDurationId ();
    } 
  else 
    {
      if (m_txParams.MustWaitBasicBlockAck ())
        {
          duration += GetSifs ();
          duration += GetBlockAckDuration (m_currentHdr.GetAddr1 (), dataTxMode, BASIC_BLOCK_ACK);
        }
      else if (m_txParams.MustWaitCompressedBlockAck ())
        {
          duration += GetSifs ();
          duration += GetBlockAckDuration (m_currentHdr.GetAddr1 (), dataTxMode, COMPRESSED_BLOCK_ACK);
        }
      else if (m_txParams.MustWaitAck ()) 
        {
          duration += GetSifs ();
          duration += GetAckDuration (m_currentHdr.GetAddr1 (), dataTxMode);
        }
      if (m_txParams.HasNextPacket ()) 
        {
          duration += GetSifs ();
          duration += m_phy->CalculateTxDuration (m_txParams.GetNextPacketSize (), 
                                                  dataTxMode, WIFI_PREAMBLE_LONG);
          if (m_txParams.MustWaitAck ()) 
            {
              duration += GetSifs ();
              duration += GetAckDuration (m_currentHdr.GetAddr1 (), dataTxMode);
            }
        }
    }
  m_currentHdr.SetDuration (duration);

  m_currentPacket->AddHeader (m_currentHdr);
  WifiMacTrailer fcs;
  m_currentPacket->AddTrailer (fcs);

  ForwardDown (m_currentPacket, &m_currentHdr, dataTxMode);
  m_currentPacket = 0;
}

bool 
MacLow::IsNavZero (void) const
{
  if (m_lastNavStart + m_lastNavDuration < Simulator::Now ()) 
    {
      return true;
    } 
  else 
    {
      return false;
    }
}

WifiRemoteStation *
MacLow::GetStation (Mac48Address ad) const
{
  return m_stationManager->Lookup (ad);
}

void
MacLow::SendCtsAfterRts (Mac48Address source, Time duration, WifiMode rtsTxMode, double rtsSnr)
{
  NS_LOG_FUNCTION (this << source << duration << rtsTxMode << rtsSnr);
  /* send a CTS when you receive a RTS 
   * right after SIFS.
   */
  WifiMode ctsTxMode = GetCtsTxModeForRts (source, rtsTxMode);
  WifiMacHeader cts;
  cts.SetType (WIFI_MAC_CTL_CTS);
  cts.SetDsNotFrom ();
  cts.SetDsNotTo ();
  cts.SetNoMoreFragments ();
  cts.SetNoRetry ();
  cts.SetAddr1 (source);
  duration -= GetCtsDuration (source, rtsTxMode);
  duration -= GetSifs ();
  NS_ASSERT (duration >= MicroSeconds (0));
  cts.SetDuration (duration);

  Ptr<Packet> packet = Create<Packet> ();
  packet->AddHeader (cts);
  WifiMacTrailer fcs;
  packet->AddTrailer (fcs);

  struct SnrTag tag;
  tag.Set (rtsSnr);
  packet->AddPacketTag (tag);

  ForwardDown (packet, &cts, ctsTxMode);
}

void
MacLow::SendDataAfterCts (Mac48Address source, Time duration, WifiMode txMode)
{
  NS_LOG_FUNCTION (this);
  /* send the third step in a 
   * RTS/CTS/DATA/ACK hanshake 
   */
  NS_ASSERT (m_currentPacket != 0);
  StartDataTxTimers ();

  WifiMode dataTxMode = GetDataTxMode (m_currentPacket, &m_currentHdr);
  Time newDuration = Seconds (0);
  newDuration += GetSifs ();
  newDuration += GetAckDuration (m_currentHdr.GetAddr1 (), dataTxMode);
  Time txDuration = m_phy->CalculateTxDuration (GetSize (m_currentPacket, &m_currentHdr), 
                                                dataTxMode, WIFI_PREAMBLE_LONG);
  duration -= txDuration;
  duration -= GetSifs ();

  duration = std::max (duration, newDuration);
  NS_ASSERT (duration >= MicroSeconds (0));
  m_currentHdr.SetDuration (duration);

  m_currentPacket->AddHeader (m_currentHdr);
  WifiMacTrailer fcs;
  m_currentPacket->AddTrailer (fcs);

  ForwardDown (m_currentPacket, &m_currentHdr, dataTxMode);
  m_currentPacket = 0;
}

void 
MacLow::WaitSifsAfterEndTx (void)
{
  m_listener->StartNext ();
}

void
MacLow::FastAckFailedTimeout (void)
{
  NS_LOG_FUNCTION (this);
  MacLowTransmissionListener *listener = m_listener;
  m_listener = 0;
  listener->MissedAck ();
  NS_LOG_DEBUG ("fast Ack busy but missed");
}

void
MacLow::SendAckAfterData (Mac48Address source, Time duration, WifiMode dataTxMode, double dataSnr)
{
  NS_LOG_FUNCTION (this);
  /* send an ACK when you receive 
   * a packet after SIFS. 
   */
  WifiMode ackTxMode = GetAckTxModeForData (source, dataTxMode);
  WifiMacHeader ack;
  ack.SetType (WIFI_MAC_CTL_ACK);
  ack.SetDsNotFrom ();
  ack.SetDsNotTo ();
  ack.SetNoRetry ();
  ack.SetNoMoreFragments ();
  ack.SetAddr1 (source);
  duration -= GetAckDuration (source, dataTxMode);
  duration -= GetSifs ();
  NS_ASSERT (duration >= MicroSeconds (0));
  ack.SetDuration (duration);

  Ptr<Packet> packet = Create<Packet> ();
  packet->AddHeader (ack);
  WifiMacTrailer fcs;
  packet->AddTrailer (fcs);

  struct SnrTag tag;
  tag.Set (dataSnr);
  packet->AddPacketTag (tag);

  ForwardDown (packet, &ack, ackTxMode);
}

bool
MacLow::StoreMpduIfNeeded (Ptr<Packet> packet, WifiMacHeader hdr)
{
  AgreementsI it = m_bAckAgreements.find (std::make_pair (hdr.GetAddr2 (), hdr.GetQosTid ()));
  if (it != m_bAckAgreements.end ())
   {
     WifiMacTrailer fcs;
     packet->RemoveTrailer (fcs);
     BufferedPacket bufferedPacket (packet, hdr);

     uint16_t endSequence = ((*it).second.first.GetStartingSequence () + 2047) % 4096;
     uint16_t mappedSeqControl = QosUtilsMapSeqControlToUniqueInteger (hdr.GetSequenceControl () ,endSequence);

     BufferedPacketI i = (*it).second.second.begin ();
     for (; i != (*it).second.second.end () &&
            QosUtilsMapSeqControlToUniqueInteger ((*i).second.GetSequenceControl (), endSequence) < mappedSeqControl; i++);
     (*it).second.second.insert (i, bufferedPacket);
     return true;
   }
  return false;
}

void
MacLow::CreateBlockAckAgreement (const MgtAddBaResponseHeader *respHdr, Mac48Address originator,
                                 uint16_t startingSeq)
{
  uint8_t tid = respHdr->GetTid ();
  BlockAckAgreement agreement (originator, tid);
  if (respHdr->IsImmediateBlockAck ())
    {
      agreement.SetImmediateBlockAck ();
    }
  else
    {
      agreement.SetDelayedBlockAck ();
    }
  agreement.SetAmsduSupport (respHdr->IsAmsduSupported ());
  agreement.SetBufferSize (respHdr->GetBufferSize ());
  agreement.SetTimeout (respHdr->GetTimeout ());
  agreement.SetStartingSequence (startingSeq);
  
  std::list<BufferedPacket> buffer (0);
  AgreementKey key (originator, respHdr->GetTid ());
  AgreementValue value (agreement, buffer);
  m_bAckAgreements.insert (std::make_pair (key, value));
  
  if (respHdr->GetTimeout () != 0)
    {
      AgreementsI it = m_bAckAgreements.find (std::make_pair (originator, respHdr->GetTid ()));
      Time timeout = MicroSeconds (1024 * agreement.GetTimeout ());
 
      AccessClass ac = QosUtilsMapTidToAc (agreement.GetTid ());
      
      it->second.first.m_inactivityEvent = Simulator::Schedule (timeout,
                                                                &MacLowBlockAckEventListener::BlockAckInactivityTimeout,
                                                                m_edcaListeners[ac],
                                                                originator, tid);
    }
}

void
MacLow::DestroyBlockAckAgreement (Mac48Address originator, uint8_t tid)
{
  AgreementsI it = m_bAckAgreements.find (std::make_pair (originator, tid));
  if (it != m_bAckAgreements.end ())
    {
      RxCompleteBufferedPacketsWithSmallerSequence (it->second.first.GetStartingSequence (), originator, tid);
      RxCompleteBufferedPackets (originator, tid);
      m_bAckAgreements.erase (it);
    }
}

void
MacLow::RxCompleteBufferedPacketsWithSmallerSequence (uint16_t seq, Mac48Address originator, uint8_t tid)
{
  AgreementsI it = m_bAckAgreements.find (std::make_pair (originator, tid));
  if (it != m_bAckAgreements.end ())
    {
      BufferedPacketI i = (*it).second.second.begin ();
      uint16_t endSequence = ((*it).second.first.GetStartingSequence () + 2047) % 4096;
      uint16_t mappedStart = QosUtilsMapSeqControlToUniqueInteger (seq, endSequence);
      uint16_t guard = (*it).second.first.GetStartingSequence ();
      BufferedPacketI last = (*it).second.second.begin ();

      for (; i != (*it).second.second.end () &&
             QosUtilsMapSeqControlToUniqueInteger ((*i).second.GetSequenceNumber (), endSequence) < mappedStart;)
        {
          while (i != (*it).second.second.end () && guard == (*i).second.GetSequenceControl ())
            {
              if (!(*i).second.IsMoreFragments ())
                {
                  while (last != i)
                    {
                      m_rxCallback ((*last).first, &(*last).second);
                      last++;
                    }
                  m_rxCallback ((*last).first, &(*last).second);
                  last++;
                }
              guard = (*i).second.IsMoreFragments () ? (guard + 1) : ((guard + 16) & 0xfff0);
            }
          /* go to next packet */
          while (i != (*it).second.second.end () && ((guard >> 4) & 0x0fff) == (*i).second.GetSequenceNumber ())
            {
              i++;
            }
          if (i != (*it).second.second.end ())
            {
              guard = (*i).second.GetSequenceControl () & 0xfff0;
              last = i;
            }
        }
      (*it).second.second.erase ((*it).second.second.begin (), i);
    }
}

void
MacLow::RxCompleteBufferedPackets (Mac48Address originator, uint8_t tid)
{
  AgreementsI it = m_bAckAgreements.find (std::make_pair (originator, tid));
  if (it != m_bAckAgreements.end ())
    {
      uint16_t startingSeqCtrl = ((*it).second.first.GetStartingSequence ()<<4) & 0xfff0;
      uint16_t guard = startingSeqCtrl;

      BufferedPacketI lastComplete = (*it).second.second.begin ();
      BufferedPacketI i = (*it).second.second.begin ();
      for (;i != (*it).second.second.end() && guard == (*i).second.GetSequenceControl (); i++)
        {
          if (!(*i).second.IsMoreFragments ())
            {
              while (lastComplete != i)
                {
                  m_rxCallback ((*lastComplete).first, &(*lastComplete).second);
                  lastComplete++;
                }
               m_rxCallback ((*lastComplete).first, &(*lastComplete).second);
               lastComplete++;
            }
          guard = (*i).second.IsMoreFragments () ? (guard + 1) : ((guard + 16) & 0xfff0);
        }
      (*it).second.first.SetStartingSequence ((guard>>4)&0x0fff);
      /* All packets already forwarded to WifiMac must be removed from buffer: 
      [begin (), lastComplete) */
      (*it).second.second.erase ((*it).second.second.begin (), lastComplete);
    }
}

void
MacLow::SendBlockAckResponse (const CtrlBAckResponseHeader* blockAck, Mac48Address originator, bool immediate,
                              Time duration, WifiMode blockAckReqTxMode)
{
  Ptr<Packet> packet = Create<Packet> ();
  packet->AddHeader (*blockAck);

  WifiMacHeader hdr;
  hdr.SetType (WIFI_MAC_CTL_BACKRESP);
  hdr.SetAddr1 (originator);
  hdr.SetAddr2 (GetAddress ());
  hdr.SetDsNotFrom ();
  hdr.SetDsNotTo ();
  hdr.SetNoRetry ();
  hdr.SetNoMoreFragments ();

  m_currentPacket = packet;
  m_currentHdr = hdr;
  if (immediate)
    {
      m_txParams.DisableAck ();
      duration -= GetSifs ();
      if (blockAck->IsBasic ())
        {
          duration -= GetBlockAckDuration (originator, blockAckReqTxMode, BASIC_BLOCK_ACK);
        }
      else if (blockAck->IsCompressed ())
        {
          duration -= GetBlockAckDuration (originator, blockAckReqTxMode, COMPRESSED_BLOCK_ACK);
        }
      else if (blockAck->IsMultiTid ())
        {
          NS_FATAL_ERROR ("Multi-tid block ack is not supported.");
        }
    }
  else
    {
      m_txParams.EnableAck ();
      duration += GetSifs ();
      duration += GetAckDuration (originator, blockAckReqTxMode);
    }
  m_txParams.DisableNextData ();

  StartDataTxTimers ();

  NS_ASSERT (duration >= MicroSeconds (0));
  hdr.SetDuration (duration);
  //here should be present a control about immediate or delayed block ack
  //for now we assume immediate
  packet->AddHeader (hdr);
  WifiMacTrailer fcs;
  packet->AddTrailer (fcs);
  ForwardDown (packet, &hdr, blockAckReqTxMode);
  m_currentPacket = 0;
}

void
MacLow::SendBlockAckAfterBlockAckRequest (const CtrlBAckRequestHeader reqHdr, Mac48Address originator,
                                          Time duration, WifiMode blockAckReqTxMode)
{
  NS_LOG_FUNCTION (this);
  CtrlBAckResponseHeader blockAck;
  uint8_t tid;
  bool immediate = false;
  if (!reqHdr.IsMultiTid ())
    {
      blockAck.SetStartingSequence (reqHdr.GetStartingSequence ());
      blockAck.SetTidInfo (reqHdr.GetTidInfo ());
      
      tid = reqHdr.GetTidInfo ();
      AgreementsI it;
      it = m_bAckAgreements.find (std::make_pair (originator, tid));
      if (it != m_bAckAgreements.end ())
        {
          immediate = (*it).second.first.IsImmediateBlockAck ();
          uint16_t startingSeqCtrl = reqHdr.GetStartingSequenceControl ();

          /* All packets with smaller sequence than starting sequence control must be passed up to Wifimac 
           * See 9.10.3 in IEEE8022.11e standard.
           */
          RxCompleteBufferedPacketsWithSmallerSequence ((startingSeqCtrl>>4)&0xfff0, originator, tid);

          std::list<BufferedPacket>::iterator i = (*it).second.second.begin ();

          /* For more details about next operations see section 9.10.4 of IEEE802.11e standard */
          if (reqHdr.IsBasic ())
            {
              blockAck.SetType (BASIC_BLOCK_ACK);
              uint16_t guard = startingSeqCtrl;
              std::list<BufferedPacket>::iterator lastComplete = (*it).second.second.begin ();
              for (; i != (*it).second.second.end () && guard == (*i).second.GetSequenceControl (); i++)
                {
                  blockAck.SetReceivedFragment ((*i).second.GetSequenceNumber (),
                                                (*i).second.GetFragmentNumber ());
                   /* Section 9.10.4 in IEEE802.11n: the recipient shall pass up to WifiMac the 
                    * MSDUs and A-MSDUs starting with the starting sequence number 
                    * sequentially until there is an incomplete MSDU or A-MSDU in the buffer */
                  if (!(*i).second.IsMoreFragments ())
                    {
                      while (lastComplete != i)
                        {
                          m_rxCallback ((*lastComplete).first, &(*lastComplete).second);
                          lastComplete++;
                        }
                      m_rxCallback ((*lastComplete).first, &(*lastComplete).second);
                      lastComplete++;
                    }
                  guard = (*i).second.IsMoreFragments () ? (guard + 1) : (guard + 16) & 0xfff0;
                }
              (*it).second.first.SetStartingSequence ((guard>>4)&0x0fff);
              /* All packets already forwarded to WifiMac must be removed from buffer: 
                 [begin (), lastComplete) */
              (*it).second.second.erase ((*it).second.second.begin (), lastComplete);
              for (i = lastComplete; i != (*it).second.second.end (); i++)
                { 
                  blockAck.SetReceivedFragment ((*i).second.GetSequenceNumber (),
                                                (*i).second.GetFragmentNumber ());  
                }
            }
          else if (reqHdr.IsCompressed ())
            {
              blockAck.SetType (COMPRESSED_BLOCK_ACK);
              uint16_t guard = startingSeqCtrl;
              std::list<BufferedPacket>::iterator lastComplete = (*it).second.second.begin ();
              for (; i != (*it).second.second.end () && guard == (*i).second.GetSequenceControl (); i++)
                {
                  if (!(*i).second.IsMoreFragments ())
                    {
                      blockAck.SetReceivedPacket ((*i).second.GetSequenceNumber ());
                      while (lastComplete != i)
                        {
                          m_rxCallback ((*lastComplete).first, &(*lastComplete).second);
                          lastComplete++;
                        }
                      m_rxCallback ((*lastComplete).first, &(*lastComplete).second);
                      lastComplete++;
                    }
                  guard = (*i).second.IsMoreFragments () ? (guard + 1) : (guard + 16) & 0xfff0;
                }
              (*it).second.first.SetStartingSequence ((guard>>4)&0x0fff);
              /* All packets already forwarded to WifiMac must be removed from buffer:
                 [begin (), lastcomplete) */
              (*it).second.second.erase ((*it).second.second.begin (), lastComplete);
              i = lastComplete;
              if (i != (*it).second.second.end ())
                {
                  guard = (*i).second.GetSequenceControl () & 0xfff0;
                }
              for (; i != (*it).second.second.end ();)
                {
                  for (; i != (*it).second.second.end () && guard == (*i).second.GetSequenceControl (); i++)
                    {
                      if (!(*i).second.IsMoreFragments ())
                        {
                          guard = (guard + 16) & 0xfff0;
                          blockAck.SetReceivedPacket ((*i).second.GetSequenceNumber ());
                        }
                      else
                        {
                          guard += 1;
                        }
                    }
                  while (i != (*it).second.second.end () && ((guard >> 4) & 0x0fff) == (*i).second.GetSequenceNumber ())
                    {
                      i++;
                    }
                  if (i != (*it).second.second.end ())
                    {
                      guard = (*i).second.GetSequenceControl () & 0xfff0;
                    }
                }
            }
        }
      else
        {
          NS_LOG_DEBUG ("there's not a valid block ack agreement with "<<originator);
        }
    }
  else
    {
      NS_FATAL_ERROR ("Multi-tid block ack is not supported.");
    }

  SendBlockAckResponse (&blockAck, originator, immediate, duration, blockAckReqTxMode);
}

void
MacLow::ResetBlockAckInactivityTimerIfNeeded (BlockAckAgreement &agreement)
{
  if (agreement.GetTimeout () != 0)
    {
      NS_ASSERT (agreement.m_inactivityEvent.IsRunning ());
      agreement.m_inactivityEvent.Cancel ();
      Time timeout = MicroSeconds (1024 * agreement.GetTimeout ());

      AccessClass ac = QosUtilsMapTidToAc (agreement.GetTid ());
      //std::map<AccessClass, MacLowTransmissionListener*>::iterator it = m_edcaListeners.find (ac);
      //NS_ASSERT (it != m_edcaListeners.end ());

      agreement.m_inactivityEvent = Simulator::Schedule (timeout, 
                                                         &MacLowBlockAckEventListener::BlockAckInactivityTimeout, 
                                                         m_edcaListeners[ac],
                                                         agreement.GetPeer (),
                                                         agreement.GetTid ());
    }
}

void
MacLow::RegisterBlockAckListenerForAc (enum AccessClass ac, MacLowBlockAckEventListener *listener)
{
  m_edcaListeners.insert (std::make_pair (ac, listener));
}

} // namespace ns3
