from pybindgen import Module, FileCodeSink, param, retval, cppclass, typehandlers


import pybindgen.settings
import warnings

class ErrorHandler(pybindgen.settings.ErrorHandler):
    def handle_error(self, wrapper, exception, traceback_):
        warnings.warn("exception %r in wrapper %s" % (exception, wrapper))
        return True
pybindgen.settings.error_handler = ErrorHandler()


import sys
import ns3_module_core
import ns3_module_simulator
import ns3_module_test
import ns3_module_common
import ns3_module_mobility
import ns3_module_contrib
import ns3_module_node
import ns3_module_bridge
import ns3_module_csma
import ns3_module_emu
import ns3_module_global_routing
import ns3_module_internet_stack
import ns3_module_list_routing
import ns3_module_onoff
import ns3_module_packet_sink
import ns3_module_point_to_point
import ns3_module_static_routing
import ns3_module_stats
import ns3_module_tap_bridge
import ns3_module_v4ping
import ns3_module_virtual_net_device
import ns3_module_wifi
import ns3_module_aodv
import ns3_module_flow_monitor
import ns3_module_nix_vector_routing
import ns3_module_olsr
import ns3_module_ping6
import ns3_module_radvd
import ns3_module_udp_client_server
import ns3_module_udp_echo
import ns3_module_mesh
import ns3_module_helper
import ns3_module_dot11s
import ns3_module_flame

def module_init():
    root_module = Module('ns3', cpp_namespace='::ns3')
    return root_module

def register_types(module):
    root_module = module.get_root()
    
    root_module.begin_section('ns3_module_core')
    ns3_module_core.register_types(module)
    
    try:
        import ns3_module_core__local
    except ImportError:
        pass
    else:
        ns3_module_core__local.register_types(module)
    
    root_module.end_section('ns3_module_core')
    root_module.begin_section('ns3_module_simulator')
    ns3_module_simulator.register_types(module)
    
    try:
        import ns3_module_simulator__local
    except ImportError:
        pass
    else:
        ns3_module_simulator__local.register_types(module)
    
    root_module.end_section('ns3_module_simulator')
    root_module.begin_section('ns3_module_test')
    ns3_module_test.register_types(module)
    
    try:
        import ns3_module_test__local
    except ImportError:
        pass
    else:
        ns3_module_test__local.register_types(module)
    
    root_module.end_section('ns3_module_test')
    root_module.begin_section('ns3_module_common')
    ns3_module_common.register_types(module)
    
    try:
        import ns3_module_common__local
    except ImportError:
        pass
    else:
        ns3_module_common__local.register_types(module)
    
    root_module.end_section('ns3_module_common')
    root_module.begin_section('ns3_module_mobility')
    ns3_module_mobility.register_types(module)
    
    try:
        import ns3_module_mobility__local
    except ImportError:
        pass
    else:
        ns3_module_mobility__local.register_types(module)
    
    root_module.end_section('ns3_module_mobility')
    root_module.begin_section('ns3_module_contrib')
    ns3_module_contrib.register_types(module)
    
    try:
        import ns3_module_contrib__local
    except ImportError:
        pass
    else:
        ns3_module_contrib__local.register_types(module)
    
    root_module.end_section('ns3_module_contrib')
    root_module.begin_section('ns3_module_node')
    ns3_module_node.register_types(module)
    
    try:
        import ns3_module_node__local
    except ImportError:
        pass
    else:
        ns3_module_node__local.register_types(module)
    
    root_module.end_section('ns3_module_node')
    root_module.begin_section('ns3_module_bridge')
    ns3_module_bridge.register_types(module)
    
    try:
        import ns3_module_bridge__local
    except ImportError:
        pass
    else:
        ns3_module_bridge__local.register_types(module)
    
    root_module.end_section('ns3_module_bridge')
    root_module.begin_section('ns3_module_csma')
    ns3_module_csma.register_types(module)
    
    try:
        import ns3_module_csma__local
    except ImportError:
        pass
    else:
        ns3_module_csma__local.register_types(module)
    
    root_module.end_section('ns3_module_csma')
    root_module.begin_section('ns3_module_emu')
    ns3_module_emu.register_types(module)
    
    try:
        import ns3_module_emu__local
    except ImportError:
        pass
    else:
        ns3_module_emu__local.register_types(module)
    
    root_module.end_section('ns3_module_emu')
    root_module.begin_section('ns3_module_global_routing')
    ns3_module_global_routing.register_types(module)
    
    try:
        import ns3_module_global_routing__local
    except ImportError:
        pass
    else:
        ns3_module_global_routing__local.register_types(module)
    
    root_module.end_section('ns3_module_global_routing')
    root_module.begin_section('ns3_module_internet_stack')
    ns3_module_internet_stack.register_types(module)
    
    try:
        import ns3_module_internet_stack__local
    except ImportError:
        pass
    else:
        ns3_module_internet_stack__local.register_types(module)
    
    root_module.end_section('ns3_module_internet_stack')
    root_module.begin_section('ns3_module_list_routing')
    ns3_module_list_routing.register_types(module)
    
    try:
        import ns3_module_list_routing__local
    except ImportError:
        pass
    else:
        ns3_module_list_routing__local.register_types(module)
    
    root_module.end_section('ns3_module_list_routing')
    root_module.begin_section('ns3_module_onoff')
    ns3_module_onoff.register_types(module)
    
    try:
        import ns3_module_onoff__local
    except ImportError:
        pass
    else:
        ns3_module_onoff__local.register_types(module)
    
    root_module.end_section('ns3_module_onoff')
    root_module.begin_section('ns3_module_packet_sink')
    ns3_module_packet_sink.register_types(module)
    
    try:
        import ns3_module_packet_sink__local
    except ImportError:
        pass
    else:
        ns3_module_packet_sink__local.register_types(module)
    
    root_module.end_section('ns3_module_packet_sink')
    root_module.begin_section('ns3_module_point_to_point')
    ns3_module_point_to_point.register_types(module)
    
    try:
        import ns3_module_point_to_point__local
    except ImportError:
        pass
    else:
        ns3_module_point_to_point__local.register_types(module)
    
    root_module.end_section('ns3_module_point_to_point')
    root_module.begin_section('ns3_module_static_routing')
    ns3_module_static_routing.register_types(module)
    
    try:
        import ns3_module_static_routing__local
    except ImportError:
        pass
    else:
        ns3_module_static_routing__local.register_types(module)
    
    root_module.end_section('ns3_module_static_routing')
    root_module.begin_section('ns3_module_stats')
    ns3_module_stats.register_types(module)
    
    try:
        import ns3_module_stats__local
    except ImportError:
        pass
    else:
        ns3_module_stats__local.register_types(module)
    
    root_module.end_section('ns3_module_stats')
    root_module.begin_section('ns3_module_tap_bridge')
    ns3_module_tap_bridge.register_types(module)
    
    try:
        import ns3_module_tap_bridge__local
    except ImportError:
        pass
    else:
        ns3_module_tap_bridge__local.register_types(module)
    
    root_module.end_section('ns3_module_tap_bridge')
    root_module.begin_section('ns3_module_v4ping')
    ns3_module_v4ping.register_types(module)
    
    try:
        import ns3_module_v4ping__local
    except ImportError:
        pass
    else:
        ns3_module_v4ping__local.register_types(module)
    
    root_module.end_section('ns3_module_v4ping')
    root_module.begin_section('ns3_module_virtual_net_device')
    ns3_module_virtual_net_device.register_types(module)
    
    try:
        import ns3_module_virtual_net_device__local
    except ImportError:
        pass
    else:
        ns3_module_virtual_net_device__local.register_types(module)
    
    root_module.end_section('ns3_module_virtual_net_device')
    root_module.begin_section('ns3_module_wifi')
    ns3_module_wifi.register_types(module)
    
    try:
        import ns3_module_wifi__local
    except ImportError:
        pass
    else:
        ns3_module_wifi__local.register_types(module)
    
    root_module.end_section('ns3_module_wifi')
    root_module.begin_section('ns3_module_aodv')
    ns3_module_aodv.register_types(module)
    
    try:
        import ns3_module_aodv__local
    except ImportError:
        pass
    else:
        ns3_module_aodv__local.register_types(module)
    
    root_module.end_section('ns3_module_aodv')
    root_module.begin_section('ns3_module_flow_monitor')
    ns3_module_flow_monitor.register_types(module)
    
    try:
        import ns3_module_flow_monitor__local
    except ImportError:
        pass
    else:
        ns3_module_flow_monitor__local.register_types(module)
    
    root_module.end_section('ns3_module_flow_monitor')
    root_module.begin_section('ns3_module_nix_vector_routing')
    ns3_module_nix_vector_routing.register_types(module)
    
    try:
        import ns3_module_nix_vector_routing__local
    except ImportError:
        pass
    else:
        ns3_module_nix_vector_routing__local.register_types(module)
    
    root_module.end_section('ns3_module_nix_vector_routing')
    root_module.begin_section('ns3_module_olsr')
    ns3_module_olsr.register_types(module)
    
    try:
        import ns3_module_olsr__local
    except ImportError:
        pass
    else:
        ns3_module_olsr__local.register_types(module)
    
    root_module.end_section('ns3_module_olsr')
    root_module.begin_section('ns3_module_ping6')
    ns3_module_ping6.register_types(module)
    
    try:
        import ns3_module_ping6__local
    except ImportError:
        pass
    else:
        ns3_module_ping6__local.register_types(module)
    
    root_module.end_section('ns3_module_ping6')
    root_module.begin_section('ns3_module_radvd')
    ns3_module_radvd.register_types(module)
    
    try:
        import ns3_module_radvd__local
    except ImportError:
        pass
    else:
        ns3_module_radvd__local.register_types(module)
    
    root_module.end_section('ns3_module_radvd')
    root_module.begin_section('ns3_module_udp_client_server')
    ns3_module_udp_client_server.register_types(module)
    
    try:
        import ns3_module_udp_client_server__local
    except ImportError:
        pass
    else:
        ns3_module_udp_client_server__local.register_types(module)
    
    root_module.end_section('ns3_module_udp_client_server')
    root_module.begin_section('ns3_module_udp_echo')
    ns3_module_udp_echo.register_types(module)
    
    try:
        import ns3_module_udp_echo__local
    except ImportError:
        pass
    else:
        ns3_module_udp_echo__local.register_types(module)
    
    root_module.end_section('ns3_module_udp_echo')
    root_module.begin_section('ns3_module_mesh')
    ns3_module_mesh.register_types(module)
    
    try:
        import ns3_module_mesh__local
    except ImportError:
        pass
    else:
        ns3_module_mesh__local.register_types(module)
    
    root_module.end_section('ns3_module_mesh')
    root_module.begin_section('ns3_module_helper')
    ns3_module_helper.register_types(module)
    
    try:
        import ns3_module_helper__local
    except ImportError:
        pass
    else:
        ns3_module_helper__local.register_types(module)
    
    root_module.end_section('ns3_module_helper')
    root_module.begin_section('ns3_module_dot11s')
    ns3_module_dot11s.register_types(module)
    
    try:
        import ns3_module_dot11s__local
    except ImportError:
        pass
    else:
        ns3_module_dot11s__local.register_types(module)
    
    root_module.end_section('ns3_module_dot11s')
    root_module.begin_section('ns3_module_flame')
    ns3_module_flame.register_types(module)
    
    try:
        import ns3_module_flame__local
    except ImportError:
        pass
    else:
        ns3_module_flame__local.register_types(module)
    
    root_module.end_section('ns3_module_flame')
    module.add_container('std::vector< unsigned int >', 'unsigned int', container_type='vector')
    module.add_container('std::vector< bool >', 'bool', container_type='vector')
    module.add_container('std::vector< unsigned long long >', 'long long unsigned int', container_type='vector')
    module.add_container('std::list< unsigned int >', 'unsigned int', container_type='list')
    module.add_container('std::list< std::pair< ns3::Ptr< ns3::Packet >, ns3::AmsduSubframeHeader > >', 'std::pair< ns3::Ptr< ns3::Packet >, ns3::AmsduSubframeHeader >', container_type='list')
    
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
    
    module.add_container('std::vector< std::string >', 'std::string', container_type='vector')

def register_types_ns3_TimeStepPrecision(module):
    root_module = module.get_root()
    

def register_types_ns3_addressUtils(module):
    root_module = module.get_root()
    

def register_types_ns3_aodv(module):
    root_module = module.get_root()
    
    module.add_container('std::map< ns3::Ipv4Address, unsigned int >', ('ns3::Ipv4Address', 'unsigned int'), container_type='map')

def register_types_ns3_dot11s(module):
    root_module = module.get_root()
    

def register_types_ns3_flame(module):
    root_module = module.get_root()
    

def register_types_ns3_internal(module):
    root_module = module.get_root()
    

def register_types_ns3_olsr(module):
    root_module = module.get_root()
    

def register_methods(root_module):
    root_module.begin_section('ns3_module_core')
    ns3_module_core.register_methods(root_module)
    
    try:
        import ns3_module_core__local
    except ImportError:
        pass
    else:
        ns3_module_core__local.register_methods(root_module)
    
    root_module.end_section('ns3_module_core')
    root_module.begin_section('ns3_module_simulator')
    ns3_module_simulator.register_methods(root_module)
    
    try:
        import ns3_module_simulator__local
    except ImportError:
        pass
    else:
        ns3_module_simulator__local.register_methods(root_module)
    
    root_module.end_section('ns3_module_simulator')
    root_module.begin_section('ns3_module_test')
    ns3_module_test.register_methods(root_module)
    
    try:
        import ns3_module_test__local
    except ImportError:
        pass
    else:
        ns3_module_test__local.register_methods(root_module)
    
    root_module.end_section('ns3_module_test')
    root_module.begin_section('ns3_module_common')
    ns3_module_common.register_methods(root_module)
    
    try:
        import ns3_module_common__local
    except ImportError:
        pass
    else:
        ns3_module_common__local.register_methods(root_module)
    
    root_module.end_section('ns3_module_common')
    root_module.begin_section('ns3_module_mobility')
    ns3_module_mobility.register_methods(root_module)
    
    try:
        import ns3_module_mobility__local
    except ImportError:
        pass
    else:
        ns3_module_mobility__local.register_methods(root_module)
    
    root_module.end_section('ns3_module_mobility')
    root_module.begin_section('ns3_module_contrib')
    ns3_module_contrib.register_methods(root_module)
    
    try:
        import ns3_module_contrib__local
    except ImportError:
        pass
    else:
        ns3_module_contrib__local.register_methods(root_module)
    
    root_module.end_section('ns3_module_contrib')
    root_module.begin_section('ns3_module_node')
    ns3_module_node.register_methods(root_module)
    
    try:
        import ns3_module_node__local
    except ImportError:
        pass
    else:
        ns3_module_node__local.register_methods(root_module)
    
    root_module.end_section('ns3_module_node')
    root_module.begin_section('ns3_module_bridge')
    ns3_module_bridge.register_methods(root_module)
    
    try:
        import ns3_module_bridge__local
    except ImportError:
        pass
    else:
        ns3_module_bridge__local.register_methods(root_module)
    
    root_module.end_section('ns3_module_bridge')
    root_module.begin_section('ns3_module_csma')
    ns3_module_csma.register_methods(root_module)
    
    try:
        import ns3_module_csma__local
    except ImportError:
        pass
    else:
        ns3_module_csma__local.register_methods(root_module)
    
    root_module.end_section('ns3_module_csma')
    root_module.begin_section('ns3_module_emu')
    ns3_module_emu.register_methods(root_module)
    
    try:
        import ns3_module_emu__local
    except ImportError:
        pass
    else:
        ns3_module_emu__local.register_methods(root_module)
    
    root_module.end_section('ns3_module_emu')
    root_module.begin_section('ns3_module_global_routing')
    ns3_module_global_routing.register_methods(root_module)
    
    try:
        import ns3_module_global_routing__local
    except ImportError:
        pass
    else:
        ns3_module_global_routing__local.register_methods(root_module)
    
    root_module.end_section('ns3_module_global_routing')
    root_module.begin_section('ns3_module_internet_stack')
    ns3_module_internet_stack.register_methods(root_module)
    
    try:
        import ns3_module_internet_stack__local
    except ImportError:
        pass
    else:
        ns3_module_internet_stack__local.register_methods(root_module)
    
    root_module.end_section('ns3_module_internet_stack')
    root_module.begin_section('ns3_module_list_routing')
    ns3_module_list_routing.register_methods(root_module)
    
    try:
        import ns3_module_list_routing__local
    except ImportError:
        pass
    else:
        ns3_module_list_routing__local.register_methods(root_module)
    
    root_module.end_section('ns3_module_list_routing')
    root_module.begin_section('ns3_module_onoff')
    ns3_module_onoff.register_methods(root_module)
    
    try:
        import ns3_module_onoff__local
    except ImportError:
        pass
    else:
        ns3_module_onoff__local.register_methods(root_module)
    
    root_module.end_section('ns3_module_onoff')
    root_module.begin_section('ns3_module_packet_sink')
    ns3_module_packet_sink.register_methods(root_module)
    
    try:
        import ns3_module_packet_sink__local
    except ImportError:
        pass
    else:
        ns3_module_packet_sink__local.register_methods(root_module)
    
    root_module.end_section('ns3_module_packet_sink')
    root_module.begin_section('ns3_module_point_to_point')
    ns3_module_point_to_point.register_methods(root_module)
    
    try:
        import ns3_module_point_to_point__local
    except ImportError:
        pass
    else:
        ns3_module_point_to_point__local.register_methods(root_module)
    
    root_module.end_section('ns3_module_point_to_point')
    root_module.begin_section('ns3_module_static_routing')
    ns3_module_static_routing.register_methods(root_module)
    
    try:
        import ns3_module_static_routing__local
    except ImportError:
        pass
    else:
        ns3_module_static_routing__local.register_methods(root_module)
    
    root_module.end_section('ns3_module_static_routing')
    root_module.begin_section('ns3_module_stats')
    ns3_module_stats.register_methods(root_module)
    
    try:
        import ns3_module_stats__local
    except ImportError:
        pass
    else:
        ns3_module_stats__local.register_methods(root_module)
    
    root_module.end_section('ns3_module_stats')
    root_module.begin_section('ns3_module_tap_bridge')
    ns3_module_tap_bridge.register_methods(root_module)
    
    try:
        import ns3_module_tap_bridge__local
    except ImportError:
        pass
    else:
        ns3_module_tap_bridge__local.register_methods(root_module)
    
    root_module.end_section('ns3_module_tap_bridge')
    root_module.begin_section('ns3_module_v4ping')
    ns3_module_v4ping.register_methods(root_module)
    
    try:
        import ns3_module_v4ping__local
    except ImportError:
        pass
    else:
        ns3_module_v4ping__local.register_methods(root_module)
    
    root_module.end_section('ns3_module_v4ping')
    root_module.begin_section('ns3_module_virtual_net_device')
    ns3_module_virtual_net_device.register_methods(root_module)
    
    try:
        import ns3_module_virtual_net_device__local
    except ImportError:
        pass
    else:
        ns3_module_virtual_net_device__local.register_methods(root_module)
    
    root_module.end_section('ns3_module_virtual_net_device')
    root_module.begin_section('ns3_module_wifi')
    ns3_module_wifi.register_methods(root_module)
    
    try:
        import ns3_module_wifi__local
    except ImportError:
        pass
    else:
        ns3_module_wifi__local.register_methods(root_module)
    
    root_module.end_section('ns3_module_wifi')
    root_module.begin_section('ns3_module_aodv')
    ns3_module_aodv.register_methods(root_module)
    
    try:
        import ns3_module_aodv__local
    except ImportError:
        pass
    else:
        ns3_module_aodv__local.register_methods(root_module)
    
    root_module.end_section('ns3_module_aodv')
    root_module.begin_section('ns3_module_flow_monitor')
    ns3_module_flow_monitor.register_methods(root_module)
    
    try:
        import ns3_module_flow_monitor__local
    except ImportError:
        pass
    else:
        ns3_module_flow_monitor__local.register_methods(root_module)
    
    root_module.end_section('ns3_module_flow_monitor')
    root_module.begin_section('ns3_module_nix_vector_routing')
    ns3_module_nix_vector_routing.register_methods(root_module)
    
    try:
        import ns3_module_nix_vector_routing__local
    except ImportError:
        pass
    else:
        ns3_module_nix_vector_routing__local.register_methods(root_module)
    
    root_module.end_section('ns3_module_nix_vector_routing')
    root_module.begin_section('ns3_module_olsr')
    ns3_module_olsr.register_methods(root_module)
    
    try:
        import ns3_module_olsr__local
    except ImportError:
        pass
    else:
        ns3_module_olsr__local.register_methods(root_module)
    
    root_module.end_section('ns3_module_olsr')
    root_module.begin_section('ns3_module_ping6')
    ns3_module_ping6.register_methods(root_module)
    
    try:
        import ns3_module_ping6__local
    except ImportError:
        pass
    else:
        ns3_module_ping6__local.register_methods(root_module)
    
    root_module.end_section('ns3_module_ping6')
    root_module.begin_section('ns3_module_radvd')
    ns3_module_radvd.register_methods(root_module)
    
    try:
        import ns3_module_radvd__local
    except ImportError:
        pass
    else:
        ns3_module_radvd__local.register_methods(root_module)
    
    root_module.end_section('ns3_module_radvd')
    root_module.begin_section('ns3_module_udp_client_server')
    ns3_module_udp_client_server.register_methods(root_module)
    
    try:
        import ns3_module_udp_client_server__local
    except ImportError:
        pass
    else:
        ns3_module_udp_client_server__local.register_methods(root_module)
    
    root_module.end_section('ns3_module_udp_client_server')
    root_module.begin_section('ns3_module_udp_echo')
    ns3_module_udp_echo.register_methods(root_module)
    
    try:
        import ns3_module_udp_echo__local
    except ImportError:
        pass
    else:
        ns3_module_udp_echo__local.register_methods(root_module)
    
    root_module.end_section('ns3_module_udp_echo')
    root_module.begin_section('ns3_module_mesh')
    ns3_module_mesh.register_methods(root_module)
    
    try:
        import ns3_module_mesh__local
    except ImportError:
        pass
    else:
        ns3_module_mesh__local.register_methods(root_module)
    
    root_module.end_section('ns3_module_mesh')
    root_module.begin_section('ns3_module_helper')
    ns3_module_helper.register_methods(root_module)
    
    try:
        import ns3_module_helper__local
    except ImportError:
        pass
    else:
        ns3_module_helper__local.register_methods(root_module)
    
    root_module.end_section('ns3_module_helper')
    root_module.begin_section('ns3_module_dot11s')
    ns3_module_dot11s.register_methods(root_module)
    
    try:
        import ns3_module_dot11s__local
    except ImportError:
        pass
    else:
        ns3_module_dot11s__local.register_methods(root_module)
    
    root_module.end_section('ns3_module_dot11s')
    root_module.begin_section('ns3_module_flame')
    ns3_module_flame.register_methods(root_module)
    
    try:
        import ns3_module_flame__local
    except ImportError:
        pass
    else:
        ns3_module_flame__local.register_methods(root_module)
    
    root_module.end_section('ns3_module_flame')
    return

def register_functions(root_module):
    module = root_module
    root_module.begin_section('ns3_module_core')
    ns3_module_core.register_functions(root_module)
    
    try:
        import ns3_module_core__local
    except ImportError:
        pass
    else:
        ns3_module_core__local.register_functions(root_module)
    
    root_module.end_section('ns3_module_core')
    root_module.begin_section('ns3_module_simulator')
    ns3_module_simulator.register_functions(root_module)
    
    try:
        import ns3_module_simulator__local
    except ImportError:
        pass
    else:
        ns3_module_simulator__local.register_functions(root_module)
    
    root_module.end_section('ns3_module_simulator')
    root_module.begin_section('ns3_module_test')
    ns3_module_test.register_functions(root_module)
    
    try:
        import ns3_module_test__local
    except ImportError:
        pass
    else:
        ns3_module_test__local.register_functions(root_module)
    
    root_module.end_section('ns3_module_test')
    root_module.begin_section('ns3_module_common')
    ns3_module_common.register_functions(root_module)
    
    try:
        import ns3_module_common__local
    except ImportError:
        pass
    else:
        ns3_module_common__local.register_functions(root_module)
    
    root_module.end_section('ns3_module_common')
    root_module.begin_section('ns3_module_mobility')
    ns3_module_mobility.register_functions(root_module)
    
    try:
        import ns3_module_mobility__local
    except ImportError:
        pass
    else:
        ns3_module_mobility__local.register_functions(root_module)
    
    root_module.end_section('ns3_module_mobility')
    root_module.begin_section('ns3_module_contrib')
    ns3_module_contrib.register_functions(root_module)
    
    try:
        import ns3_module_contrib__local
    except ImportError:
        pass
    else:
        ns3_module_contrib__local.register_functions(root_module)
    
    root_module.end_section('ns3_module_contrib')
    root_module.begin_section('ns3_module_node')
    ns3_module_node.register_functions(root_module)
    
    try:
        import ns3_module_node__local
    except ImportError:
        pass
    else:
        ns3_module_node__local.register_functions(root_module)
    
    root_module.end_section('ns3_module_node')
    root_module.begin_section('ns3_module_bridge')
    ns3_module_bridge.register_functions(root_module)
    
    try:
        import ns3_module_bridge__local
    except ImportError:
        pass
    else:
        ns3_module_bridge__local.register_functions(root_module)
    
    root_module.end_section('ns3_module_bridge')
    root_module.begin_section('ns3_module_csma')
    ns3_module_csma.register_functions(root_module)
    
    try:
        import ns3_module_csma__local
    except ImportError:
        pass
    else:
        ns3_module_csma__local.register_functions(root_module)
    
    root_module.end_section('ns3_module_csma')
    root_module.begin_section('ns3_module_emu')
    ns3_module_emu.register_functions(root_module)
    
    try:
        import ns3_module_emu__local
    except ImportError:
        pass
    else:
        ns3_module_emu__local.register_functions(root_module)
    
    root_module.end_section('ns3_module_emu')
    root_module.begin_section('ns3_module_global_routing')
    ns3_module_global_routing.register_functions(root_module)
    
    try:
        import ns3_module_global_routing__local
    except ImportError:
        pass
    else:
        ns3_module_global_routing__local.register_functions(root_module)
    
    root_module.end_section('ns3_module_global_routing')
    root_module.begin_section('ns3_module_internet_stack')
    ns3_module_internet_stack.register_functions(root_module)
    
    try:
        import ns3_module_internet_stack__local
    except ImportError:
        pass
    else:
        ns3_module_internet_stack__local.register_functions(root_module)
    
    root_module.end_section('ns3_module_internet_stack')
    root_module.begin_section('ns3_module_list_routing')
    ns3_module_list_routing.register_functions(root_module)
    
    try:
        import ns3_module_list_routing__local
    except ImportError:
        pass
    else:
        ns3_module_list_routing__local.register_functions(root_module)
    
    root_module.end_section('ns3_module_list_routing')
    root_module.begin_section('ns3_module_onoff')
    ns3_module_onoff.register_functions(root_module)
    
    try:
        import ns3_module_onoff__local
    except ImportError:
        pass
    else:
        ns3_module_onoff__local.register_functions(root_module)
    
    root_module.end_section('ns3_module_onoff')
    root_module.begin_section('ns3_module_packet_sink')
    ns3_module_packet_sink.register_functions(root_module)
    
    try:
        import ns3_module_packet_sink__local
    except ImportError:
        pass
    else:
        ns3_module_packet_sink__local.register_functions(root_module)
    
    root_module.end_section('ns3_module_packet_sink')
    root_module.begin_section('ns3_module_point_to_point')
    ns3_module_point_to_point.register_functions(root_module)
    
    try:
        import ns3_module_point_to_point__local
    except ImportError:
        pass
    else:
        ns3_module_point_to_point__local.register_functions(root_module)
    
    root_module.end_section('ns3_module_point_to_point')
    root_module.begin_section('ns3_module_static_routing')
    ns3_module_static_routing.register_functions(root_module)
    
    try:
        import ns3_module_static_routing__local
    except ImportError:
        pass
    else:
        ns3_module_static_routing__local.register_functions(root_module)
    
    root_module.end_section('ns3_module_static_routing')
    root_module.begin_section('ns3_module_stats')
    ns3_module_stats.register_functions(root_module)
    
    try:
        import ns3_module_stats__local
    except ImportError:
        pass
    else:
        ns3_module_stats__local.register_functions(root_module)
    
    root_module.end_section('ns3_module_stats')
    root_module.begin_section('ns3_module_tap_bridge')
    ns3_module_tap_bridge.register_functions(root_module)
    
    try:
        import ns3_module_tap_bridge__local
    except ImportError:
        pass
    else:
        ns3_module_tap_bridge__local.register_functions(root_module)
    
    root_module.end_section('ns3_module_tap_bridge')
    root_module.begin_section('ns3_module_v4ping')
    ns3_module_v4ping.register_functions(root_module)
    
    try:
        import ns3_module_v4ping__local
    except ImportError:
        pass
    else:
        ns3_module_v4ping__local.register_functions(root_module)
    
    root_module.end_section('ns3_module_v4ping')
    root_module.begin_section('ns3_module_virtual_net_device')
    ns3_module_virtual_net_device.register_functions(root_module)
    
    try:
        import ns3_module_virtual_net_device__local
    except ImportError:
        pass
    else:
        ns3_module_virtual_net_device__local.register_functions(root_module)
    
    root_module.end_section('ns3_module_virtual_net_device')
    root_module.begin_section('ns3_module_wifi')
    ns3_module_wifi.register_functions(root_module)
    
    try:
        import ns3_module_wifi__local
    except ImportError:
        pass
    else:
        ns3_module_wifi__local.register_functions(root_module)
    
    root_module.end_section('ns3_module_wifi')
    root_module.begin_section('ns3_module_aodv')
    ns3_module_aodv.register_functions(root_module)
    
    try:
        import ns3_module_aodv__local
    except ImportError:
        pass
    else:
        ns3_module_aodv__local.register_functions(root_module)
    
    root_module.end_section('ns3_module_aodv')
    root_module.begin_section('ns3_module_flow_monitor')
    ns3_module_flow_monitor.register_functions(root_module)
    
    try:
        import ns3_module_flow_monitor__local
    except ImportError:
        pass
    else:
        ns3_module_flow_monitor__local.register_functions(root_module)
    
    root_module.end_section('ns3_module_flow_monitor')
    root_module.begin_section('ns3_module_nix_vector_routing')
    ns3_module_nix_vector_routing.register_functions(root_module)
    
    try:
        import ns3_module_nix_vector_routing__local
    except ImportError:
        pass
    else:
        ns3_module_nix_vector_routing__local.register_functions(root_module)
    
    root_module.end_section('ns3_module_nix_vector_routing')
    root_module.begin_section('ns3_module_olsr')
    ns3_module_olsr.register_functions(root_module)
    
    try:
        import ns3_module_olsr__local
    except ImportError:
        pass
    else:
        ns3_module_olsr__local.register_functions(root_module)
    
    root_module.end_section('ns3_module_olsr')
    root_module.begin_section('ns3_module_ping6')
    ns3_module_ping6.register_functions(root_module)
    
    try:
        import ns3_module_ping6__local
    except ImportError:
        pass
    else:
        ns3_module_ping6__local.register_functions(root_module)
    
    root_module.end_section('ns3_module_ping6')
    root_module.begin_section('ns3_module_radvd')
    ns3_module_radvd.register_functions(root_module)
    
    try:
        import ns3_module_radvd__local
    except ImportError:
        pass
    else:
        ns3_module_radvd__local.register_functions(root_module)
    
    root_module.end_section('ns3_module_radvd')
    root_module.begin_section('ns3_module_udp_client_server')
    ns3_module_udp_client_server.register_functions(root_module)
    
    try:
        import ns3_module_udp_client_server__local
    except ImportError:
        pass
    else:
        ns3_module_udp_client_server__local.register_functions(root_module)
    
    root_module.end_section('ns3_module_udp_client_server')
    root_module.begin_section('ns3_module_udp_echo')
    ns3_module_udp_echo.register_functions(root_module)
    
    try:
        import ns3_module_udp_echo__local
    except ImportError:
        pass
    else:
        ns3_module_udp_echo__local.register_functions(root_module)
    
    root_module.end_section('ns3_module_udp_echo')
    root_module.begin_section('ns3_module_mesh')
    ns3_module_mesh.register_functions(root_module)
    
    try:
        import ns3_module_mesh__local
    except ImportError:
        pass
    else:
        ns3_module_mesh__local.register_functions(root_module)
    
    root_module.end_section('ns3_module_mesh')
    root_module.begin_section('ns3_module_helper')
    ns3_module_helper.register_functions(root_module)
    
    try:
        import ns3_module_helper__local
    except ImportError:
        pass
    else:
        ns3_module_helper__local.register_functions(root_module)
    
    root_module.end_section('ns3_module_helper')
    root_module.begin_section('ns3_module_dot11s')
    ns3_module_dot11s.register_functions(root_module)
    
    try:
        import ns3_module_dot11s__local
    except ImportError:
        pass
    else:
        ns3_module_dot11s__local.register_functions(root_module)
    
    root_module.end_section('ns3_module_dot11s')
    root_module.begin_section('ns3_module_flame')
    ns3_module_flame.register_functions(root_module)
    
    try:
        import ns3_module_flame__local
    except ImportError:
        pass
    else:
        ns3_module_flame__local.register_functions(root_module)
    
    root_module.end_section('ns3_module_flame')
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

def main():
    out = FileCodeSink(sys.stdout)
    root_module = module_init()
    register_types(root_module)
    register_methods(root_module)
    register_functions(root_module)
    root_module.generate(out)

if __name__ == '__main__':
    main()

