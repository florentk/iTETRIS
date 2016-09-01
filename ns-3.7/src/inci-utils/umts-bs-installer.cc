/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, CBT EU FP7 iTETRIS project
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
 * Author: Sendoa Vaz <svaz@cbt.es>
 */

#include "umts-bs-installer.h"

NS_LOG_COMPONENT_DEFINE ("UmtsBsInstaller");

namespace ns3
{

NS_OBJECT_ENSURE_REGISTERED (UmtsBsInstaller);

TypeId UmtsBsInstaller::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::UmtsBsInstaller")
    .SetParent<Object> ()     
    .AddConstructor<UmtsBsInstaller>()
    ;
  return tid;
}

void 
UmtsBsInstaller::DoInstall (NodeContainer container, NetDeviceContainer devices)
{
  NS_LOG_INFO ("*** UmtsBsInstaller ***");
  
  uint32_t index = 0;

  for (NodeContainer::Iterator i = container.Begin (); i != container.End (); i++)
    {
      // Check if the base station has the object UmtsBsMgnt already installed
      Ptr<IpBaseStaMgnt> umtsBsMgnt = (*i)->GetObject <UmtsBsMgnt> ();
      if (umtsBsMgnt  == NULL)
	{
          Ptr<NetDevice> device = devices.Get(index);
          umtsBsMgnt = CreateObject <UmtsBsMgnt> ();
          umtsBsMgnt->SetNode (*i);
          umtsBsMgnt->SetNetDevice (device);
          (*i)->AggregateObject (umtsBsMgnt);
          NS_LOG_INFO ("The object UmtsBsMgnt has been installed in the base station");
        }
        
        DynamicCast<UMTSNetDevice>(devices.Get(index))->SetIpAddress();
        
      // Check if the vehicle has the Facilties already installed
//     Ptr<iTETRISns3Facilities> facilities = (*i)->GetObject <iTETRISns3Facilities> ();
    Ptr<IPCIUFacilities> facilities = (*i)->GetObject <IPCIUFacilities> ();
    if (facilities == NULL)
      {
	IPCIUFacilitiesHelper facilitiesHelper;
	facilitiesHelper.SetServiceListHelper (m_servListHelper);
	facilitiesHelper.Install (*i);
	NS_LOG_INFO ("The object IPCIUFacilities has been installed in the vehicle");
      }
      
      index ++;
    }
    
    
    
}

// void
// UmtsBsInstaller::AddVehicles(NetDeviceContainer netDevices)
// {
//   for (NetDeviceContainer::Iterator i = baseStationContainer.Begin (); i != baseStationContainer.End (); ++i)
//   {
//       for(NetDeviceContainer::Iterator iterator=netDevices.Begin();iterator!=netDevices.End();++iterator)
//       {
// 	(*i)->GetObject <UmtsBsMgnt> ()->AddVehicle(DynamicCast<UMTSNetDevice>(*iterator));
//       }
//       
//   }
// }

} // namespace ns3
