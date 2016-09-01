from pybindgen import Module, FileCodeSink, param, retval, cppclass, typehandlers

def register_types(module):
    root_module = module.get_root()
    
    ## animation-interface.h: ns3::AnimationInterface [class]
    module.add_class('AnimationInterface')
    ## application-container.h: ns3::ApplicationContainer [class]
    module.add_class('ApplicationContainer')
    ## athstats-helper.h: ns3::AthstatsHelper [class]
    module.add_class('AthstatsHelper', allow_subclassing=False)
    ## bridge-helper.h: ns3::BridgeHelper [class]
    module.add_class('BridgeHelper', allow_subclassing=False)
    ## csma-helper.h: ns3::CsmaHelper [class]
    module.add_class('CsmaHelper', allow_subclassing=False)
    ## csma-star-helper.h: ns3::CsmaStarHelper [class]
    module.add_class('CsmaStarHelper', allow_subclassing=False)
    ## emu-helper.h: ns3::EmuHelper [class]
    module.add_class('EmuHelper', allow_subclassing=False)
    ## flow-monitor-helper.h: ns3::FlowMonitorHelper [class]
    module.add_class('FlowMonitorHelper', allow_subclassing=False)
    ## internet-stack-helper.h: ns3::InternetStackHelper [class]
    module.add_class('InternetStackHelper', allow_subclassing=False)
    ## ipv4-address-helper.h: ns3::Ipv4AddressHelper [class]
    module.add_class('Ipv4AddressHelper', allow_subclassing=False)
    ## ipv4-interface-container.h: ns3::Ipv4InterfaceContainer [class]
    module.add_class('Ipv4InterfaceContainer')
    ## ipv4-routing-helper.h: ns3::Ipv4RoutingHelper [class]
    module.add_class('Ipv4RoutingHelper', allow_subclassing=False)
    ## ipv4-static-routing-helper.h: ns3::Ipv4StaticRoutingHelper [class]
    module.add_class('Ipv4StaticRoutingHelper', allow_subclassing=False, parent=root_module['ns3::Ipv4RoutingHelper'])
    ## ipv6-address-helper.h: ns3::Ipv6AddressHelper [class]
    module.add_class('Ipv6AddressHelper', allow_subclassing=False)
    ## ipv6-interface-container.h: ns3::Ipv6InterfaceContainer [class]
    module.add_class('Ipv6InterfaceContainer')
    ## ipv6-routing-helper.h: ns3::Ipv6RoutingHelper [class]
    module.add_class('Ipv6RoutingHelper', allow_subclassing=False)
    ## ipv6-static-routing-helper.h: ns3::Ipv6StaticRoutingHelper [class]
    module.add_class('Ipv6StaticRoutingHelper', allow_subclassing=False, parent=root_module['ns3::Ipv6RoutingHelper'])
    ## mesh-helper.h: ns3::MeshHelper [class]
    module.add_class('MeshHelper', allow_subclassing=False)
    ## mesh-helper.h: ns3::MeshHelper::ChannelPolicy [enumeration]
    module.add_enum('ChannelPolicy', ['SPREAD_CHANNELS', 'ZERO_CHANNEL'], outer_class=root_module['ns3::MeshHelper'])
    ## mobility-helper.h: ns3::MobilityHelper [class]
    module.add_class('MobilityHelper', allow_subclassing=False)
    ## net-device-container.h: ns3::NetDeviceContainer [class]
    module.add_class('NetDeviceContainer')
    ## node-container.h: ns3::NodeContainer [class]
    module.add_class('NodeContainer')
    ## ns2-mobility-helper.h: ns3::Ns2MobilityHelper [class]
    module.add_class('Ns2MobilityHelper', allow_subclassing=False)
    ## olsr-helper.h: ns3::OlsrHelper [class]
    module.add_class('OlsrHelper', allow_subclassing=False, parent=root_module['ns3::Ipv4RoutingHelper'])
    ## on-off-helper.h: ns3::OnOffHelper [class]
    module.add_class('OnOffHelper', allow_subclassing=False)
    ## packet-sink-helper.h: ns3::PacketSinkHelper [class]
    module.add_class('PacketSinkHelper', allow_subclassing=False)
    ## packet-socket-helper.h: ns3::PacketSocketHelper [class]
    module.add_class('PacketSocketHelper', allow_subclassing=False)
    ## ping6-helper.h: ns3::Ping6Helper [class]
    module.add_class('Ping6Helper', allow_subclassing=False)
    ## point-to-point-dumbbell-helper.h: ns3::PointToPointDumbbellHelper [class]
    module.add_class('PointToPointDumbbellHelper', allow_subclassing=False)
    ## point-to-point-grid-helper.h: ns3::PointToPointGridHelper [class]
    module.add_class('PointToPointGridHelper', allow_subclassing=False)
    ## point-to-point-helper.h: ns3::PointToPointHelper [class]
    module.add_class('PointToPointHelper', allow_subclassing=False)
    ## point-to-point-star-helper.h: ns3::PointToPointStarHelper [class]
    module.add_class('PointToPointStarHelper', allow_subclassing=False)
    ## tap-bridge-helper.h: ns3::TapBridgeHelper [class]
    module.add_class('TapBridgeHelper', allow_subclassing=False)
    ## udp-client-server-helper.h: ns3::UdpClientHelper [class]
    module.add_class('UdpClientHelper', allow_subclassing=False)
    ## udp-echo-helper.h: ns3::UdpEchoClientHelper [class]
    module.add_class('UdpEchoClientHelper', allow_subclassing=False)
    ## udp-echo-helper.h: ns3::UdpEchoServerHelper [class]
    module.add_class('UdpEchoServerHelper', allow_subclassing=False)
    ## udp-client-server-helper.h: ns3::UdpServerHelper [class]
    module.add_class('UdpServerHelper', allow_subclassing=False)
    ## udp-client-server-helper.h: ns3::UdpTraceClientHelper [class]
    module.add_class('UdpTraceClientHelper', allow_subclassing=False)
    ## v4ping-helper.h: ns3::V4PingHelper [class]
    module.add_class('V4PingHelper', allow_subclassing=False)
    ## wifi-helper.h: ns3::WifiHelper [class]
    module.add_class('WifiHelper', allow_subclassing=False)
    ## wifi-helper.h: ns3::WifiMacHelper [class]
    module.add_class('WifiMacHelper', allow_subclassing=False)
    ## wifi-helper.h: ns3::WifiPhyHelper [class]
    module.add_class('WifiPhyHelper', allow_subclassing=False)
    ## yans-wifi-helper.h: ns3::YansWifiChannelHelper [class]
    module.add_class('YansWifiChannelHelper', allow_subclassing=False)
    ## yans-wifi-helper.h: ns3::YansWifiPhyHelper [class]
    module.add_class('YansWifiPhyHelper', allow_subclassing=False, parent=root_module['ns3::WifiPhyHelper'])
    ## yans-wifi-helper.h: ns3::YansWifiPhyHelper::PcapFormat [enumeration]
    module.add_enum('PcapFormat', ['PCAP_FORMAT_80211', 'PCAP_FORMAT_80211_PRISM', 'PCAP_FORMAT_80211_RADIOTAP'], outer_class=root_module['ns3::YansWifiPhyHelper'])
    ## aodv-helper.h: ns3::AodvHelper [class]
    module.add_class('AodvHelper', allow_subclassing=False, parent=root_module['ns3::Ipv4RoutingHelper'])
    ## ipv4-global-routing-helper.h: ns3::Ipv4GlobalRoutingHelper [class]
    module.add_class('Ipv4GlobalRoutingHelper', allow_subclassing=False, parent=root_module['ns3::Ipv4RoutingHelper'])
    ## ipv4-list-routing-helper.h: ns3::Ipv4ListRoutingHelper [class]
    module.add_class('Ipv4ListRoutingHelper', allow_subclassing=False, parent=root_module['ns3::Ipv4RoutingHelper'])
    ## ipv4-nix-vector-helper.h: ns3::Ipv4NixVectorHelper [class]
    module.add_class('Ipv4NixVectorHelper', allow_subclassing=False, parent=root_module['ns3::Ipv4RoutingHelper'])
    ## ipv6-list-routing-helper.h: ns3::Ipv6ListRoutingHelper [class]
    module.add_class('Ipv6ListRoutingHelper', allow_subclassing=False, parent=root_module['ns3::Ipv6RoutingHelper'])
    ## nqos-wifi-mac-helper.h: ns3::NqosWifiMacHelper [class]
    module.add_class('NqosWifiMacHelper', allow_subclassing=False, parent=root_module['ns3::WifiMacHelper'])
    ## qos-wifi-mac-helper.h: ns3::QosWifiMacHelper [class]
    module.add_class('QosWifiMacHelper', allow_subclassing=False, parent=root_module['ns3::WifiMacHelper'])
    ## athstats-helper.h: ns3::AthstatsWifiTraceSink [class]
    module.add_class('AthstatsWifiTraceSink', parent=root_module['ns3::Object'])
    ## canvas-location.h: ns3::CanvasLocation [class]
    module.add_class('CanvasLocation', parent=root_module['ns3::Object'])
    ## mesh-stack-installer.h: ns3::MeshStack [class]
    module.add_class('MeshStack', parent=root_module['ns3::Object'])
    ## dot11s-installer.h: ns3::Dot11sStack [class]
    module.add_class('Dot11sStack', parent=root_module['ns3::MeshStack'])
    ## flame-installer.h: ns3::FlameStack [class]
    module.add_class('FlameStack', parent=root_module['ns3::MeshStack'])
    
    ## Register a nested module for the namespace Config
    
    nested_module = module.add_cpp_namespace('Config')
    register_types_ns3_Config(nested_module)
    
    
    ## Register a nested module for the namespace TimeStepPrecision
    
    nested_module = module.add_cpp_namespace('TimeStepPrecision')
    register_types_ns3_TimeStepPrecision(nested_module)
    
    
    ## Register a nested module for the namespace addressUtils
    
    nested_module = module.add_cpp_namespace('addressUtils')
    register_types_ns3_addressUtils(nested_module)
    
    
    ## Register a nested module for the namespace aodv
    
    nested_module = module.add_cpp_namespace('aodv')
    register_types_ns3_aodv(nested_module)
    
    
    ## Register a nested module for the namespace dot11s
    
    nested_module = module.add_cpp_namespace('dot11s')
    register_types_ns3_dot11s(nested_module)
    
    
    ## Register a nested module for the namespace flame
    
    nested_module = module.add_cpp_namespace('flame')
    register_types_ns3_flame(nested_module)
    
    
    ## Register a nested module for the namespace internal
    
    nested_module = module.add_cpp_namespace('internal')
    register_types_ns3_internal(nested_module)
    
    
    ## Register a nested module for the namespace olsr
    
    nested_module = module.add_cpp_namespace('olsr')
    register_types_ns3_olsr(nested_module)
    

def register_types_ns3_Config(module):
    root_module = module.get_root()
    

def register_types_ns3_TimeStepPrecision(module):
    root_module = module.get_root()
    

def register_types_ns3_addressUtils(module):
    root_module = module.get_root()
    

def register_types_ns3_aodv(module):
    root_module = module.get_root()
    

def register_types_ns3_dot11s(module):
    root_module = module.get_root()
    

def register_types_ns3_flame(module):
    root_module = module.get_root()
    

def register_types_ns3_internal(module):
    root_module = module.get_root()
    

def register_types_ns3_olsr(module):
    root_module = module.get_root()
    

def register_methods(root_module):
    register_Ns3AnimationInterface_methods(root_module, root_module['ns3::AnimationInterface'])
    register_Ns3ApplicationContainer_methods(root_module, root_module['ns3::ApplicationContainer'])
    register_Ns3AthstatsHelper_methods(root_module, root_module['ns3::AthstatsHelper'])
    register_Ns3BridgeHelper_methods(root_module, root_module['ns3::BridgeHelper'])
    register_Ns3CsmaHelper_methods(root_module, root_module['ns3::CsmaHelper'])
    register_Ns3CsmaStarHelper_methods(root_module, root_module['ns3::CsmaStarHelper'])
    register_Ns3EmuHelper_methods(root_module, root_module['ns3::EmuHelper'])
    register_Ns3FlowMonitorHelper_methods(root_module, root_module['ns3::FlowMonitorHelper'])
    register_Ns3InternetStackHelper_methods(root_module, root_module['ns3::InternetStackHelper'])
    register_Ns3Ipv4AddressHelper_methods(root_module, root_module['ns3::Ipv4AddressHelper'])
    register_Ns3Ipv4InterfaceContainer_methods(root_module, root_module['ns3::Ipv4InterfaceContainer'])
    register_Ns3Ipv4RoutingHelper_methods(root_module, root_module['ns3::Ipv4RoutingHelper'])
    register_Ns3Ipv4StaticRoutingHelper_methods(root_module, root_module['ns3::Ipv4StaticRoutingHelper'])
    register_Ns3Ipv6AddressHelper_methods(root_module, root_module['ns3::Ipv6AddressHelper'])
    register_Ns3Ipv6InterfaceContainer_methods(root_module, root_module['ns3::Ipv6InterfaceContainer'])
    register_Ns3Ipv6RoutingHelper_methods(root_module, root_module['ns3::Ipv6RoutingHelper'])
    register_Ns3Ipv6StaticRoutingHelper_methods(root_module, root_module['ns3::Ipv6StaticRoutingHelper'])
    register_Ns3MeshHelper_methods(root_module, root_module['ns3::MeshHelper'])
    register_Ns3MobilityHelper_methods(root_module, root_module['ns3::MobilityHelper'])
    register_Ns3NetDeviceContainer_methods(root_module, root_module['ns3::NetDeviceContainer'])
    register_Ns3NodeContainer_methods(root_module, root_module['ns3::NodeContainer'])
    register_Ns3Ns2MobilityHelper_methods(root_module, root_module['ns3::Ns2MobilityHelper'])
    register_Ns3OlsrHelper_methods(root_module, root_module['ns3::OlsrHelper'])
    register_Ns3OnOffHelper_methods(root_module, root_module['ns3::OnOffHelper'])
    register_Ns3PacketSinkHelper_methods(root_module, root_module['ns3::PacketSinkHelper'])
    register_Ns3PacketSocketHelper_methods(root_module, root_module['ns3::PacketSocketHelper'])
    register_Ns3Ping6Helper_methods(root_module, root_module['ns3::Ping6Helper'])
    register_Ns3PointToPointDumbbellHelper_methods(root_module, root_module['ns3::PointToPointDumbbellHelper'])
    register_Ns3PointToPointGridHelper_methods(root_module, root_module['ns3::PointToPointGridHelper'])
    register_Ns3PointToPointHelper_methods(root_module, root_module['ns3::PointToPointHelper'])
    register_Ns3PointToPointStarHelper_methods(root_module, root_module['ns3::PointToPointStarHelper'])
    register_Ns3TapBridgeHelper_methods(root_module, root_module['ns3::TapBridgeHelper'])
    register_Ns3UdpClientHelper_methods(root_module, root_module['ns3::UdpClientHelper'])
    register_Ns3UdpEchoClientHelper_methods(root_module, root_module['ns3::UdpEchoClientHelper'])
    register_Ns3UdpEchoServerHelper_methods(root_module, root_module['ns3::UdpEchoServerHelper'])
    register_Ns3UdpServerHelper_methods(root_module, root_module['ns3::UdpServerHelper'])
    register_Ns3UdpTraceClientHelper_methods(root_module, root_module['ns3::UdpTraceClientHelper'])
    register_Ns3V4PingHelper_methods(root_module, root_module['ns3::V4PingHelper'])
    register_Ns3WifiHelper_methods(root_module, root_module['ns3::WifiHelper'])
    register_Ns3WifiMacHelper_methods(root_module, root_module['ns3::WifiMacHelper'])
    register_Ns3WifiPhyHelper_methods(root_module, root_module['ns3::WifiPhyHelper'])
    register_Ns3YansWifiChannelHelper_methods(root_module, root_module['ns3::YansWifiChannelHelper'])
    register_Ns3YansWifiPhyHelper_methods(root_module, root_module['ns3::YansWifiPhyHelper'])
    register_Ns3AodvHelper_methods(root_module, root_module['ns3::AodvHelper'])
    register_Ns3Ipv4GlobalRoutingHelper_methods(root_module, root_module['ns3::Ipv4GlobalRoutingHelper'])
    register_Ns3Ipv4ListRoutingHelper_methods(root_module, root_module['ns3::Ipv4ListRoutingHelper'])
    register_Ns3Ipv4NixVectorHelper_methods(root_module, root_module['ns3::Ipv4NixVectorHelper'])
    register_Ns3Ipv6ListRoutingHelper_methods(root_module, root_module['ns3::Ipv6ListRoutingHelper'])
    register_Ns3NqosWifiMacHelper_methods(root_module, root_module['ns3::NqosWifiMacHelper'])
    register_Ns3QosWifiMacHelper_methods(root_module, root_module['ns3::QosWifiMacHelper'])
    register_Ns3AthstatsWifiTraceSink_methods(root_module, root_module['ns3::AthstatsWifiTraceSink'])
    register_Ns3CanvasLocation_methods(root_module, root_module['ns3::CanvasLocation'])
    register_Ns3MeshStack_methods(root_module, root_module['ns3::MeshStack'])
    register_Ns3Dot11sStack_methods(root_module, root_module['ns3::Dot11sStack'])
    register_Ns3FlameStack_methods(root_module, root_module['ns3::FlameStack'])
    return

def register_Ns3AnimationInterface_methods(root_module, cls):
    ## animation-interface.h: ns3::AnimationInterface::AnimationInterface(ns3::AnimationInterface const & arg0) [copy constructor]
    cls.add_constructor([param('ns3::AnimationInterface const &', 'arg0')])
    ## animation-interface.h: ns3::AnimationInterface::AnimationInterface() [constructor]
    cls.add_constructor([])
    ## animation-interface.h: bool ns3::AnimationInterface::SetOutputFile(std::string const & fn) [member function]
    cls.add_method('SetOutputFile', 
                   'bool', 
                   [param('std::string const &', 'fn')])
    ## animation-interface.h: bool ns3::AnimationInterface::SetServerPort(uint16_t port) [member function]
    cls.add_method('SetServerPort', 
                   'bool', 
                   [param('uint16_t', 'port')])
    ## animation-interface.h: void ns3::AnimationInterface::StartAnimation() [member function]
    cls.add_method('StartAnimation', 
                   'void', 
                   [])
    ## animation-interface.h: void ns3::AnimationInterface::StopAnimation() [member function]
    cls.add_method('StopAnimation', 
                   'void', 
                   [])
    return

def register_Ns3ApplicationContainer_methods(root_module, cls):
    ## application-container.h: ns3::ApplicationContainer::ApplicationContainer(ns3::ApplicationContainer const & arg0) [copy constructor]
    cls.add_constructor([param('ns3::ApplicationContainer const &', 'arg0')])
    ## application-container.h: ns3::ApplicationContainer::ApplicationContainer() [constructor]
    cls.add_constructor([])
    ## application-container.h: ns3::ApplicationContainer::ApplicationContainer(ns3::Ptr<ns3::Application> application) [constructor]
    cls.add_constructor([param('ns3::Ptr< ns3::Application >', 'application')])
    ## application-container.h: ns3::ApplicationContainer::ApplicationContainer(std::string name) [constructor]
    cls.add_constructor([param('std::string', 'name')])
    ## application-container.h: void ns3::ApplicationContainer::Add(ns3::ApplicationContainer other) [member function]
    cls.add_method('Add', 
                   'void', 
                   [param('ns3::ApplicationContainer', 'other')])
    ## application-container.h: void ns3::ApplicationContainer::Add(ns3::Ptr<ns3::Application> application) [member function]
    cls.add_method('Add', 
                   'void', 
                   [param('ns3::Ptr< ns3::Application >', 'application')])
    ## application-container.h: void ns3::ApplicationContainer::Add(std::string name) [member function]
    cls.add_method('Add', 
                   'void', 
                   [param('std::string', 'name')])
    ## application-container.h: __gnu_cxx::__normal_iterator<const ns3::Ptr<ns3::Application>*,std::vector<ns3::Ptr<ns3::Application>, std::allocator<ns3::Ptr<ns3::Application> > > > ns3::ApplicationContainer::Begin() const [member function]
    cls.add_method('Begin', 
                   '__gnu_cxx::__normal_iterator< ns3::Ptr< ns3::Application > const, std::vector< ns3::Ptr< ns3::Application > > >', 
                   [], 
                   is_const=True)
    ## application-container.h: __gnu_cxx::__normal_iterator<const ns3::Ptr<ns3::Application>*,std::vector<ns3::Ptr<ns3::Application>, std::allocator<ns3::Ptr<ns3::Application> > > > ns3::ApplicationContainer::End() const [member function]
    cls.add_method('End', 
                   '__gnu_cxx::__normal_iterator< ns3::Ptr< ns3::Application > const, std::vector< ns3::Ptr< ns3::Application > > >', 
                   [], 
                   is_const=True)
    ## application-container.h: ns3::Ptr<ns3::Application> ns3::ApplicationContainer::Get(uint32_t i) const [member function]
    cls.add_method('Get', 
                   'ns3::Ptr< ns3::Application >', 
                   [param('uint32_t', 'i')], 
                   is_const=True)
    ## application-container.h: uint32_t ns3::ApplicationContainer::GetN() const [member function]
    cls.add_method('GetN', 
                   'uint32_t', 
                   [], 
                   is_const=True)
    ## application-container.h: void ns3::ApplicationContainer::Start(ns3::Time start) [member function]
    cls.add_method('Start', 
                   'void', 
                   [param('ns3::Time', 'start')])
    ## application-container.h: void ns3::ApplicationContainer::Stop(ns3::Time stop) [member function]
    cls.add_method('Stop', 
                   'void', 
                   [param('ns3::Time', 'stop')])
    return

def register_Ns3AthstatsHelper_methods(root_module, cls):
    ## athstats-helper.h: ns3::AthstatsHelper::AthstatsHelper(ns3::AthstatsHelper const & arg0) [copy constructor]
    cls.add_constructor([param('ns3::AthstatsHelper const &', 'arg0')])
    ## athstats-helper.h: ns3::AthstatsHelper::AthstatsHelper() [constructor]
    cls.add_constructor([])
    ## athstats-helper.h: void ns3::AthstatsHelper::EnableAthstats(std::string filename, uint32_t nodeid, uint32_t deviceid) [member function]
    cls.add_method('EnableAthstats', 
                   'void', 
                   [param('std::string', 'filename'), param('uint32_t', 'nodeid'), param('uint32_t', 'deviceid')])
    ## athstats-helper.h: void ns3::AthstatsHelper::EnableAthstats(std::string filename, ns3::Ptr<ns3::NetDevice> nd) [member function]
    cls.add_method('EnableAthstats', 
                   'void', 
                   [param('std::string', 'filename'), param('ns3::Ptr< ns3::NetDevice >', 'nd')])
    ## athstats-helper.h: void ns3::AthstatsHelper::EnableAthstats(std::string filename, ns3::NetDeviceContainer d) [member function]
    cls.add_method('EnableAthstats', 
                   'void', 
                   [param('std::string', 'filename'), param('ns3::NetDeviceContainer', 'd')])
    ## athstats-helper.h: void ns3::AthstatsHelper::EnableAthstats(std::string filename, ns3::NodeContainer n) [member function]
    cls.add_method('EnableAthstats', 
                   'void', 
                   [param('std::string', 'filename'), param('ns3::NodeContainer', 'n')])
    return

def register_Ns3BridgeHelper_methods(root_module, cls):
    ## bridge-helper.h: ns3::BridgeHelper::BridgeHelper(ns3::BridgeHelper const & arg0) [copy constructor]
    cls.add_constructor([param('ns3::BridgeHelper const &', 'arg0')])
    ## bridge-helper.h: ns3::BridgeHelper::BridgeHelper() [constructor]
    cls.add_constructor([])
    ## bridge-helper.h: ns3::NetDeviceContainer ns3::BridgeHelper::Install(ns3::Ptr<ns3::Node> node, ns3::NetDeviceContainer c) [member function]
    cls.add_method('Install', 
                   'ns3::NetDeviceContainer', 
                   [param('ns3::Ptr< ns3::Node >', 'node'), param('ns3::NetDeviceContainer', 'c')])
    ## bridge-helper.h: ns3::NetDeviceContainer ns3::BridgeHelper::Install(std::string nodeName, ns3::NetDeviceContainer c) [member function]
    cls.add_method('Install', 
                   'ns3::NetDeviceContainer', 
                   [param('std::string', 'nodeName'), param('ns3::NetDeviceContainer', 'c')])
    ## bridge-helper.h: void ns3::BridgeHelper::SetDeviceAttribute(std::string n1, ns3::AttributeValue const & v1) [member function]
    cls.add_method('SetDeviceAttribute', 
                   'void', 
                   [param('std::string', 'n1'), param('ns3::AttributeValue const &', 'v1')])
    return

def register_Ns3CsmaHelper_methods(root_module, cls):
    ## csma-helper.h: ns3::CsmaHelper::CsmaHelper(ns3::CsmaHelper const & arg0) [copy constructor]
    cls.add_constructor([param('ns3::CsmaHelper const &', 'arg0')])
    ## csma-helper.h: ns3::CsmaHelper::CsmaHelper() [constructor]
    cls.add_constructor([])
    ## csma-helper.h: static void ns3::CsmaHelper::EnableAscii(std::ostream & os, uint32_t nodeid, uint32_t deviceid) [member function]
    cls.add_method('EnableAscii', 
                   'void', 
                   [param('std::ostream &', 'os'), param('uint32_t', 'nodeid'), param('uint32_t', 'deviceid')], 
                   is_static=True)
    ## csma-helper.h: static void ns3::CsmaHelper::EnableAscii(std::ostream & os, ns3::NetDeviceContainer d) [member function]
    cls.add_method('EnableAscii', 
                   'void', 
                   [param('std::ostream &', 'os'), param('ns3::NetDeviceContainer', 'd')], 
                   is_static=True)
    ## csma-helper.h: static void ns3::CsmaHelper::EnableAscii(std::ostream & os, ns3::NodeContainer n) [member function]
    cls.add_method('EnableAscii', 
                   'void', 
                   [param('std::ostream &', 'os'), param('ns3::NodeContainer', 'n')], 
                   is_static=True)
    ## csma-helper.h: static void ns3::CsmaHelper::EnableAsciiAll(std::ostream & os) [member function]
    cls.add_method('EnableAsciiAll', 
                   'void', 
                   [param('std::ostream &', 'os')], 
                   is_static=True)
    ## csma-helper.h: static void ns3::CsmaHelper::EnablePcap(std::string filename, uint32_t nodeid, uint32_t deviceid, bool promiscuous) [member function]
    cls.add_method('EnablePcap', 
                   'void', 
                   [param('std::string', 'filename'), param('uint32_t', 'nodeid'), param('uint32_t', 'deviceid'), param('bool', 'promiscuous')], 
                   is_static=True)
    ## csma-helper.h: static void ns3::CsmaHelper::EnablePcap(std::string filename, ns3::Ptr<ns3::NetDevice> nd, bool promiscuous) [member function]
    cls.add_method('EnablePcap', 
                   'void', 
                   [param('std::string', 'filename'), param('ns3::Ptr< ns3::NetDevice >', 'nd'), param('bool', 'promiscuous')], 
                   is_static=True)
    ## csma-helper.h: static void ns3::CsmaHelper::EnablePcap(std::string filename, std::string ndName, bool promiscuous) [member function]
    cls.add_method('EnablePcap', 
                   'void', 
                   [param('std::string', 'filename'), param('std::string', 'ndName'), param('bool', 'promiscuous')], 
                   is_static=True)
    ## csma-helper.h: static void ns3::CsmaHelper::EnablePcap(std::string filename, ns3::NetDeviceContainer d, bool promiscuous) [member function]
    cls.add_method('EnablePcap', 
                   'void', 
                   [param('std::string', 'filename'), param('ns3::NetDeviceContainer', 'd'), param('bool', 'promiscuous')], 
                   is_static=True)
    ## csma-helper.h: static void ns3::CsmaHelper::EnablePcap(std::string filename, ns3::NodeContainer n, bool promiscuous) [member function]
    cls.add_method('EnablePcap', 
                   'void', 
                   [param('std::string', 'filename'), param('ns3::NodeContainer', 'n'), param('bool', 'promiscuous')], 
                   is_static=True)
    ## csma-helper.h: static void ns3::CsmaHelper::EnablePcapAll(std::string filename, bool promiscuous) [member function]
    cls.add_method('EnablePcapAll', 
                   'void', 
                   [param('std::string', 'filename'), param('bool', 'promiscuous')], 
                   is_static=True)
    ## csma-helper.h: ns3::NetDeviceContainer ns3::CsmaHelper::Install(ns3::Ptr<ns3::Node> node) const [member function]
    cls.add_method('Install', 
                   'ns3::NetDeviceContainer', 
                   [param('ns3::Ptr< ns3::Node >', 'node')], 
                   is_const=True)
    ## csma-helper.h: ns3::NetDeviceContainer ns3::CsmaHelper::Install(std::string name) const [member function]
    cls.add_method('Install', 
                   'ns3::NetDeviceContainer', 
                   [param('std::string', 'name')], 
                   is_const=True)
    ## csma-helper.h: ns3::NetDeviceContainer ns3::CsmaHelper::Install(ns3::Ptr<ns3::Node> node, ns3::Ptr<ns3::CsmaChannel> channel) const [member function]
    cls.add_method('Install', 
                   'ns3::NetDeviceContainer', 
                   [param('ns3::Ptr< ns3::Node >', 'node'), param('ns3::Ptr< ns3::CsmaChannel >', 'channel')], 
                   is_const=True)
    ## csma-helper.h: ns3::NetDeviceContainer ns3::CsmaHelper::Install(ns3::Ptr<ns3::Node> node, std::string channelName) const [member function]
    cls.add_method('Install', 
                   'ns3::NetDeviceContainer', 
                   [param('ns3::Ptr< ns3::Node >', 'node'), param('std::string', 'channelName')], 
                   is_const=True)
    ## csma-helper.h: ns3::NetDeviceContainer ns3::CsmaHelper::Install(std::string nodeName, ns3::Ptr<ns3::CsmaChannel> channel) const [member function]
    cls.add_method('Install', 
                   'ns3::NetDeviceContainer', 
                   [param('std::string', 'nodeName'), param('ns3::Ptr< ns3::CsmaChannel >', 'channel')], 
                   is_const=True)
    ## csma-helper.h: ns3::NetDeviceContainer ns3::CsmaHelper::Install(std::string nodeName, std::string channelName) const [member function]
    cls.add_method('Install', 
                   'ns3::NetDeviceContainer', 
                   [param('std::string', 'nodeName'), param('std::string', 'channelName')], 
                   is_const=True)
    ## csma-helper.h: ns3::NetDeviceContainer ns3::CsmaHelper::Install(ns3::NodeContainer const & c) const [member function]
    cls.add_method('Install', 
                   'ns3::NetDeviceContainer', 
                   [param('ns3::NodeContainer const &', 'c')], 
                   is_const=True)
    ## csma-helper.h: ns3::NetDeviceContainer ns3::CsmaHelper::Install(ns3::NodeContainer const & c, ns3::Ptr<ns3::CsmaChannel> channel) const [member function]
    cls.add_method('Install', 
                   'ns3::NetDeviceContainer', 
                   [param('ns3::NodeContainer const &', 'c'), param('ns3::Ptr< ns3::CsmaChannel >', 'channel')], 
                   is_const=True)
    ## csma-helper.h: ns3::NetDeviceContainer ns3::CsmaHelper::Install(ns3::NodeContainer const & c, std::string channelName) const [member function]
    cls.add_method('Install', 
                   'ns3::NetDeviceContainer', 
                   [param('ns3::NodeContainer const &', 'c'), param('std::string', 'channelName')], 
                   is_const=True)
    ## csma-helper.h: void ns3::CsmaHelper::SetChannelAttribute(std::string n1, ns3::AttributeValue const & v1) [member function]
    cls.add_method('SetChannelAttribute', 
                   'void', 
                   [param('std::string', 'n1'), param('ns3::AttributeValue const &', 'v1')])
    ## csma-helper.h: void ns3::CsmaHelper::SetDeviceAttribute(std::string n1, ns3::AttributeValue const & v1) [member function]
    cls.add_method('SetDeviceAttribute', 
                   'void', 
                   [param('std::string', 'n1'), param('ns3::AttributeValue const &', 'v1')])
    ## csma-helper.h: void ns3::CsmaHelper::SetQueue(std::string type, std::string n1="", ns3::AttributeValue const & v1=ns3::EmptyAttributeValue(), std::string n2="", ns3::AttributeValue const & v2=ns3::EmptyAttributeValue(), std::string n3="", ns3::AttributeValue const & v3=ns3::EmptyAttributeValue(), std::string n4="", ns3::AttributeValue const & v4=ns3::EmptyAttributeValue()) [member function]
    cls.add_method('SetQueue', 
                   'void', 
                   [param('std::string', 'type'), param('std::string', 'n1', default_value='""'), param('ns3::AttributeValue const &', 'v1', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n2', default_value='""'), param('ns3::AttributeValue const &', 'v2', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n3', default_value='""'), param('ns3::AttributeValue const &', 'v3', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n4', default_value='""'), param('ns3::AttributeValue const &', 'v4', default_value='ns3::EmptyAttributeValue()')])
    return

def register_Ns3CsmaStarHelper_methods(root_module, cls):
    ## csma-star-helper.h: ns3::CsmaStarHelper::CsmaStarHelper(ns3::CsmaStarHelper const & arg0) [copy constructor]
    cls.add_constructor([param('ns3::CsmaStarHelper const &', 'arg0')])
    ## csma-star-helper.h: ns3::CsmaStarHelper::CsmaStarHelper(uint32_t numSpokes, ns3::CsmaHelper csmaHelper) [constructor]
    cls.add_constructor([param('uint32_t', 'numSpokes'), param('ns3::CsmaHelper', 'csmaHelper')])
    ## csma-star-helper.h: void ns3::CsmaStarHelper::AssignIpv4Addresses(ns3::Ipv4AddressHelper address) [member function]
    cls.add_method('AssignIpv4Addresses', 
                   'void', 
                   [param('ns3::Ipv4AddressHelper', 'address')])
    ## csma-star-helper.h: ns3::Ptr<ns3::Node> ns3::CsmaStarHelper::GetHub() const [member function]
    cls.add_method('GetHub', 
                   'ns3::Ptr< ns3::Node >', 
                   [], 
                   is_const=True)
    ## csma-star-helper.h: ns3::NetDeviceContainer ns3::CsmaStarHelper::GetHubDevices() const [member function]
    cls.add_method('GetHubDevices', 
                   'ns3::NetDeviceContainer', 
                   [], 
                   is_const=True)
    ## csma-star-helper.h: ns3::Ipv4Address ns3::CsmaStarHelper::GetHubIpv4Address(uint32_t i) const [member function]
    cls.add_method('GetHubIpv4Address', 
                   'ns3::Ipv4Address', 
                   [param('uint32_t', 'i')], 
                   is_const=True)
    ## csma-star-helper.h: ns3::NetDeviceContainer ns3::CsmaStarHelper::GetSpokeDevices() const [member function]
    cls.add_method('GetSpokeDevices', 
                   'ns3::NetDeviceContainer', 
                   [], 
                   is_const=True)
    ## csma-star-helper.h: ns3::Ipv4Address ns3::CsmaStarHelper::GetSpokeIpv4Address(uint32_t i) const [member function]
    cls.add_method('GetSpokeIpv4Address', 
                   'ns3::Ipv4Address', 
                   [param('uint32_t', 'i')], 
                   is_const=True)
    ## csma-star-helper.h: ns3::Ptr<ns3::Node> ns3::CsmaStarHelper::GetSpokeNode(uint32_t i) const [member function]
    cls.add_method('GetSpokeNode', 
                   'ns3::Ptr< ns3::Node >', 
                   [param('uint32_t', 'i')], 
                   is_const=True)
    ## csma-star-helper.h: void ns3::CsmaStarHelper::InstallStack(ns3::InternetStackHelper stack) [member function]
    cls.add_method('InstallStack', 
                   'void', 
                   [param('ns3::InternetStackHelper', 'stack')])
    ## csma-star-helper.h: uint32_t ns3::CsmaStarHelper::SpokeCount() const [member function]
    cls.add_method('SpokeCount', 
                   'uint32_t', 
                   [], 
                   is_const=True)
    return

def register_Ns3EmuHelper_methods(root_module, cls):
    ## emu-helper.h: ns3::EmuHelper::EmuHelper(ns3::EmuHelper const & arg0) [copy constructor]
    cls.add_constructor([param('ns3::EmuHelper const &', 'arg0')])
    ## emu-helper.h: ns3::EmuHelper::EmuHelper() [constructor]
    cls.add_constructor([])
    ## emu-helper.h: static void ns3::EmuHelper::EnableAscii(std::ostream & os, uint32_t nodeid, uint32_t deviceid) [member function]
    cls.add_method('EnableAscii', 
                   'void', 
                   [param('std::ostream &', 'os'), param('uint32_t', 'nodeid'), param('uint32_t', 'deviceid')], 
                   is_static=True)
    ## emu-helper.h: static void ns3::EmuHelper::EnableAscii(std::ostream & os, ns3::NetDeviceContainer d) [member function]
    cls.add_method('EnableAscii', 
                   'void', 
                   [param('std::ostream &', 'os'), param('ns3::NetDeviceContainer', 'd')], 
                   is_static=True)
    ## emu-helper.h: static void ns3::EmuHelper::EnableAscii(std::ostream & os, ns3::NodeContainer n) [member function]
    cls.add_method('EnableAscii', 
                   'void', 
                   [param('std::ostream &', 'os'), param('ns3::NodeContainer', 'n')], 
                   is_static=True)
    ## emu-helper.h: static void ns3::EmuHelper::EnableAsciiAll(std::ostream & os) [member function]
    cls.add_method('EnableAsciiAll', 
                   'void', 
                   [param('std::ostream &', 'os')], 
                   is_static=True)
    ## emu-helper.h: static void ns3::EmuHelper::EnablePcap(std::string filename, uint32_t nodeid, uint32_t deviceid, bool promiscuous) [member function]
    cls.add_method('EnablePcap', 
                   'void', 
                   [param('std::string', 'filename'), param('uint32_t', 'nodeid'), param('uint32_t', 'deviceid'), param('bool', 'promiscuous')], 
                   is_static=True)
    ## emu-helper.h: static void ns3::EmuHelper::EnablePcap(std::string filename, ns3::Ptr<ns3::NetDevice> nd, bool promiscuous) [member function]
    cls.add_method('EnablePcap', 
                   'void', 
                   [param('std::string', 'filename'), param('ns3::Ptr< ns3::NetDevice >', 'nd'), param('bool', 'promiscuous')], 
                   is_static=True)
    ## emu-helper.h: static void ns3::EmuHelper::EnablePcap(std::string filename, std::string ndName, bool promiscuous) [member function]
    cls.add_method('EnablePcap', 
                   'void', 
                   [param('std::string', 'filename'), param('std::string', 'ndName'), param('bool', 'promiscuous')], 
                   is_static=True)
    ## emu-helper.h: static void ns3::EmuHelper::EnablePcap(std::string filename, ns3::NetDeviceContainer d, bool promiscuous) [member function]
    cls.add_method('EnablePcap', 
                   'void', 
                   [param('std::string', 'filename'), param('ns3::NetDeviceContainer', 'd'), param('bool', 'promiscuous')], 
                   is_static=True)
    ## emu-helper.h: static void ns3::EmuHelper::EnablePcap(std::string filename, ns3::NodeContainer n, bool promiscuous) [member function]
    cls.add_method('EnablePcap', 
                   'void', 
                   [param('std::string', 'filename'), param('ns3::NodeContainer', 'n'), param('bool', 'promiscuous')], 
                   is_static=True)
    ## emu-helper.h: static void ns3::EmuHelper::EnablePcapAll(std::string filename, bool promiscuous) [member function]
    cls.add_method('EnablePcapAll', 
                   'void', 
                   [param('std::string', 'filename'), param('bool', 'promiscuous')], 
                   is_static=True)
    ## emu-helper.h: ns3::NetDeviceContainer ns3::EmuHelper::Install(ns3::Ptr<ns3::Node> node) const [member function]
    cls.add_method('Install', 
                   'ns3::NetDeviceContainer', 
                   [param('ns3::Ptr< ns3::Node >', 'node')], 
                   is_const=True)
    ## emu-helper.h: ns3::NetDeviceContainer ns3::EmuHelper::Install(std::string nodeName) const [member function]
    cls.add_method('Install', 
                   'ns3::NetDeviceContainer', 
                   [param('std::string', 'nodeName')], 
                   is_const=True)
    ## emu-helper.h: ns3::NetDeviceContainer ns3::EmuHelper::Install(ns3::NodeContainer const & c) const [member function]
    cls.add_method('Install', 
                   'ns3::NetDeviceContainer', 
                   [param('ns3::NodeContainer const &', 'c')], 
                   is_const=True)
    ## emu-helper.h: void ns3::EmuHelper::SetAttribute(std::string n1, ns3::AttributeValue const & v1) [member function]
    cls.add_method('SetAttribute', 
                   'void', 
                   [param('std::string', 'n1'), param('ns3::AttributeValue const &', 'v1')])
    ## emu-helper.h: void ns3::EmuHelper::SetQueue(std::string type, std::string n1="", ns3::AttributeValue const & v1=ns3::EmptyAttributeValue(), std::string n2="", ns3::AttributeValue const & v2=ns3::EmptyAttributeValue(), std::string n3="", ns3::AttributeValue const & v3=ns3::EmptyAttributeValue(), std::string n4="", ns3::AttributeValue const & v4=ns3::EmptyAttributeValue()) [member function]
    cls.add_method('SetQueue', 
                   'void', 
                   [param('std::string', 'type'), param('std::string', 'n1', default_value='""'), param('ns3::AttributeValue const &', 'v1', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n2', default_value='""'), param('ns3::AttributeValue const &', 'v2', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n3', default_value='""'), param('ns3::AttributeValue const &', 'v3', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n4', default_value='""'), param('ns3::AttributeValue const &', 'v4', default_value='ns3::EmptyAttributeValue()')])
    return

def register_Ns3FlowMonitorHelper_methods(root_module, cls):
    ## flow-monitor-helper.h: ns3::FlowMonitorHelper::FlowMonitorHelper(ns3::FlowMonitorHelper const & arg0) [copy constructor]
    cls.add_constructor([param('ns3::FlowMonitorHelper const &', 'arg0')])
    ## flow-monitor-helper.h: ns3::FlowMonitorHelper::FlowMonitorHelper() [constructor]
    cls.add_constructor([])
    ## flow-monitor-helper.h: ns3::Ptr<ns3::FlowClassifier> ns3::FlowMonitorHelper::GetClassifier() [member function]
    cls.add_method('GetClassifier', 
                   'ns3::Ptr< ns3::FlowClassifier >', 
                   [])
    ## flow-monitor-helper.h: ns3::Ptr<ns3::FlowMonitor> ns3::FlowMonitorHelper::GetMonitor() [member function]
    cls.add_method('GetMonitor', 
                   'ns3::Ptr< ns3::FlowMonitor >', 
                   [])
    ## flow-monitor-helper.h: ns3::Ptr<ns3::FlowMonitor> ns3::FlowMonitorHelper::Install(ns3::NodeContainer nodes) [member function]
    cls.add_method('Install', 
                   'ns3::Ptr< ns3::FlowMonitor >', 
                   [param('ns3::NodeContainer', 'nodes')])
    ## flow-monitor-helper.h: ns3::Ptr<ns3::FlowMonitor> ns3::FlowMonitorHelper::Install(ns3::Ptr<ns3::Node> node) [member function]
    cls.add_method('Install', 
                   'ns3::Ptr< ns3::FlowMonitor >', 
                   [param('ns3::Ptr< ns3::Node >', 'node')])
    ## flow-monitor-helper.h: ns3::Ptr<ns3::FlowMonitor> ns3::FlowMonitorHelper::InstallAll() [member function]
    cls.add_method('InstallAll', 
                   'ns3::Ptr< ns3::FlowMonitor >', 
                   [])
    ## flow-monitor-helper.h: void ns3::FlowMonitorHelper::SetMonitorAttribute(std::string n1, ns3::AttributeValue const & v1) [member function]
    cls.add_method('SetMonitorAttribute', 
                   'void', 
                   [param('std::string', 'n1'), param('ns3::AttributeValue const &', 'v1')])
    return

def register_Ns3InternetStackHelper_methods(root_module, cls):
    ## internet-stack-helper.h: ns3::InternetStackHelper::InternetStackHelper() [constructor]
    cls.add_constructor([])
    ## internet-stack-helper.h: ns3::InternetStackHelper::InternetStackHelper(ns3::InternetStackHelper const & arg0) [copy constructor]
    cls.add_constructor([param('ns3::InternetStackHelper const &', 'arg0')])
    ## internet-stack-helper.h: static void ns3::InternetStackHelper::EnableAscii(std::ostream & os, ns3::NodeContainer n) [member function]
    cls.add_method('EnableAscii', 
                   'void', 
                   [param('std::ostream &', 'os'), param('ns3::NodeContainer', 'n')], 
                   is_static=True)
    ## internet-stack-helper.h: static void ns3::InternetStackHelper::EnableAsciiAll(std::ostream & os) [member function]
    cls.add_method('EnableAsciiAll', 
                   'void', 
                   [param('std::ostream &', 'os')], 
                   is_static=True)
    ## internet-stack-helper.h: static void ns3::InternetStackHelper::EnablePcapAll(std::string filename) [member function]
    cls.add_method('EnablePcapAll', 
                   'void', 
                   [param('std::string', 'filename')], 
                   is_static=True)
    ## internet-stack-helper.h: void ns3::InternetStackHelper::Install(std::string nodeName) const [member function]
    cls.add_method('Install', 
                   'void', 
                   [param('std::string', 'nodeName')], 
                   is_const=True)
    ## internet-stack-helper.h: void ns3::InternetStackHelper::Install(ns3::Ptr<ns3::Node> node) const [member function]
    cls.add_method('Install', 
                   'void', 
                   [param('ns3::Ptr< ns3::Node >', 'node')], 
                   is_const=True)
    ## internet-stack-helper.h: void ns3::InternetStackHelper::Install(ns3::NodeContainer c) const [member function]
    cls.add_method('Install', 
                   'void', 
                   [param('ns3::NodeContainer', 'c')], 
                   is_const=True)
    ## internet-stack-helper.h: void ns3::InternetStackHelper::InstallAll() const [member function]
    cls.add_method('InstallAll', 
                   'void', 
                   [], 
                   is_const=True)
    ## internet-stack-helper.h: void ns3::InternetStackHelper::Reset() [member function]
    cls.add_method('Reset', 
                   'void', 
                   [])
    ## internet-stack-helper.h: void ns3::InternetStackHelper::SetIpv4StackInstall(bool enable) [member function]
    cls.add_method('SetIpv4StackInstall', 
                   'void', 
                   [param('bool', 'enable')])
    ## internet-stack-helper.h: void ns3::InternetStackHelper::SetIpv6StackInstall(bool enable) [member function]
    cls.add_method('SetIpv6StackInstall', 
                   'void', 
                   [param('bool', 'enable')])
    ## internet-stack-helper.h: void ns3::InternetStackHelper::SetRoutingHelper(ns3::Ipv4RoutingHelper const & routing) [member function]
    cls.add_method('SetRoutingHelper', 
                   'void', 
                   [param('ns3::Ipv4RoutingHelper const &', 'routing')])
    ## internet-stack-helper.h: void ns3::InternetStackHelper::SetRoutingHelper(ns3::Ipv6RoutingHelper const & routing) [member function]
    cls.add_method('SetRoutingHelper', 
                   'void', 
                   [param('ns3::Ipv6RoutingHelper const &', 'routing')])
    ## internet-stack-helper.h: void ns3::InternetStackHelper::SetTcp(std::string tid) [member function]
    cls.add_method('SetTcp', 
                   'void', 
                   [param('std::string', 'tid')])
    ## internet-stack-helper.h: void ns3::InternetStackHelper::SetTcp(std::string tid, std::string attr, ns3::AttributeValue const & val) [member function]
    cls.add_method('SetTcp', 
                   'void', 
                   [param('std::string', 'tid'), param('std::string', 'attr'), param('ns3::AttributeValue const &', 'val')])
    return

def register_Ns3Ipv4AddressHelper_methods(root_module, cls):
    ## ipv4-address-helper.h: ns3::Ipv4AddressHelper::Ipv4AddressHelper(ns3::Ipv4AddressHelper const & arg0) [copy constructor]
    cls.add_constructor([param('ns3::Ipv4AddressHelper const &', 'arg0')])
    ## ipv4-address-helper.h: ns3::Ipv4AddressHelper::Ipv4AddressHelper() [constructor]
    cls.add_constructor([])
    ## ipv4-address-helper.h: ns3::Ipv4AddressHelper::Ipv4AddressHelper(ns3::Ipv4Address network, ns3::Ipv4Mask mask, ns3::Ipv4Address base="0.0.0.1") [constructor]
    cls.add_constructor([param('ns3::Ipv4Address', 'network'), param('ns3::Ipv4Mask', 'mask'), param('ns3::Ipv4Address', 'base', default_value='"0.0.0.1"')])
    ## ipv4-address-helper.h: ns3::Ipv4InterfaceContainer ns3::Ipv4AddressHelper::Assign(ns3::NetDeviceContainer const & c) [member function]
    cls.add_method('Assign', 
                   'ns3::Ipv4InterfaceContainer', 
                   [param('ns3::NetDeviceContainer const &', 'c')])
    ## ipv4-address-helper.h: ns3::Ipv4Address ns3::Ipv4AddressHelper::NewAddress() [member function]
    cls.add_method('NewAddress', 
                   'ns3::Ipv4Address', 
                   [])
    ## ipv4-address-helper.h: ns3::Ipv4Address ns3::Ipv4AddressHelper::NewNetwork() [member function]
    cls.add_method('NewNetwork', 
                   'ns3::Ipv4Address', 
                   [])
    ## ipv4-address-helper.h: void ns3::Ipv4AddressHelper::SetBase(ns3::Ipv4Address network, ns3::Ipv4Mask mask, ns3::Ipv4Address base="0.0.0.1") [member function]
    cls.add_method('SetBase', 
                   'void', 
                   [param('ns3::Ipv4Address', 'network'), param('ns3::Ipv4Mask', 'mask'), param('ns3::Ipv4Address', 'base', default_value='"0.0.0.1"')])
    return

def register_Ns3Ipv4InterfaceContainer_methods(root_module, cls):
    ## ipv4-interface-container.h: ns3::Ipv4InterfaceContainer::Ipv4InterfaceContainer(ns3::Ipv4InterfaceContainer const & arg0) [copy constructor]
    cls.add_constructor([param('ns3::Ipv4InterfaceContainer const &', 'arg0')])
    ## ipv4-interface-container.h: ns3::Ipv4InterfaceContainer::Ipv4InterfaceContainer() [constructor]
    cls.add_constructor([])
    ## ipv4-interface-container.h: void ns3::Ipv4InterfaceContainer::Add(ns3::Ipv4InterfaceContainer other) [member function]
    cls.add_method('Add', 
                   'void', 
                   [param('ns3::Ipv4InterfaceContainer', 'other')])
    ## ipv4-interface-container.h: void ns3::Ipv4InterfaceContainer::Add(ns3::Ptr<ns3::Ipv4> ipv4, uint32_t interface) [member function]
    cls.add_method('Add', 
                   'void', 
                   [param('ns3::Ptr< ns3::Ipv4 >', 'ipv4'), param('uint32_t', 'interface')])
    ## ipv4-interface-container.h: void ns3::Ipv4InterfaceContainer::Add(std::pair<ns3::Ptr<ns3::Ipv4>,unsigned int> ipInterfacePair) [member function]
    cls.add_method('Add', 
                   'void', 
                   [param('std::pair< ns3::Ptr< ns3::Ipv4 >, unsigned int >', 'ipInterfacePair')])
    ## ipv4-interface-container.h: void ns3::Ipv4InterfaceContainer::Add(std::string ipv4Name, uint32_t interface) [member function]
    cls.add_method('Add', 
                   'void', 
                   [param('std::string', 'ipv4Name'), param('uint32_t', 'interface')])
    ## ipv4-interface-container.h: std::pair<ns3::Ptr<ns3::Ipv4>,unsigned int> ns3::Ipv4InterfaceContainer::Get(uint32_t i) const [member function]
    cls.add_method('Get', 
                   'std::pair< ns3::Ptr< ns3::Ipv4 >, unsigned int >', 
                   [param('uint32_t', 'i')], 
                   is_const=True)
    ## ipv4-interface-container.h: ns3::Ipv4Address ns3::Ipv4InterfaceContainer::GetAddress(uint32_t i, uint32_t j=0) const [member function]
    cls.add_method('GetAddress', 
                   'ns3::Ipv4Address', 
                   [param('uint32_t', 'i'), param('uint32_t', 'j', default_value='0')], 
                   is_const=True)
    ## ipv4-interface-container.h: uint32_t ns3::Ipv4InterfaceContainer::GetN() const [member function]
    cls.add_method('GetN', 
                   'uint32_t', 
                   [], 
                   is_const=True)
    ## ipv4-interface-container.h: void ns3::Ipv4InterfaceContainer::SetMetric(uint32_t i, uint16_t metric) [member function]
    cls.add_method('SetMetric', 
                   'void', 
                   [param('uint32_t', 'i'), param('uint16_t', 'metric')])
    return

def register_Ns3Ipv4RoutingHelper_methods(root_module, cls):
    ## ipv4-routing-helper.h: ns3::Ipv4RoutingHelper::Ipv4RoutingHelper() [constructor]
    cls.add_constructor([])
    ## ipv4-routing-helper.h: ns3::Ipv4RoutingHelper::Ipv4RoutingHelper(ns3::Ipv4RoutingHelper const & arg0) [copy constructor]
    cls.add_constructor([param('ns3::Ipv4RoutingHelper const &', 'arg0')])
    ## ipv4-routing-helper.h: ns3::Ipv4RoutingHelper * ns3::Ipv4RoutingHelper::Copy() const [member function]
    cls.add_method('Copy', 
                   'ns3::Ipv4RoutingHelper *', 
                   [], 
                   is_pure_virtual=True, is_const=True, is_virtual=True)
    ## ipv4-routing-helper.h: ns3::Ptr<ns3::Ipv4RoutingProtocol> ns3::Ipv4RoutingHelper::Create(ns3::Ptr<ns3::Node> node) const [member function]
    cls.add_method('Create', 
                   'ns3::Ptr< ns3::Ipv4RoutingProtocol >', 
                   [param('ns3::Ptr< ns3::Node >', 'node')], 
                   is_pure_virtual=True, is_const=True, is_virtual=True)
    return

def register_Ns3Ipv4StaticRoutingHelper_methods(root_module, cls):
    ## ipv4-static-routing-helper.h: ns3::Ipv4StaticRoutingHelper::Ipv4StaticRoutingHelper() [constructor]
    cls.add_constructor([])
    ## ipv4-static-routing-helper.h: ns3::Ipv4StaticRoutingHelper::Ipv4StaticRoutingHelper(ns3::Ipv4StaticRoutingHelper const & arg0) [copy constructor]
    cls.add_constructor([param('ns3::Ipv4StaticRoutingHelper const &', 'arg0')])
    ## ipv4-static-routing-helper.h: ns3::Ipv4StaticRoutingHelper * ns3::Ipv4StaticRoutingHelper::Copy() const [member function]
    cls.add_method('Copy', 
                   'ns3::Ipv4StaticRoutingHelper *', 
                   [], 
                   is_const=True, is_virtual=True)
    ## ipv4-static-routing-helper.h: ns3::Ptr<ns3::Ipv4RoutingProtocol> ns3::Ipv4StaticRoutingHelper::Create(ns3::Ptr<ns3::Node> node) const [member function]
    cls.add_method('Create', 
                   'ns3::Ptr< ns3::Ipv4RoutingProtocol >', 
                   [param('ns3::Ptr< ns3::Node >', 'node')], 
                   is_const=True, is_virtual=True)
    ## ipv4-static-routing-helper.h: ns3::Ptr<ns3::Ipv4StaticRouting> ns3::Ipv4StaticRoutingHelper::GetStaticRouting(ns3::Ptr<ns3::Ipv4> ipv4) const [member function]
    cls.add_method('GetStaticRouting', 
                   'ns3::Ptr< ns3::Ipv4StaticRouting >', 
                   [param('ns3::Ptr< ns3::Ipv4 >', 'ipv4')], 
                   is_const=True)
    ## ipv4-static-routing-helper.h: void ns3::Ipv4StaticRoutingHelper::AddMulticastRoute(ns3::Ptr<ns3::Node> n, ns3::Ipv4Address source, ns3::Ipv4Address group, ns3::Ptr<ns3::NetDevice> input, ns3::NetDeviceContainer output) [member function]
    cls.add_method('AddMulticastRoute', 
                   'void', 
                   [param('ns3::Ptr< ns3::Node >', 'n'), param('ns3::Ipv4Address', 'source'), param('ns3::Ipv4Address', 'group'), param('ns3::Ptr< ns3::NetDevice >', 'input'), param('ns3::NetDeviceContainer', 'output')])
    ## ipv4-static-routing-helper.h: void ns3::Ipv4StaticRoutingHelper::AddMulticastRoute(std::string n, ns3::Ipv4Address source, ns3::Ipv4Address group, ns3::Ptr<ns3::NetDevice> input, ns3::NetDeviceContainer output) [member function]
    cls.add_method('AddMulticastRoute', 
                   'void', 
                   [param('std::string', 'n'), param('ns3::Ipv4Address', 'source'), param('ns3::Ipv4Address', 'group'), param('ns3::Ptr< ns3::NetDevice >', 'input'), param('ns3::NetDeviceContainer', 'output')])
    ## ipv4-static-routing-helper.h: void ns3::Ipv4StaticRoutingHelper::AddMulticastRoute(ns3::Ptr<ns3::Node> n, ns3::Ipv4Address source, ns3::Ipv4Address group, std::string inputName, ns3::NetDeviceContainer output) [member function]
    cls.add_method('AddMulticastRoute', 
                   'void', 
                   [param('ns3::Ptr< ns3::Node >', 'n'), param('ns3::Ipv4Address', 'source'), param('ns3::Ipv4Address', 'group'), param('std::string', 'inputName'), param('ns3::NetDeviceContainer', 'output')])
    ## ipv4-static-routing-helper.h: void ns3::Ipv4StaticRoutingHelper::AddMulticastRoute(std::string nName, ns3::Ipv4Address source, ns3::Ipv4Address group, std::string inputName, ns3::NetDeviceContainer output) [member function]
    cls.add_method('AddMulticastRoute', 
                   'void', 
                   [param('std::string', 'nName'), param('ns3::Ipv4Address', 'source'), param('ns3::Ipv4Address', 'group'), param('std::string', 'inputName'), param('ns3::NetDeviceContainer', 'output')])
    ## ipv4-static-routing-helper.h: void ns3::Ipv4StaticRoutingHelper::SetDefaultMulticastRoute(ns3::Ptr<ns3::Node> n, ns3::Ptr<ns3::NetDevice> nd) [member function]
    cls.add_method('SetDefaultMulticastRoute', 
                   'void', 
                   [param('ns3::Ptr< ns3::Node >', 'n'), param('ns3::Ptr< ns3::NetDevice >', 'nd')])
    ## ipv4-static-routing-helper.h: void ns3::Ipv4StaticRoutingHelper::SetDefaultMulticastRoute(ns3::Ptr<ns3::Node> n, std::string ndName) [member function]
    cls.add_method('SetDefaultMulticastRoute', 
                   'void', 
                   [param('ns3::Ptr< ns3::Node >', 'n'), param('std::string', 'ndName')])
    ## ipv4-static-routing-helper.h: void ns3::Ipv4StaticRoutingHelper::SetDefaultMulticastRoute(std::string nName, ns3::Ptr<ns3::NetDevice> nd) [member function]
    cls.add_method('SetDefaultMulticastRoute', 
                   'void', 
                   [param('std::string', 'nName'), param('ns3::Ptr< ns3::NetDevice >', 'nd')])
    ## ipv4-static-routing-helper.h: void ns3::Ipv4StaticRoutingHelper::SetDefaultMulticastRoute(std::string nName, std::string ndName) [member function]
    cls.add_method('SetDefaultMulticastRoute', 
                   'void', 
                   [param('std::string', 'nName'), param('std::string', 'ndName')])
    return

def register_Ns3Ipv6AddressHelper_methods(root_module, cls):
    ## ipv6-address-helper.h: ns3::Ipv6AddressHelper::Ipv6AddressHelper(ns3::Ipv6AddressHelper const & arg0) [copy constructor]
    cls.add_constructor([param('ns3::Ipv6AddressHelper const &', 'arg0')])
    ## ipv6-address-helper.h: ns3::Ipv6AddressHelper::Ipv6AddressHelper() [constructor]
    cls.add_constructor([])
    ## ipv6-address-helper.h: ns3::Ipv6InterfaceContainer ns3::Ipv6AddressHelper::Assign(ns3::NetDeviceContainer const & c) [member function]
    cls.add_method('Assign', 
                   'ns3::Ipv6InterfaceContainer', 
                   [param('ns3::NetDeviceContainer const &', 'c')])
    ## ipv6-address-helper.h: ns3::Ipv6InterfaceContainer ns3::Ipv6AddressHelper::Assign(ns3::NetDeviceContainer const & c, std::vector<bool,std::allocator<bool> > withConfiguration) [member function]
    cls.add_method('Assign', 
                   'ns3::Ipv6InterfaceContainer', 
                   [param('ns3::NetDeviceContainer const &', 'c'), param('std::vector< bool >', 'withConfiguration')])
    ## ipv6-address-helper.h: ns3::Ipv6InterfaceContainer ns3::Ipv6AddressHelper::AssignWithoutAddress(ns3::NetDeviceContainer const & c) [member function]
    cls.add_method('AssignWithoutAddress', 
                   'ns3::Ipv6InterfaceContainer', 
                   [param('ns3::NetDeviceContainer const &', 'c')])
    ## ipv6-address-helper.h: ns3::Ipv6Address ns3::Ipv6AddressHelper::NewAddress(ns3::Address addr) [member function]
    cls.add_method('NewAddress', 
                   'ns3::Ipv6Address', 
                   [param('ns3::Address', 'addr')])
    ## ipv6-address-helper.h: void ns3::Ipv6AddressHelper::NewNetwork(ns3::Ipv6Address network, ns3::Ipv6Prefix prefix) [member function]
    cls.add_method('NewNetwork', 
                   'void', 
                   [param('ns3::Ipv6Address', 'network'), param('ns3::Ipv6Prefix', 'prefix')])
    return

def register_Ns3Ipv6InterfaceContainer_methods(root_module, cls):
    ## ipv6-interface-container.h: ns3::Ipv6InterfaceContainer::Ipv6InterfaceContainer(ns3::Ipv6InterfaceContainer const & arg0) [copy constructor]
    cls.add_constructor([param('ns3::Ipv6InterfaceContainer const &', 'arg0')])
    ## ipv6-interface-container.h: ns3::Ipv6InterfaceContainer::Ipv6InterfaceContainer() [constructor]
    cls.add_constructor([])
    ## ipv6-interface-container.h: void ns3::Ipv6InterfaceContainer::Add(ns3::Ptr<ns3::Ipv6> ipv6, uint32_t interface) [member function]
    cls.add_method('Add', 
                   'void', 
                   [param('ns3::Ptr< ns3::Ipv6 >', 'ipv6'), param('uint32_t', 'interface')])
    ## ipv6-interface-container.h: void ns3::Ipv6InterfaceContainer::Add(ns3::Ipv6InterfaceContainer & c) [member function]
    cls.add_method('Add', 
                   'void', 
                   [param('ns3::Ipv6InterfaceContainer &', 'c')])
    ## ipv6-interface-container.h: void ns3::Ipv6InterfaceContainer::Add(std::string ipv6Name, uint32_t interface) [member function]
    cls.add_method('Add', 
                   'void', 
                   [param('std::string', 'ipv6Name'), param('uint32_t', 'interface')])
    ## ipv6-interface-container.h: ns3::Ipv6Address ns3::Ipv6InterfaceContainer::GetAddress(uint32_t i, uint32_t j) const [member function]
    cls.add_method('GetAddress', 
                   'ns3::Ipv6Address', 
                   [param('uint32_t', 'i'), param('uint32_t', 'j')], 
                   is_const=True)
    ## ipv6-interface-container.h: uint32_t ns3::Ipv6InterfaceContainer::GetInterfaceIndex(uint32_t i) const [member function]
    cls.add_method('GetInterfaceIndex', 
                   'uint32_t', 
                   [param('uint32_t', 'i')], 
                   is_const=True)
    ## ipv6-interface-container.h: uint32_t ns3::Ipv6InterfaceContainer::GetN() const [member function]
    cls.add_method('GetN', 
                   'uint32_t', 
                   [], 
                   is_const=True)
    ## ipv6-interface-container.h: void ns3::Ipv6InterfaceContainer::SetDefaultRoute(uint32_t i, uint32_t router) [member function]
    cls.add_method('SetDefaultRoute', 
                   'void', 
                   [param('uint32_t', 'i'), param('uint32_t', 'router')])
    ## ipv6-interface-container.h: void ns3::Ipv6InterfaceContainer::SetRouter(uint32_t i, bool router) [member function]
    cls.add_method('SetRouter', 
                   'void', 
                   [param('uint32_t', 'i'), param('bool', 'router')])
    return

def register_Ns3Ipv6RoutingHelper_methods(root_module, cls):
    ## ipv6-routing-helper.h: ns3::Ipv6RoutingHelper::Ipv6RoutingHelper() [constructor]
    cls.add_constructor([])
    ## ipv6-routing-helper.h: ns3::Ipv6RoutingHelper::Ipv6RoutingHelper(ns3::Ipv6RoutingHelper const & arg0) [copy constructor]
    cls.add_constructor([param('ns3::Ipv6RoutingHelper const &', 'arg0')])
    ## ipv6-routing-helper.h: ns3::Ipv6RoutingHelper * ns3::Ipv6RoutingHelper::Copy() const [member function]
    cls.add_method('Copy', 
                   'ns3::Ipv6RoutingHelper *', 
                   [], 
                   is_pure_virtual=True, is_const=True, is_virtual=True)
    ## ipv6-routing-helper.h: ns3::Ptr<ns3::Ipv6RoutingProtocol> ns3::Ipv6RoutingHelper::Create(ns3::Ptr<ns3::Node> node) const [member function]
    cls.add_method('Create', 
                   'ns3::Ptr< ns3::Ipv6RoutingProtocol >', 
                   [param('ns3::Ptr< ns3::Node >', 'node')], 
                   is_pure_virtual=True, is_const=True, is_virtual=True)
    return

def register_Ns3Ipv6StaticRoutingHelper_methods(root_module, cls):
    ## ipv6-static-routing-helper.h: ns3::Ipv6StaticRoutingHelper::Ipv6StaticRoutingHelper() [constructor]
    cls.add_constructor([])
    ## ipv6-static-routing-helper.h: ns3::Ipv6StaticRoutingHelper::Ipv6StaticRoutingHelper(ns3::Ipv6StaticRoutingHelper const & arg0) [copy constructor]
    cls.add_constructor([param('ns3::Ipv6StaticRoutingHelper const &', 'arg0')])
    ## ipv6-static-routing-helper.h: ns3::Ipv6StaticRoutingHelper * ns3::Ipv6StaticRoutingHelper::Copy() const [member function]
    cls.add_method('Copy', 
                   'ns3::Ipv6StaticRoutingHelper *', 
                   [], 
                   is_const=True, is_virtual=True)
    ## ipv6-static-routing-helper.h: ns3::Ptr<ns3::Ipv6RoutingProtocol> ns3::Ipv6StaticRoutingHelper::Create(ns3::Ptr<ns3::Node> node) const [member function]
    cls.add_method('Create', 
                   'ns3::Ptr< ns3::Ipv6RoutingProtocol >', 
                   [param('ns3::Ptr< ns3::Node >', 'node')], 
                   is_const=True, is_virtual=True)
    ## ipv6-static-routing-helper.h: ns3::Ptr<ns3::Ipv6StaticRouting> ns3::Ipv6StaticRoutingHelper::GetStaticRouting(ns3::Ptr<ns3::Ipv6> ipv6) const [member function]
    cls.add_method('GetStaticRouting', 
                   'ns3::Ptr< ns3::Ipv6StaticRouting >', 
                   [param('ns3::Ptr< ns3::Ipv6 >', 'ipv6')], 
                   is_const=True)
    ## ipv6-static-routing-helper.h: void ns3::Ipv6StaticRoutingHelper::AddMulticastRoute(ns3::Ptr<ns3::Node> n, ns3::Ipv6Address source, ns3::Ipv6Address group, ns3::Ptr<ns3::NetDevice> input, ns3::NetDeviceContainer output) [member function]
    cls.add_method('AddMulticastRoute', 
                   'void', 
                   [param('ns3::Ptr< ns3::Node >', 'n'), param('ns3::Ipv6Address', 'source'), param('ns3::Ipv6Address', 'group'), param('ns3::Ptr< ns3::NetDevice >', 'input'), param('ns3::NetDeviceContainer', 'output')])
    ## ipv6-static-routing-helper.h: void ns3::Ipv6StaticRoutingHelper::AddMulticastRoute(std::string n, ns3::Ipv6Address source, ns3::Ipv6Address group, ns3::Ptr<ns3::NetDevice> input, ns3::NetDeviceContainer output) [member function]
    cls.add_method('AddMulticastRoute', 
                   'void', 
                   [param('std::string', 'n'), param('ns3::Ipv6Address', 'source'), param('ns3::Ipv6Address', 'group'), param('ns3::Ptr< ns3::NetDevice >', 'input'), param('ns3::NetDeviceContainer', 'output')])
    ## ipv6-static-routing-helper.h: void ns3::Ipv6StaticRoutingHelper::AddMulticastRoute(ns3::Ptr<ns3::Node> n, ns3::Ipv6Address source, ns3::Ipv6Address group, std::string inputName, ns3::NetDeviceContainer output) [member function]
    cls.add_method('AddMulticastRoute', 
                   'void', 
                   [param('ns3::Ptr< ns3::Node >', 'n'), param('ns3::Ipv6Address', 'source'), param('ns3::Ipv6Address', 'group'), param('std::string', 'inputName'), param('ns3::NetDeviceContainer', 'output')])
    ## ipv6-static-routing-helper.h: void ns3::Ipv6StaticRoutingHelper::AddMulticastRoute(std::string nName, ns3::Ipv6Address source, ns3::Ipv6Address group, std::string inputName, ns3::NetDeviceContainer output) [member function]
    cls.add_method('AddMulticastRoute', 
                   'void', 
                   [param('std::string', 'nName'), param('ns3::Ipv6Address', 'source'), param('ns3::Ipv6Address', 'group'), param('std::string', 'inputName'), param('ns3::NetDeviceContainer', 'output')])
    return

def register_Ns3MeshHelper_methods(root_module, cls):
    ## mesh-helper.h: ns3::MeshHelper::MeshHelper(ns3::MeshHelper const & arg0) [copy constructor]
    cls.add_constructor([param('ns3::MeshHelper const &', 'arg0')])
    ## mesh-helper.h: ns3::MeshHelper::MeshHelper() [constructor]
    cls.add_constructor([])
    ## mesh-helper.h: static ns3::MeshHelper ns3::MeshHelper::Default() [member function]
    cls.add_method('Default', 
                   'ns3::MeshHelper', 
                   [], 
                   is_static=True)
    ## mesh-helper.h: ns3::NetDeviceContainer ns3::MeshHelper::Install(ns3::WifiPhyHelper const & phyHelper, ns3::NodeContainer c) const [member function]
    cls.add_method('Install', 
                   'ns3::NetDeviceContainer', 
                   [param('ns3::WifiPhyHelper const &', 'phyHelper'), param('ns3::NodeContainer', 'c')], 
                   is_const=True)
    ## mesh-helper.h: void ns3::MeshHelper::Report(ns3::Ptr<ns3::NetDevice> const & arg0, std::ostream & arg1) [member function]
    cls.add_method('Report', 
                   'void', 
                   [param('ns3::Ptr< ns3::NetDevice > const &', 'arg0'), param('std::ostream &', 'arg1')])
    ## mesh-helper.h: void ns3::MeshHelper::ResetStats(ns3::Ptr<ns3::NetDevice> const & arg0) [member function]
    cls.add_method('ResetStats', 
                   'void', 
                   [param('ns3::Ptr< ns3::NetDevice > const &', 'arg0')])
    ## mesh-helper.h: void ns3::MeshHelper::SetMacType(std::string n0="", ns3::AttributeValue const & v0=ns3::EmptyAttributeValue(), std::string n1="", ns3::AttributeValue const & v1=ns3::EmptyAttributeValue(), std::string n2="", ns3::AttributeValue const & v2=ns3::EmptyAttributeValue(), std::string n3="", ns3::AttributeValue const & v3=ns3::EmptyAttributeValue(), std::string n4="", ns3::AttributeValue const & v4=ns3::EmptyAttributeValue(), std::string n5="", ns3::AttributeValue const & v5=ns3::EmptyAttributeValue(), std::string n6="", ns3::AttributeValue const & v6=ns3::EmptyAttributeValue(), std::string n7="", ns3::AttributeValue const & v7=ns3::EmptyAttributeValue()) [member function]
    cls.add_method('SetMacType', 
                   'void', 
                   [param('std::string', 'n0', default_value='""'), param('ns3::AttributeValue const &', 'v0', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n1', default_value='""'), param('ns3::AttributeValue const &', 'v1', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n2', default_value='""'), param('ns3::AttributeValue const &', 'v2', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n3', default_value='""'), param('ns3::AttributeValue const &', 'v3', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n4', default_value='""'), param('ns3::AttributeValue const &', 'v4', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n5', default_value='""'), param('ns3::AttributeValue const &', 'v5', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n6', default_value='""'), param('ns3::AttributeValue const &', 'v6', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n7', default_value='""'), param('ns3::AttributeValue const &', 'v7', default_value='ns3::EmptyAttributeValue()')])
    ## mesh-helper.h: void ns3::MeshHelper::SetNumberOfInterfaces(uint32_t nInterfaces) [member function]
    cls.add_method('SetNumberOfInterfaces', 
                   'void', 
                   [param('uint32_t', 'nInterfaces')])
    ## mesh-helper.h: void ns3::MeshHelper::SetRemoteStationManager(std::string type, std::string n0="", ns3::AttributeValue const & v0=ns3::EmptyAttributeValue(), std::string n1="", ns3::AttributeValue const & v1=ns3::EmptyAttributeValue(), std::string n2="", ns3::AttributeValue const & v2=ns3::EmptyAttributeValue(), std::string n3="", ns3::AttributeValue const & v3=ns3::EmptyAttributeValue(), std::string n4="", ns3::AttributeValue const & v4=ns3::EmptyAttributeValue(), std::string n5="", ns3::AttributeValue const & v5=ns3::EmptyAttributeValue(), std::string n6="", ns3::AttributeValue const & v6=ns3::EmptyAttributeValue(), std::string n7="", ns3::AttributeValue const & v7=ns3::EmptyAttributeValue()) [member function]
    cls.add_method('SetRemoteStationManager', 
                   'void', 
                   [param('std::string', 'type'), param('std::string', 'n0', default_value='""'), param('ns3::AttributeValue const &', 'v0', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n1', default_value='""'), param('ns3::AttributeValue const &', 'v1', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n2', default_value='""'), param('ns3::AttributeValue const &', 'v2', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n3', default_value='""'), param('ns3::AttributeValue const &', 'v3', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n4', default_value='""'), param('ns3::AttributeValue const &', 'v4', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n5', default_value='""'), param('ns3::AttributeValue const &', 'v5', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n6', default_value='""'), param('ns3::AttributeValue const &', 'v6', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n7', default_value='""'), param('ns3::AttributeValue const &', 'v7', default_value='ns3::EmptyAttributeValue()')])
    ## mesh-helper.h: void ns3::MeshHelper::SetSpreadInterfaceChannels(ns3::MeshHelper::ChannelPolicy arg0) [member function]
    cls.add_method('SetSpreadInterfaceChannels', 
                   'void', 
                   [param('ns3::MeshHelper::ChannelPolicy', 'arg0')])
    ## mesh-helper.h: void ns3::MeshHelper::SetStackInstaller(std::string type, std::string n0="", ns3::AttributeValue const & v0=ns3::EmptyAttributeValue(), std::string n1="", ns3::AttributeValue const & v1=ns3::EmptyAttributeValue(), std::string n2="", ns3::AttributeValue const & v2=ns3::EmptyAttributeValue(), std::string n3="", ns3::AttributeValue const & v3=ns3::EmptyAttributeValue(), std::string n4="", ns3::AttributeValue const & v4=ns3::EmptyAttributeValue(), std::string n5="", ns3::AttributeValue const & v5=ns3::EmptyAttributeValue(), std::string n6="", ns3::AttributeValue const & v6=ns3::EmptyAttributeValue(), std::string n7="", ns3::AttributeValue const & v7=ns3::EmptyAttributeValue()) [member function]
    cls.add_method('SetStackInstaller', 
                   'void', 
                   [param('std::string', 'type'), param('std::string', 'n0', default_value='""'), param('ns3::AttributeValue const &', 'v0', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n1', default_value='""'), param('ns3::AttributeValue const &', 'v1', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n2', default_value='""'), param('ns3::AttributeValue const &', 'v2', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n3', default_value='""'), param('ns3::AttributeValue const &', 'v3', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n4', default_value='""'), param('ns3::AttributeValue const &', 'v4', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n5', default_value='""'), param('ns3::AttributeValue const &', 'v5', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n6', default_value='""'), param('ns3::AttributeValue const &', 'v6', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n7', default_value='""'), param('ns3::AttributeValue const &', 'v7', default_value='ns3::EmptyAttributeValue()')])
    ## mesh-helper.h: void ns3::MeshHelper::SetStandard(ns3::WifiPhyStandard standard) [member function]
    cls.add_method('SetStandard', 
                   'void', 
                   [param('ns3::WifiPhyStandard', 'standard')])
    return

def register_Ns3MobilityHelper_methods(root_module, cls):
    ## mobility-helper.h: ns3::MobilityHelper::MobilityHelper(ns3::MobilityHelper const & arg0) [copy constructor]
    cls.add_constructor([param('ns3::MobilityHelper const &', 'arg0')])
    ## mobility-helper.h: ns3::MobilityHelper::MobilityHelper() [constructor]
    cls.add_constructor([])
    ## mobility-helper.h: static void ns3::MobilityHelper::EnableAscii(std::ostream & os, uint32_t nodeid) [member function]
    cls.add_method('EnableAscii', 
                   'void', 
                   [param('std::ostream &', 'os'), param('uint32_t', 'nodeid')], 
                   is_static=True)
    ## mobility-helper.h: static void ns3::MobilityHelper::EnableAscii(std::ostream & os, ns3::NodeContainer n) [member function]
    cls.add_method('EnableAscii', 
                   'void', 
                   [param('std::ostream &', 'os'), param('ns3::NodeContainer', 'n')], 
                   is_static=True)
    ## mobility-helper.h: static void ns3::MobilityHelper::EnableAsciiAll(std::ostream & os) [member function]
    cls.add_method('EnableAsciiAll', 
                   'void', 
                   [param('std::ostream &', 'os')], 
                   is_static=True)
    ## mobility-helper.h: std::string ns3::MobilityHelper::GetMobilityModelType() const [member function]
    cls.add_method('GetMobilityModelType', 
                   'std::string', 
                   [], 
                   is_const=True)
    ## mobility-helper.h: void ns3::MobilityHelper::Install(ns3::Ptr<ns3::Node> node) const [member function]
    cls.add_method('Install', 
                   'void', 
                   [param('ns3::Ptr< ns3::Node >', 'node')], 
                   is_const=True)
    ## mobility-helper.h: void ns3::MobilityHelper::Install(std::string nodeName) const [member function]
    cls.add_method('Install', 
                   'void', 
                   [param('std::string', 'nodeName')], 
                   is_const=True)
    ## mobility-helper.h: void ns3::MobilityHelper::Install(ns3::NodeContainer container) const [member function]
    cls.add_method('Install', 
                   'void', 
                   [param('ns3::NodeContainer', 'container')], 
                   is_const=True)
    ## mobility-helper.h: void ns3::MobilityHelper::InstallAll() [member function]
    cls.add_method('InstallAll', 
                   'void', 
                   [])
    ## mobility-helper.h: void ns3::MobilityHelper::PopReferenceMobilityModel() [member function]
    cls.add_method('PopReferenceMobilityModel', 
                   'void', 
                   [])
    ## mobility-helper.h: void ns3::MobilityHelper::PushReferenceMobilityModel(ns3::Ptr<ns3::Object> reference) [member function]
    cls.add_method('PushReferenceMobilityModel', 
                   'void', 
                   [param('ns3::Ptr< ns3::Object >', 'reference')])
    ## mobility-helper.h: void ns3::MobilityHelper::PushReferenceMobilityModel(std::string referenceName) [member function]
    cls.add_method('PushReferenceMobilityModel', 
                   'void', 
                   [param('std::string', 'referenceName')])
    ## mobility-helper.h: void ns3::MobilityHelper::SetMobilityModel(std::string type, std::string n1="", ns3::AttributeValue const & v1=ns3::EmptyAttributeValue(), std::string n2="", ns3::AttributeValue const & v2=ns3::EmptyAttributeValue(), std::string n3="", ns3::AttributeValue const & v3=ns3::EmptyAttributeValue(), std::string n4="", ns3::AttributeValue const & v4=ns3::EmptyAttributeValue(), std::string n5="", ns3::AttributeValue const & v5=ns3::EmptyAttributeValue(), std::string n6="", ns3::AttributeValue const & v6=ns3::EmptyAttributeValue(), std::string n7="", ns3::AttributeValue const & v7=ns3::EmptyAttributeValue(), std::string n8="", ns3::AttributeValue const & v8=ns3::EmptyAttributeValue(), std::string n9="", ns3::AttributeValue const & v9=ns3::EmptyAttributeValue()) [member function]
    cls.add_method('SetMobilityModel', 
                   'void', 
                   [param('std::string', 'type'), param('std::string', 'n1', default_value='""'), param('ns3::AttributeValue const &', 'v1', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n2', default_value='""'), param('ns3::AttributeValue const &', 'v2', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n3', default_value='""'), param('ns3::AttributeValue const &', 'v3', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n4', default_value='""'), param('ns3::AttributeValue const &', 'v4', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n5', default_value='""'), param('ns3::AttributeValue const &', 'v5', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n6', default_value='""'), param('ns3::AttributeValue const &', 'v6', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n7', default_value='""'), param('ns3::AttributeValue const &', 'v7', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n8', default_value='""'), param('ns3::AttributeValue const &', 'v8', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n9', default_value='""'), param('ns3::AttributeValue const &', 'v9', default_value='ns3::EmptyAttributeValue()')])
    ## mobility-helper.h: void ns3::MobilityHelper::SetPositionAllocator(ns3::Ptr<ns3::PositionAllocator> allocator) [member function]
    cls.add_method('SetPositionAllocator', 
                   'void', 
                   [param('ns3::Ptr< ns3::PositionAllocator >', 'allocator')])
    ## mobility-helper.h: void ns3::MobilityHelper::SetPositionAllocator(std::string type, std::string n1="", ns3::AttributeValue const & v1=ns3::EmptyAttributeValue(), std::string n2="", ns3::AttributeValue const & v2=ns3::EmptyAttributeValue(), std::string n3="", ns3::AttributeValue const & v3=ns3::EmptyAttributeValue(), std::string n4="", ns3::AttributeValue const & v4=ns3::EmptyAttributeValue(), std::string n5="", ns3::AttributeValue const & v5=ns3::EmptyAttributeValue(), std::string n6="", ns3::AttributeValue const & v6=ns3::EmptyAttributeValue(), std::string n7="", ns3::AttributeValue const & v7=ns3::EmptyAttributeValue(), std::string n8="", ns3::AttributeValue const & v8=ns3::EmptyAttributeValue(), std::string n9="", ns3::AttributeValue const & v9=ns3::EmptyAttributeValue()) [member function]
    cls.add_method('SetPositionAllocator', 
                   'void', 
                   [param('std::string', 'type'), param('std::string', 'n1', default_value='""'), param('ns3::AttributeValue const &', 'v1', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n2', default_value='""'), param('ns3::AttributeValue const &', 'v2', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n3', default_value='""'), param('ns3::AttributeValue const &', 'v3', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n4', default_value='""'), param('ns3::AttributeValue const &', 'v4', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n5', default_value='""'), param('ns3::AttributeValue const &', 'v5', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n6', default_value='""'), param('ns3::AttributeValue const &', 'v6', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n7', default_value='""'), param('ns3::AttributeValue const &', 'v7', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n8', default_value='""'), param('ns3::AttributeValue const &', 'v8', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n9', default_value='""'), param('ns3::AttributeValue const &', 'v9', default_value='ns3::EmptyAttributeValue()')])
    return

def register_Ns3NetDeviceContainer_methods(root_module, cls):
    ## net-device-container.h: ns3::NetDeviceContainer::NetDeviceContainer(ns3::NetDeviceContainer const & arg0) [copy constructor]
    cls.add_constructor([param('ns3::NetDeviceContainer const &', 'arg0')])
    ## net-device-container.h: ns3::NetDeviceContainer::NetDeviceContainer() [constructor]
    cls.add_constructor([])
    ## net-device-container.h: ns3::NetDeviceContainer::NetDeviceContainer(ns3::Ptr<ns3::NetDevice> dev) [constructor]
    cls.add_constructor([param('ns3::Ptr< ns3::NetDevice >', 'dev')])
    ## net-device-container.h: ns3::NetDeviceContainer::NetDeviceContainer(std::string devName) [constructor]
    cls.add_constructor([param('std::string', 'devName')])
    ## net-device-container.h: ns3::NetDeviceContainer::NetDeviceContainer(ns3::NetDeviceContainer const & a, ns3::NetDeviceContainer const & b) [constructor]
    cls.add_constructor([param('ns3::NetDeviceContainer const &', 'a'), param('ns3::NetDeviceContainer const &', 'b')])
    ## net-device-container.h: void ns3::NetDeviceContainer::Add(ns3::NetDeviceContainer other) [member function]
    cls.add_method('Add', 
                   'void', 
                   [param('ns3::NetDeviceContainer', 'other')])
    ## net-device-container.h: void ns3::NetDeviceContainer::Add(ns3::Ptr<ns3::NetDevice> device) [member function]
    cls.add_method('Add', 
                   'void', 
                   [param('ns3::Ptr< ns3::NetDevice >', 'device')])
    ## net-device-container.h: void ns3::NetDeviceContainer::Add(std::string deviceName) [member function]
    cls.add_method('Add', 
                   'void', 
                   [param('std::string', 'deviceName')])
    ## net-device-container.h: __gnu_cxx::__normal_iterator<const ns3::Ptr<ns3::NetDevice>*,std::vector<ns3::Ptr<ns3::NetDevice>, std::allocator<ns3::Ptr<ns3::NetDevice> > > > ns3::NetDeviceContainer::Begin() const [member function]
    cls.add_method('Begin', 
                   '__gnu_cxx::__normal_iterator< ns3::Ptr< ns3::NetDevice > const, std::vector< ns3::Ptr< ns3::NetDevice > > >', 
                   [], 
                   is_const=True)
    ## net-device-container.h: __gnu_cxx::__normal_iterator<const ns3::Ptr<ns3::NetDevice>*,std::vector<ns3::Ptr<ns3::NetDevice>, std::allocator<ns3::Ptr<ns3::NetDevice> > > > ns3::NetDeviceContainer::End() const [member function]
    cls.add_method('End', 
                   '__gnu_cxx::__normal_iterator< ns3::Ptr< ns3::NetDevice > const, std::vector< ns3::Ptr< ns3::NetDevice > > >', 
                   [], 
                   is_const=True)
    ## net-device-container.h: ns3::Ptr<ns3::NetDevice> ns3::NetDeviceContainer::Get(uint32_t i) const [member function]
    cls.add_method('Get', 
                   'ns3::Ptr< ns3::NetDevice >', 
                   [param('uint32_t', 'i')], 
                   is_const=True)
    ## net-device-container.h: uint32_t ns3::NetDeviceContainer::GetN() const [member function]
    cls.add_method('GetN', 
                   'uint32_t', 
                   [], 
                   is_const=True)
    return

def register_Ns3NodeContainer_methods(root_module, cls):
    ## node-container.h: ns3::NodeContainer::NodeContainer(ns3::NodeContainer const & arg0) [copy constructor]
    cls.add_constructor([param('ns3::NodeContainer const &', 'arg0')])
    ## node-container.h: ns3::NodeContainer::NodeContainer() [constructor]
    cls.add_constructor([])
    ## node-container.h: ns3::NodeContainer::NodeContainer(ns3::Ptr<ns3::Node> node) [constructor]
    cls.add_constructor([param('ns3::Ptr< ns3::Node >', 'node')])
    ## node-container.h: ns3::NodeContainer::NodeContainer(std::string nodeName) [constructor]
    cls.add_constructor([param('std::string', 'nodeName')])
    ## node-container.h: ns3::NodeContainer::NodeContainer(ns3::NodeContainer const & a, ns3::NodeContainer const & b) [constructor]
    cls.add_constructor([param('ns3::NodeContainer const &', 'a'), param('ns3::NodeContainer const &', 'b')])
    ## node-container.h: ns3::NodeContainer::NodeContainer(ns3::NodeContainer const & a, ns3::NodeContainer const & b, ns3::NodeContainer const & c) [constructor]
    cls.add_constructor([param('ns3::NodeContainer const &', 'a'), param('ns3::NodeContainer const &', 'b'), param('ns3::NodeContainer const &', 'c')])
    ## node-container.h: ns3::NodeContainer::NodeContainer(ns3::NodeContainer const & a, ns3::NodeContainer const & b, ns3::NodeContainer const & c, ns3::NodeContainer const & d) [constructor]
    cls.add_constructor([param('ns3::NodeContainer const &', 'a'), param('ns3::NodeContainer const &', 'b'), param('ns3::NodeContainer const &', 'c'), param('ns3::NodeContainer const &', 'd')])
    ## node-container.h: ns3::NodeContainer::NodeContainer(ns3::NodeContainer const & a, ns3::NodeContainer const & b, ns3::NodeContainer const & c, ns3::NodeContainer const & d, ns3::NodeContainer const & e) [constructor]
    cls.add_constructor([param('ns3::NodeContainer const &', 'a'), param('ns3::NodeContainer const &', 'b'), param('ns3::NodeContainer const &', 'c'), param('ns3::NodeContainer const &', 'd'), param('ns3::NodeContainer const &', 'e')])
    ## node-container.h: void ns3::NodeContainer::Add(ns3::NodeContainer other) [member function]
    cls.add_method('Add', 
                   'void', 
                   [param('ns3::NodeContainer', 'other')])
    ## node-container.h: void ns3::NodeContainer::Add(ns3::Ptr<ns3::Node> node) [member function]
    cls.add_method('Add', 
                   'void', 
                   [param('ns3::Ptr< ns3::Node >', 'node')])
    ## node-container.h: void ns3::NodeContainer::Add(std::string nodeName) [member function]
    cls.add_method('Add', 
                   'void', 
                   [param('std::string', 'nodeName')])
    ## node-container.h: __gnu_cxx::__normal_iterator<const ns3::Ptr<ns3::Node>*,std::vector<ns3::Ptr<ns3::Node>, std::allocator<ns3::Ptr<ns3::Node> > > > ns3::NodeContainer::Begin() const [member function]
    cls.add_method('Begin', 
                   '__gnu_cxx::__normal_iterator< ns3::Ptr< ns3::Node > const, std::vector< ns3::Ptr< ns3::Node > > >', 
                   [], 
                   is_const=True)
    ## node-container.h: void ns3::NodeContainer::Create(uint32_t n) [member function]
    cls.add_method('Create', 
                   'void', 
                   [param('uint32_t', 'n')])
    ## node-container.h: __gnu_cxx::__normal_iterator<const ns3::Ptr<ns3::Node>*,std::vector<ns3::Ptr<ns3::Node>, std::allocator<ns3::Ptr<ns3::Node> > > > ns3::NodeContainer::End() const [member function]
    cls.add_method('End', 
                   '__gnu_cxx::__normal_iterator< ns3::Ptr< ns3::Node > const, std::vector< ns3::Ptr< ns3::Node > > >', 
                   [], 
                   is_const=True)
    ## node-container.h: ns3::Ptr<ns3::Node> ns3::NodeContainer::Get(uint32_t i) const [member function]
    cls.add_method('Get', 
                   'ns3::Ptr< ns3::Node >', 
                   [param('uint32_t', 'i')], 
                   is_const=True)
    ## node-container.h: static ns3::NodeContainer ns3::NodeContainer::GetGlobal() [member function]
    cls.add_method('GetGlobal', 
                   'ns3::NodeContainer', 
                   [], 
                   is_static=True)
    ## node-container.h: uint32_t ns3::NodeContainer::GetN() const [member function]
    cls.add_method('GetN', 
                   'uint32_t', 
                   [], 
                   is_const=True)
    return

def register_Ns3Ns2MobilityHelper_methods(root_module, cls):
    ## ns2-mobility-helper.h: ns3::Ns2MobilityHelper::Ns2MobilityHelper(ns3::Ns2MobilityHelper const & arg0) [copy constructor]
    cls.add_constructor([param('ns3::Ns2MobilityHelper const &', 'arg0')])
    ## ns2-mobility-helper.h: ns3::Ns2MobilityHelper::Ns2MobilityHelper(std::string filename) [constructor]
    cls.add_constructor([param('std::string', 'filename')])
    ## ns2-mobility-helper.h: void ns3::Ns2MobilityHelper::Install() const [member function]
    cls.add_method('Install', 
                   'void', 
                   [], 
                   is_const=True)
    return

def register_Ns3OlsrHelper_methods(root_module, cls):
    ## olsr-helper.h: ns3::OlsrHelper::OlsrHelper() [constructor]
    cls.add_constructor([])
    ## olsr-helper.h: ns3::OlsrHelper::OlsrHelper(ns3::OlsrHelper const & arg0) [copy constructor]
    cls.add_constructor([param('ns3::OlsrHelper const &', 'arg0')])
    ## olsr-helper.h: ns3::OlsrHelper * ns3::OlsrHelper::Copy() const [member function]
    cls.add_method('Copy', 
                   'ns3::OlsrHelper *', 
                   [], 
                   is_const=True, is_virtual=True)
    ## olsr-helper.h: ns3::Ptr<ns3::Ipv4RoutingProtocol> ns3::OlsrHelper::Create(ns3::Ptr<ns3::Node> node) const [member function]
    cls.add_method('Create', 
                   'ns3::Ptr< ns3::Ipv4RoutingProtocol >', 
                   [param('ns3::Ptr< ns3::Node >', 'node')], 
                   is_const=True, is_virtual=True)
    ## olsr-helper.h: void ns3::OlsrHelper::Set(std::string name, ns3::AttributeValue const & value) [member function]
    cls.add_method('Set', 
                   'void', 
                   [param('std::string', 'name'), param('ns3::AttributeValue const &', 'value')])
    return

def register_Ns3OnOffHelper_methods(root_module, cls):
    ## on-off-helper.h: ns3::OnOffHelper::OnOffHelper(ns3::OnOffHelper const & arg0) [copy constructor]
    cls.add_constructor([param('ns3::OnOffHelper const &', 'arg0')])
    ## on-off-helper.h: ns3::OnOffHelper::OnOffHelper(std::string protocol, ns3::Address address) [constructor]
    cls.add_constructor([param('std::string', 'protocol'), param('ns3::Address', 'address')])
    ## on-off-helper.h: ns3::ApplicationContainer ns3::OnOffHelper::Install(ns3::NodeContainer c) const [member function]
    cls.add_method('Install', 
                   'ns3::ApplicationContainer', 
                   [param('ns3::NodeContainer', 'c')], 
                   is_const=True)
    ## on-off-helper.h: ns3::ApplicationContainer ns3::OnOffHelper::Install(ns3::Ptr<ns3::Node> node) const [member function]
    cls.add_method('Install', 
                   'ns3::ApplicationContainer', 
                   [param('ns3::Ptr< ns3::Node >', 'node')], 
                   is_const=True)
    ## on-off-helper.h: ns3::ApplicationContainer ns3::OnOffHelper::Install(std::string nodeName) const [member function]
    cls.add_method('Install', 
                   'ns3::ApplicationContainer', 
                   [param('std::string', 'nodeName')], 
                   is_const=True)
    ## on-off-helper.h: void ns3::OnOffHelper::SetAttribute(std::string name, ns3::AttributeValue const & value) [member function]
    cls.add_method('SetAttribute', 
                   'void', 
                   [param('std::string', 'name'), param('ns3::AttributeValue const &', 'value')])
    return

def register_Ns3PacketSinkHelper_methods(root_module, cls):
    ## packet-sink-helper.h: ns3::PacketSinkHelper::PacketSinkHelper(ns3::PacketSinkHelper const & arg0) [copy constructor]
    cls.add_constructor([param('ns3::PacketSinkHelper const &', 'arg0')])
    ## packet-sink-helper.h: ns3::PacketSinkHelper::PacketSinkHelper(std::string protocol, ns3::Address address) [constructor]
    cls.add_constructor([param('std::string', 'protocol'), param('ns3::Address', 'address')])
    ## packet-sink-helper.h: ns3::ApplicationContainer ns3::PacketSinkHelper::Install(ns3::NodeContainer c) const [member function]
    cls.add_method('Install', 
                   'ns3::ApplicationContainer', 
                   [param('ns3::NodeContainer', 'c')], 
                   is_const=True)
    ## packet-sink-helper.h: ns3::ApplicationContainer ns3::PacketSinkHelper::Install(ns3::Ptr<ns3::Node> node) const [member function]
    cls.add_method('Install', 
                   'ns3::ApplicationContainer', 
                   [param('ns3::Ptr< ns3::Node >', 'node')], 
                   is_const=True)
    ## packet-sink-helper.h: ns3::ApplicationContainer ns3::PacketSinkHelper::Install(std::string nodeName) const [member function]
    cls.add_method('Install', 
                   'ns3::ApplicationContainer', 
                   [param('std::string', 'nodeName')], 
                   is_const=True)
    ## packet-sink-helper.h: void ns3::PacketSinkHelper::SetAttribute(std::string name, ns3::AttributeValue const & value) [member function]
    cls.add_method('SetAttribute', 
                   'void', 
                   [param('std::string', 'name'), param('ns3::AttributeValue const &', 'value')])
    return

def register_Ns3PacketSocketHelper_methods(root_module, cls):
    ## packet-socket-helper.h: ns3::PacketSocketHelper::PacketSocketHelper() [constructor]
    cls.add_constructor([])
    ## packet-socket-helper.h: ns3::PacketSocketHelper::PacketSocketHelper(ns3::PacketSocketHelper const & arg0) [copy constructor]
    cls.add_constructor([param('ns3::PacketSocketHelper const &', 'arg0')])
    ## packet-socket-helper.h: void ns3::PacketSocketHelper::Install(ns3::Ptr<ns3::Node> node) const [member function]
    cls.add_method('Install', 
                   'void', 
                   [param('ns3::Ptr< ns3::Node >', 'node')], 
                   is_const=True)
    ## packet-socket-helper.h: void ns3::PacketSocketHelper::Install(std::string nodeName) const [member function]
    cls.add_method('Install', 
                   'void', 
                   [param('std::string', 'nodeName')], 
                   is_const=True)
    ## packet-socket-helper.h: void ns3::PacketSocketHelper::Install(ns3::NodeContainer c) const [member function]
    cls.add_method('Install', 
                   'void', 
                   [param('ns3::NodeContainer', 'c')], 
                   is_const=True)
    return

def register_Ns3Ping6Helper_methods(root_module, cls):
    ## ping6-helper.h: ns3::Ping6Helper::Ping6Helper(ns3::Ping6Helper const & arg0) [copy constructor]
    cls.add_constructor([param('ns3::Ping6Helper const &', 'arg0')])
    ## ping6-helper.h: ns3::Ping6Helper::Ping6Helper() [constructor]
    cls.add_constructor([])
    ## ping6-helper.h: ns3::ApplicationContainer ns3::Ping6Helper::Install(ns3::NodeContainer c) [member function]
    cls.add_method('Install', 
                   'ns3::ApplicationContainer', 
                   [param('ns3::NodeContainer', 'c')])
    ## ping6-helper.h: void ns3::Ping6Helper::SetAttribute(std::string name, ns3::AttributeValue const & value) [member function]
    cls.add_method('SetAttribute', 
                   'void', 
                   [param('std::string', 'name'), param('ns3::AttributeValue const &', 'value')])
    ## ping6-helper.h: void ns3::Ping6Helper::SetIfIndex(uint32_t ifIndex) [member function]
    cls.add_method('SetIfIndex', 
                   'void', 
                   [param('uint32_t', 'ifIndex')])
    ## ping6-helper.h: void ns3::Ping6Helper::SetLocal(ns3::Ipv6Address ip) [member function]
    cls.add_method('SetLocal', 
                   'void', 
                   [param('ns3::Ipv6Address', 'ip')])
    ## ping6-helper.h: void ns3::Ping6Helper::SetRemote(ns3::Ipv6Address ip) [member function]
    cls.add_method('SetRemote', 
                   'void', 
                   [param('ns3::Ipv6Address', 'ip')])
    ## ping6-helper.h: void ns3::Ping6Helper::SetRoutersAddress(std::vector<ns3::Ipv6Address, std::allocator<ns3::Ipv6Address> > routers) [member function]
    cls.add_method('SetRoutersAddress', 
                   'void', 
                   [param('std::vector< ns3::Ipv6Address >', 'routers')])
    return

def register_Ns3PointToPointDumbbellHelper_methods(root_module, cls):
    ## point-to-point-dumbbell-helper.h: ns3::PointToPointDumbbellHelper::PointToPointDumbbellHelper(ns3::PointToPointDumbbellHelper const & arg0) [copy constructor]
    cls.add_constructor([param('ns3::PointToPointDumbbellHelper const &', 'arg0')])
    ## point-to-point-dumbbell-helper.h: ns3::PointToPointDumbbellHelper::PointToPointDumbbellHelper(uint32_t nLeftLeaf, ns3::PointToPointHelper leftHelper, uint32_t nRightLeaf, ns3::PointToPointHelper rightHelper, ns3::PointToPointHelper bottleneckHelper) [constructor]
    cls.add_constructor([param('uint32_t', 'nLeftLeaf'), param('ns3::PointToPointHelper', 'leftHelper'), param('uint32_t', 'nRightLeaf'), param('ns3::PointToPointHelper', 'rightHelper'), param('ns3::PointToPointHelper', 'bottleneckHelper')])
    ## point-to-point-dumbbell-helper.h: void ns3::PointToPointDumbbellHelper::AssignIpv4Addresses(ns3::Ipv4AddressHelper leftIp, ns3::Ipv4AddressHelper rightIp, ns3::Ipv4AddressHelper routerIp) [member function]
    cls.add_method('AssignIpv4Addresses', 
                   'void', 
                   [param('ns3::Ipv4AddressHelper', 'leftIp'), param('ns3::Ipv4AddressHelper', 'rightIp'), param('ns3::Ipv4AddressHelper', 'routerIp')])
    ## point-to-point-dumbbell-helper.h: void ns3::PointToPointDumbbellHelper::BoundingBox(double arg0, double arg1, double arg2, double arg3) [member function]
    cls.add_method('BoundingBox', 
                   'void', 
                   [param('double', 'arg0'), param('double', 'arg1'), param('double', 'arg2'), param('double', 'arg3')])
    ## point-to-point-dumbbell-helper.h: ns3::Ptr<ns3::Node> ns3::PointToPointDumbbellHelper::GetLeft() const [member function]
    cls.add_method('GetLeft', 
                   'ns3::Ptr< ns3::Node >', 
                   [], 
                   is_const=True)
    ## point-to-point-dumbbell-helper.h: ns3::Ptr<ns3::Node> ns3::PointToPointDumbbellHelper::GetLeft(uint32_t i) const [member function]
    cls.add_method('GetLeft', 
                   'ns3::Ptr< ns3::Node >', 
                   [param('uint32_t', 'i')], 
                   is_const=True)
    ## point-to-point-dumbbell-helper.h: ns3::Ipv4Address ns3::PointToPointDumbbellHelper::GetLeftIpv4Address(uint32_t i) const [member function]
    cls.add_method('GetLeftIpv4Address', 
                   'ns3::Ipv4Address', 
                   [param('uint32_t', 'i')], 
                   is_const=True)
    ## point-to-point-dumbbell-helper.h: ns3::Ptr<ns3::Node> ns3::PointToPointDumbbellHelper::GetRight() const [member function]
    cls.add_method('GetRight', 
                   'ns3::Ptr< ns3::Node >', 
                   [], 
                   is_const=True)
    ## point-to-point-dumbbell-helper.h: ns3::Ptr<ns3::Node> ns3::PointToPointDumbbellHelper::GetRight(uint32_t i) const [member function]
    cls.add_method('GetRight', 
                   'ns3::Ptr< ns3::Node >', 
                   [param('uint32_t', 'i')], 
                   is_const=True)
    ## point-to-point-dumbbell-helper.h: ns3::Ipv4Address ns3::PointToPointDumbbellHelper::GetRightIpv4Address(uint32_t i) const [member function]
    cls.add_method('GetRightIpv4Address', 
                   'ns3::Ipv4Address', 
                   [param('uint32_t', 'i')], 
                   is_const=True)
    ## point-to-point-dumbbell-helper.h: void ns3::PointToPointDumbbellHelper::InstallStack(ns3::InternetStackHelper stack) [member function]
    cls.add_method('InstallStack', 
                   'void', 
                   [param('ns3::InternetStackHelper', 'stack')])
    ## point-to-point-dumbbell-helper.h: uint32_t ns3::PointToPointDumbbellHelper::LeftCount() const [member function]
    cls.add_method('LeftCount', 
                   'uint32_t', 
                   [], 
                   is_const=True)
    ## point-to-point-dumbbell-helper.h: uint32_t ns3::PointToPointDumbbellHelper::RightCount() const [member function]
    cls.add_method('RightCount', 
                   'uint32_t', 
                   [], 
                   is_const=True)
    return

def register_Ns3PointToPointGridHelper_methods(root_module, cls):
    ## point-to-point-grid-helper.h: ns3::PointToPointGridHelper::PointToPointGridHelper(ns3::PointToPointGridHelper const & arg0) [copy constructor]
    cls.add_constructor([param('ns3::PointToPointGridHelper const &', 'arg0')])
    ## point-to-point-grid-helper.h: ns3::PointToPointGridHelper::PointToPointGridHelper(uint32_t nRows, uint32_t nCols, ns3::PointToPointHelper pointToPoint) [constructor]
    cls.add_constructor([param('uint32_t', 'nRows'), param('uint32_t', 'nCols'), param('ns3::PointToPointHelper', 'pointToPoint')])
    ## point-to-point-grid-helper.h: void ns3::PointToPointGridHelper::AssignIpv4Addresses(ns3::Ipv4AddressHelper rowIp, ns3::Ipv4AddressHelper colIp) [member function]
    cls.add_method('AssignIpv4Addresses', 
                   'void', 
                   [param('ns3::Ipv4AddressHelper', 'rowIp'), param('ns3::Ipv4AddressHelper', 'colIp')])
    ## point-to-point-grid-helper.h: void ns3::PointToPointGridHelper::BoundingBox(double ulx, double uly, double lrx, double lry) [member function]
    cls.add_method('BoundingBox', 
                   'void', 
                   [param('double', 'ulx'), param('double', 'uly'), param('double', 'lrx'), param('double', 'lry')])
    ## point-to-point-grid-helper.h: ns3::Ipv4Address ns3::PointToPointGridHelper::GetIpv4Address(uint32_t row, uint32_t col) [member function]
    cls.add_method('GetIpv4Address', 
                   'ns3::Ipv4Address', 
                   [param('uint32_t', 'row'), param('uint32_t', 'col')])
    ## point-to-point-grid-helper.h: ns3::Ptr<ns3::Node> ns3::PointToPointGridHelper::GetNode(uint32_t row, uint32_t col) [member function]
    cls.add_method('GetNode', 
                   'ns3::Ptr< ns3::Node >', 
                   [param('uint32_t', 'row'), param('uint32_t', 'col')])
    ## point-to-point-grid-helper.h: void ns3::PointToPointGridHelper::InstallStack(ns3::InternetStackHelper stack) [member function]
    cls.add_method('InstallStack', 
                   'void', 
                   [param('ns3::InternetStackHelper', 'stack')])
    return

def register_Ns3PointToPointHelper_methods(root_module, cls):
    ## point-to-point-helper.h: ns3::PointToPointHelper::PointToPointHelper(ns3::PointToPointHelper const & arg0) [copy constructor]
    cls.add_constructor([param('ns3::PointToPointHelper const &', 'arg0')])
    ## point-to-point-helper.h: ns3::PointToPointHelper::PointToPointHelper() [constructor]
    cls.add_constructor([])
    ## point-to-point-helper.h: static void ns3::PointToPointHelper::EnableAscii(std::ostream & os, uint32_t nodeid, uint32_t deviceid) [member function]
    cls.add_method('EnableAscii', 
                   'void', 
                   [param('std::ostream &', 'os'), param('uint32_t', 'nodeid'), param('uint32_t', 'deviceid')], 
                   is_static=True)
    ## point-to-point-helper.h: static void ns3::PointToPointHelper::EnableAscii(std::ostream & os, ns3::NetDeviceContainer d) [member function]
    cls.add_method('EnableAscii', 
                   'void', 
                   [param('std::ostream &', 'os'), param('ns3::NetDeviceContainer', 'd')], 
                   is_static=True)
    ## point-to-point-helper.h: static void ns3::PointToPointHelper::EnableAscii(std::ostream & os, ns3::NodeContainer n) [member function]
    cls.add_method('EnableAscii', 
                   'void', 
                   [param('std::ostream &', 'os'), param('ns3::NodeContainer', 'n')], 
                   is_static=True)
    ## point-to-point-helper.h: static void ns3::PointToPointHelper::EnableAsciiAll(std::ostream & os) [member function]
    cls.add_method('EnableAsciiAll', 
                   'void', 
                   [param('std::ostream &', 'os')], 
                   is_static=True)
    ## point-to-point-helper.h: static void ns3::PointToPointHelper::EnablePcap(std::string filename, uint32_t nodeid, uint32_t deviceid) [member function]
    cls.add_method('EnablePcap', 
                   'void', 
                   [param('std::string', 'filename'), param('uint32_t', 'nodeid'), param('uint32_t', 'deviceid')], 
                   is_static=True)
    ## point-to-point-helper.h: static void ns3::PointToPointHelper::EnablePcap(std::string filename, ns3::Ptr<ns3::NetDevice> nd) [member function]
    cls.add_method('EnablePcap', 
                   'void', 
                   [param('std::string', 'filename'), param('ns3::Ptr< ns3::NetDevice >', 'nd')], 
                   is_static=True)
    ## point-to-point-helper.h: static void ns3::PointToPointHelper::EnablePcap(std::string filename, std::string ndName) [member function]
    cls.add_method('EnablePcap', 
                   'void', 
                   [param('std::string', 'filename'), param('std::string', 'ndName')], 
                   is_static=True)
    ## point-to-point-helper.h: static void ns3::PointToPointHelper::EnablePcap(std::string filename, ns3::NetDeviceContainer d) [member function]
    cls.add_method('EnablePcap', 
                   'void', 
                   [param('std::string', 'filename'), param('ns3::NetDeviceContainer', 'd')], 
                   is_static=True)
    ## point-to-point-helper.h: static void ns3::PointToPointHelper::EnablePcap(std::string filename, ns3::NodeContainer n) [member function]
    cls.add_method('EnablePcap', 
                   'void', 
                   [param('std::string', 'filename'), param('ns3::NodeContainer', 'n')], 
                   is_static=True)
    ## point-to-point-helper.h: static void ns3::PointToPointHelper::EnablePcapAll(std::string filename) [member function]
    cls.add_method('EnablePcapAll', 
                   'void', 
                   [param('std::string', 'filename')], 
                   is_static=True)
    ## point-to-point-helper.h: ns3::NetDeviceContainer ns3::PointToPointHelper::Install(ns3::NodeContainer c) [member function]
    cls.add_method('Install', 
                   'ns3::NetDeviceContainer', 
                   [param('ns3::NodeContainer', 'c')])
    ## point-to-point-helper.h: ns3::NetDeviceContainer ns3::PointToPointHelper::Install(ns3::Ptr<ns3::Node> a, ns3::Ptr<ns3::Node> b) [member function]
    cls.add_method('Install', 
                   'ns3::NetDeviceContainer', 
                   [param('ns3::Ptr< ns3::Node >', 'a'), param('ns3::Ptr< ns3::Node >', 'b')])
    ## point-to-point-helper.h: ns3::NetDeviceContainer ns3::PointToPointHelper::Install(ns3::Ptr<ns3::Node> a, std::string bName) [member function]
    cls.add_method('Install', 
                   'ns3::NetDeviceContainer', 
                   [param('ns3::Ptr< ns3::Node >', 'a'), param('std::string', 'bName')])
    ## point-to-point-helper.h: ns3::NetDeviceContainer ns3::PointToPointHelper::Install(std::string aName, ns3::Ptr<ns3::Node> b) [member function]
    cls.add_method('Install', 
                   'ns3::NetDeviceContainer', 
                   [param('std::string', 'aName'), param('ns3::Ptr< ns3::Node >', 'b')])
    ## point-to-point-helper.h: ns3::NetDeviceContainer ns3::PointToPointHelper::Install(std::string aNode, std::string bNode) [member function]
    cls.add_method('Install', 
                   'ns3::NetDeviceContainer', 
                   [param('std::string', 'aNode'), param('std::string', 'bNode')])
    ## point-to-point-helper.h: void ns3::PointToPointHelper::SetChannelAttribute(std::string name, ns3::AttributeValue const & value) [member function]
    cls.add_method('SetChannelAttribute', 
                   'void', 
                   [param('std::string', 'name'), param('ns3::AttributeValue const &', 'value')])
    ## point-to-point-helper.h: void ns3::PointToPointHelper::SetDeviceAttribute(std::string name, ns3::AttributeValue const & value) [member function]
    cls.add_method('SetDeviceAttribute', 
                   'void', 
                   [param('std::string', 'name'), param('ns3::AttributeValue const &', 'value')])
    ## point-to-point-helper.h: void ns3::PointToPointHelper::SetQueue(std::string type, std::string n1="", ns3::AttributeValue const & v1=ns3::EmptyAttributeValue(), std::string n2="", ns3::AttributeValue const & v2=ns3::EmptyAttributeValue(), std::string n3="", ns3::AttributeValue const & v3=ns3::EmptyAttributeValue(), std::string n4="", ns3::AttributeValue const & v4=ns3::EmptyAttributeValue()) [member function]
    cls.add_method('SetQueue', 
                   'void', 
                   [param('std::string', 'type'), param('std::string', 'n1', default_value='""'), param('ns3::AttributeValue const &', 'v1', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n2', default_value='""'), param('ns3::AttributeValue const &', 'v2', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n3', default_value='""'), param('ns3::AttributeValue const &', 'v3', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n4', default_value='""'), param('ns3::AttributeValue const &', 'v4', default_value='ns3::EmptyAttributeValue()')])
    return

def register_Ns3PointToPointStarHelper_methods(root_module, cls):
    ## point-to-point-star-helper.h: ns3::PointToPointStarHelper::PointToPointStarHelper(ns3::PointToPointStarHelper const & arg0) [copy constructor]
    cls.add_constructor([param('ns3::PointToPointStarHelper const &', 'arg0')])
    ## point-to-point-star-helper.h: ns3::PointToPointStarHelper::PointToPointStarHelper(uint32_t numSpokes, ns3::PointToPointHelper p2pHelper) [constructor]
    cls.add_constructor([param('uint32_t', 'numSpokes'), param('ns3::PointToPointHelper', 'p2pHelper')])
    ## point-to-point-star-helper.h: void ns3::PointToPointStarHelper::AssignIpv4Addresses(ns3::Ipv4AddressHelper address) [member function]
    cls.add_method('AssignIpv4Addresses', 
                   'void', 
                   [param('ns3::Ipv4AddressHelper', 'address')])
    ## point-to-point-star-helper.h: void ns3::PointToPointStarHelper::BoundingBox(double ulx, double uly, double lrx, double lry) [member function]
    cls.add_method('BoundingBox', 
                   'void', 
                   [param('double', 'ulx'), param('double', 'uly'), param('double', 'lrx'), param('double', 'lry')])
    ## point-to-point-star-helper.h: ns3::Ptr<ns3::Node> ns3::PointToPointStarHelper::GetHub() const [member function]
    cls.add_method('GetHub', 
                   'ns3::Ptr< ns3::Node >', 
                   [], 
                   is_const=True)
    ## point-to-point-star-helper.h: ns3::Ipv4Address ns3::PointToPointStarHelper::GetHubIpv4Address(uint32_t i) const [member function]
    cls.add_method('GetHubIpv4Address', 
                   'ns3::Ipv4Address', 
                   [param('uint32_t', 'i')], 
                   is_const=True)
    ## point-to-point-star-helper.h: ns3::Ipv4Address ns3::PointToPointStarHelper::GetSpokeIpv4Address(uint32_t i) const [member function]
    cls.add_method('GetSpokeIpv4Address', 
                   'ns3::Ipv4Address', 
                   [param('uint32_t', 'i')], 
                   is_const=True)
    ## point-to-point-star-helper.h: ns3::Ptr<ns3::Node> ns3::PointToPointStarHelper::GetSpokeNode(uint32_t i) const [member function]
    cls.add_method('GetSpokeNode', 
                   'ns3::Ptr< ns3::Node >', 
                   [param('uint32_t', 'i')], 
                   is_const=True)
    ## point-to-point-star-helper.h: void ns3::PointToPointStarHelper::InstallStack(ns3::InternetStackHelper stack) [member function]
    cls.add_method('InstallStack', 
                   'void', 
                   [param('ns3::InternetStackHelper', 'stack')])
    ## point-to-point-star-helper.h: uint32_t ns3::PointToPointStarHelper::SpokeCount() const [member function]
    cls.add_method('SpokeCount', 
                   'uint32_t', 
                   [], 
                   is_const=True)
    return

def register_Ns3TapBridgeHelper_methods(root_module, cls):
    ## tap-bridge-helper.h: ns3::TapBridgeHelper::TapBridgeHelper(ns3::TapBridgeHelper const & arg0) [copy constructor]
    cls.add_constructor([param('ns3::TapBridgeHelper const &', 'arg0')])
    ## tap-bridge-helper.h: ns3::TapBridgeHelper::TapBridgeHelper() [constructor]
    cls.add_constructor([])
    ## tap-bridge-helper.h: ns3::TapBridgeHelper::TapBridgeHelper(ns3::Ipv4Address gateway) [constructor]
    cls.add_constructor([param('ns3::Ipv4Address', 'gateway')])
    ## tap-bridge-helper.h: ns3::Ptr<ns3::NetDevice> ns3::TapBridgeHelper::Install(ns3::Ptr<ns3::Node> node, ns3::Ptr<ns3::NetDevice> nd) [member function]
    cls.add_method('Install', 
                   'ns3::Ptr< ns3::NetDevice >', 
                   [param('ns3::Ptr< ns3::Node >', 'node'), param('ns3::Ptr< ns3::NetDevice >', 'nd')])
    ## tap-bridge-helper.h: ns3::Ptr<ns3::NetDevice> ns3::TapBridgeHelper::Install(std::string nodeName, ns3::Ptr<ns3::NetDevice> nd) [member function]
    cls.add_method('Install', 
                   'ns3::Ptr< ns3::NetDevice >', 
                   [param('std::string', 'nodeName'), param('ns3::Ptr< ns3::NetDevice >', 'nd')])
    ## tap-bridge-helper.h: ns3::Ptr<ns3::NetDevice> ns3::TapBridgeHelper::Install(ns3::Ptr<ns3::Node> node, std::string ndName) [member function]
    cls.add_method('Install', 
                   'ns3::Ptr< ns3::NetDevice >', 
                   [param('ns3::Ptr< ns3::Node >', 'node'), param('std::string', 'ndName')])
    ## tap-bridge-helper.h: ns3::Ptr<ns3::NetDevice> ns3::TapBridgeHelper::Install(std::string nodeName, std::string ndName) [member function]
    cls.add_method('Install', 
                   'ns3::Ptr< ns3::NetDevice >', 
                   [param('std::string', 'nodeName'), param('std::string', 'ndName')])
    ## tap-bridge-helper.h: ns3::Ptr<ns3::NetDevice> ns3::TapBridgeHelper::Install(ns3::Ptr<ns3::Node> node, ns3::Ptr<ns3::NetDevice> nd, ns3::AttributeValue const & bridgeType) [member function]
    cls.add_method('Install', 
                   'ns3::Ptr< ns3::NetDevice >', 
                   [param('ns3::Ptr< ns3::Node >', 'node'), param('ns3::Ptr< ns3::NetDevice >', 'nd'), param('ns3::AttributeValue const &', 'bridgeType')])
    ## tap-bridge-helper.h: void ns3::TapBridgeHelper::SetAttribute(std::string n1, ns3::AttributeValue const & v1) [member function]
    cls.add_method('SetAttribute', 
                   'void', 
                   [param('std::string', 'n1'), param('ns3::AttributeValue const &', 'v1')])
    return

def register_Ns3UdpClientHelper_methods(root_module, cls):
    ## udp-client-server-helper.h: ns3::UdpClientHelper::UdpClientHelper(ns3::UdpClientHelper const & arg0) [copy constructor]
    cls.add_constructor([param('ns3::UdpClientHelper const &', 'arg0')])
    ## udp-client-server-helper.h: ns3::UdpClientHelper::UdpClientHelper() [constructor]
    cls.add_constructor([])
    ## udp-client-server-helper.h: ns3::UdpClientHelper::UdpClientHelper(ns3::Ipv4Address ip, uint16_t port) [constructor]
    cls.add_constructor([param('ns3::Ipv4Address', 'ip'), param('uint16_t', 'port')])
    ## udp-client-server-helper.h: ns3::ApplicationContainer ns3::UdpClientHelper::Install(ns3::NodeContainer c) [member function]
    cls.add_method('Install', 
                   'ns3::ApplicationContainer', 
                   [param('ns3::NodeContainer', 'c')])
    ## udp-client-server-helper.h: void ns3::UdpClientHelper::SetAttribute(std::string name, ns3::AttributeValue const & value) [member function]
    cls.add_method('SetAttribute', 
                   'void', 
                   [param('std::string', 'name'), param('ns3::AttributeValue const &', 'value')])
    return

def register_Ns3UdpEchoClientHelper_methods(root_module, cls):
    ## udp-echo-helper.h: ns3::UdpEchoClientHelper::UdpEchoClientHelper(ns3::UdpEchoClientHelper const & arg0) [copy constructor]
    cls.add_constructor([param('ns3::UdpEchoClientHelper const &', 'arg0')])
    ## udp-echo-helper.h: ns3::UdpEchoClientHelper::UdpEchoClientHelper(ns3::Ipv4Address ip, uint16_t port) [constructor]
    cls.add_constructor([param('ns3::Ipv4Address', 'ip'), param('uint16_t', 'port')])
    ## udp-echo-helper.h: ns3::ApplicationContainer ns3::UdpEchoClientHelper::Install(ns3::Ptr<ns3::Node> node) const [member function]
    cls.add_method('Install', 
                   'ns3::ApplicationContainer', 
                   [param('ns3::Ptr< ns3::Node >', 'node')], 
                   is_const=True)
    ## udp-echo-helper.h: ns3::ApplicationContainer ns3::UdpEchoClientHelper::Install(std::string nodeName) const [member function]
    cls.add_method('Install', 
                   'ns3::ApplicationContainer', 
                   [param('std::string', 'nodeName')], 
                   is_const=True)
    ## udp-echo-helper.h: ns3::ApplicationContainer ns3::UdpEchoClientHelper::Install(ns3::NodeContainer c) const [member function]
    cls.add_method('Install', 
                   'ns3::ApplicationContainer', 
                   [param('ns3::NodeContainer', 'c')], 
                   is_const=True)
    ## udp-echo-helper.h: void ns3::UdpEchoClientHelper::SetAttribute(std::string name, ns3::AttributeValue const & value) [member function]
    cls.add_method('SetAttribute', 
                   'void', 
                   [param('std::string', 'name'), param('ns3::AttributeValue const &', 'value')])
    ## udp-echo-helper.h: void ns3::UdpEchoClientHelper::SetFill(ns3::Ptr<ns3::Application> app, std::string fill) [member function]
    cls.add_method('SetFill', 
                   'void', 
                   [param('ns3::Ptr< ns3::Application >', 'app'), param('std::string', 'fill')])
    ## udp-echo-helper.h: void ns3::UdpEchoClientHelper::SetFill(ns3::Ptr<ns3::Application> app, uint8_t fill, uint32_t dataLength) [member function]
    cls.add_method('SetFill', 
                   'void', 
                   [param('ns3::Ptr< ns3::Application >', 'app'), param('uint8_t', 'fill'), param('uint32_t', 'dataLength')])
    ## udp-echo-helper.h: void ns3::UdpEchoClientHelper::SetFill(ns3::Ptr<ns3::Application> app, uint8_t * fill, uint32_t fillLength, uint32_t dataLength) [member function]
    cls.add_method('SetFill', 
                   'void', 
                   [param('ns3::Ptr< ns3::Application >', 'app'), param('uint8_t *', 'fill'), param('uint32_t', 'fillLength'), param('uint32_t', 'dataLength')])
    return

def register_Ns3UdpEchoServerHelper_methods(root_module, cls):
    ## udp-echo-helper.h: ns3::UdpEchoServerHelper::UdpEchoServerHelper(ns3::UdpEchoServerHelper const & arg0) [copy constructor]
    cls.add_constructor([param('ns3::UdpEchoServerHelper const &', 'arg0')])
    ## udp-echo-helper.h: ns3::UdpEchoServerHelper::UdpEchoServerHelper(uint16_t port) [constructor]
    cls.add_constructor([param('uint16_t', 'port')])
    ## udp-echo-helper.h: ns3::ApplicationContainer ns3::UdpEchoServerHelper::Install(ns3::Ptr<ns3::Node> node) const [member function]
    cls.add_method('Install', 
                   'ns3::ApplicationContainer', 
                   [param('ns3::Ptr< ns3::Node >', 'node')], 
                   is_const=True)
    ## udp-echo-helper.h: ns3::ApplicationContainer ns3::UdpEchoServerHelper::Install(std::string nodeName) const [member function]
    cls.add_method('Install', 
                   'ns3::ApplicationContainer', 
                   [param('std::string', 'nodeName')], 
                   is_const=True)
    ## udp-echo-helper.h: ns3::ApplicationContainer ns3::UdpEchoServerHelper::Install(ns3::NodeContainer c) const [member function]
    cls.add_method('Install', 
                   'ns3::ApplicationContainer', 
                   [param('ns3::NodeContainer', 'c')], 
                   is_const=True)
    ## udp-echo-helper.h: void ns3::UdpEchoServerHelper::SetAttribute(std::string name, ns3::AttributeValue const & value) [member function]
    cls.add_method('SetAttribute', 
                   'void', 
                   [param('std::string', 'name'), param('ns3::AttributeValue const &', 'value')])
    return

def register_Ns3UdpServerHelper_methods(root_module, cls):
    ## udp-client-server-helper.h: ns3::UdpServerHelper::UdpServerHelper(ns3::UdpServerHelper const & arg0) [copy constructor]
    cls.add_constructor([param('ns3::UdpServerHelper const &', 'arg0')])
    ## udp-client-server-helper.h: ns3::UdpServerHelper::UdpServerHelper() [constructor]
    cls.add_constructor([])
    ## udp-client-server-helper.h: ns3::UdpServerHelper::UdpServerHelper(uint16_t port) [constructor]
    cls.add_constructor([param('uint16_t', 'port')])
    ## udp-client-server-helper.h: ns3::Ptr<ns3::UdpServer> ns3::UdpServerHelper::GetServer() [member function]
    cls.add_method('GetServer', 
                   'ns3::Ptr< ns3::UdpServer >', 
                   [])
    ## udp-client-server-helper.h: ns3::ApplicationContainer ns3::UdpServerHelper::Install(ns3::NodeContainer c) [member function]
    cls.add_method('Install', 
                   'ns3::ApplicationContainer', 
                   [param('ns3::NodeContainer', 'c')])
    ## udp-client-server-helper.h: void ns3::UdpServerHelper::SetAttribute(std::string name, ns3::AttributeValue const & value) [member function]
    cls.add_method('SetAttribute', 
                   'void', 
                   [param('std::string', 'name'), param('ns3::AttributeValue const &', 'value')])
    return

def register_Ns3UdpTraceClientHelper_methods(root_module, cls):
    ## udp-client-server-helper.h: ns3::UdpTraceClientHelper::UdpTraceClientHelper(ns3::UdpTraceClientHelper const & arg0) [copy constructor]
    cls.add_constructor([param('ns3::UdpTraceClientHelper const &', 'arg0')])
    ## udp-client-server-helper.h: ns3::UdpTraceClientHelper::UdpTraceClientHelper() [constructor]
    cls.add_constructor([])
    ## udp-client-server-helper.h: ns3::UdpTraceClientHelper::UdpTraceClientHelper(ns3::Ipv4Address ip, uint16_t port, std::string filename) [constructor]
    cls.add_constructor([param('ns3::Ipv4Address', 'ip'), param('uint16_t', 'port'), param('std::string', 'filename')])
    ## udp-client-server-helper.h: ns3::ApplicationContainer ns3::UdpTraceClientHelper::Install(ns3::NodeContainer c) [member function]
    cls.add_method('Install', 
                   'ns3::ApplicationContainer', 
                   [param('ns3::NodeContainer', 'c')])
    ## udp-client-server-helper.h: void ns3::UdpTraceClientHelper::SetAttribute(std::string name, ns3::AttributeValue const & value) [member function]
    cls.add_method('SetAttribute', 
                   'void', 
                   [param('std::string', 'name'), param('ns3::AttributeValue const &', 'value')])
    return

def register_Ns3V4PingHelper_methods(root_module, cls):
    ## v4ping-helper.h: ns3::V4PingHelper::V4PingHelper(ns3::V4PingHelper const & arg0) [copy constructor]
    cls.add_constructor([param('ns3::V4PingHelper const &', 'arg0')])
    ## v4ping-helper.h: ns3::V4PingHelper::V4PingHelper(ns3::Ipv4Address remote) [constructor]
    cls.add_constructor([param('ns3::Ipv4Address', 'remote')])
    ## v4ping-helper.h: ns3::ApplicationContainer ns3::V4PingHelper::Install(ns3::NodeContainer nodes) const [member function]
    cls.add_method('Install', 
                   'ns3::ApplicationContainer', 
                   [param('ns3::NodeContainer', 'nodes')], 
                   is_const=True)
    ## v4ping-helper.h: ns3::ApplicationContainer ns3::V4PingHelper::Install(ns3::Ptr<ns3::Node> node) const [member function]
    cls.add_method('Install', 
                   'ns3::ApplicationContainer', 
                   [param('ns3::Ptr< ns3::Node >', 'node')], 
                   is_const=True)
    ## v4ping-helper.h: ns3::ApplicationContainer ns3::V4PingHelper::Install(std::string nodeName) const [member function]
    cls.add_method('Install', 
                   'ns3::ApplicationContainer', 
                   [param('std::string', 'nodeName')], 
                   is_const=True)
    ## v4ping-helper.h: void ns3::V4PingHelper::SetAttribute(std::string name, ns3::AttributeValue const & value) [member function]
    cls.add_method('SetAttribute', 
                   'void', 
                   [param('std::string', 'name'), param('ns3::AttributeValue const &', 'value')])
    return

def register_Ns3WifiHelper_methods(root_module, cls):
    ## wifi-helper.h: ns3::WifiHelper::WifiHelper(ns3::WifiHelper const & arg0) [copy constructor]
    cls.add_constructor([param('ns3::WifiHelper const &', 'arg0')])
    ## wifi-helper.h: ns3::WifiHelper::WifiHelper() [constructor]
    cls.add_constructor([])
    ## wifi-helper.h: static ns3::WifiHelper ns3::WifiHelper::Default() [member function]
    cls.add_method('Default', 
                   'ns3::WifiHelper', 
                   [], 
                   is_static=True)
    ## wifi-helper.h: static void ns3::WifiHelper::EnableLogComponents() [member function]
    cls.add_method('EnableLogComponents', 
                   'void', 
                   [], 
                   is_static=True)
    ## wifi-helper.h: ns3::NetDeviceContainer ns3::WifiHelper::Install(ns3::WifiPhyHelper const & phy, ns3::WifiMacHelper const & mac, ns3::NodeContainer c) const [member function]
    cls.add_method('Install', 
                   'ns3::NetDeviceContainer', 
                   [param('ns3::WifiPhyHelper const &', 'phy'), param('ns3::WifiMacHelper const &', 'mac'), param('ns3::NodeContainer', 'c')], 
                   is_const=True)
    ## wifi-helper.h: ns3::NetDeviceContainer ns3::WifiHelper::Install(ns3::WifiPhyHelper const & phy, ns3::WifiMacHelper const & mac, ns3::Ptr<ns3::Node> node) const [member function]
    cls.add_method('Install', 
                   'ns3::NetDeviceContainer', 
                   [param('ns3::WifiPhyHelper const &', 'phy'), param('ns3::WifiMacHelper const &', 'mac'), param('ns3::Ptr< ns3::Node >', 'node')], 
                   is_const=True)
    ## wifi-helper.h: ns3::NetDeviceContainer ns3::WifiHelper::Install(ns3::WifiPhyHelper const & phy, ns3::WifiMacHelper const & mac, std::string nodeName) const [member function]
    cls.add_method('Install', 
                   'ns3::NetDeviceContainer', 
                   [param('ns3::WifiPhyHelper const &', 'phy'), param('ns3::WifiMacHelper const &', 'mac'), param('std::string', 'nodeName')], 
                   is_const=True)
    ## wifi-helper.h: void ns3::WifiHelper::SetRemoteStationManager(std::string type, std::string n0="", ns3::AttributeValue const & v0=ns3::EmptyAttributeValue(), std::string n1="", ns3::AttributeValue const & v1=ns3::EmptyAttributeValue(), std::string n2="", ns3::AttributeValue const & v2=ns3::EmptyAttributeValue(), std::string n3="", ns3::AttributeValue const & v3=ns3::EmptyAttributeValue(), std::string n4="", ns3::AttributeValue const & v4=ns3::EmptyAttributeValue(), std::string n5="", ns3::AttributeValue const & v5=ns3::EmptyAttributeValue(), std::string n6="", ns3::AttributeValue const & v6=ns3::EmptyAttributeValue(), std::string n7="", ns3::AttributeValue const & v7=ns3::EmptyAttributeValue()) [member function]
    cls.add_method('SetRemoteStationManager', 
                   'void', 
                   [param('std::string', 'type'), param('std::string', 'n0', default_value='""'), param('ns3::AttributeValue const &', 'v0', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n1', default_value='""'), param('ns3::AttributeValue const &', 'v1', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n2', default_value='""'), param('ns3::AttributeValue const &', 'v2', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n3', default_value='""'), param('ns3::AttributeValue const &', 'v3', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n4', default_value='""'), param('ns3::AttributeValue const &', 'v4', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n5', default_value='""'), param('ns3::AttributeValue const &', 'v5', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n6', default_value='""'), param('ns3::AttributeValue const &', 'v6', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n7', default_value='""'), param('ns3::AttributeValue const &', 'v7', default_value='ns3::EmptyAttributeValue()')])
    ## wifi-helper.h: void ns3::WifiHelper::SetStandard(ns3::WifiPhyStandard standard) [member function]
    cls.add_method('SetStandard', 
                   'void', 
                   [param('ns3::WifiPhyStandard', 'standard')])
    return

def register_Ns3WifiMacHelper_methods(root_module, cls):
    ## wifi-helper.h: ns3::WifiMacHelper::WifiMacHelper() [constructor]
    cls.add_constructor([])
    ## wifi-helper.h: ns3::WifiMacHelper::WifiMacHelper(ns3::WifiMacHelper const & arg0) [copy constructor]
    cls.add_constructor([param('ns3::WifiMacHelper const &', 'arg0')])
    ## wifi-helper.h: ns3::Ptr<ns3::WifiMac> ns3::WifiMacHelper::Create() const [member function]
    cls.add_method('Create', 
                   'ns3::Ptr< ns3::WifiMac >', 
                   [], 
                   is_pure_virtual=True, is_const=True, is_virtual=True)
    return

def register_Ns3WifiPhyHelper_methods(root_module, cls):
    ## wifi-helper.h: ns3::WifiPhyHelper::WifiPhyHelper() [constructor]
    cls.add_constructor([])
    ## wifi-helper.h: ns3::WifiPhyHelper::WifiPhyHelper(ns3::WifiPhyHelper const & arg0) [copy constructor]
    cls.add_constructor([param('ns3::WifiPhyHelper const &', 'arg0')])
    ## wifi-helper.h: ns3::Ptr<ns3::WifiPhy> ns3::WifiPhyHelper::Create(ns3::Ptr<ns3::Node> node, ns3::Ptr<ns3::WifiNetDevice> device) const [member function]
    cls.add_method('Create', 
                   'ns3::Ptr< ns3::WifiPhy >', 
                   [param('ns3::Ptr< ns3::Node >', 'node'), param('ns3::Ptr< ns3::WifiNetDevice >', 'device')], 
                   is_pure_virtual=True, is_const=True, is_virtual=True)
    return

def register_Ns3YansWifiChannelHelper_methods(root_module, cls):
    ## yans-wifi-helper.h: ns3::YansWifiChannelHelper::YansWifiChannelHelper(ns3::YansWifiChannelHelper const & arg0) [copy constructor]
    cls.add_constructor([param('ns3::YansWifiChannelHelper const &', 'arg0')])
    ## yans-wifi-helper.h: ns3::YansWifiChannelHelper::YansWifiChannelHelper() [constructor]
    cls.add_constructor([])
    ## yans-wifi-helper.h: void ns3::YansWifiChannelHelper::AddPropagationLoss(std::string name, std::string n0="", ns3::AttributeValue const & v0=ns3::EmptyAttributeValue(), std::string n1="", ns3::AttributeValue const & v1=ns3::EmptyAttributeValue(), std::string n2="", ns3::AttributeValue const & v2=ns3::EmptyAttributeValue(), std::string n3="", ns3::AttributeValue const & v3=ns3::EmptyAttributeValue(), std::string n4="", ns3::AttributeValue const & v4=ns3::EmptyAttributeValue(), std::string n5="", ns3::AttributeValue const & v5=ns3::EmptyAttributeValue(), std::string n6="", ns3::AttributeValue const & v6=ns3::EmptyAttributeValue(), std::string n7="", ns3::AttributeValue const & v7=ns3::EmptyAttributeValue()) [member function]
    cls.add_method('AddPropagationLoss', 
                   'void', 
                   [param('std::string', 'name'), param('std::string', 'n0', default_value='""'), param('ns3::AttributeValue const &', 'v0', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n1', default_value='""'), param('ns3::AttributeValue const &', 'v1', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n2', default_value='""'), param('ns3::AttributeValue const &', 'v2', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n3', default_value='""'), param('ns3::AttributeValue const &', 'v3', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n4', default_value='""'), param('ns3::AttributeValue const &', 'v4', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n5', default_value='""'), param('ns3::AttributeValue const &', 'v5', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n6', default_value='""'), param('ns3::AttributeValue const &', 'v6', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n7', default_value='""'), param('ns3::AttributeValue const &', 'v7', default_value='ns3::EmptyAttributeValue()')])
    ## yans-wifi-helper.h: ns3::Ptr<ns3::YansWifiChannel> ns3::YansWifiChannelHelper::Create() const [member function]
    cls.add_method('Create', 
                   'ns3::Ptr< ns3::YansWifiChannel >', 
                   [], 
                   is_const=True)
    ## yans-wifi-helper.h: static ns3::YansWifiChannelHelper ns3::YansWifiChannelHelper::Default() [member function]
    cls.add_method('Default', 
                   'ns3::YansWifiChannelHelper', 
                   [], 
                   is_static=True)
    ## yans-wifi-helper.h: void ns3::YansWifiChannelHelper::SetPropagationDelay(std::string name, std::string n0="", ns3::AttributeValue const & v0=ns3::EmptyAttributeValue(), std::string n1="", ns3::AttributeValue const & v1=ns3::EmptyAttributeValue(), std::string n2="", ns3::AttributeValue const & v2=ns3::EmptyAttributeValue(), std::string n3="", ns3::AttributeValue const & v3=ns3::EmptyAttributeValue(), std::string n4="", ns3::AttributeValue const & v4=ns3::EmptyAttributeValue(), std::string n5="", ns3::AttributeValue const & v5=ns3::EmptyAttributeValue(), std::string n6="", ns3::AttributeValue const & v6=ns3::EmptyAttributeValue(), std::string n7="", ns3::AttributeValue const & v7=ns3::EmptyAttributeValue()) [member function]
    cls.add_method('SetPropagationDelay', 
                   'void', 
                   [param('std::string', 'name'), param('std::string', 'n0', default_value='""'), param('ns3::AttributeValue const &', 'v0', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n1', default_value='""'), param('ns3::AttributeValue const &', 'v1', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n2', default_value='""'), param('ns3::AttributeValue const &', 'v2', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n3', default_value='""'), param('ns3::AttributeValue const &', 'v3', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n4', default_value='""'), param('ns3::AttributeValue const &', 'v4', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n5', default_value='""'), param('ns3::AttributeValue const &', 'v5', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n6', default_value='""'), param('ns3::AttributeValue const &', 'v6', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n7', default_value='""'), param('ns3::AttributeValue const &', 'v7', default_value='ns3::EmptyAttributeValue()')])
    return

def register_Ns3YansWifiPhyHelper_methods(root_module, cls):
    ## yans-wifi-helper.h: ns3::YansWifiPhyHelper::YansWifiPhyHelper(ns3::YansWifiPhyHelper const & arg0) [copy constructor]
    cls.add_constructor([param('ns3::YansWifiPhyHelper const &', 'arg0')])
    ## yans-wifi-helper.h: ns3::YansWifiPhyHelper::YansWifiPhyHelper() [constructor]
    cls.add_constructor([])
    ## yans-wifi-helper.h: static ns3::YansWifiPhyHelper ns3::YansWifiPhyHelper::Default() [member function]
    cls.add_method('Default', 
                   'ns3::YansWifiPhyHelper', 
                   [], 
                   is_static=True)
    ## yans-wifi-helper.h: static void ns3::YansWifiPhyHelper::EnableAscii(std::ostream & os, uint32_t nodeid, uint32_t deviceid) [member function]
    cls.add_method('EnableAscii', 
                   'void', 
                   [param('std::ostream &', 'os'), param('uint32_t', 'nodeid'), param('uint32_t', 'deviceid')], 
                   is_static=True)
    ## yans-wifi-helper.h: static void ns3::YansWifiPhyHelper::EnableAscii(std::ostream & os, ns3::NetDeviceContainer d) [member function]
    cls.add_method('EnableAscii', 
                   'void', 
                   [param('std::ostream &', 'os'), param('ns3::NetDeviceContainer', 'd')], 
                   is_static=True)
    ## yans-wifi-helper.h: static void ns3::YansWifiPhyHelper::EnableAscii(std::ostream & os, ns3::NodeContainer n) [member function]
    cls.add_method('EnableAscii', 
                   'void', 
                   [param('std::ostream &', 'os'), param('ns3::NodeContainer', 'n')], 
                   is_static=True)
    ## yans-wifi-helper.h: static void ns3::YansWifiPhyHelper::EnableAsciiAll(std::ostream & os) [member function]
    cls.add_method('EnableAsciiAll', 
                   'void', 
                   [param('std::ostream &', 'os')], 
                   is_static=True)
    ## yans-wifi-helper.h: void ns3::YansWifiPhyHelper::EnablePcap(std::string filename, uint32_t nodeid, uint32_t deviceid) [member function]
    cls.add_method('EnablePcap', 
                   'void', 
                   [param('std::string', 'filename'), param('uint32_t', 'nodeid'), param('uint32_t', 'deviceid')])
    ## yans-wifi-helper.h: void ns3::YansWifiPhyHelper::EnablePcap(std::string filename, ns3::Ptr<ns3::NetDevice> nd) [member function]
    cls.add_method('EnablePcap', 
                   'void', 
                   [param('std::string', 'filename'), param('ns3::Ptr< ns3::NetDevice >', 'nd')])
    ## yans-wifi-helper.h: void ns3::YansWifiPhyHelper::EnablePcap(std::string filename, std::string ndName) [member function]
    cls.add_method('EnablePcap', 
                   'void', 
                   [param('std::string', 'filename'), param('std::string', 'ndName')])
    ## yans-wifi-helper.h: void ns3::YansWifiPhyHelper::EnablePcap(std::string filename, ns3::NetDeviceContainer d) [member function]
    cls.add_method('EnablePcap', 
                   'void', 
                   [param('std::string', 'filename'), param('ns3::NetDeviceContainer', 'd')])
    ## yans-wifi-helper.h: void ns3::YansWifiPhyHelper::EnablePcap(std::string filename, ns3::NodeContainer n) [member function]
    cls.add_method('EnablePcap', 
                   'void', 
                   [param('std::string', 'filename'), param('ns3::NodeContainer', 'n')])
    ## yans-wifi-helper.h: void ns3::YansWifiPhyHelper::EnablePcapAll(std::string filename) [member function]
    cls.add_method('EnablePcapAll', 
                   'void', 
                   [param('std::string', 'filename')])
    ## yans-wifi-helper.h: void ns3::YansWifiPhyHelper::Set(std::string name, ns3::AttributeValue const & v) [member function]
    cls.add_method('Set', 
                   'void', 
                   [param('std::string', 'name'), param('ns3::AttributeValue const &', 'v')])
    ## yans-wifi-helper.h: void ns3::YansWifiPhyHelper::SetChannel(ns3::Ptr<ns3::YansWifiChannel> channel) [member function]
    cls.add_method('SetChannel', 
                   'void', 
                   [param('ns3::Ptr< ns3::YansWifiChannel >', 'channel')])
    ## yans-wifi-helper.h: void ns3::YansWifiPhyHelper::SetChannel(std::string channelName) [member function]
    cls.add_method('SetChannel', 
                   'void', 
                   [param('std::string', 'channelName')])
    ## yans-wifi-helper.h: void ns3::YansWifiPhyHelper::SetErrorRateModel(std::string name, std::string n0="", ns3::AttributeValue const & v0=ns3::EmptyAttributeValue(), std::string n1="", ns3::AttributeValue const & v1=ns3::EmptyAttributeValue(), std::string n2="", ns3::AttributeValue const & v2=ns3::EmptyAttributeValue(), std::string n3="", ns3::AttributeValue const & v3=ns3::EmptyAttributeValue(), std::string n4="", ns3::AttributeValue const & v4=ns3::EmptyAttributeValue(), std::string n5="", ns3::AttributeValue const & v5=ns3::EmptyAttributeValue(), std::string n6="", ns3::AttributeValue const & v6=ns3::EmptyAttributeValue(), std::string n7="", ns3::AttributeValue const & v7=ns3::EmptyAttributeValue()) [member function]
    cls.add_method('SetErrorRateModel', 
                   'void', 
                   [param('std::string', 'name'), param('std::string', 'n0', default_value='""'), param('ns3::AttributeValue const &', 'v0', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n1', default_value='""'), param('ns3::AttributeValue const &', 'v1', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n2', default_value='""'), param('ns3::AttributeValue const &', 'v2', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n3', default_value='""'), param('ns3::AttributeValue const &', 'v3', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n4', default_value='""'), param('ns3::AttributeValue const &', 'v4', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n5', default_value='""'), param('ns3::AttributeValue const &', 'v5', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n6', default_value='""'), param('ns3::AttributeValue const &', 'v6', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n7', default_value='""'), param('ns3::AttributeValue const &', 'v7', default_value='ns3::EmptyAttributeValue()')])
    ## yans-wifi-helper.h: void ns3::YansWifiPhyHelper::SetPcapFormat(ns3::YansWifiPhyHelper::PcapFormat format) [member function]
    cls.add_method('SetPcapFormat', 
                   'void', 
                   [param('ns3::YansWifiPhyHelper::PcapFormat', 'format')])
    ## yans-wifi-helper.h: ns3::Ptr<ns3::WifiPhy> ns3::YansWifiPhyHelper::Create(ns3::Ptr<ns3::Node> node, ns3::Ptr<ns3::WifiNetDevice> device) const [member function]
    cls.add_method('Create', 
                   'ns3::Ptr< ns3::WifiPhy >', 
                   [param('ns3::Ptr< ns3::Node >', 'node'), param('ns3::Ptr< ns3::WifiNetDevice >', 'device')], 
                   is_const=True, visibility='private', is_virtual=True)
    return

def register_Ns3AodvHelper_methods(root_module, cls):
    ## aodv-helper.h: ns3::AodvHelper::AodvHelper(ns3::AodvHelper const & arg0) [copy constructor]
    cls.add_constructor([param('ns3::AodvHelper const &', 'arg0')])
    ## aodv-helper.h: ns3::AodvHelper::AodvHelper() [constructor]
    cls.add_constructor([])
    ## aodv-helper.h: ns3::AodvHelper * ns3::AodvHelper::Copy() const [member function]
    cls.add_method('Copy', 
                   'ns3::AodvHelper *', 
                   [], 
                   is_const=True, is_virtual=True)
    ## aodv-helper.h: ns3::Ptr<ns3::Ipv4RoutingProtocol> ns3::AodvHelper::Create(ns3::Ptr<ns3::Node> node) const [member function]
    cls.add_method('Create', 
                   'ns3::Ptr< ns3::Ipv4RoutingProtocol >', 
                   [param('ns3::Ptr< ns3::Node >', 'node')], 
                   is_const=True, is_virtual=True)
    ## aodv-helper.h: void ns3::AodvHelper::Set(std::string name, ns3::AttributeValue const & value) [member function]
    cls.add_method('Set', 
                   'void', 
                   [param('std::string', 'name'), param('ns3::AttributeValue const &', 'value')])
    return

def register_Ns3Ipv4GlobalRoutingHelper_methods(root_module, cls):
    ## ipv4-global-routing-helper.h: ns3::Ipv4GlobalRoutingHelper::Ipv4GlobalRoutingHelper() [constructor]
    cls.add_constructor([])
    ## ipv4-global-routing-helper.h: ns3::Ipv4GlobalRoutingHelper::Ipv4GlobalRoutingHelper(ns3::Ipv4GlobalRoutingHelper const & arg0) [copy constructor]
    cls.add_constructor([param('ns3::Ipv4GlobalRoutingHelper const &', 'arg0')])
    ## ipv4-global-routing-helper.h: ns3::Ipv4GlobalRoutingHelper * ns3::Ipv4GlobalRoutingHelper::Copy() const [member function]
    cls.add_method('Copy', 
                   'ns3::Ipv4GlobalRoutingHelper *', 
                   [], 
                   is_const=True, is_virtual=True)
    ## ipv4-global-routing-helper.h: ns3::Ptr<ns3::Ipv4RoutingProtocol> ns3::Ipv4GlobalRoutingHelper::Create(ns3::Ptr<ns3::Node> node) const [member function]
    cls.add_method('Create', 
                   'ns3::Ptr< ns3::Ipv4RoutingProtocol >', 
                   [param('ns3::Ptr< ns3::Node >', 'node')], 
                   is_const=True, is_virtual=True)
    ## ipv4-global-routing-helper.h: static void ns3::Ipv4GlobalRoutingHelper::PopulateRoutingTables() [member function]
    cls.add_method('PopulateRoutingTables', 
                   'void', 
                   [], 
                   is_static=True)
    ## ipv4-global-routing-helper.h: static void ns3::Ipv4GlobalRoutingHelper::RecomputeRoutingTables() [member function]
    cls.add_method('RecomputeRoutingTables', 
                   'void', 
                   [], 
                   is_static=True)
    return

def register_Ns3Ipv4ListRoutingHelper_methods(root_module, cls):
    ## ipv4-list-routing-helper.h: ns3::Ipv4ListRoutingHelper::Ipv4ListRoutingHelper() [constructor]
    cls.add_constructor([])
    ## ipv4-list-routing-helper.h: ns3::Ipv4ListRoutingHelper::Ipv4ListRoutingHelper(ns3::Ipv4ListRoutingHelper const & arg0) [copy constructor]
    cls.add_constructor([param('ns3::Ipv4ListRoutingHelper const &', 'arg0')])
    ## ipv4-list-routing-helper.h: ns3::Ipv4ListRoutingHelper * ns3::Ipv4ListRoutingHelper::Copy() const [member function]
    cls.add_method('Copy', 
                   'ns3::Ipv4ListRoutingHelper *', 
                   [], 
                   is_const=True, is_virtual=True)
    ## ipv4-list-routing-helper.h: void ns3::Ipv4ListRoutingHelper::Add(ns3::Ipv4RoutingHelper const & routing, int16_t priority) [member function]
    cls.add_method('Add', 
                   'void', 
                   [param('ns3::Ipv4RoutingHelper const &', 'routing'), param('int16_t', 'priority')])
    ## ipv4-list-routing-helper.h: ns3::Ptr<ns3::Ipv4RoutingProtocol> ns3::Ipv4ListRoutingHelper::Create(ns3::Ptr<ns3::Node> node) const [member function]
    cls.add_method('Create', 
                   'ns3::Ptr< ns3::Ipv4RoutingProtocol >', 
                   [param('ns3::Ptr< ns3::Node >', 'node')], 
                   is_const=True, is_virtual=True)
    return

def register_Ns3Ipv4NixVectorHelper_methods(root_module, cls):
    ## ipv4-nix-vector-helper.h: ns3::Ipv4NixVectorHelper::Ipv4NixVectorHelper() [constructor]
    cls.add_constructor([])
    ## ipv4-nix-vector-helper.h: ns3::Ipv4NixVectorHelper::Ipv4NixVectorHelper(ns3::Ipv4NixVectorHelper const & arg0) [copy constructor]
    cls.add_constructor([param('ns3::Ipv4NixVectorHelper const &', 'arg0')])
    ## ipv4-nix-vector-helper.h: ns3::Ipv4NixVectorHelper * ns3::Ipv4NixVectorHelper::Copy() const [member function]
    cls.add_method('Copy', 
                   'ns3::Ipv4NixVectorHelper *', 
                   [], 
                   is_const=True, is_virtual=True)
    ## ipv4-nix-vector-helper.h: ns3::Ptr<ns3::Ipv4RoutingProtocol> ns3::Ipv4NixVectorHelper::Create(ns3::Ptr<ns3::Node> node) const [member function]
    cls.add_method('Create', 
                   'ns3::Ptr< ns3::Ipv4RoutingProtocol >', 
                   [param('ns3::Ptr< ns3::Node >', 'node')], 
                   is_const=True, is_virtual=True)
    return

def register_Ns3Ipv6ListRoutingHelper_methods(root_module, cls):
    ## ipv6-list-routing-helper.h: ns3::Ipv6ListRoutingHelper::Ipv6ListRoutingHelper() [constructor]
    cls.add_constructor([])
    ## ipv6-list-routing-helper.h: ns3::Ipv6ListRoutingHelper::Ipv6ListRoutingHelper(ns3::Ipv6ListRoutingHelper const & arg0) [copy constructor]
    cls.add_constructor([param('ns3::Ipv6ListRoutingHelper const &', 'arg0')])
    ## ipv6-list-routing-helper.h: ns3::Ipv6ListRoutingHelper * ns3::Ipv6ListRoutingHelper::Copy() const [member function]
    cls.add_method('Copy', 
                   'ns3::Ipv6ListRoutingHelper *', 
                   [], 
                   is_const=True, is_virtual=True)
    ## ipv6-list-routing-helper.h: void ns3::Ipv6ListRoutingHelper::Add(ns3::Ipv6RoutingHelper const & routing, int16_t priority) [member function]
    cls.add_method('Add', 
                   'void', 
                   [param('ns3::Ipv6RoutingHelper const &', 'routing'), param('int16_t', 'priority')])
    ## ipv6-list-routing-helper.h: ns3::Ptr<ns3::Ipv6RoutingProtocol> ns3::Ipv6ListRoutingHelper::Create(ns3::Ptr<ns3::Node> node) const [member function]
    cls.add_method('Create', 
                   'ns3::Ptr< ns3::Ipv6RoutingProtocol >', 
                   [param('ns3::Ptr< ns3::Node >', 'node')], 
                   is_const=True, is_virtual=True)
    return

def register_Ns3NqosWifiMacHelper_methods(root_module, cls):
    ## nqos-wifi-mac-helper.h: ns3::NqosWifiMacHelper::NqosWifiMacHelper(ns3::NqosWifiMacHelper const & arg0) [copy constructor]
    cls.add_constructor([param('ns3::NqosWifiMacHelper const &', 'arg0')])
    ## nqos-wifi-mac-helper.h: ns3::NqosWifiMacHelper::NqosWifiMacHelper() [constructor]
    cls.add_constructor([])
    ## nqos-wifi-mac-helper.h: static ns3::NqosWifiMacHelper ns3::NqosWifiMacHelper::Default() [member function]
    cls.add_method('Default', 
                   'ns3::NqosWifiMacHelper', 
                   [], 
                   is_static=True)
    ## nqos-wifi-mac-helper.h: void ns3::NqosWifiMacHelper::SetType(std::string type, std::string n0="", ns3::AttributeValue const & v0=ns3::EmptyAttributeValue(), std::string n1="", ns3::AttributeValue const & v1=ns3::EmptyAttributeValue(), std::string n2="", ns3::AttributeValue const & v2=ns3::EmptyAttributeValue(), std::string n3="", ns3::AttributeValue const & v3=ns3::EmptyAttributeValue(), std::string n4="", ns3::AttributeValue const & v4=ns3::EmptyAttributeValue(), std::string n5="", ns3::AttributeValue const & v5=ns3::EmptyAttributeValue(), std::string n6="", ns3::AttributeValue const & v6=ns3::EmptyAttributeValue(), std::string n7="", ns3::AttributeValue const & v7=ns3::EmptyAttributeValue()) [member function]
    cls.add_method('SetType', 
                   'void', 
                   [param('std::string', 'type'), param('std::string', 'n0', default_value='""'), param('ns3::AttributeValue const &', 'v0', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n1', default_value='""'), param('ns3::AttributeValue const &', 'v1', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n2', default_value='""'), param('ns3::AttributeValue const &', 'v2', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n3', default_value='""'), param('ns3::AttributeValue const &', 'v3', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n4', default_value='""'), param('ns3::AttributeValue const &', 'v4', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n5', default_value='""'), param('ns3::AttributeValue const &', 'v5', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n6', default_value='""'), param('ns3::AttributeValue const &', 'v6', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n7', default_value='""'), param('ns3::AttributeValue const &', 'v7', default_value='ns3::EmptyAttributeValue()')])
    ## nqos-wifi-mac-helper.h: ns3::Ptr<ns3::WifiMac> ns3::NqosWifiMacHelper::Create() const [member function]
    cls.add_method('Create', 
                   'ns3::Ptr< ns3::WifiMac >', 
                   [], 
                   is_const=True, visibility='private', is_virtual=True)
    return

def register_Ns3QosWifiMacHelper_methods(root_module, cls):
    ## qos-wifi-mac-helper.h: ns3::QosWifiMacHelper::QosWifiMacHelper(ns3::QosWifiMacHelper const & arg0) [copy constructor]
    cls.add_constructor([param('ns3::QosWifiMacHelper const &', 'arg0')])
    ## qos-wifi-mac-helper.h: ns3::QosWifiMacHelper::QosWifiMacHelper() [constructor]
    cls.add_constructor([])
    ## qos-wifi-mac-helper.h: static ns3::QosWifiMacHelper ns3::QosWifiMacHelper::Default() [member function]
    cls.add_method('Default', 
                   'ns3::QosWifiMacHelper', 
                   [], 
                   is_static=True)
    ## qos-wifi-mac-helper.h: void ns3::QosWifiMacHelper::SetMsduAggregatorForAc(ns3::AccessClass accessClass, std::string type, std::string n0="", ns3::AttributeValue const & v0=ns3::EmptyAttributeValue(), std::string n1="", ns3::AttributeValue const & v1=ns3::EmptyAttributeValue(), std::string n2="", ns3::AttributeValue const & v2=ns3::EmptyAttributeValue(), std::string n3="", ns3::AttributeValue const & v3=ns3::EmptyAttributeValue()) [member function]
    cls.add_method('SetMsduAggregatorForAc', 
                   'void', 
                   [param('ns3::AccessClass', 'accessClass'), param('std::string', 'type'), param('std::string', 'n0', default_value='""'), param('ns3::AttributeValue const &', 'v0', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n1', default_value='""'), param('ns3::AttributeValue const &', 'v1', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n2', default_value='""'), param('ns3::AttributeValue const &', 'v2', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n3', default_value='""'), param('ns3::AttributeValue const &', 'v3', default_value='ns3::EmptyAttributeValue()')])
    ## qos-wifi-mac-helper.h: void ns3::QosWifiMacHelper::SetType(std::string type, std::string n0="", ns3::AttributeValue const & v0=ns3::EmptyAttributeValue(), std::string n1="", ns3::AttributeValue const & v1=ns3::EmptyAttributeValue(), std::string n2="", ns3::AttributeValue const & v2=ns3::EmptyAttributeValue(), std::string n3="", ns3::AttributeValue const & v3=ns3::EmptyAttributeValue(), std::string n4="", ns3::AttributeValue const & v4=ns3::EmptyAttributeValue(), std::string n5="", ns3::AttributeValue const & v5=ns3::EmptyAttributeValue(), std::string n6="", ns3::AttributeValue const & v6=ns3::EmptyAttributeValue(), std::string n7="", ns3::AttributeValue const & v7=ns3::EmptyAttributeValue()) [member function]
    cls.add_method('SetType', 
                   'void', 
                   [param('std::string', 'type'), param('std::string', 'n0', default_value='""'), param('ns3::AttributeValue const &', 'v0', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n1', default_value='""'), param('ns3::AttributeValue const &', 'v1', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n2', default_value='""'), param('ns3::AttributeValue const &', 'v2', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n3', default_value='""'), param('ns3::AttributeValue const &', 'v3', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n4', default_value='""'), param('ns3::AttributeValue const &', 'v4', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n5', default_value='""'), param('ns3::AttributeValue const &', 'v5', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n6', default_value='""'), param('ns3::AttributeValue const &', 'v6', default_value='ns3::EmptyAttributeValue()'), param('std::string', 'n7', default_value='""'), param('ns3::AttributeValue const &', 'v7', default_value='ns3::EmptyAttributeValue()')])
    ## qos-wifi-mac-helper.h: ns3::Ptr<ns3::WifiMac> ns3::QosWifiMacHelper::Create() const [member function]
    cls.add_method('Create', 
                   'ns3::Ptr< ns3::WifiMac >', 
                   [], 
                   is_const=True, visibility='private', is_virtual=True)
    return

def register_Ns3AthstatsWifiTraceSink_methods(root_module, cls):
    ## athstats-helper.h: ns3::AthstatsWifiTraceSink::AthstatsWifiTraceSink(ns3::AthstatsWifiTraceSink const & arg0) [copy constructor]
    cls.add_constructor([param('ns3::AthstatsWifiTraceSink const &', 'arg0')])
    ## athstats-helper.h: ns3::AthstatsWifiTraceSink::AthstatsWifiTraceSink() [constructor]
    cls.add_constructor([])
    ## athstats-helper.h: void ns3::AthstatsWifiTraceSink::DevRxTrace(std::string context, ns3::Ptr<ns3::Packet const> p) [member function]
    cls.add_method('DevRxTrace', 
                   'void', 
                   [param('std::string', 'context'), param('ns3::Ptr< ns3::Packet const >', 'p')])
    ## athstats-helper.h: void ns3::AthstatsWifiTraceSink::DevTxTrace(std::string context, ns3::Ptr<ns3::Packet const> p) [member function]
    cls.add_method('DevTxTrace', 
                   'void', 
                   [param('std::string', 'context'), param('ns3::Ptr< ns3::Packet const >', 'p')])
    ## athstats-helper.h: static ns3::TypeId ns3::AthstatsWifiTraceSink::GetTypeId() [member function]
    cls.add_method('GetTypeId', 
                   'ns3::TypeId', 
                   [], 
                   is_static=True)
    ## athstats-helper.h: void ns3::AthstatsWifiTraceSink::Open(std::string const & name) [member function]
    cls.add_method('Open', 
                   'void', 
                   [param('std::string const &', 'name')])
    ## athstats-helper.h: void ns3::AthstatsWifiTraceSink::PhyRxErrorTrace(std::string context, ns3::Ptr<ns3::Packet const> packet, double snr) [member function]
    cls.add_method('PhyRxErrorTrace', 
                   'void', 
                   [param('std::string', 'context'), param('ns3::Ptr< ns3::Packet const >', 'packet'), param('double', 'snr')])
    ## athstats-helper.h: void ns3::AthstatsWifiTraceSink::PhyRxOkTrace(std::string context, ns3::Ptr<ns3::Packet const> packet, double snr, ns3::WifiMode mode, ns3::WifiPreamble preamble) [member function]
    cls.add_method('PhyRxOkTrace', 
                   'void', 
                   [param('std::string', 'context'), param('ns3::Ptr< ns3::Packet const >', 'packet'), param('double', 'snr'), param('ns3::WifiMode', 'mode'), param('ns3::WifiPreamble', 'preamble')])
    ## athstats-helper.h: void ns3::AthstatsWifiTraceSink::PhyStateTrace(std::string context, ns3::Time start, ns3::Time duration, ns3::WifiPhy::State state) [member function]
    cls.add_method('PhyStateTrace', 
                   'void', 
                   [param('std::string', 'context'), param('ns3::Time', 'start'), param('ns3::Time', 'duration'), param('ns3::WifiPhy::State', 'state')])
    ## athstats-helper.h: void ns3::AthstatsWifiTraceSink::PhyTxTrace(std::string context, ns3::Ptr<ns3::Packet const> packet, ns3::WifiMode mode, ns3::WifiPreamble preamble, uint8_t txPower) [member function]
    cls.add_method('PhyTxTrace', 
                   'void', 
                   [param('std::string', 'context'), param('ns3::Ptr< ns3::Packet const >', 'packet'), param('ns3::WifiMode', 'mode'), param('ns3::WifiPreamble', 'preamble'), param('uint8_t', 'txPower')])
    ## athstats-helper.h: void ns3::AthstatsWifiTraceSink::TxDataFailedTrace(std::string context, ns3::Mac48Address address) [member function]
    cls.add_method('TxDataFailedTrace', 
                   'void', 
                   [param('std::string', 'context'), param('ns3::Mac48Address', 'address')])
    ## athstats-helper.h: void ns3::AthstatsWifiTraceSink::TxFinalDataFailedTrace(std::string context, ns3::Mac48Address address) [member function]
    cls.add_method('TxFinalDataFailedTrace', 
                   'void', 
                   [param('std::string', 'context'), param('ns3::Mac48Address', 'address')])
    ## athstats-helper.h: void ns3::AthstatsWifiTraceSink::TxFinalRtsFailedTrace(std::string context, ns3::Mac48Address address) [member function]
    cls.add_method('TxFinalRtsFailedTrace', 
                   'void', 
                   [param('std::string', 'context'), param('ns3::Mac48Address', 'address')])
    ## athstats-helper.h: void ns3::AthstatsWifiTraceSink::TxRtsFailedTrace(std::string context, ns3::Mac48Address address) [member function]
    cls.add_method('TxRtsFailedTrace', 
                   'void', 
                   [param('std::string', 'context'), param('ns3::Mac48Address', 'address')])
    return

def register_Ns3CanvasLocation_methods(root_module, cls):
    ## canvas-location.h: ns3::CanvasLocation::CanvasLocation(ns3::CanvasLocation const & arg0) [copy constructor]
    cls.add_constructor([param('ns3::CanvasLocation const &', 'arg0')])
    ## canvas-location.h: ns3::CanvasLocation::CanvasLocation() [constructor]
    cls.add_constructor([])
    ## canvas-location.h: ns3::Vector ns3::CanvasLocation::GetLocation() const [member function]
    cls.add_method('GetLocation', 
                   'ns3::Vector', 
                   [], 
                   is_const=True)
    ## canvas-location.h: static ns3::TypeId ns3::CanvasLocation::GetTypeId() [member function]
    cls.add_method('GetTypeId', 
                   'ns3::TypeId', 
                   [], 
                   is_static=True)
    ## canvas-location.h: void ns3::CanvasLocation::SetLocation(ns3::Vector const & location) [member function]
    cls.add_method('SetLocation', 
                   'void', 
                   [param('ns3::Vector const &', 'location')])
    return

def register_Ns3MeshStack_methods(root_module, cls):
    ## mesh-stack-installer.h: ns3::MeshStack::MeshStack() [constructor]
    cls.add_constructor([])
    ## mesh-stack-installer.h: ns3::MeshStack::MeshStack(ns3::MeshStack const & arg0) [copy constructor]
    cls.add_constructor([param('ns3::MeshStack const &', 'arg0')])
    ## mesh-stack-installer.h: bool ns3::MeshStack::InstallStack(ns3::Ptr<ns3::MeshPointDevice> mp) [member function]
    cls.add_method('InstallStack', 
                   'bool', 
                   [param('ns3::Ptr< ns3::MeshPointDevice >', 'mp')], 
                   is_pure_virtual=True, is_virtual=True)
    ## mesh-stack-installer.h: void ns3::MeshStack::Report(ns3::Ptr<ns3::MeshPointDevice> const mp, std::ostream & arg1) [member function]
    cls.add_method('Report', 
                   'void', 
                   [param('ns3::Ptr< ns3::MeshPointDevice > const', 'mp'), param('std::ostream &', 'arg1')], 
                   is_pure_virtual=True, is_virtual=True)
    ## mesh-stack-installer.h: void ns3::MeshStack::ResetStats(ns3::Ptr<ns3::MeshPointDevice> const mp) [member function]
    cls.add_method('ResetStats', 
                   'void', 
                   [param('ns3::Ptr< ns3::MeshPointDevice > const', 'mp')], 
                   is_pure_virtual=True, is_virtual=True)
    return

def register_Ns3Dot11sStack_methods(root_module, cls):
    ## dot11s-installer.h: ns3::Dot11sStack::Dot11sStack(ns3::Dot11sStack const & arg0) [copy constructor]
    cls.add_constructor([param('ns3::Dot11sStack const &', 'arg0')])
    ## dot11s-installer.h: ns3::Dot11sStack::Dot11sStack() [constructor]
    cls.add_constructor([])
    ## dot11s-installer.h: void ns3::Dot11sStack::DoDispose() [member function]
    cls.add_method('DoDispose', 
                   'void', 
                   [], 
                   is_virtual=True)
    ## dot11s-installer.h: static ns3::TypeId ns3::Dot11sStack::GetTypeId() [member function]
    cls.add_method('GetTypeId', 
                   'ns3::TypeId', 
                   [], 
                   is_static=True)
    ## dot11s-installer.h: bool ns3::Dot11sStack::InstallStack(ns3::Ptr<ns3::MeshPointDevice> mp) [member function]
    cls.add_method('InstallStack', 
                   'bool', 
                   [param('ns3::Ptr< ns3::MeshPointDevice >', 'mp')], 
                   is_virtual=True)
    ## dot11s-installer.h: void ns3::Dot11sStack::Report(ns3::Ptr<ns3::MeshPointDevice> const mp, std::ostream & arg1) [member function]
    cls.add_method('Report', 
                   'void', 
                   [param('ns3::Ptr< ns3::MeshPointDevice > const', 'mp'), param('std::ostream &', 'arg1')], 
                   is_virtual=True)
    ## dot11s-installer.h: void ns3::Dot11sStack::ResetStats(ns3::Ptr<ns3::MeshPointDevice> const mp) [member function]
    cls.add_method('ResetStats', 
                   'void', 
                   [param('ns3::Ptr< ns3::MeshPointDevice > const', 'mp')], 
                   is_virtual=True)
    return

def register_Ns3FlameStack_methods(root_module, cls):
    ## flame-installer.h: ns3::FlameStack::FlameStack(ns3::FlameStack const & arg0) [copy constructor]
    cls.add_constructor([param('ns3::FlameStack const &', 'arg0')])
    ## flame-installer.h: ns3::FlameStack::FlameStack() [constructor]
    cls.add_constructor([])
    ## flame-installer.h: void ns3::FlameStack::DoDispose() [member function]
    cls.add_method('DoDispose', 
                   'void', 
                   [], 
                   is_virtual=True)
    ## flame-installer.h: static ns3::TypeId ns3::FlameStack::GetTypeId() [member function]
    cls.add_method('GetTypeId', 
                   'ns3::TypeId', 
                   [], 
                   is_static=True)
    ## flame-installer.h: bool ns3::FlameStack::InstallStack(ns3::Ptr<ns3::MeshPointDevice> mp) [member function]
    cls.add_method('InstallStack', 
                   'bool', 
                   [param('ns3::Ptr< ns3::MeshPointDevice >', 'mp')], 
                   is_virtual=True)
    ## flame-installer.h: void ns3::FlameStack::Report(ns3::Ptr<ns3::MeshPointDevice> const mp, std::ostream & arg1) [member function]
    cls.add_method('Report', 
                   'void', 
                   [param('ns3::Ptr< ns3::MeshPointDevice > const', 'mp'), param('std::ostream &', 'arg1')], 
                   is_virtual=True)
    ## flame-installer.h: void ns3::FlameStack::ResetStats(ns3::Ptr<ns3::MeshPointDevice> const mp) [member function]
    cls.add_method('ResetStats', 
                   'void', 
                   [param('ns3::Ptr< ns3::MeshPointDevice > const', 'mp')], 
                   is_virtual=True)
    return

def register_functions(root_module):
    module = root_module
    register_functions_ns3_Config(module.get_submodule('Config'), root_module)
    register_functions_ns3_TimeStepPrecision(module.get_submodule('TimeStepPrecision'), root_module)
    register_functions_ns3_addressUtils(module.get_submodule('addressUtils'), root_module)
    register_functions_ns3_aodv(module.get_submodule('aodv'), root_module)
    register_functions_ns3_dot11s(module.get_submodule('dot11s'), root_module)
    register_functions_ns3_flame(module.get_submodule('flame'), root_module)
    register_functions_ns3_internal(module.get_submodule('internal'), root_module)
    register_functions_ns3_olsr(module.get_submodule('olsr'), root_module)
    return

def register_functions_ns3_Config(module, root_module):
    return

def register_functions_ns3_TimeStepPrecision(module, root_module):
    return

def register_functions_ns3_addressUtils(module, root_module):
    return

def register_functions_ns3_aodv(module, root_module):
    return

def register_functions_ns3_dot11s(module, root_module):
    return

def register_functions_ns3_flame(module, root_module):
    return

def register_functions_ns3_internal(module, root_module):
    return

def register_functions_ns3_olsr(module, root_module):
    return

