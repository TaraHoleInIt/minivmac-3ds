/*
	LTOVRUDP.h

	Copyright (C) 2012 Michael Fort, Paul C. Pratt, Rob Mitchelmore

	You can redistribute this file and/or modify it under the terms
	of version 2 of the GNU General Public License as published by
	the Free Software Foundation.  You should have received a copy
	of the license along with this file; see the file COPYING.

	This file is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	license for more details.
*/

/*
	LocalTalk OVeR User Datagram Protocol
*/


#if ! LT_MayHaveEcho
include <ifaddrs.h>
#endif


#define UDP_dolog (dbglog_HAVE && 0)

#ifndef use_winsock
#define use_winsock 0
#endif

#if use_winsock
#define my_INVALID_SOCKET INVALID_SOCKET
#define my_SOCKET SOCKET
#define my_closesocket closesocket
#define socklen_t int
#else
#define my_INVALID_SOCKET (-1)
#define my_SOCKET int
#define my_closesocket close
#endif

#if UDP_dolog
LOCALPROC dbglog_writeSockErr(char *s)
{
	dbglog_writeCStr(s);
	dbglog_writeCStr(": err ");
#if use_winsock
	dbglog_writeNum(WSAGetLastError());
#else
	dbglog_writeNum(errno);
	dbglog_writeCStr(" (");
	dbglog_writeCStr(strerror(errno));
	dbglog_writeCStr(")");
#endif
	dbglog_writeReturn();
}
#endif

/*
	Transmit buffer for localtalk data and its metadata
*/
LOCALVAR ui3b tx_buffer[4 + LT_TxBfMxSz] =
	"pppp";


/*
	Receive buffer for LocalTalk data and its metadata
*/
LOCALVAR unsigned int rx_buffer_allocation = 1800;

LOCALVAR my_SOCKET sock_fd = my_INVALID_SOCKET;
LOCALVAR blnr udp_ok = falseblnr;

#if use_winsock
LOCALVAR blnr have_winsock = falseblnr;
#endif

LOCALPROC start_udp(void)
{
#if use_winsock
	WSADATA wsaData;
#endif
	struct sockaddr_in addr;
	struct ip_mreq mreq;
	int one = 1;

#if use_winsock
	if (0 != WSAStartup(MAKEWORD(2, 2), &wsaData)) {
#if UDP_dolog
		dbglog_writeln("WSAStartup fails");
#endif
		return;
	}
	have_winsock = trueblnr;
#endif

	if (my_INVALID_SOCKET == (sock_fd =
		socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)))
	{
#if UDP_dolog
		dbglog_writeSockErr("socket");
#endif
		return;
	}

	if (0 != setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR,
		(const void *)&one, sizeof(one)))
	{
#if UDP_dolog
		dbglog_writeSockErr("setsockopt SO_REUSEADDR");
#endif
		return;
	}

#if use_SO_REUSEPORT
	if (0 != setsockopt(sock_fd, SOL_SOCKET, SO_REUSEPORT,
		(const void *)&one, sizeof(one)))
	{
#if UDP_dolog
		dbglog_writeSockErr("setsockopt SO_REUSEPORT");
#endif
		return;
	}
	/*
		https://stackoverflow.com/questions/14388706/
		how-do-so-reuseaddr-and-so-reuseport-differ
			claims that SO_REUSEPORT is the same as SO_REUSEADDR for
			multicast addresses.
	*/
#endif

	/* bind it to any address it fancies */
	memset((char*)&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(1954);

	/* bind it */
	errno = 0;
	if (0 != bind(sock_fd, (struct sockaddr*)&addr, sizeof(addr))) {
#if UDP_dolog
		dbglog_writeSockErr("bind");
#endif
		return;
	}

	/* whack it on a multicast group */
	mreq.imr_multiaddr.s_addr = inet_addr("239.192.76.84");
	mreq.imr_interface.s_addr = INADDR_ANY;

	if (0 != setsockopt(sock_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP,
		(const void *)&mreq, sizeof(mreq)))
	{
#if UDP_dolog
		dbglog_writeSockErr("setsockopt IP_ADD_MEMBERSHIP");
#endif
	}

	/* non-blocking I/O is good for the soul */
#if use_winsock
	{
		int iResult;
		u_long iMode = 1;

		iResult = ioctlsocket(sock_fd, FIONBIO, &iMode);
		if (iResult != NO_ERROR) {
			/*
				printf("ioctlsocket failed with error: %ld\n", iResult);
			*/
		}
	}
#else
	fcntl(sock_fd, F_SETFL, O_NONBLOCK);
#endif

	udp_ok = trueblnr;
}

LOCALVAR unsigned char *MyRxBuffer = NULL;
LOCALVAR struct sockaddr_in MyRxAddress;

/*
	External function needed at startup to initialize the LocalTalk
	functionality.
*/
LOCALFUNC blnr InitLocalTalk(void)
{
	LT_PickStampNodeHint();

	LT_TxBuffer = &tx_buffer[4];

	MyRxBuffer = malloc(rx_buffer_allocation);
	if (NULL == MyRxBuffer) {
		return falseblnr;
	}

	/* Set up UDP socket */
	start_udp();

	/* Initialized properly */
	return trueblnr;
}

LOCALPROC UnInitLocalTalk(void)
{
	if (my_INVALID_SOCKET != sock_fd) {
		if (0 != my_closesocket(sock_fd)) {
#if UDP_dolog
			dbglog_writeSockErr("my_closesocket sock_fd");
#endif
		}
	}

#if use_winsock
	if (have_winsock) {
		if (0 != WSACleanup()) {
#if UDP_dolog
			dbglog_writeSockErr("WSACleanup");
#endif
		}
	}
#endif

	if (NULL != MyRxBuffer) {
		free(MyRxBuffer);
	}
}

LOCALPROC embedMyPID(void)
{
	/*
		embeds my process ID in network byte order in the start of the
		Tx buffer we assume a pid is at most 32 bits.  As far as I know
		there's no actual implementation of POSIX with 64-bit PIDs so we
		should be ok.
	*/
	int i;

#if LT_MayHaveEcho
	ui5r v = LT_MyStamp;
#else
	ui5r v = (ui5r)getpid();
#endif

	for (i = 0; i < 4; i++) {
		tx_buffer[i] = (v >> (3 - i)*8) & 0xff;
	}
}

GLOBALOSGLUPROC LT_TransmitPacket(void)
{
	size_t bytes;
	/* Write the packet to UDP */
#if UDP_dolog
	dbglog_writeln("writing to udp");
#endif
	embedMyPID();
	if (udp_ok) {
		struct sockaddr_in dest;
		memset((char*)&dest, 0, sizeof(dest));
		dest.sin_family = AF_INET;
		dest.sin_addr.s_addr = inet_addr("239.192.76.84");
		dest.sin_port = htons(1954);

		bytes = sendto(sock_fd,
			(const void *)tx_buffer, LT_TxBuffSz + 4, 0,
			(struct sockaddr*)&dest, sizeof(dest));
#if UDP_dolog
		dbglog_writeCStr("sent ");
		dbglog_writeNum(bytes);
		dbglog_writeCStr(" bytes");
		dbglog_writeReturn();
#endif
		(void) bytes; /* avoid warning about unused */
	}
}

/*
	pidInPacketIsMine returns 1 if the process ID embedded in the packet
	is the same as the process ID of the current process
*/
LOCALFUNC int pidInPacketIsMine(void)
{
	/* is the PID in the packet my own PID? */
	int i;
	ui5r v;

#if LT_MayHaveEcho
	v = LT_MyStamp;
#else
	v = (ui5r)getpid();
#endif

	for (i = 0; i < 4; i++) {
		if (MyRxBuffer[i] != ((v >> (3 - i)*8) & 0xff)) {
			return 0;
		}
	}

	return 1;
}

/*
	ipInPacketIsMine returns 1 if the source IP for the just-received
	UDP packet is an IP address that is attached to an interface on this
	machine.
*/
#if ! LT_MayHaveEcho
LOCALFUNC int ipInPacketIsMine(void)
{
	if (MyRxAddress.sin_family != AF_INET) {
#if UDP_dolog
		dbglog_writeln(
			"baffling error: got a non-inet packet on an inet socket");
#endif
		return 1;
			/*
				because we should drop this garbled packet on the floor
			*/
	}
	in_addr_t raddr = MyRxAddress.sin_addr.s_addr;

	/*
		Now we need to iterate through all the interfaces on the machine
	*/
	struct ifaddrs *iflist, *ifptr;
	struct sockaddr_in *addr;

	int foundAddress = 0;

	getifaddrs(&iflist);
	for (ifptr = iflist; ifptr; ifptr = ifptr->ifa_next) {
		/* if there is no address in this slot, skip it and move on */
		if (! ifptr->ifa_addr) {
			continue;
		}

		/* if it's not an af_inet then we skip it */
		if (ifptr->ifa_addr->sa_family != AF_INET) {
			continue;
		}

		addr = (struct sockaddr_in*)ifptr->ifa_addr;

		if (addr->sin_addr.s_addr == raddr) {
			foundAddress = 1;
		}
	}
	freeifaddrs(iflist);

	return foundAddress;
}
#endif

/*
	packetIsOneISent returns 1 if this looks like a packet that this
	process sent and 0 if it looks like a packet that a different
	process sent.  This provides loopback protection so that we do not
	try to consume packets that we sent ourselves.  We do this by
	checking the process ID embedded in the packet and the IP address
	the packet was sent from.  It would be neater to just look at the
	LocalTalk node ID embedded in the LLAP packet, but this doesn't
	actually work, because during address acquisition it is entirely
	legitimate (and, in the case of collision, *required*) for another
	node to send a packet from what we think is our own node ID.
*/
#if ! LT_MayHaveEcho
LOCALFUNC int packetIsOneISent(void)
{
	/*
		do the PID comparison first because it's faster and most of the
		time will disambiguate for us
	*/
	if (pidInPacketIsMine()) {
		return ipInPacketIsMine();
	}
	return 0;
}
#endif

LOCALFUNC int GetNextPacket(void)
{
	unsigned char* device_buffer = MyRxBuffer;
	socklen_t addrlen = sizeof(MyRxAddress);

	errno = 0;
	int bytes = recvfrom(sock_fd, (void *)device_buffer,
		rx_buffer_allocation, 0,
		(struct sockaddr*)&MyRxAddress, &addrlen);
	if (bytes < 0) {
		if (errno !=
#if use_winsock
			WSAEWOULDBLOCK
#else
			EAGAIN
#endif
			)
		{
#if UDP_dolog
			dbglog_writeCStr("ret");
			dbglog_writeNum(bytes);
			dbglog_writeCStr(", bufsize ");
			dbglog_writeNum(rx_buffer_allocation);
			dbglog_writeCStr(", errno = ");
			dbglog_writeCStr(strerror(errno));
			dbglog_writeReturn();
#endif
		}
	} else {
#if UDP_dolog
		dbglog_writeCStr("got ");
		dbglog_writeNum(bytes);
		dbglog_writeCStr(", bufsize ");
		dbglog_writeNum(rx_buffer_allocation);
		dbglog_writeReturn();
#endif
	}
	return bytes;
}

GLOBALOSGLUPROC LT_ReceivePacket(void)
{
	int bytes;

#if ! LT_MayHaveEcho
label_retry:
#endif
	bytes = GetNextPacket();
	if (bytes > 0) {
#if LT_MayHaveEcho
		CertainlyNotMyPacket = ! pidInPacketIsMine();
#endif

#if ! LT_MayHaveEcho
		if (packetIsOneISent()) {
			goto label_retry;
		}
#endif

		{
#if UDP_dolog
			dbglog_writeCStr("passing ");
			dbglog_writeNum(bytes - 4);
			dbglog_writeCStr(" bytes to receiver");
			dbglog_writeReturn();
#endif
			LT_RxBuffer = MyRxBuffer + 4;
			LT_RxBuffSz = bytes - 4;
		}
	}
}
