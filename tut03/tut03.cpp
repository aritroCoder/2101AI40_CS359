#include <iostream>
#include <tins/tins.h>
#include <iomanip> // Include this header for std::setw

using namespace Tins;
using namespace std;

void sniff_udp_packets(){
    try{
        // Specify the network interface to capture packets from (e.g., "eth0")
        Sniffer sniffer("eth0");
        vector<EthernetII> v;
        // Start capturing packets
        sniffer.sniff_loop([&](Packet &packet){
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
            return true;
        });
        PacketWriter writer("./output/UDP_Packets_2101AI40.pcap", DataLinkType<EthernetII>());
        writer.write(v.begin(), v.end());
    }
    catch (std::exception &ex){
        std::cerr << "Error: " << ex.what() << std::endl;
    }
}

int main(){
    sniff_udp_packets();
    return 0;
}