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

#include "inci-packet.h"

namespace ns3 {

InciPacket::InciPacket (void)
{}

InciPacket::InciPacket (uint64_t senderId, std::string msgType, uint32_t ts, uint32_t tsSeqNo)
{
  m_data.senderId = senderId;
  m_data.msgType = msgType;
  m_data.ts = ts;
  m_data.tsSeqNo = tsSeqNo;
}

void
InciPacket::SetData (struct ReceivedInciPacket data)
{
  m_data.senderId = data.senderId;
  m_data.msgType = data.msgType;
  m_data.ts = data.ts;
  m_data.ts = data.tsSeqNo;

}

struct InciPacket::ReceivedInciPacket
InciPacket::GetData (void) const
{
  return (m_data);
}

};
