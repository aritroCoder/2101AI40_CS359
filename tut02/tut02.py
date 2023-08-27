import scapy.all as scapy

target_ip = "104.124.137.161" # https://www.hepsiburada.com/
target_port = 443

def open_connection():
    print(f"Opening a TCP connection to {target_ip}:{target_port}...")
    
    # Create a SYN packet (initiating the 3-way handshake)
    syn_packet = scapy.IP(dst=target_ip) / scapy.TCP(dport=target_port, flags="S", seq=1000)

    # Send the SYN packet and receive the SYN-ACK response
    syn_ack_response = scapy.sr1(syn_packet)

    if syn_ack_response:
        # Create an ACK packet to complete the 3-way handshake
        ack_packet = scapy.IP(dst=target_ip) / scapy.TCP(dport=target_port, flags="A", seq=syn_ack_response.ack, ack=syn_ack_response.seq + 1)
        
        # Send the ACK packet
        scapy.send(ack_packet)
        
        # Capture traffic including the handshake in a pcap file
        pcap_filename = "./output/TCP_3_way_handshake_start_2101AI40.pcap"
        scapy.wrpcap(pcap_filename, [syn_packet, syn_ack_response, ack_packet])
        print(f"TCP handshake captured and stored in '{pcap_filename}'.")
    else:
        print("No response was received for the SYN packet.")

def close_connection():
    print(f"Opening a TCP connection to {target_ip}:{target_port}...")
    
    # Create a SYN packet (initiating the 3-way handshake)
    syn_packet = scapy.IP(dst=target_ip) / scapy.TCP(dport=target_port, flags="S", seq=1000)

    # Send the SYN packet and receive the SYN-ACK response
    syn_ack_response = scapy.sr1(syn_packet)

    if syn_ack_response:
        # Create an ACK packet to complete the 3-way handshake
        ack_packet = scapy.IP(dst=target_ip) / scapy.TCP(dport=target_port, flags="A", seq=syn_ack_response.ack, ack=syn_ack_response.seq + 1)
        
        # Send the ACK packet
        scapy.send(ack_packet)
        
        print(f"Closing the TCP connection to {target_ip}:{target_port}...")
    
        # Create a FIN packet (initiating the 4-way handshake)
        fin_packet = scapy.IP(dst=target_ip) / scapy.TCP(dport=target_port, flags="FA", seq=syn_ack_response.ack, ack=syn_ack_response.seq + 1)

        # Send the FIN packet and receive the FIN-ACK response
        fin_ack_response = scapy.sr1(fin_packet)

        if fin_ack_response:
            # Create an ACK packet to complete the 4-way handshake
            ack_packet = scapy.IP(dst=target_ip) / scapy.TCP(dport=target_port, flags="A", seq=fin_ack_response.ack, ack=fin_ack_response.seq + 1)
            
            # Send the ACK packet
            scapy.send(ack_packet)
            
            # Capture traffic including the handshake in a pcap file
            pcap_filename = "./output/TCP_handshake_close_2101AI40.pcap"
            scapy.wrpcap(pcap_filename, [fin_packet, fin_ack_response, ack_packet])
            print(f"TCP handshake captured and stored in '{pcap_filename}'.")
        else:
            print("No response was received for the FIN packet.")
    else:
        print("No response was received for the SYN packet.")
        
def sniff_packets():
    packet_list = []
    def packet_handler(packet):
        if packet.haslayer(scapy.TCP) and len(packet_list) < 2:
            print(packet.summary())
            res_packet = packet.copy()
            res_packet.src, res_packet.dst = packet.dst, packet.src
            print(res_packet.summary())
            packet_list.append(packet)
            packet_list.append(res_packet)

    # Start sniffing packets on the specified interface (e.g., "eth0")
    interface = "eth0"
    print(f"Sniffing packets on interface '{interface}'...")
    scapy.sniff(iface=interface, prn=packet_handler, timeout=5)
    scapy.wrpcap("./output/TCP_Packets_2101AI40.pcap", packet_list)
    print("Finished sniffing packets.")
    
def sniff_udp_packets():
    packet_list = []
    def packet_handler(packet):
        if len(packet_list) < 2:
            print("source packet: "+ packet.src + " to "+ packet.dst)
            res_packet = packet.copy()
            res_packet.src, res_packet.dst = packet.dst, packet.src
            print("response packet: "+ res_packet.src + " to "+ res_packet.dst)
            packet_list.append(packet)
            packet_list.append(res_packet)

    # Start sniffing packets on the specified interface (e.g., "eth0")
    interface = "eth0"
    print(f"Sniffing UDP packets on interface '{interface}'...")
    scapy.sniff(iface=interface, filter="udp", prn=packet_handler, timeout=10)
    scapy.wrpcap("./output/UDP_Packets_2101AI40.pcap", packet_list)
    print("Finished sniffing packets.")

if __name__ == "__main__":
    open_connection()
    close_connection()
    sniff_packets()
    # sniff_udp_packets()
