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

#include "mobility-model-installer.h"
#include "ns3/double.h"
#include "ns3/string.h"
#include "ns3/log.h"

NS_LOG_COMPONENT_DEFINE ("MobilityModelInstaller");

namespace ns3
{

NS_OBJECT_ENSURE_REGISTERED (MobilityModelInstaller);

TypeId MobilityModelInstaller::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::MobilityModelInstaller")
    .SetParent<Object> ()     
    .AddConstructor<MobilityModelInstaller> ()             
    ;
  return tid;
}
    
MobilityModelInstaller::MobilityModelInstaller () 
{
   m_mobility.SetMobilityModel ("ns3::ItetrisMobilityModel");
} 

void
MobilityModelInstaller::Install (NodeContainer container) 
{
  m_mobility.Install (container);
}

}