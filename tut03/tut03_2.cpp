#include <iostream>
#include <tins/tins.h>
#include <pthread.h>
#include <unistd.h>

using namespace Tins;
using namespace std;

// const string target_ip = "104.124.137.161"; // https://www.hepsiburada.com/
const string target_ip = "104.124.137.161";

void* scan(void*){
    // Specify the network interface to capture packets from (e.g., "eth0")
    Sniffer sniffer("eth0");
    bool syn_received = false;
    bool syn_ack_received = false;
    bool ack_received = false;

    vector<EthernetII> v;

    // Specify the network interface to send the packet from(e.g., "eth0")
    NetworkInterface iface("eth0");
    // Start capturing packets
    sniffer.sniff_loop([&syn_received, &syn_ack_received, &ack_received, &v](Packet &packet){
        const IP &ip = packet.pdu()->rfind_pdu<IP>();
        const TCP &tcp = packet.pdu()->rfind_pdu<TCP>();
        const RawPDU &raw = packet.pdu()->rfind_pdu<RawPDU>();

        cout<<"source: "<< ip.src_addr()<<" destination: "<<ip.dst_addr()<<endl;

        if(ip.dst_addr() == target_ip && v.size()<3){
            if (!syn_received && tcp.get_flag(TCP::SYN)){
                syn_received = true;
                cout<<"SYN received"<<endl;
                v.push_back(EthernetII() / ip / tcp / raw);
            }
            else if (syn_received && syn_ack_received && tcp.get_flag(TCP::ACK)){
                ack_received = true;
                cout<<"ACK received"<<endl;
                v.push_back(EthernetII() / ip / tcp / raw);
            }
        }else if(ip.src_addr() == target_ip && v.size()<3){
            if (syn_received && !syn_ack_received && tcp.get_flag(TCP::SYN) && tcp.get_flag(TCP::ACK)){
                syn_ack_received = true;
                cout<<"SYN-ACK received"<<endl;
                v.push_back(EthernetII() / ip / tcp / raw);
            }
        }
        return !syn_received || !syn_ack_received || !ack_received;
    });
    // write to pcap file
    if (v.size() == 3)
    {
        PacketWriter writer("./output/TCP_3_way_handshake_start_2101AI40.pcap", DataLinkType<EthernetII>());
        writer.write(v.begin(), v.end());
    }
    return 0;
}

void send_syn_packet(){
    // Specify the network interface to send the packet from(e.g., "eth0")
    NetworkInterface iface("eth0");

    /* Retrieve this structure which holds the interface's IP,
     * broadcast, hardware address and the network mask.
     */
    NetworkInterface::Info info = iface.addresses();

    // Create an Ethernet frame
    EthernetII eth = EthernetII() / IP(target_ip, info.ip_addr) / TCP(443, 12345) / RawPDU("Hello World");

    // Set the TCP flags to SYN
    eth.rfind_pdu<TCP>().set_flag(TCP::SYN, 1);

    // Send the SYN packet
    PacketSender sender;
    sender.send(eth, iface);

    std::cout << "SYN packet sent to initiate the TCP 3-way handshake." << std::endl;
}

void tcp_3_way_handshake(){
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, scan,  NULL);
    sleep(1);
    send_syn_packet();
    void* dummy;
    pthread_join(thread_id, &dummy);
}

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

void sniff_tcp_packets()
{
    try
    {
        // Specify the network interface to capture packets from (e.g., "eth0")
        Sniffer sniffer("eth0");
        vector<EthernetII> v;
        // Start capturing packets
        sniffer.sniff_loop([&](Packet &packet){
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
            return true;
        });
        PacketWriter writer("./output/TCP_Packets_2101AI40.pcap", DataLinkType<EthernetII>());
        writer.write(v.begin(), v.end());
    }
    catch (std::exception &ex)
    {
        std::cerr << "Error: " << ex.what() << std::endl;
    }
}

int main(){
    // sniff_udp_packets();
    // sniff_tcp_packets();
    tcp_3_way_handshake();
    return 0;
}