#	$NetBSD: Makefile,v 1.2 1997/04/17 07:46:24 thorpej Exp $

S=	${.CURDIR}/../../../..

PROG=		ofwboot
SRCS=		Locore.c boot.c ofdev.c net.c netif_of.c alloc.c
.PATH:		${S}/arch/powerpc/powerpc
SRCS+=		ofwmagic.S
#CFLAGS+=	-DDEBUG -DNETIF_DEBUG
NOMAN=
STRIPFLAG=
BINMODE=	444
OBJCOPY?=	objcopy

NEWVERSWHAT=	"OpenFirmware Boot"

# For now...
RELOC=		20000

ENTRY=		_start

CLEANFILES+=	vers.c vers.o

CPPFLAGS+=	-I${.CURDIR}/../../.. -I${.CURDIR}/../../../..
CPPFLAGS+=	-DRELOC=0x${RELOC}
CPPFLAGS+=	-DFIRMWORKSBUGS
CPPFLAGS+=	-DPOWERPC_BOOT_AOUT
CPPFLAGS+=	-DPOWERPC_BOOT_ELF
CPPFLAGS+=	-DXCOFF_GLUE		# for booting PCI Powermacs

### find out what to use for libkern
KERN_AS=	library
.include "${S}/lib/libkern/Makefile.inc"
LIBKERN=	${KERNLIB}

### find out what to use for libz
Z_AS=		library
.include "${S}/lib/libz/Makefile.inc"
LIBZ=		${ZLIB}

### find out what to use for libsa
SA_AS=		library
SAMISCMAKEFLAGS= SA_USE_CREAD=yes
.include "${S}/lib/libsa/Makefile.inc"
LIBSA=		${SALIB}

${PROG}: ${OBJS} ${LIBSA} ${LIBZ} ${LIBKERN}
	sh ${.CURDIR}/../newvers.sh ${.CURDIR}/version ${NEWVERSWHAT}
	${COMPILE.c} vers.c
	${LD} -X -Ttext ${RELOC} -e ${ENTRY} -o ${PROG} \
	    ${OBJS} vers.o ${LIBSA} ${LIBZ} ${LIBKERN}
	mv ${PROG} ${PROG}.elf
	${OBJCOPY} --input-target=elf32-powerpc \
	    --output-target=xcoff-powermac ${PROG}.elf ${PROG}.xcf

.include <bsd.prog.mk>
