#include <iostream>
#include <tins/tins.h>
#include <pthread.h>
#include <unistd.h>

using namespace Tins;
using namespace std;

int main(){
    Sniffer sniffer("eth0");
    while(true){
        try{
            PDU *pdu = sniffer.next_packet();
            if(pdu){
                const IP &ip = pdu->rfind_pdu<IP>();
                const TCP &tcp = pdu->rfind_pdu<TCP>();
                const RawPDU &raw = pdu->rfind_pdu<RawPDU>();
                if (ip.src_addr() == "10.11.22.12" || ip.dst_addr() == "10.11.22.12"){
                    cout << ip.src_addr() << " " << ip.dst_addr() << endl;
                    cout << tcp.flags() << endl;
                }
            }
        }
        catch(...){
        }
    }
    return 0;
}
