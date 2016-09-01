/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2007,2008,2009 INRIA, UDcast
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
 * Authors: Jahanzeb Farooq <jahanzeb.farooq@sophia.inria.fr>
 *          Mohamed Amine Ismail <amine.ismail@sophia.inria.fr>
 *                               <amine.ismail@UDcast.com>
 */

#include "connection-manager.h"
#include "ns3/log.h"
#include "cid-factory.h"
#include "ss-record.h"
#include "mac-messages.h"
#include "ns3/pointer.h"
#include "ns3/enum.h"
#include "service-flow.h"
#include "ss-net-device.h"
#include "bs-net-device.h"

NS_LOG_COMPONENT_DEFINE ("ConnectionManager");

namespace ns3 {

ConnectionManager::ConnectionManager (void)
  : m_cidFactory (0)
{
}

void
ConnectionManager::DoDispose (void)
{
}

ConnectionManager::~ConnectionManager (void)
{
}

void
ConnectionManager::SetCidFactory (CidFactory *cidFactory)
{
  m_cidFactory = cidFactory;
}

void
ConnectionManager::AllocateManagementConnections (SSRecord *ssRecord, RngRsp *rngrsp)
{
  Ptr<WimaxConnection> basicConnection = CreateConnection (Cid::CID_BASIC);
  ssRecord->SetBasicCid (basicConnection->GetCid ());

  Ptr<WimaxConnection> primaryConnection = CreateConnection (Cid::CID_PRIMARY);
  ssRecord->SetPrimaryCid (primaryConnection->GetCid ());

  rngrsp->SetBasicCid (basicConnection->GetCid ());
  rngrsp->SetPrimaryCid (primaryConnection->GetCid ());
}

Ptr<WimaxConnection>
ConnectionManager::CreateConnection (Cid::TypeCid type)
{
  Cid cid;
  switch (type)
    {
    case Cid::CID_BASIC:
    case Cid::CID_MULTICAST:
    case Cid::CID_PRIMARY:
      cid = m_cidFactory->Allocate (type);
      break;
    case Cid::CID_TRANSPORT:
      cid = m_cidFactory->AllocateTransportOrSecondary ();
      break;
    default:
      NS_FATAL_ERROR ("Invalid connection type");
      break;
    }
  Ptr<WimaxConnection> connection = CreateObject<WimaxConnection> (cid, type);
  AddConnection (connection, type);
  return connection;
}

void
ConnectionManager::AddConnection (Ptr<WimaxConnection> connection, Cid::TypeCid type)
{
  switch (type)
    {
    case Cid::CID_BASIC:
      m_basicConnections.push_back (connection);
      break;
    case Cid::CID_PRIMARY:
      m_primaryConnections.push_back (connection);
      break;
    case Cid::CID_TRANSPORT:
      m_transportConnections.push_back (connection);
      break;
    case Cid::CID_MULTICAST:
      m_multicastConnections.push_back (connection);
      break;
    default:
      NS_FATAL_ERROR ("Invalid connection type");
      break;
    }
}

Ptr<WimaxConnection>
ConnectionManager::GetConnection (Cid cid)
{
  std::vector<Ptr<WimaxConnection> >::const_iterator iter;
  for (iter = m_basicConnections.begin (); iter != m_basicConnections.end (); ++iter)
    {
      if ((*iter)->GetCid () == cid)
        {
          return *iter;
        }
    }

  for (iter = m_primaryConnections.begin (); iter != m_primaryConnections.end (); ++iter)
    {
      if ((*iter)->GetCid () == cid)
        {
          return *iter;
        }
    }

  // Added by Ramon
  for (iter = m_multicastConnections.begin (); iter != m_multicastConnections.end (); ++iter)
    {
      if ((*iter)->GetCid () == cid)
	{
	  return *iter;
	}
    }

  for (iter = m_transportConnections.begin (); iter != m_transportConnections.end (); ++iter)
    {
      if ((*iter)->GetCid () == cid)
        {
          return *iter;
        }
    }
  return 0;
}

std::vector<Ptr<WimaxConnection> >
ConnectionManager::GetConnections (Cid::TypeCid type) const
{
  std::vector<Ptr<WimaxConnection> > connections;

  switch (type)
    {
    case Cid::CID_BASIC:
      connections = m_basicConnections;
      break;
    case Cid::CID_PRIMARY:
      connections = m_primaryConnections;
      break;
    case Cid::CID_TRANSPORT:
      connections = m_transportConnections;
      break;
    // Added by Ramon
    case Cid::CID_MULTICAST:
      connections = m_multicastConnections;
      break;
    default:
      NS_FATAL_ERROR ("Invalid connection type");
      break;
    }

  return connections;
}

uint32_t
ConnectionManager::GetNPackets (Cid::TypeCid type, ServiceFlow::SchedulingType schedulingType) const
{
  uint32_t nrPackets = 0;

  switch (type)
    {
    case Cid::CID_BASIC:
      {
        for (std::vector<Ptr<WimaxConnection> >::const_iterator iter = m_basicConnections.begin (); iter
             != m_basicConnections.end (); ++iter)
          {
            nrPackets += (*iter)->GetQueue ()->GetSize ();
          }
        break;
      }
    case Cid::CID_PRIMARY:
      {
        for (std::vector<Ptr<WimaxConnection> >::const_iterator iter = m_primaryConnections.begin (); iter
             != m_primaryConnections.end (); ++iter)
          {
            nrPackets += (*iter)->GetQueue ()->GetSize ();
          }
        break;
      }
    case Cid::CID_TRANSPORT:
      {
        for (std::vector<Ptr<WimaxConnection> >::const_iterator iter = m_transportConnections.begin (); iter
             != m_transportConnections.end (); ++iter)
          {
            if (schedulingType == ServiceFlow::SF_TYPE_ALL || (*iter)->GetSchedulingType () == schedulingType)
              {
                nrPackets += (*iter)->GetQueue ()->GetSize ();
              }
          }
        break;
      }
    default:
      NS_FATAL_ERROR ("Invalid connection type");
      break;
    }

  return nrPackets;
}

bool
ConnectionManager::HasPackets (void) const
{
  std::vector<Ptr<WimaxConnection> >::const_iterator iter;
  for (iter = m_basicConnections.begin (); iter != m_basicConnections.end (); ++iter)
    {
      if ((*iter)->HasPackets ())
        {
          return true;
        }
    }

  for (iter = m_primaryConnections.begin (); iter != m_primaryConnections.end (); ++iter)
    {
      if ((*iter)->HasPackets ())
        {
          return true;
        }
    }

  for (iter = m_transportConnections.begin (); iter != m_transportConnections.end (); ++iter)
    {
      if ((*iter)->HasPackets ())
        {
          return true;
        }
    }

  return false;
}

// Added by Ramon
void
ConnectionManager::ClearConnection (Cid cid)
{
  std::vector<Ptr<WimaxConnection> >::iterator iter;
  Ptr<WimaxConnection> connection = GetConnection (cid);
  switch (connection->GetType())
    {
      case Cid::CID_BASIC:
	{
	  for (iter = m_basicConnections.begin (); iter != m_basicConnections.end (); ++iter)
	    {
	      if ((*iter)->GetCid () == cid)
		{
		  m_basicConnections.erase(iter);
		  break;  
		}
	    }
	  break;
	}
      case Cid::CID_PRIMARY:
	{
	  for (iter = m_primaryConnections.begin (); iter != m_primaryConnections.end (); ++iter)
	    {
	      if ((*iter)->GetCid () == cid)
		{
		  m_primaryConnections.erase(iter);
		  break;  
		}
	    }
	  break;
	}
      case Cid::CID_TRANSPORT:
	{
	  for (iter = m_transportConnections.begin (); iter != m_transportConnections.end (); ++iter)
	    {
	      if ((*iter)->GetCid () == cid)
		{
		  m_transportConnections.erase(iter);
		  break;  
		}
	    }
	  break;
	}
      case Cid::CID_MULTICAST:
	{
	  for (iter = m_multicastConnections.begin (); iter != m_multicastConnections.end (); ++iter)
	    {
	      if ((*iter)->GetCid () == cid)
		{
		  m_multicastConnections.erase(iter);
		  break;  
		}
	    }
	  break;
	}
      default:
	{
	  NS_FATAL_ERROR ("Invalid connection type");
	  break;
	}
    }
}

// Added by Ramon
void
ConnectionManager::DeleteSetupConnection (const Mac48Address &address) const
{
  std::list<std::pair<Mac48Address, Cid* > >::iterator iter;
  for (iter = m_connectionSetupListItetris->begin (); iter !=  m_connectionSetupListItetris->end (); ++iter)
    {
      if (address == (*iter).first)
        { 
          m_connectionSetupListItetris->erase (iter);
          break;
        }
    }
  return;
}

} // namespace ns3


