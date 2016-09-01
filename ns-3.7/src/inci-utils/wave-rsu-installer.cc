/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, Uwicore Laboratory (www.uwicore.umh.es),
 *                          University Miguel Hernandez, EU FP7 iTETRIS project
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

#include "wave-rsu-installer.h"
#include "ns3/boolean.h"
#include "ns3/log.h"
#include "ns3/wifi-net-device.h"
#include "ns3/wifi-phy.h"

// Includes for facilties
#include "ns3/c2c-facilities-helper.h"
#include "ns3/iTETRISns3Facilities.h"

// Specific objects for RsuVehicleStas
#include "ns3/rsu-sta-mgnt.h"

NS_LOG_COMPONENT_DEFINE ("WaveRsuInstaller");

namespace ns3
{

NS_OBJECT_ENSURE_REGISTERED (WaveRsuInstaller);

TypeId WaveRsuInstaller::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::WaveRsuInstaller")
    .SetParent<WaveInstaller> ()     
    .AddConstructor<WaveRsuInstaller> ()             
    ;
  return tid;
}
    
void 
WaveRsuInstaller::DoInstall (NodeContainer container, NetDeviceContainer cchDevices, NetDeviceContainer schDevices)
{
  NS_LOG_INFO ("WaveRsuInstaller");
  uint32_t index = 0;
  for (NodeContainer::Iterator i = container.Begin (); i != container.End (); i++)
    {
      // Check if the RSU has the object RsuStaMgnt already installed
      Ptr<RsuStaMgnt> rsuStaMgnt = (*i)->GetObject <RsuStaMgnt> ();
      if (rsuStaMgnt == NULL)
	{
          rsuStaMgnt = CreateObject <RsuStaMgnt> ();
          rsuStaMgnt->SetNode (*i);
          (*i)->AggregateObject (rsuStaMgnt);
          NS_LOG_INFO ("The object RsuStaMgnt has been installed in the RSU");
        }

      // Check if the vehicle has the Facilities already installed
      Ptr<iTETRISns3Facilities> facilities = (*i)->GetObject <iTETRISns3Facilities> ();
      if (facilities == NULL)
	{
          C2CFacilitiesHelper facilitiesHelper;
          facilitiesHelper.AddDefaultServices (m_servListHelper);
          facilitiesHelper.Install (*i);
          NS_LOG_INFO ("The object iTETRISns3Facilities has been installed in the RSU");
        }
      else
        {
          C2CFacilitiesHelper facilitiesHelper;
          facilitiesHelper.SetServiceListHelper (m_servListHelper);
          facilitiesHelper.AddServices (facilities, *i);
          NS_LOG_INFO ("New services have been installed in the RSU");
        }

      index ++;
    }

  // Activate CCH devices
  for (NetDeviceContainer::Iterator i = cchDevices.Begin (); i != cchDevices.End (); i++)
    {
      DynamicCast<WifiNetDevice>(*i)->GetPhy()->SetNodeStatus(true);
    }

  // Activate SCH devices
  for (NetDeviceContainer::Iterator i = schDevices.Begin (); i != schDevices.End (); i++)
    {
      DynamicCast<WifiNetDevice>(*i)->GetPhy()->SetNodeStatus(true);
    }
}

} // namespace ns3
