/*	$NetBSD: ofisa.c,v 1.1 1998/02/07 00:46:52 cgd Exp $	*/

/*
 * Copyright 1997, 1998
 * Digital Equipment Corporation. All rights reserved.
 *
 * This software is furnished under license and may be used and
 * copied only in accordance with the following terms and conditions.
 * Subject to these conditions, you may download, copy, install,
 * use, modify and distribute this software in source and/or binary
 * form. No title or ownership is transferred hereby.
 *
 * 1) Any source code used, modified or distributed must reproduce
 *    and retain this copyright notice and list of conditions as
 *    they appear in the source file.
 *
 * 2) No right is granted to use any trade name, trademark, or logo of
 *    Digital Equipment Corporation. Neither the "Digital Equipment
 *    Corporation" name nor any trademark or logo of Digital Equipment
 *    Corporation may be used to endorse or promote products derived
 *    from this software without the prior written permission of
 *    Digital Equipment Corporation.
 *
 * 3) This software is provided "AS-IS" and any express or implied
 *    warranties, including but not limited to, any implied warranties
 *    of merchantability, fitness for a particular purpose, or
 *    non-infringement are disclaimed. In no event shall DIGITAL be
 *    liable for any damages whatsoever, and in particular, DIGITAL
 *    shall not be liable for special, indirect, consequential, or
 *    incidental damages or damages for lost profits, loss of
 *    revenue or loss of use, whether such damages arise in contract,
 *    negligence, tort, under statute, in equity, at law or otherwise,
 *    even if advised of the possibility of such damage.
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/device.h>
#include <sys/malloc.h>
#include <machine/bus.h>
#include <machine/intr.h>

#include <dev/ofw/openfirm.h>
#include <dev/isa/isavar.h>
#include <dev/ofisa/ofisavar.h>

#define	OFW_MAX_STACK_BUF_SIZE	256

static int	ofisamatch __P((struct device *, struct cfdata *, void *));
static void	ofisaattach __P((struct device *, struct device *, void *));

struct cfattach ofisa_ca = {
	sizeof(struct device), ofisamatch, ofisaattach
};

struct cfdriver ofisa_cd = {
	NULL, "ofisa", DV_DULL
};

static int	ofisaprint __P((void *, const char *));

static int
ofisaprint(aux, pnp)
	void *aux;
	const char *pnp;
{
	struct ofprobe *ofp = aux;
	char name[64];

	(void)of_packagename(ofp->phandle, name, sizeof name);
	if (pnp)
		printf("%s at %s", name, pnp);
	else
		printf(" (%s)", name);
	return UNCONF;
}

int
ofisamatch(parent, cf, aux)
	struct device *parent;
	struct cfdata *cf;
	void *aux;
{
	struct ofprobe *ofp = aux;
	const char *compatible_strings[] = { "pnpPNP,a00", NULL };
	int rv = 0;

	if (of_compatible(ofp->phandle, compatible_strings) != -1)
		rv = 5;

#ifdef _OFISA_MD_MATCH
	if (!rv)
		rv = ofisa_md_match(parent, cf, aux);
#endif

	return (rv);
}

void
ofisaattach(parent, dev, aux)
	struct device *parent, *dev;
	void *aux;
{

	struct ofprobe *ofp = aux;
	struct isabus_attach_args iba;
	struct ofisa_attach_args aa;
	int child;

	if (ofisa_get_isabus_data(ofp->phandle, &iba) < 0) {
		printf(": couldn't get essential bus data\n");
		return;
	}

	printf("\n");

	for (child = OF_child(ofp->phandle); child;
	    child = OF_peer(child)) {
		if (ofisa_ignore_child(ofp->phandle, child))
			continue;

		bzero(&aa, sizeof aa);

		aa.ofp.phandle = child;
		aa.iot = iba.iba_iot;
		aa.memt = iba.iba_memt;
		aa.dmat = iba.iba_dmat;
		aa.ic = iba.iba_ic;

		config_found(dev, &aa, ofisaprint);
	}
}

int
ofisa_reg_count(phandle)
	int phandle;
{
	int len;

	len = OF_getproplen(phandle, "reg");

	/* nonexistant or obviously malformed "reg" property */
	if (len < 0 || (len % 12) != 0)
		return (-1);
	return (len / 12);
}

int
ofisa_reg_get(phandle, descp, ndescs)
	int phandle;
	struct ofisa_reg_desc *descp;
	int ndescs;
{
	char *buf, *bp;
	int i, allocated, rv;

	i = ofisa_reg_count(phandle);
	if (i < 0)
		return (-1);
	ndescs = min(ndescs, i);

	i = ndescs * 12;
	if (i > OFW_MAX_STACK_BUF_SIZE) {
		buf = malloc(i, M_TEMP, M_WAITOK);
		allocated = 1;
	} else {
		buf = alloca(i);
		allocated = 0;
	}

	if (OF_getprop(phandle, "reg", buf, i) != i) {
		rv = -1;
		goto out;
	}

	for (i = 0, bp = buf; i < ndescs; i++, bp += 12) {
		if (of_decode_int(&bp[0]) & 1)
			descp[i].type = OFISA_REG_TYPE_IO;
		else
			descp[i].type = OFISA_REG_TYPE_MEM;
		descp[i].addr = of_decode_int(&bp[4]);
		descp[i].len = of_decode_int(&bp[8]);
	}
	rv = i;		/* number of descriptors processed (== ndescs) */

out:
	if (allocated)
		free(buf, M_TEMP);
	return (rv);
}

void
ofisa_reg_print(descp, ndescs)
	struct ofisa_reg_desc *descp;
	int ndescs;
{
	int i;

	if (ndescs == 0) {
		printf("none");
		return;
	}

	for (i = 0; i < ndescs; i++) {
		printf("%s%s 0x%lx/%ld", i ? ", " : "",
		    descp[i].type == OFISA_REG_TYPE_IO ? "io" : "mem",
		    (long)descp[i].addr, (long)descp[i].len);
	}
}

int
ofisa_intr_count(phandle)
	int phandle;
{
	int len;

	len = OF_getproplen(phandle, "interrupts");

	/* nonexistant or obviously malformed "reg" property */
	if (len < 0 || (len % 8) != 0)
		return (-1);
	return (len / 8);
}

int
ofisa_intr_get(phandle, descp, ndescs)
	int phandle;
	struct ofisa_intr_desc *descp;
	int ndescs;
{
	char *buf, *bp;
	int i, allocated, rv;

	i = ofisa_intr_count(phandle);
	if (i < 0)
		return (-1);
	ndescs = min(ndescs, i);

	i = ndescs * 8;
	if (i > OFW_MAX_STACK_BUF_SIZE) {
		buf = malloc(i, M_TEMP, M_WAITOK);
		allocated = 1;
	} else {
		buf = alloca(i);
		allocated = 0;
	}

	if (OF_getprop(phandle, "interrupts", buf, i) != i) {
		rv = -1;
		goto out;
	}

	for (i = 0, bp = buf; i < ndescs; i++, bp += 8) {
		descp[i].irq = of_decode_int(&bp[0]);
		switch (of_decode_int(&bp[4])) {
		case 0:
		case 1:
			descp[i].share = IST_LEVEL;
			break;
		case 2:
		case 3:
			descp[i].share = IST_EDGE;
			break;
#ifdef DIAGNOSTIC
		default:
			/* Dunno what to do, so fail. */
			printf("ofisa_intr_get: unknown intrerrupt type %d\n",
			    of_decode_int(&bp[4]));
			rv = -1;
			goto out;
#endif
		}
	}
	rv = i;		/* number of descriptors processed (== ndescs) */

out:
	if (allocated)
		free(buf, M_TEMP);
	return (rv);
}

void
ofisa_intr_print(descp, ndescs)
	struct ofisa_intr_desc *descp;
	int ndescs;
{
	int i;

	if (ndescs == 0) {
		printf("none");
		return;
	}

	for (i = 0; i < ndescs; i++) {
		printf("%s%d (%s)", i ? ", " : "", descp[i].irq,
		    descp[i].share == IST_LEVEL ? "level" : "edge");
	}
}
