/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2006,2007 INRIA
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

#include <unistd.h>
#include <math.h>

#include "mobility-model.h"
#include "ns3/trace-source-accessor.h"

namespace ns3 {

TypeId 
MobilityModel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::MobilityModel")
    .SetParent<Object> ()
    .AddAttribute ("Position", "The current position of the mobility model.",
                   TypeId::ATTR_SET | TypeId::ATTR_GET,
                   VectorValue (Vector (0.0, 0.0, 0.0)),
                   MakeVectorAccessor (&MobilityModel::SetPosition,
                                        &MobilityModel::GetPosition),
                   MakeVectorChecker ())
    .AddAttribute ("Velocity", "The current velocity of the mobility model.",
                   TypeId::ATTR_GET,
                   VectorValue (Vector (0.0, 0.0, 0.0)), // ignored initial value.
                   MakeVectorAccessor (&MobilityModel::GetVelocity),
                   MakeVectorChecker ())
    .AddTraceSource ("CourseChange", 
                     "The value of the position and/or velocity vector changed",
                     MakeTraceSourceAccessor (&MobilityModel::m_courseChangeTrace))
    ;
  return tid;
}

MobilityModel::MobilityModel ()
  : m_node(0), // Added Ramon Bauza 16/09/10
    m_antennaHeight(1.5) // Added Ramon Bauza 21/09/10
{}

MobilityModel::~MobilityModel ()
{}

Vector
MobilityModel::GetPosition (void) const
{
  return DoGetPosition ();
}
Vector
MobilityModel::GetVelocity (void) const
{
  return DoGetVelocity ();
}

void 
MobilityModel::SetPosition (const Vector &position)
{
  DoSetPosition (position);
}

double 
MobilityModel::GetDistanceFrom (Ptr<const MobilityModel> other) const
{
  Vector oPosition = other->DoGetPosition ();
  Vector position = DoGetPosition ();
  return CalculateDistance (position, oPosition);
}

void
MobilityModel::NotifyCourseChange (void) const
{
  m_courseChangeTrace(this);
}

// Added Ramon Bauza 16/09/10
float
MobilityModel::GetAntennaHeight (void) const
{
  return DoGetAntennaHeight ();
}

// Added Ramon Bauza 21/09/10
void 
MobilityModel::SetAntennaHeight (const float &antennaHeight)
{
  m_antennaHeight = antennaHeight;
}

// Added Ramon Bauza 21/09/10
float 
MobilityModel::DoGetAntennaHeight (void) const
{
  return m_antennaHeight;
}

// Added Ramon Bauza 16/09/10
Ptr<Node> 
MobilityModel::GetNode (void) const
{
  return m_node;
}

// Added Ramon Bauza 16/09/10
void 
MobilityModel::SetNode (Ptr<Node> node) 
{
  m_node = node;
}

} // namespace ns3
