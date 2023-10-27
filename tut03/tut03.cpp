#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <pthread.h>
#include <set>
#include <string>
#include <tins/ethernetII.h>
#include <tins/ip.h>
#include <tins/ip_address.h>
#include <tins/network_interface.h>
#include <tins/packet_sender.h>
#include <tins/sniffer.h>
#include <tins/tcp.h>
#include <tins/tins.h>
#include <tins/utils.h>
#include <unistd.h>
#include <vector>

using namespace Tins;
using namespace std;

const std::string target_ip = "10.11.22.12";

// Utitlity Functions

// Function to check the packet is a SYN packet
bool is_syn_packet(PDU& packet)
{
    TCP& tcp = packet.rfind_pdu<TCP>();
    return tcp.flags() == TCP::SYN;
}

// Function to check the packet is a RST packet
bool is_rst_packet(PDU& packet)
{
    TCP& tcp = packet.rfind_pdu<TCP>();
    return tcp.flags() == TCP::RST;
}

// Function to check the packet is a SYN-ACK packet
bool is_syn_ack_packet(PDU& packet)
{
    TCP& tcp = packet.rfind_pdu<TCP>();
    return tcp.flags() == (TCP::SYN | TCP::ACK);
}

// Function to check the packet is a FIN-ACK packet
bool is_fin_ack_packet(PDU& packet)
{
    TCP& tcp = packet.rfind_pdu<TCP>();
    return tcp.flags() == (TCP::FIN | TCP::ACK);
}

// Function to check the packet is a ACK packet
bool is_ack_packet(PDU& packet)
{
    TCP& tcp = packet.rfind_pdu<TCP>();
    return tcp.flags() == TCP::ACK;
}

// Function to capture the three way handshake
// We have to capture the tcp packet one from my ip to a destination and one from the same destination to my ip
void three_way_handshake_start()
{
    SnifferConfiguration config;
    config.set_promisc_mode(true);
    Sniffer sniffer("eth0", config);

    PacketWriter writer("./output/TCP_3_way_handshake_start_2101AI40.pcap", DataLinkType<EthernetII>());

    try {
        bool syn_received = false;
        bool syn_ack_received = false;
        bool ack_received = false;
        while (true) {
            PtrPacket packet = sniffer.next_packet();
            if (!packet) {
                continue; 
            }
            PDU* packet_new = packet.pdu();
            if (packet_new->find_pdu<IP>()) {
                const IP& ip = packet_new->rfind_pdu<IP>();
                if (ip.dst_addr() == target_ip || ip.src_addr() == target_ip) {
                    if (is_syn_packet(*packet_new)) {
                        if (!syn_received) {
                            syn_received = true;
                            std::cout << "SYN packet received" << std::endl;
                            writer.write(*packet_new);
                        }
                    } else if (is_syn_ack_packet(*packet_new) && syn_received && !syn_ack_received && !ack_received) {
                        syn_ack_received = true;
                        std::cout << "SYN-ACK packet received" << std::endl;
                        writer.write(*packet_new);
                    } else if (is_ack_packet(*packet_new) && syn_received && syn_ack_received && !ack_received) {
                        ack_received = true;
                        std::cout << "ACK packet received" << std::endl;
                        writer.write(*packet_new);
                        break;
                    }
                }
            }
        }
    } catch (std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return;
    }
}

// Function to capture the three way handshake closure FIN-ACK, RST, ACK
void three_way_handshake_close()
{
    SnifferConfiguration config;
    config.set_promisc_mode(true);
    Sniffer sniffer("eth0", config);

    PacketWriter writer("./output/TCP_3_way_handshake_close_2101AI40.pcap", DataLinkType<EthernetII>());

    try {
        bool fin_ack_received = false;
        bool rst_received = false;
        bool ack_received = false;
        while (true) {
            PtrPacket packet = sniffer.next_packet();
            if (!packet) {
                continue; 
            }
            PDU* packet_new = packet.pdu();
            if (packet_new->find_pdu<IP>()) {
                const IP& ip = packet_new->rfind_pdu<IP>();
                if (ip.dst_addr() == target_ip || ip.src_addr() == target_ip) {
                    if (is_fin_ack_packet(*packet_new)) {
                        if (!fin_ack_received) {
                            fin_ack_received = true;
                            std::cout << "FIN ACK packet received" << std::endl;
                            writer.write(*packet_new);
                        }
                    } else if (is_rst_packet(*packet_new) && fin_ack_received && !rst_received && !ack_received) {
                        rst_received = true;
                        std::cout << "RST packet received" << std::endl;
                        writer.write(*packet_new);
                    } else if (is_ack_packet(*packet_new) && fin_ack_received && fin_ack_received && !ack_received) {
                        ack_received = true;
                        std::cout << "ACK packet received" << std::endl;
                        writer.write(*packet_new);
                        break; 
                    }
                }
            }
        }
    } catch (std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return;
    }
}

void sniff_tcp_packets()
{
    try
    {
        // Specify the network interface to capture packets from (e.g., "eth0")
        Sniffer sniffer("eth0");
        vector<EthernetII> v;
        // Start capturing packets
        sniffer.sniff_loop([&](Packet &packet)
                           {
            if(v.size()>=2) return false;
            const IP &ip = packet.pdu()->rfind_pdu<IP>();
            const TCP &tcp = packet.pdu()->rfind_pdu<TCP>();
            const RawPDU &raw = packet.pdu()->rfind_pdu<RawPDU>();
            // add the packet to vector and write to pcap file
            v.push_back(EthernetII() / ip / tcp / raw);
            auto new_pkt = EthernetII() / ip / tcp / raw;
            // swap the source and destination IP address
            new_pkt.rfind_pdu<IP>().src_addr(ip.dst_addr());
            new_pkt.rfind_pdu<IP>().dst_addr(ip.src_addr());
            // add the packet to vector and write to pcap file
            v.push_back(new_pkt);
            // send the packet
            PacketSender sender;
            sender.send(new_pkt, "eth0");
            return true; });
        PacketWriter writer("./output/TCP_Packets_2101AI40.pcap", DataLinkType<EthernetII>());
        writer.write(v.begin(), v.end());
    }
    catch (std::exception &ex)
    {
        std::cerr << "Error: sniff_tcp_packets" << ex.what() << std::endl;
    }
}

void sniff_udp_packets()
{
    try
    {
        // Specify the network interface to capture packets from (e.g., "eth0")
        Sniffer sniffer("eth0");
        vector<EthernetII> v;
        // Start capturing packets
        sniffer.sniff_loop([&](Packet &packet)
                           {
            if(v.size()>=2) return false;
            const IP &ip = packet.pdu()->rfind_pdu<IP>();
            const UDP &udp = packet.pdu()->rfind_pdu<UDP>();
            const RawPDU &raw = packet.pdu()->rfind_pdu<RawPDU>();
            // add the packet to vector and write to pcap file
            v.push_back(EthernetII() / ip / udp / raw);
            auto new_pkt = EthernetII() / ip / udp / raw;
            // swap the source and destination IP address
            new_pkt.rfind_pdu<IP>().src_addr(ip.dst_addr());
            new_pkt.rfind_pdu<IP>().dst_addr(ip.src_addr());
            // add the packet to vector and write to pcap file
            v.push_back(new_pkt);
            // send the packet
            PacketSender sender;
            sender.send(new_pkt, "eth0");
            return true; });
        PacketWriter writer("./output/UDP_Packets_2101AI40.pcap", DataLinkType<EthernetII>());
        writer.write(v.begin(), v.end());
    }
    catch (std::exception &ex)
    {
        std::cerr << "Error: sniff_udp_packets: " << ex.what() << std::endl;
    }
}

int main()
{
    three_way_handshake_start();
    three_way_handshake_close();
    sniff_tcp_packets();
    sniff_udp_packets();
    return 0;
}
