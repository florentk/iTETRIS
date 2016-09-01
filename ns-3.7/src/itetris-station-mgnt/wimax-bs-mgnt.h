/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, EU FP7 iTETRIS project
 *                          Uwicore Laboratory (www.uwicore.umh.es), University Miguel Hernandez 
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

#ifndef WIMAX_BS_MGNT_H
#define WIMAX_BS_MGNT_H

#include "ns3/simulator.h"
#include "ns3/wimax-net-device.h"
#include "ns3/object.h"
#include "ns3/ip-base-sta-mgnt.h"

namespace ns3
{

class BsCommandManager;

/**
 * The base class WimaxBsMgnt implements the IpBaseStaMgnfor the technology Wimax
 */
class WimaxBsMgnt : public IpBaseStaMgnt
{

  public:
    static TypeId GetTypeId (void);
    WimaxBsMgnt();
    virtual ~WimaxBsMgnt();
    Ipv4Address* GetIpAddress (uint32_t nodeId) const;
    uint32_t GetNumberOfActiveConnections (void) const;
    uint32_t GetNumberOfRegisteredUsers (void) const;
    double GetCoverageRange (void) const;
    void SetBsCommandManager (void);

  private:
    void TriggerVehiclesScanning (void) const;
    Ptr<BsCommandManager> m_bsCommandManager;
    typedef std::list<Ptr<WimaxNetDevice> > VehicleList;
    VehicleList vehicleList;

};

}

#endif
