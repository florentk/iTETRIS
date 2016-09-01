/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009-2010, Uwicore Laboratory (www.uwicore.umh.es),
 *                          University Miguel Hernandez,
 *                          EURECOM (www.eurecom.fr), EU FP7 iTETRIS project
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
 * Author:  Michele Rondinone <mrondinone@umh.es>, Jerome Haerri <Jerome.Haerri@eurecom.fr>
 */

#include "ns3/object.h"
#include "ns3/names.h"
#include "ns3/ptr.h"
#include "ns3/node.h"
#include "ns3/log.h"
#include "ns3/event-id.h"
#include "ns3/itetris-types.h"
#include "ns3/iTETRISNodeManager.h"
#include "ns3/mobility-model.h"
#include "ns3/node-container.h"
#include "ns3/rsu-sta-mgnt.h"
#include "ns3/ip-base-sta-mgnt.h"
#include "math.h"
#include "MWFacilities.h"

#include "mw-comm-ch-selector.h"

NS_LOG_COMPONENT_DEFINE ("MWCOMMchSelector");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (MWCOMMchSelector);

TypeId 
MWCOMMchSelector::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::MWCOMMchSelector")
    .SetParent<Object> ()
    .AddConstructor<MWCOMMchSelector> ()
    ;
  return tid;
}

MWCOMMchSelector::MWCOMMchSelector ()
{
}

MWCOMMchSelector::~MWCOMMchSelector ()
{
}

void
MWCOMMchSelector::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  m_node = 0;
  Object::DoDispose ();
}

void 
MWCOMMchSelector::SetNode (Ptr<Node> node)
{
  NS_LOG_FUNCTION (this);
  m_node = node;
}

void
MWCOMMchSelector::NotifyNewAggregate ()
{
  if (m_node == 0)
    {
      Ptr<Node>node = this->GetObject<Node>();
      // verify that it's a valid node and that
      // the node has not been set before
      if (node != 0)
        {
          this->SetNode (node);
        }
    }
  Object::NotifyNewAggregate ();
}

disseminationProfile
MWCOMMchSelector::GetDisseminationProfile(uint32_t commProfile, CircularGeoAddress destination, TechnologyList technologies)
{  
    disseminationProfile profile;

    /* \brief basic example of MW intelligence proposed by F. Hrizi and J. Haerri (EURECOM). Works as follows:
     *       - take the first preferred techno
     *       - get the closest ICU in the area
     * 	     - check the availability (up and at least one attached node)
     */
    uint32_t minDist = 1e6;
    Ptr<Node> closestNode = NULL;
    std::string closestNodeTechno = "WaveRsu";

    // need to retrieve the list of infrastructure nodes (ICU) connected to the TMC; Need the facilities
    Ptr<MWFacilities> facilities = m_node->GetObject<MWFacilities> ();

    for(std::vector<std::string >::iterator technoIter=technologies.begin();technoIter != technologies.end();technoIter++) {
    	std::string techno = *technoIter;

    	// we retrieve the list of ICU per technology
    	NodeContainer* nodes = facilities->getInfrastructureTechNodes(techno);
    	if (nodes != NULL) {
    		for (NodeContainer::Iterator i = nodes->Begin(); i != nodes->End(); ++i) {
    			Ptr<Node> node = *i;
    			//if (node->isActive() && !node->IsMobileNode() ) {
    			if (!node->IsMobileNode() ) {
    				// TODO include a check on the interface: if the interface is not active, jump to next node
    				// TODO only IP stations and vehicles stations can be 'activated' harmonization needed

    				uint32_t nodesCovered = 0;
    				double coverageRange = 0.0;

    				if(techno == "WaveRsu") {
    					Ptr<RsuStaMgnt> staMgnt = node->GetObject<RsuStaMgnt> ();
    					NS_ASSERT_MSG (staMgnt, "RsuStaMgnt object not found in the RSU");
    					coverageRange = staMgnt->GetCoverageRange();
    					nodesCovered = staMgnt->GetNumberOfNodesInCoverage();
    				}
    				else  {
    					Ptr<IpBaseStaMgnt> staMgnt = node->GetObject<IpBaseStaMgnt> ();
    					NS_ASSERT_MSG (staMgnt, "IpBaseStaMgnt object not found in the UMTS/DVBH/WIMAX");
    					coverageRange = staMgnt->GetCoverageRange();
    					nodesCovered = staMgnt->GetNumberOfRegisteredUsers();
    				}

    				Ptr<MobilityModel> mob = node->GetObject<MobilityModel> ();
    				double lat = (double) mob->GetPosition().x;
    				double lon = (double) mob->GetPosition().y;
    				double dist = sqrt(pow((double)destination.lat - lat,2) + pow((double)destination.lon - lon,2));

    				if ((dist <=coverageRange) && (dist < minDist) && (nodesCovered > 0) ) { // closest covering ICU with at least 1 covered node
    					minDist = dist;
    					closestNode = node;
    					closestNodeTechno = techno;
    				}
    			}
    		}
    	}
    	if (closestNode != NULL) {
    		profile.disseminator = closestNode;
    		if (closestNodeTechno == "WaveRsu") {
    			profile.stack= C2C;
    			if((commProfile == 1) || (commProfile == 2))
    				profile.tech = ITS_CCH;
    			else
    				profile.tech = ITS_SCH;
    		}
    		else {
    			profile.stack= IPv4;
    			if (closestNodeTechno == "UmtsBs") {
    				profile.tech= UMTS;
    			}
    			else if (closestNodeTechno == "DvbhBs") {
    				profile.tech= DVB;
    			}
    			else if (closestNodeTechno == "WimaxBs") {
    			    profile.tech= WIMAX;
    			}
    			else {
    				profile.tech= DEV_UNDEF;
    				NS_FATAL_ERROR  ("Unavailable Technology for node "<< m_node->GetId() << ": with selected techno " << closestNodeTechno<<"");
    				return profile;
    			}
    		}
    	}
    }
    return profile;
}

} //namespace ns3
