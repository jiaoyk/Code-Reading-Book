# bootpef/Makefile
# $NetBSD: Makefile,v 1.6.2.1 1997/11/08 15:11:19 lukem Exp $

WARNS?=	0
PROG=	bootpef
SRCDIR= ${.CURDIR}/../bootpd
CPPFLAGS+= -DETC_ETHERS -DDEBUG -I${SRCDIR}
.PATH: ${SRCDIR}

SRCS=	bootpef.c dovend.c readfile.c hash.c dumptab.c \
	 lookup.c hwaddr.c report.c tzone.c

MAN=	bootpef.8

.include <bsd.prog.mk>

