/**
 * Packet Sniffer - IPK 2nd project
 *
 * author: Roland Schulz
 */

#include <iostream>
#include <functional>
#include <unistd.h>
#include <vector>
#include <netinet/in.h>
#include <csignal>
#include <fstream>
#include <arpa/inet.h>
#include <pcap/pcap.h>
#include "lib/cxxopts.hpp"
#include <cerrno>
#include <ifaddrs.h>
#include <sys/types.h>
#include <netinet/ether.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip_icmp.h>
#include <netinet/ip.h>

using namespace std;

pcap_t* handle;
int linkhdrlen;
int packets;

/**
 * Replace every occurrence of \p remove in \p str with \p insert
 *
 * @param str template string, possibly containing \p remove
 * @param remove string to replace with \p insert
 * @param insert string used as value to replace \p remove with
 * @return \p str with every occurrence of \p remove replaced with \p insert
 */
string replaceEvery(string str, const string &remove, const string &insert) {
    string::size_type pos = 0;
    while ((pos = str.find(remove, pos)) != string::npos)
        str.replace(pos++, remove.size(), insert);

    return str;
}

/**
 * Fills in \p templateStr based on defined replacements
 *
 * @param templateStr string used as a template
 * @param replacements key->value pairs to place into \p templateStr
 * @return \p templateStr formatted with replacements from \p replacements
 */
string renderTemplate(string templateStr, const vector<pair<string, string>>& replacements){
    for(const auto& replace : replacements) {
        templateStr = replaceEvery(templateStr, "{" + replace.first + "}", replace.second);
    }
    return templateStr;
}

bool two_of(vector<bool> v){
    size_t const half_length = v.size() / 2;
    vector<bool> first_half(v.begin(), v.begin() + half_length);
    vector<bool> second_half(v.begin() + half_length, v.end());
    auto pred = [](bool p){return p;};
    return any_of(first_half.begin(), first_half.end(), pred) &&
           any_of(second_half.begin(), second_half.end(), pred);
}

void printNICs() {
   struct ifaddrs* ptr_ifaddrs = nullptr;

    auto result = getifaddrs(&ptr_ifaddrs);
    if( result != 0 ){
        std::cout << "`getifaddrs()` failed: " << strerror(errno) << std::endl;
    }

    for(struct ifaddrs* ptr_entry = ptr_ifaddrs; ptr_entry != nullptr; ptr_entry = ptr_entry->ifa_next ){
        std::string interface_name = std::string(ptr_entry->ifa_name);
        std::cout << interface_name << std::endl;
    }

    freeifaddrs(ptr_ifaddrs);
}

string getPacketFilter(cxxopts::ParseResult args) {
    string filter="";
    for (string pf: {"tcp", "udp", "icmp", "arp"}) {
        if(!!args.count(pf))
            filter.append(pf);
    }

    if (!!args.count("port")){
        filter.append(" port ");
        filter.append(to_string(args["port"].as<int>()));
    }

    return filter;
}

cxxopts::ParseResult handleArguments(char *programName, int argCount, char **args){
    cxxopts::Options options(programName);
    options.add_options()
            ("i,interface", "Network interface to listen for packets", cxxopts::value<string>()->default_value(""))
            ("p,port", "Port number on which to listen for traffic", cxxopts::value<int>()->default_value("-1"))
            ("t,tcp", "Show only TCP packets", cxxopts::value<bool>()->default_value("false"))
            ("u,udp", "Show only UDP packets", cxxopts::value<bool>()->default_value("false"))
            ("arp", "Show only ARP frames", cxxopts::value<bool>()->default_value("false"))
            ("icmp", "Show only ICMPv4 and ICMPv6 packets", cxxopts::value<bool>()->default_value("false"))
            ("n", "Number of packets to display", cxxopts::value<int>()->default_value("1"));

    cxxopts::ParseResult result = options.parse(argCount, args);

    if (two_of({ !!result.count("arp"), !!result.count("icmp"), !!result.count("tcp"), !!result.count("udp")})){
        throw cxxopts::OptionException("Can use only one of arp, icmp, tcp or udp filters");
    }
    return result;
}

void printHexaDump(unsigned char *packet, int size) {
    int line_counter = 1;
    printf("0x0000:  ");
    for (int i = 0; i < size; i++) {
        if ((i != 0 && i % 16 == 0)) {
            printf(" ");
            for (int j = i - 16; j < i; j++) {
                printf("%c", isprint(packet[j]) ? (unsigned char) packet[j] : '.');
            }
            printf("\n");
            printf("0x%04d:  ", line_counter++ * 10);
        }
        if (((i - 8) % 16) == 0)
            printf(" ");
        printf("%02X ", (unsigned char) packet[i]);

    }
}

void stopCapturing(int _){
    pcap_close(handle);
    exit(0);
}

// https://www.binarytides.com/packet-sniffer-code-c-libpcap-linux-sockets/
pcap_t* create_pcap_handle(const char* device, const char* filter) {
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *handle = NULL;
    struct bpf_program bpf;
    bpf_u_int32 netmask;
    bpf_u_int32 srcip;

    // Get network device source IP address and netmask.
    if (pcap_lookupnet(device, &srcip, &netmask, errbuf) == PCAP_ERROR) {
        fprintf(stderr, "pcap_lookupnet: %s\n", errbuf);
        return nullptr;
    }

    // Open the device for live capture.
    handle = pcap_open_live(device, BUFSIZ, 1, 1000, errbuf);
    if (handle == NULL) {
        fprintf(stderr, "pcap_open_live(): %s\n", errbuf);
        return nullptr;
    }

    // Convert the packet filter expression into a packet filter binary.
    if (pcap_compile(handle, &bpf, filter, 1, netmask) == PCAP_ERROR) {
        fprintf(stderr, "pcap_compile(): %s\n", pcap_geterr(handle));
        return nullptr;
    }

    // Bind the packet filter to the libpcap handle.
    if (pcap_setfilter(handle, &bpf) == PCAP_ERROR) {
        fprintf(stderr, "pcap_setfilter(): %s\n", pcap_geterr(handle));
        return nullptr;
    }

    return handle;
}

int get_link_header_len(pcap_t* handle) {
    int linktype = 0;

    // Determine the datalink layer type.
    if ((linktype = pcap_datalink(handle)) == PCAP_ERROR) {
        printf("pcap_datalink(): %s\n", pcap_geterr(handle));
        return -1;
    }

    // Set the datalink layer header size.
    switch (linktype) {
        case DLT_NULL:
            linkhdrlen = 4;
            break;

        case DLT_EN10MB:
            linkhdrlen = 14;
            break;

        case DLT_SLIP:
        case DLT_PPP:
            linkhdrlen = 24;
            break;

        default:
            printf("Unsupported datalink (%d)\n", linktype);
            linkhdrlen = 0;
    }
    return linktype;
}

string getTimestamp(struct timeval tv){
    char buf[64];
    auto ts = tv.tv_sec;
    auto lts = localtime(&ts);
    strftime(buf, sizeof buf, "%Y-%m-%d %H:%M:%S", lts);
    return string(buf);
}

// https://www.binarytides.com/packet-sniffer-code-c-libpcap-linux-sockets/
void packetHandler(u_char *_, const struct pcap_pkthdr *packethdr, const u_char *packetptr) {
    struct ip* iphdr;
    struct tcphdr* tcphdr;
    struct udphdr* udphdr;
    struct icmp* icmphdr;
    char iphdrInfo[256];
    string portSrc, portDst = "";
    vector<pair<string, string>> portData;
    iphdr = (struct ip*)packetptr;

    const string outputTemplate = "timestamp: {TIMESTAMP}\n"
                                  "src MAC: {MAC_SRC}\n"
                                  "dst MAC: {MAC_DST}\n"
                                  "frame length: {FRAME_LEN}\n"
                                  "src IP: {IP_SRC}\n"
                                  "dst IP: {IP_DST}\n"
                                  "src port: {PORT_SRC}\n"
                                  "dst port: {PORT_DST}\n"
                                  "{DUMP}"
                                  "";
    packetptr += linkhdrlen;
    packetptr += 4*iphdr->ip_hl;
    auto eptr = (struct ether_header *) packetptr;
    switch (iphdr->ip_p) {
    case IPPROTO_TCP:
        tcphdr = (struct tcphdr*)packetptr;
        portSrc = to_string(ntohs(tcphdr->th_sport));
        portDst = to_string(ntohs(tcphdr->th_dport));
        packets += 1;
        break;

    case IPPROTO_UDP:
        udphdr = (struct udphdr*)packetptr;
        portSrc = to_string(ntohs(udphdr->uh_sport));
        portDst = to_string(ntohs(udphdr->uh_dport));
        packets += 1;
        break;

    case IPPROTO_ICMP:
        icmphdr = (struct icmp*)packetptr;
        packets += 1;
        break;
    }

    vector<pair<string, string>> templateData = {
            {"TIMESTAMP", getTimestamp(packethdr->ts)},
            {"MAC_SRC", string(ether_ntoa((const struct ether_addr *)&eptr->ether_shost))},
            {"MAC_DST", string(ether_ntoa((const struct ether_addr *)&eptr->ether_dhost))},
            {"FRAME_LEN", to_string(packethdr->len)},
            {"PORT_SRC", portSrc},
            {"PORT_DST", portDst},
            {"IP_SRC", string(inet_ntoa(iphdr->ip_src))},
            {"IP_DST", string(inet_ntoa(iphdr->ip_dst))},
            {"DUMP", ""},
    };
    cout << renderTemplate(outputTemplate, templateData) << endl;
    printHexaDump((unsigned char *)packetptr, packethdr->caplen);
    printf("\n\n");
}

int main(int argc, char ** argv) {
    signal(SIGINT, stopCapturing);
    signal(SIGTERM, stopCapturing);
    signal(SIGTERM, stopCapturing);

    cxxopts::ParseResult args;
    try {
        args = handleArguments(argv[0], argc, argv);
    } catch (const cxxopts::OptionException &e) {
        std::cerr << argv[0] << ": " << e.what() << '\n';
        return 1;
    }

    if (args["interface"].as<string>().length() == 0){
        printNICs();
        return 0;
    }

    string filter = getPacketFilter(args);

    handle = create_pcap_handle(args["interface"].as<string>().c_str(), filter.c_str());
    if (handle == nullptr)
        return 1;

    auto link_layer_type = get_link_header_len(handle);
    if (link_layer_type <= 0)
        return 1;

    if (pcap_loop(handle, args["n"].as<int>(), packetHandler, nullptr) < 0)
        return 1;

    return 0;
}
