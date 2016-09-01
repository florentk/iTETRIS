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

#ifndef INCI_PACKET_H
#define INCI_PACKET_H

#include <stdint.h>
#include <string>

namespace ns3
{
/**
 * @class InciPacket
 * @brief The class InciPacket represents an iTETRIS packet (i.e. CAM). The data fields of the InciPacket are passed up to the iCS so that the iCS can track the transmission and reception of packets in ns-3
 */

class InciPacket
{
public:

  /**
   * The struct contains the data fields of the CAM 
   */
  struct ReceivedInciPacket {
    uint64_t senderId;
    std::string msgType;
    uint32_t ts;
    uint32_t tsSeqNo;
  };

  InciPacket (void);
  InciPacket (uint64_t senderId, std::string msgType, uint32_t ts, uint32_t tsSeqNo );
  void SetData (struct ReceivedInciPacket data);
  struct ReceivedInciPacket GetData (void) const;

private:

  struct ReceivedInciPacket m_data;

};

};

#endif
