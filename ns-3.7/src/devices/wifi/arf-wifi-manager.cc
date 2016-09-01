/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2004,2005,2006 INRIA
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

#include "arf-wifi-manager.h"
#include "ns3/assert.h"
#include "ns3/log.h"
#include "ns3/uinteger.h"

NS_LOG_COMPONENT_DEFINE ("ns3::ArfWifiManager");


namespace ns3 {
  
ArfWifiRemoteStation::ArfWifiRemoteStation (Ptr<ArfWifiManager> manager)
  : m_manager (manager)
{
  m_successThreshold = m_manager->m_successThreshold;
  m_timerTimeout = m_manager->m_timerThreshold;
  m_rate = GetMinRate ();

  m_success = 0;
  m_failed = 0;
  m_recovery = false;
  m_retry = 0;
  m_timer = 0;
}
ArfWifiRemoteStation::~ArfWifiRemoteStation ()
{}

uint32_t
ArfWifiRemoteStation::GetMaxRate (void)
{
  return GetNSupportedModes ();
}
uint32_t
ArfWifiRemoteStation::GetMinRate (void)
{
  return 0;
}

bool 
ArfWifiRemoteStation::NeedRecoveryFallback (void)
{
  if (m_retry == 1) 
    {
      return true;
    } 
  else 
    {
      return false;
    }
}
bool 
ArfWifiRemoteStation::NeedNormalFallback (void)
{
  int retryMod = (m_retry - 1) % 2;
  if (retryMod == 1) 
    {
      return true;
    } 
  else 
    {
      return false;
    }
}



void 
ArfWifiRemoteStation::DoReportRtsFailed (void)
{}
/**
 * It is important to realize that "recovery" mode starts after failure of
 * the first transmission after a rate increase and ends at the first successful
 * transmission. Specifically, recovery mode transcends retransmissions boundaries.
 * Fundamentally, ARF handles each data transmission independently, whether it
 * is the initial transmission of a packet or the retransmission of a packet.
 * The fundamental reason for this is that there is a backoff between each data
 * transmission, be it an initial transmission or a retransmission.
 */
void 
ArfWifiRemoteStation::DoReportDataFailed (void)
{
  m_timer++;
  m_failed++;
  m_retry++;
  m_success = 0;

  if (m_recovery) 
    {
      NS_ASSERT (m_retry >= 1);
      if (NeedRecoveryFallback ()) 
        {
          ReportRecoveryFailure ();
          if (m_rate != GetMinRate ()) 
            {
              m_rate--;
            }
        }
      m_timer = 0;
    } 
  else 
    {
      NS_ASSERT (m_retry >= 1);
      if (NeedNormalFallback ()) 
        {
          ReportFailure ();
          if (m_rate != GetMinRate ()) 
            {
              m_rate--;
            }
        }
      if (m_retry >= 2) 
        {
          m_timer = 0;
        }
    }
}
void 
ArfWifiRemoteStation::DoReportRxOk (double rxSnr, WifiMode txMode)
{}
void ArfWifiRemoteStation::DoReportRtsOk (double ctsSnr, WifiMode ctsMode, double rtsSnr)
{
  NS_LOG_DEBUG ("self="<<this<<" rts ok");
}
void ArfWifiRemoteStation::DoReportDataOk (double ackSnr, WifiMode ackMode, double dataSnr)
{
  m_timer++;
  m_success++;
  m_failed = 0;
  m_recovery = false;
  m_retry = 0;
  NS_LOG_DEBUG ("self="<<this<<" data ok success="<<m_success<<", timer="<<m_timer);
  if ((m_success == GetSuccessThreshold () ||
       m_timer == GetTimerTimeout ()) &&
      (m_rate < (GetMaxRate () - 1))) 
    {
      NS_LOG_DEBUG ("self="<<this<<" inc rate");
      m_rate++;
      m_timer = 0;
      m_success = 0;
      m_recovery = true;
    }
}
void 
ArfWifiRemoteStation::DoReportFinalRtsFailed (void)
{}
void 
ArfWifiRemoteStation::DoReportFinalDataFailed (void)
{}

WifiMode
ArfWifiRemoteStation::DoGetDataMode (uint32_t size)
{
  return GetSupportedMode (m_rate);
}
WifiMode
ArfWifiRemoteStation::DoGetRtsMode (void)
{
  // XXX: we could/should implement the Arf algorithm for
  // RTS only by picking a single rate within the BasicRateSet.
  return GetSupportedMode (0);
}

void ArfWifiRemoteStation::ReportRecoveryFailure (void)
{}
void ArfWifiRemoteStation::ReportFailure (void)
{}
uint32_t ArfWifiRemoteStation::GetMinTimerTimeout (void)
{
  return m_manager->m_timerThreshold;
}
uint32_t ArfWifiRemoteStation::GetMinSuccessThreshold (void)
{
  return m_manager->m_successThreshold;
}
uint32_t ArfWifiRemoteStation::GetTimerTimeout (void)
{
  return m_timerTimeout;
}
uint32_t ArfWifiRemoteStation::GetSuccessThreshold (void)
{
  return m_successThreshold;
}
void ArfWifiRemoteStation::SetTimerTimeout (uint32_t timerTimeout)
{
  NS_ASSERT (timerTimeout >= m_manager->m_timerThreshold);
  m_timerTimeout = timerTimeout;
}
void ArfWifiRemoteStation::SetSuccessThreshold (uint32_t successThreshold)
{
  NS_ASSERT (successThreshold >= m_manager->m_successThreshold);
  m_successThreshold = successThreshold;
}
Ptr<WifiRemoteStationManager>
ArfWifiRemoteStation::GetManager (void) const
{
  return m_manager;
}

NS_OBJECT_ENSURE_REGISTERED (ArfWifiManager);

TypeId 
ArfWifiManager::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::ArfWifiManager")
    .SetParent<WifiRemoteStationManager> ()
    .AddConstructor<ArfWifiManager> ()
    .AddAttribute ("TimerThreshold", "The 'timer' threshold in the ARF algorithm.",
                   UintegerValue (15),
                   MakeUintegerAccessor (&ArfWifiManager::m_timerThreshold),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("SuccessThreshold",
                   "The minimum number of sucessfull transmissions to try a new rate.",
                   UintegerValue (10),
                   MakeUintegerAccessor (&ArfWifiManager::m_successThreshold),
                   MakeUintegerChecker<uint32_t> ())
    ;
  return tid;
}

ArfWifiManager::ArfWifiManager ()
{}
ArfWifiManager::~ArfWifiManager ()
{}
WifiRemoteStation *
ArfWifiManager::CreateStation (void)
{
  return new ArfWifiRemoteStation (this);
}

} // namespace ns3
