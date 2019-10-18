const char source_code[] = "\
/* -*- p4lang -*- */\n\
#include <core.p4>\n\
#include <v1model.p4>\n\
\n\
const bit<16> TYPE_IPV4 = 0x800;\n\
\n\
/*************************************************************************\n\
*********************** H E A D E R S  ***********************************\n\
*************************************************************************/\n\
\n\
typedef bit<9>  egressSpec_t;\n\
typedef bit<48> macAddr_t;\n\
typedef bit<32> ip4Addr_t;\n\
\n\
header ethernet_t {\n\
    macAddr_t dstAddr;\n\
    macAddr_t srcAddr;\n\
    bit<16>   etherType;\n\
}\n\
\n\
header ipv4_t {\n\
    bit<4>    version;\n\
    bit<4>    ihl;\n\
    bit<8>    diffserv;\n\
    bit<16>   totalLen;\n\
    bit<16>   identification;\n\
    bit<3>    flags;\n\
    bit<13>   fragOffset;\n\
    bit<8>    ttl;\n\
    bit<8>    protocol;\n\
    bit<16>   hdrChecksum;\n\
    ip4Addr_t srcAddr;\n\
    ip4Addr_t dstAddr;\n\
}\n\
\n\
struct metadata {\n\
    /* empty */\n\
}\n\
\n\
struct headers {\n\
    ethernet_t   ethernet;\n\
    ipv4_t       ipv4;\n\
}\n\
\n\
/*************************************************************************\n\
*********************** P A R S E R  ***********************************\n\
*************************************************************************/\n\
\n\
parser MyParser(packet_in packet,\n\
                out headers hdr,\n\
                inout metadata meta,\n\
                inout standard_metadata_t standard_metadata) {\n\
\n\
    state start {\n\
        transition parse_ethernet;\n\
    }\n\
\n\
    state parse_ethernet {\n\
        packet.extract(hdr.ethernet);\n\
        transition select(hdr.ethernet.etherType) {\n\
            TYPE_IPV4: parse_ipv4;\n\
            default: accept;\n\
        }\n\
    }\n\
\n\
    state parse_ipv4 {\n\
        packet.extract(hdr.ipv4);\n\
        transition accept;\n\
    }\n\
\n\
}\n\
\n\
/*************************************************************************\n\
************   C H E C K S U M    V E R I F I C A T I O N   *************\n\
*************************************************************************/\n\
\n\
control MyVerifyChecksum(inout headers hdr, inout metadata meta) {\n\
    apply {  }\n\
}\n\
\n\
\n\
/*************************************************************************\n\
**************  I N G R E S S   P R O C E S S I N G   *******************\n\
*************************************************************************/\n\
\n\
control MyIngress(inout headers hdr,\n\
                  inout metadata meta,\n\
                  inout standard_metadata_t standard_metadata) {\n\
    action drop() {\n\
        mark_to_drop();\n\
    }\n\
\n\
    action ipv4_forward(macAddr_t dstAddr, egressSpec_t port) {\n\
        standard_metadata.egress_spec = port;\n\
        hdr.ethernet.srcAddr = hdr.ethernet.dstAddr;\n\
        hdr.ethernet.dstAddr = dstAddr;\n\
        hdr.ipv4.ttl = hdr.ipv4.ttl - 1;\n\
    }\n\
\n\
    table ipv4_lpm {\n\
        key = {\n\
            hdr.ipv4.dstAddr: lpm;\n\
        }\n\
        actions = {\n\
            ipv4_forward;\n\
            drop;\n\
            NoAction;\n\
        }\n\
        size = 1024;\n\
        default_action = drop();\n\
    }\n\
\n\
    apply {\n\
        if (hdr.ipv4.isValid()) {\n\
            ipv4_lpm.apply();\n\
        }\n\
    }\n\
}\n\
\n\
/*************************************************************************\n\
****************  E G R E S S   P R O C E S S I N G   *******************\n\
*************************************************************************/\n\
\n\
control MyEgress(inout headers hdr,\n\
                 inout metadata meta,\n\
                 inout standard_metadata_t standard_metadata) {\n\
    apply {  }\n\
}\n\
\n\
/*************************************************************************\n\
*************   C H E C K S U M    C O M P U T A T I O N   **************\n\
*************************************************************************/\n\
\n\
control MyComputeChecksum(inout headers  hdr, inout metadata meta) {\n\
     apply {\n\
        update_checksum(\n\
            hdr.ipv4.isValid(),\n\
            { hdr.ipv4.version,\n\
              hdr.ipv4.ihl,\n\
              hdr.ipv4.diffserv,\n\
              hdr.ipv4.totalLen,\n\
              hdr.ipv4.identification,\n\
              hdr.ipv4.flags,\n\
              hdr.ipv4.fragOffset,\n\
              hdr.ipv4.ttl,\n\
              hdr.ipv4.protocol,\n\
              hdr.ipv4.srcAddr,\n\
              hdr.ipv4.dstAddr },\n\
            hdr.ipv4.hdrChecksum,\n\
            HashAlgorithm.csum16);\n\
    }\n\
}\n\
\n\
/*************************************************************************\n\
***********************  D E P A R S E R  *******************************\n\
*************************************************************************/\n\
\n\
control MyDeparser(packet_out packet, in headers hdr) {\n\
    apply {\n\
        packet.emit(hdr.ethernet);\n\
        packet.emit(hdr.ipv4);\n\
    }\n\
}\n\
\n\
/*************************************************************************\n\
***********************  S W I T C H  *******************************\n\
*************************************************************************/\n\
\n\
V1Switch(\n\
MyParser(),\n\
MyVerifyChecksum(),\n\
MyIngress(),\n\
MyEgress(),\n\
MyComputeChecksum(),\n\
MyDeparser()\n\
) main;\n\
";
