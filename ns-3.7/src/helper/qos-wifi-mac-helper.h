/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009 MIRKO BANCHI
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
 * Author: Mirko Banchi <mk.banchi@gmail.com>
 */
#ifndef QOS_WIFI_MAC_HELPER_H
#define QOS_WIFI_MAC_HELPER_H

#include "wifi-helper.h"
#include "ns3/qos-utils.h"

#include <map>

namespace ns3 {

/**
 * \brief create qos MAC layers for a ns3::WifiNetDevice. 
 * This class can create MACs of type ns3::QapWifiMac, ns3::QstaWifiMac, and, ns3::QadhocWifiMac
 */
class QosWifiMacHelper : public WifiMacHelper
{
public:
  /**
   * Create a QosWifiMacHelper that is used to make life easier when working
   * with Wifi devices using a QOS MAC layer.
   */
  QosWifiMacHelper ();

  /**
   * \internal
   * Destroy a QosWifiMacHelper
   */
  virtual ~QosWifiMacHelper ();

  /**
   * Create a mac helper in a default working state.
   */
  static QosWifiMacHelper Default (void);

  /**
   * Set the underlying type of the MAC and its attributes.
   *
   * \param type the type of ns3::WifiMac to create.
   * \param n0 the name of the attribute to set
   * \param v0 the value of the attribute to set
   * \param n1 the name of the attribute to set
   * \param v1 the value of the attribute to set
   * \param n2 the name of the attribute to set
   * \param v2 the value of the attribute to set
   * \param n3 the name of the attribute to set
   * \param v3 the value of the attribute to set
   * \param n4 the name of the attribute to set
   * \param v4 the value of the attribute to set
   * \param n5 the name of the attribute to set
   * \param v5 the value of the attribute to set
   * \param n6 the name of the attribute to set
   * \param v6 the value of the attribute to set
   * \param n7 the name of the attribute to set
   * \param v7 the value of the attribute to set
   *
   * All the attributes specified in this method should exist
   * in the requested mac.
   */
  void SetType (std::string type,
                std::string n0 = "", const AttributeValue &v0 = EmptyAttributeValue (),
                std::string n1 = "", const AttributeValue &v1 = EmptyAttributeValue (),
                std::string n2 = "", const AttributeValue &v2 = EmptyAttributeValue (),
                std::string n3 = "", const AttributeValue &v3 = EmptyAttributeValue (),
                std::string n4 = "", const AttributeValue &v4 = EmptyAttributeValue (),
                std::string n5 = "", const AttributeValue &v5 = EmptyAttributeValue (),
                std::string n6 = "", const AttributeValue &v6 = EmptyAttributeValue (),
                std::string n7 = "", const AttributeValue &v7 = EmptyAttributeValue ());
  /**
   * Set the class, type and attribuytes for the Msdu agregator
   *
   * \param accessClass access class for which we are setting aggregator. Possibilities
   *  are: AC_BK, AC_BE, AC_VI, AC_VO.
   * \param type the type of ns3::WifiMac to create.
   * \param n0 the name of the attribute to set
   * \param v0 the value of the attribute to set
   * \param n1 the name of the attribute to set
   * \param v1 the value of the attribute to set
   * \param n2 the name of the attribute to set
   * \param v2 the value of the attribute to set
   * \param n3 the name of the attribute to set
   * \param v3 the value of the attribute to set
   *
   * All the attributes specified in this method should exist
   * in the requested aggregator.
   */
  void SetMsduAggregatorForAc (AccessClass accessClass, std::string type,
                               std::string n0 = "", const AttributeValue &v0 = EmptyAttributeValue (),
                               std::string n1 = "", const AttributeValue &v1 = EmptyAttributeValue (),
                               std::string n2 = "", const AttributeValue &v2 = EmptyAttributeValue (),
                               std::string n3 = "", const AttributeValue &v3 = EmptyAttributeValue ());
private:
  /**
   * \internal
   * \returns a newly-created MAC object.
   *
   * This method implements the pure virtual method defined in \ref ns3::WifiMacHelper.
   */
  virtual Ptr<WifiMac> Create (void) const;
  void Setup (Ptr<WifiMac> mac, enum AccessClass ac, std::string dcaAttrName) const;


  ObjectFactory m_mac;
  std::map<AccessClass, ObjectFactory> m_aggregators;
};

} //namespace ns3

#endif /* QOS_WIFI_MAC_HELPER_H */
