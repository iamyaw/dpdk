/*-
 *   BSD LICENSE
 * 
 *   Copyright(c) 2010-2012 Intel Corporation. All rights reserved.
 *   All rights reserved.
 * 
 *   Redistribution and use in source and binary forms, with or without 
 *   modification, are permitted provided that the following conditions 
 *   are met:
 * 
 *     * Redistributions of source code must retain the above copyright 
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright 
 *       notice, this list of conditions and the following disclaimer in 
 *       the documentation and/or other materials provided with the 
 *       distribution.
 *     * Neither the name of Intel Corporation nor the names of its 
 *       contributors may be used to endorse or promote products derived 
 *       from this software without specific prior written permission.
 * 
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
 *   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
 *   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 *   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */

#include <errno.h>
#include <getopt.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <errno.h>

#include <sys/queue.h>
#include <sys/stat.h>

#include <stdint.h>
#include <unistd.h>
#include <inttypes.h>

#include <rte_common.h>
#include <rte_byteorder.h>
#include <rte_log.h>
#include <rte_debug.h>
#include <rte_cycles.h>
#include <rte_memory.h>
#include <rte_memzone.h>
#include <rte_launch.h>
#include <rte_tailq.h>
#include <rte_eal.h>
#include <rte_per_lcore.h>
#include <rte_lcore.h>
#include <rte_atomic.h>
#include <rte_branch_prediction.h>
#include <rte_ring.h>
#include <rte_mempool.h>
#include <rte_interrupts.h>
#include <rte_pci.h>
#include <rte_ether.h>
#include <rte_ethdev.h>
#include <rte_string_fns.h>
#include <cmdline_parse.h>
#include <cmdline_parse_etheraddr.h>

#include "testpmd.h"

static void
usage(char* progname)
{
	printf("usage: %s [--interactive|-i] [--help|-h] | ["
	       "--coremask=COREMASK --portmask=PORTMASK --numa "
	       "--eth-peers-configfile= | "
	       "--eth-peer=X,M:M:M:M:M:M | --nb-cores= | --nb-ports= | "
	       "--pkt-filter-mode= |"
	       "--rss-ip | --rss-udp | "
	       "--rxpt= | --rxht= | --rxwt= | --rxfreet= | "
	       "--txpt= | --txht= | --txwt= | --txfreet= | "
	       "--txrst= ]\n",
	       progname);
	printf("  --interactive: run in interactive mode\n");
	printf("  --help:   display this message and quit\n");
	printf("  --eth-peers-configfile=name of file with ethernet addresses "
	       "of peer ports\n");
	printf("  --eth-peer=X,M:M:M:M:M:M set the mac address of the X peer "
	       "port (0 <= X < %d)\n", RTE_MAX_ETHPORTS);
	printf("  --nb-cores=N set the number of forwarding cores"
	       " (1 <= N <= %d)\n", nb_lcores);
	printf("  --nb-ports=N set the number of forwarding ports"
	       " (1 <= N <= %d)\n", nb_ports);
	printf("  --coremask=COREMASK: hexadecimal bitmask of cores running "
	       "the packet forwarding test\n");
	printf("  --portmask=PORTMASK: hexadecimal bitmask of ports used "
	       "by the packet forwarding test\n");
	printf("  --numa: enable NUMA-aware allocation of RX/TX rings and of "
	       " RX memory buffers (mbufs)\n");
	printf("  --mbuf-size=N set the data size of mbuf to N bytes\n");
	printf("  --max-pkt-len=N set the maximum size of packet to N bytes\n");
	printf("  --pkt-filter-mode=N: set Flow director mode "
	       "( N: none (default mode) or signature or perfect)\n");
	printf("  --pkt-filter-report-hash=N: set Flow director report mode "
	       "( N: none  or match (default) or always)\n");
	printf("  --pkt-filter-size=N: set Flow director mode "
	       "( N: 64K (default mode) or 128K or 256K)\n");
	printf("  --pkt-filter-flexbytes-offset=N: set flexbytes-offset."
	       " The offset is defined in word units counted from the"
	       " first byte of the destination Ethernet MAC address."
	       " 0 <= N <= 32\n");
	printf("  --pkt-filter-drop-queue=N: set drop-queue."
	       " In perfect mode, when you add a rule with queue -1"
	       " the packet will be enqueued into the rx drop-queue."
	       " If the drop-queue doesn't exist, the packet is dropped."
	       " By default drop-queue=127\n");
	printf("  --crc-strip: enable CRC stripping by hardware\n");
	printf("  --enable-rx-cksum: enable rx hardware checksum offload\n");
	printf("  --disable-hw-vlan: disable hardware vlan\n");
	printf("  --disable-rss: disable rss\n");
	printf("  --port-topology=N: set port topology (N: paired (default) or "
	       "chained)\n");
	printf("  --rss-ip:  set RSS functions to IPv4/IPv6 only \n");
	printf("  --rss-udp: set RSS functions to IPv4/IPv6 + UDP\n");
	printf("  --rxq=N    set the number of RX queues per port to N\n");
	printf("  --rxd=N    set the number of descriptors in RX rings to N\n");
	printf("  --txq=N    set the number of TX queues per port to N\n");
	printf("  --txd=N    set the number of descriptors in TX rings to N\n");
	printf("  --burst=N  set the number of packets per burst to N\n");
	printf("  --mbcache=N  set the cache of mbuf memory pool to N\n");
	printf("  --rxpt=N   set prefetch threshold register of RX rings to N"
	       " (0 <= N <= 16)\n");
	printf("  --rxht=N  set the host threshold register of RX rings to N"
	       " (0 <= N <= 16)\n");
	printf("  --rxfreet=N set the free threshold of RX descriptors to N"
	       " (0 <= N < value of rxd)\n");
	printf("  --rxwt=N  set the write-back threshold register of RX rings"
	       " to N (0 <= N <= 16)\n");
	printf("  --txpt=N  set the prefetch threshold register of TX rings"
	       " to N (0 <= N <= 16)\n");
	printf("  --txht=N  set the nhost threshold register of TX rings to N"
	       " (0 <= N <= 16)\n");
	printf("  --txwt=N  set the write-back threshold register of TX rings"
	       " to N (0 <= N <= 16)\n");
	printf("  --txfreet=N set the transmit free threshold of TX rings to N"
	       " (0 <= N <= value of txd)\n");
	printf("  --txrst=N set the transmit RS bit threshold of TX rings to N"
	       " (0 <= N <= value of txd)\n");
}

static int
init_peer_eth_addrs(char *config_filename)
{
	FILE *config_file;
	portid_t i;
	char buf[50];

	config_file = fopen(config_filename, "r");
	if (config_file == NULL) {
		perror("open log file failed\n");
		return -1;
	}

	for (i = 0; i < RTE_MAX_ETHPORTS; i++) {

		if (fgets(buf, sizeof(buf), config_file) == NULL)
			break;

		if (cmdline_parse_etheraddr(NULL, buf, &peer_eth_addrs[i]) < 0 ){
			printf("bad format of mac address on line %d\n", i);
			fclose(config_file);
			return -1;
		}
	}
	fclose(config_file);
	nb_peer_eth_addrs = (portid_t) i;
	return 0;
}

/*
 * Parse the coremask given as argument (hexadecimal string) and set
 * the global configuration of forwarding cores.
 */
static void
parse_fwd_coremask(const char *coremask)
{
	char *end;
	unsigned long long int cm;

	/* parse hexadecimal string */
	end = NULL;
	cm = strtoull(coremask, &end, 16);
	if ((coremask[0] == '\0') || (end == NULL) || (*end != '\0'))
		rte_exit(EXIT_FAILURE, "Invalid fwd core mask\n");
	else
		set_fwd_lcores_mask((uint64_t) cm);
}

/*
 * Parse the coremask given as argument (hexadecimal string) and set
 * the global configuration of forwarding cores.
 */
static void
parse_fwd_portmask(const char *portmask)
{
	char *end;
	unsigned long long int pm;

	/* parse hexadecimal string */
	end = NULL;
	pm = strtoull(portmask, &end, 16);
	if ((portmask[0] == '\0') || (end == NULL) || (*end != '\0'))
		rte_exit(EXIT_FAILURE, "Invalid fwd port mask\n");
	else
		set_fwd_ports_mask((uint64_t) pm);
}

void
launch_args_parse(int argc, char** argv)
{
	int n, opt;
	char **argvopt;
	int opt_idx;
	static struct option lgopts[] = {
		{ "help",			0, 0, 0 },
		{ "interactive",		0, 0, 0 },
		{ "eth-peers-configfile",	1, 0, 0 },
		{ "eth-peer",			1, 0, 0 },
		{ "ports",			1, 0, 0 },
		{ "nb-cores",			1, 0, 0 },
		{ "nb-ports",			1, 0, 0 },
		{ "coremask",			1, 0, 0 },
		{ "portmask",			1, 0, 0 },
		{ "numa",			0, 0, 0 },
		{ "mbuf-size",			1, 0, 0 },
		{ "max-pkt-len",		1, 0, 0 },
		{ "pkt-filter-mode",            1, 0, 0 },
		{ "pkt-filter-report-hash",     1, 0, 0 },
		{ "pkt-filter-size",            1, 0, 0 },
		{ "pkt-filter-flexbytes-offset",1, 0, 0 },
		{ "pkt-filter-drop-queue",      1, 0, 0 },
		{ "crc-strip",                  0, 0, 0 },
		{ "disable-hw-vlan",            0, 0, 0 },
		{ "disable-rss",                0, 0, 0 },
		{ "port-topology",              1, 0, 0 },
		{ "rss-ip",			0, 0, 0 },
		{ "rss-udp",			0, 0, 0 },
		{ "rxq",			1, 0, 0 },
		{ "txq",			1, 0, 0 },
		{ "rxd",			1, 0, 0 },
		{ "txd",			1, 0, 0 },
		{ "burst",			1, 0, 0 },
		{ "mbcache",			1, 0, 0 },
		{ "txpt",			1, 0, 0 },
		{ "txht",			1, 0, 0 },
		{ "txwt",			1, 0, 0 },
		{ "txfreet",			1, 0, 0 },
		{ "txrst",			1, 0, 0 },
		{ "rxpt",			1, 0, 0 },
		{ "rxht",			1, 0, 0 },
		{ "rxwt",			1, 0, 0 },
		{ "rxfreet",                    1, 0, 0 },
		{ 0, 0, 0, 0 },
	};

	argvopt = argv;

	while ((opt = getopt_long(argc, argvopt, "ih",
				 lgopts, &opt_idx)) != EOF) {
		switch (opt) {
		case 'i':
			printf("Interactive-mode selected\n");
			interactive = 1;
			break;
		case 0: /*long options */
			if (!strcmp(lgopts[opt_idx].name, "help")) {
				usage(argv[0]);
				rte_exit(EXIT_SUCCESS, "Displayed help\n");
			}
			if (!strcmp(lgopts[opt_idx].name, "interactive")) {
				printf("Interactive-mode selected\n");
				interactive = 1;
			}
			if (!strcmp(lgopts[opt_idx].name,
				    "eth-peers-configfile")) {
				if (init_peer_eth_addrs(optarg) != 0)
					rte_exit(EXIT_FAILURE,
						 "Cannot open logfile\n");
			}
			if (!strcmp(lgopts[opt_idx].name, "eth-peer")) {
				char *port_end;
				uint8_t c, peer_addr[6];

				errno = 0;
				n = strtoul(optarg, &port_end, 10);
				if (errno != 0 || port_end == optarg || *port_end++ != ',')
					rte_exit(EXIT_FAILURE,
						 "Invalid eth-peer: %s", optarg);
				if (n >= RTE_MAX_ETHPORTS)
					rte_exit(EXIT_FAILURE,
						 "eth-peer: port %d >= RTE_MAX_ETHPORTS(%d)\n",
						 n, RTE_MAX_ETHPORTS);

				if (cmdline_parse_etheraddr(NULL, port_end, &peer_addr) < 0 )
					rte_exit(EXIT_FAILURE,
						 "Invalid ethernet address: %s\n",
						 port_end);
				for (c = 0; c < 6; c++)
					peer_eth_addrs[n].addr_bytes[c] =
						peer_addr[c];
				nb_peer_eth_addrs++;
			}
			if (!strcmp(lgopts[opt_idx].name, "nb-ports")) {
				n = atoi(optarg);
				if (n > 0 && n <= nb_ports)
					nb_fwd_ports = (uint8_t) n;
				else
					rte_exit(EXIT_FAILURE,
						 "nb-ports should be > 0 and <= %d\n",
						 nb_ports);
			}
			if (!strcmp(lgopts[opt_idx].name, "nb-cores")) {
				n = atoi(optarg);
				if (n > 0 && n <= nb_lcores)
					nb_fwd_lcores = (uint8_t) n;
				else
					rte_exit(EXIT_FAILURE,
						 "nb-cores should be > 0 and <= %d\n",
						 nb_lcores);
			}
			if (!strcmp(lgopts[opt_idx].name, "coremask"))
				parse_fwd_coremask(optarg);
			if (!strcmp(lgopts[opt_idx].name, "portmask"))
				parse_fwd_portmask(optarg);
			if (!strcmp(lgopts[opt_idx].name, "numa"))
				numa_support = 1;
			if (!strcmp(lgopts[opt_idx].name, "mbuf-size")) {
				n = atoi(optarg);
				if (n > 0 && n <= 0xFFFF)
					mbuf_data_size = (uint16_t) n;
				else
					rte_exit(EXIT_FAILURE,
						 "mbuf-size should be > 0 and < 65536\n");
			}
			if (!strcmp(lgopts[opt_idx].name, "max-pkt-len")) {
				n = atoi(optarg);
				if (n >= ETHER_MIN_LEN) {
					rx_mode.max_rx_pkt_len = (uint32_t) n;
					if (n > ETHER_MAX_LEN)
					    rx_mode.jumbo_frame = 1;
				} else
					rte_exit(EXIT_FAILURE,
						 "Invalid max-pkt-len=%d - should be > %d\n",
						 n, ETHER_MIN_LEN);
			}
			if (!strcmp(lgopts[opt_idx].name, "pkt-filter-mode")) {
				if (!strcmp(optarg, "signature"))
					fdir_conf.mode =
						RTE_FDIR_MODE_SIGNATURE;
				else if (!strcmp(optarg, "perfect"))
					fdir_conf.mode = RTE_FDIR_MODE_PERFECT;
				else if (!strcmp(optarg, "none"))
					fdir_conf.mode = RTE_FDIR_MODE_NONE;
				else
					rte_exit(EXIT_FAILURE,
						 "pkt-mode-invalid %s invalid - must be: "
						 "none, signature or perfect\n",
						 optarg);
			}
			if (!strcmp(lgopts[opt_idx].name,
				    "pkt-filter-report-hash")) {
				if (!strcmp(optarg, "none"))
					fdir_conf.status =
						RTE_FDIR_NO_REPORT_STATUS;
				else if (!strcmp(optarg, "match"))
					fdir_conf.status =
						RTE_FDIR_REPORT_STATUS;
				else if (!strcmp(optarg, "always"))
					fdir_conf.status =
						RTE_FDIR_REPORT_STATUS_ALWAYS;
				else
					rte_exit(EXIT_FAILURE,
						 "pkt-filter-report-hash %s invalid "
						 "- must be: none or match or always\n",
						 optarg);
			}
			if (!strcmp(lgopts[opt_idx].name, "pkt-filter-size")) {
				if (!strcmp(optarg, "64K"))
					fdir_conf.pballoc =
						RTE_FDIR_PBALLOC_64K;
				else if (!strcmp(optarg, "128K"))
					fdir_conf.pballoc =
						RTE_FDIR_PBALLOC_128K;
				else if (!strcmp(optarg, "256K"))
					fdir_conf.pballoc =
						RTE_FDIR_PBALLOC_256K;
				else
					rte_exit(EXIT_FAILURE, "pkt-filter-size %s invalid -"
						 " must be: 64K or 128K or 256K\n",
						 optarg);
			}
			if (!strcmp(lgopts[opt_idx].name,
				    "pkt-filter-flexbytes-offset")) {
				n = atoi(optarg);
				if ( n >= 0 && n <= (int) 32)
					fdir_conf.flexbytes_offset =
						(uint8_t) n;
				else
					rte_exit(EXIT_FAILURE,
						 "flexbytes %d invalid - must"
						 "be  >= 0 && <= 32\n", n);
			}
			if (!strcmp(lgopts[opt_idx].name,
				    "pkt-filter-drop-queue")) {
				n = atoi(optarg);
				if (n >= 0)
					fdir_conf.drop_queue = (uint8_t) n;
				else
					rte_exit(EXIT_FAILURE,
						 "drop queue %d invalid - must"
						 "be >= 0 \n", n);
			}
			if (!strcmp(lgopts[opt_idx].name, "crc-strip"))
				rx_mode.hw_strip_crc = 1;
			if (!strcmp(lgopts[opt_idx].name, "enable-rx-cksum"))
				rx_mode.hw_ip_checksum = 1;
			if (!strcmp(lgopts[opt_idx].name, "disable-hw-vlan"))
				rx_mode.hw_vlan_filter = 0;
			if (!strcmp(lgopts[opt_idx].name, "disable-rss"))
				rss_hf = 0;
			if (!strcmp(lgopts[opt_idx].name, "port-topology")) {
				if (!strcmp(optarg, "paired"))
					port_topology = PORT_TOPOLOGY_PAIRED;
				else if (!strcmp(optarg, "chained"))
					port_topology = PORT_TOPOLOGY_CHAINED;
				else
					rte_exit(EXIT_FAILURE, "port-topology %s invalid -"
						 " must be: paired or chained \n",
						 optarg);
			}
			if (!strcmp(lgopts[opt_idx].name, "rss-ip"))
				rss_hf = ETH_RSS_IPV4 | ETH_RSS_IPV6;
			if (!strcmp(lgopts[opt_idx].name, "rss-udp"))
				rss_hf = ETH_RSS_IPV4 | ETH_RSS_IPV6 |
					ETH_RSS_IPV4_UDP;
			if (!strcmp(lgopts[opt_idx].name, "rxq")) {
				n = atoi(optarg);
				if (n >= 1 && n <= (int) MAX_QUEUE_ID)
					nb_rxq = (queueid_t) n;
				else
					rte_exit(EXIT_FAILURE, "rxq %d invalid - must be"
						  " >= 1 && <= %d\n", n,
						  (int) MAX_QUEUE_ID);
			}
			if (!strcmp(lgopts[opt_idx].name, "txq")) {
				n = atoi(optarg);
				if (n >= 1 && n <= (int) MAX_QUEUE_ID)
					nb_txq = (queueid_t) n;
				else
					rte_exit(EXIT_FAILURE, "txq %d invalid - must be"
						  " >= 1 && <= %d\n", n,
						  (int) MAX_QUEUE_ID);
			}
			if (!strcmp(lgopts[opt_idx].name, "rxd")) {
				n = atoi(optarg);
				if (n > 0)
					nb_rxd = (uint16_t) n;
				else
					rte_exit(EXIT_FAILURE, "rxd must be > 0\n");
			}
			if (!strcmp(lgopts[opt_idx].name, "txd")) {
				n = atoi(optarg);
				if (n > 0)
					nb_txd = (uint16_t) n;
				else
					rte_exit(EXIT_FAILURE, "txd must be in > 0\n");
			}
			if (!strcmp(lgopts[opt_idx].name, "burst")) {
				n = atoi(optarg);
				if ((n >= 1) && (n <= MAX_PKT_BURST))
					nb_pkt_per_burst = (uint16_t) n;
				else
					rte_exit(EXIT_FAILURE,
						 "burst must >= 1 and <= %d]",
						 MAX_PKT_BURST);
			}
			if (!strcmp(lgopts[opt_idx].name, "mbcache")) {
				n = atoi(optarg);
				if ((n >= 0) &&
				    (n <= RTE_MEMPOOL_CACHE_MAX_SIZE))
					mb_mempool_cache = (uint16_t) n;
				else
					rte_exit(EXIT_FAILURE,
						 "mbcache must be >= 0 and <= %d\n",
						 RTE_MEMPOOL_CACHE_MAX_SIZE);
			}
			if (!strcmp(lgopts[opt_idx].name, "txpt")) {
				n = atoi(optarg);
				if (n >= 0)
					tx_thresh.pthresh = (uint8_t)n;
				else
					rte_exit(EXIT_FAILURE, "txpt must be >= 0\n");
			}
			if (!strcmp(lgopts[opt_idx].name, "txht")) {
				n = atoi(optarg);
				if (n >= 0)
					tx_thresh.hthresh = (uint8_t)n;
				else
					rte_exit(EXIT_FAILURE, "txht must be >= 0\n");
			}
			if (!strcmp(lgopts[opt_idx].name, "txwt")) {
				n = atoi(optarg);
				if (n >= 0)
					tx_thresh.wthresh = (uint8_t)n;
				else
					rte_exit(EXIT_FAILURE, "txwt must be >= 0\n");
			}
			if (!strcmp(lgopts[opt_idx].name, "txfreet")) {
				n = atoi(optarg);
				if (n >= 0)
					tx_free_thresh = (uint16_t)n;
				else
					rte_exit(EXIT_FAILURE, "txfreet must be >= 0\n");
			}
			if (!strcmp(lgopts[opt_idx].name, "txrst")) {
				n = atoi(optarg);
				if (n >= 0)
					tx_rs_thresh = (uint16_t)n;
				else
					rte_exit(EXIT_FAILURE, "txrst must be >= 0\n");
			}
			if (!strcmp(lgopts[opt_idx].name, "rxpt")) {
				n = atoi(optarg);
				if (n >= 0)
					rx_thresh.pthresh = (uint8_t)n;
				else
					rte_exit(EXIT_FAILURE, "rxpt must be >= 0\n");
			}
			if (!strcmp(lgopts[opt_idx].name, "rxht")) {
				n = atoi(optarg);
				if (n >= 0)
					rx_thresh.hthresh = (uint8_t)n;
				else
					rte_exit(EXIT_FAILURE, "rxht must be >= 0\n");
			}
			if (!strcmp(lgopts[opt_idx].name, "rxwt")) {
				n = atoi(optarg);
				if (n >= 0)
					rx_thresh.wthresh = (uint8_t)n;
				else
					rte_exit(EXIT_FAILURE, "rxwt must be >= 0\n");
			}
			if (!strcmp(lgopts[opt_idx].name, "rxd")) {
				n = atoi(optarg);
				if (n > 0) {
					if (rx_free_thresh >= n)
						rte_exit(EXIT_FAILURE,
							 "rxd must be > "
							 "rx_free_thresh(%d)\n",
							 (int)rx_free_thresh);
					else
						nb_rxd = (uint16_t) n;
				} else
					rte_exit(EXIT_FAILURE,
						 "rxd(%d) invalid - must be > 0\n",
						 n);
			}
			if (!strcmp(lgopts[opt_idx].name, "txd")) {
				n = atoi(optarg);
				if (n > 0)
					nb_txd = (uint16_t) n;
				else
					rte_exit(EXIT_FAILURE, "txd must be in > 0\n");
			}
			if (!strcmp(lgopts[opt_idx].name, "txpt")) {
				n = atoi(optarg);
				if (n >= 0)
					tx_thresh.pthresh = (uint8_t)n;
				else
					rte_exit(EXIT_FAILURE, "txpt must be >= 0\n");
			}
			if (!strcmp(lgopts[opt_idx].name, "txht")) {
				n = atoi(optarg);
				if (n >= 0)
					tx_thresh.hthresh = (uint8_t)n;
				else
					rte_exit(EXIT_FAILURE, "txht must be >= 0\n");
			}
			if (!strcmp(lgopts[opt_idx].name, "txwt")) {
				n = atoi(optarg);
				if (n >= 0)
					tx_thresh.wthresh = (uint8_t)n;
				else
					rte_exit(EXIT_FAILURE, "txwt must be >= 0\n");
			}
			if (!strcmp(lgopts[opt_idx].name, "rxpt")) {
				n = atoi(optarg);
				if (n >= 0)
					rx_thresh.pthresh = (uint8_t)n;
				else
					rte_exit(EXIT_FAILURE, "rxpt must be >= 0\n");
			}
			if (!strcmp(lgopts[opt_idx].name, "rxht")) {
				n = atoi(optarg);
				if (n >= 0)
					rx_thresh.hthresh = (uint8_t)n;
				else
					rte_exit(EXIT_FAILURE, "rxht must be >= 0\n");
			}
			if (!strcmp(lgopts[opt_idx].name, "rxwt")) {
				n = atoi(optarg);
				if (n >= 0)
					rx_thresh.wthresh = (uint8_t)n;
				else
					rte_exit(EXIT_FAILURE, "rxwt must be >= 0\n");
			}
			if (!strcmp(lgopts[opt_idx].name, "rxfreet")) {
				n = atoi(optarg);
				if (n >= 0)
					rx_free_thresh = (uint16_t)n;
				else
					rte_exit(EXIT_FAILURE, "rxfreet must be >= 0\n");
			}
			break;
		case 'h':
			usage(argv[0]);
			rte_exit(EXIT_SUCCESS, "Displayed help\n");
			break;
		default:
			usage(argv[0]);
			rte_exit(EXIT_FAILURE,
				 "Command line is incomplete or incorrect\n");
			break;
		}
	}
}
