/*	$NetBSD: md.h,v 1.1.2.2 1997/11/25 06:58:04 thorpej Exp $	*/

/*
 * Copyright 1997 Piermont Information Systems Inc.
 * All rights reserved.
 *
 * Based on code written by Philip A. Nelson for Piermont Information
 * Systems Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software develooped for the NetBSD Project by
 *      Piermont Information Systems Inc.
 * 4. The name of Piermont Information Systems Inc. may not be used to endorse
 *    or promote products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY PIERMONT INFORMATION SYSTEMS INC. ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL PIERMONT INFORMATION SYSTEMS INC. BE 
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
 * THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/* md.h -- Machine specific definitions for the arm32 */

/* Constants and defines */

/* Megs required for a full X installation. */
#define XNEEDMB 50

/* Disk names. */
EXTERN	char *disk_names[]
#ifdef MAIN
= {"wd", "sd", NULL}
#endif
;

/* Legal start character for a disk for checking input. */
#define ISDISKSTART(dn)	(dn == 'w' || dn == 's')

/* Definition of files to retreive from ftp. */
EXTERN distinfo dist_list[]
#ifdef MAIN
= {
    {"kern",	1, NULL, "Kernel       : "},
    {"base",	1, NULL, "Base         : "},
    {"etc",	1, NULL, "System (/etc): "},
    {"comp",	1, NULL, "Compiler     : "},
    {"games",	1, NULL, "Games        : "},
    {"man",	1, NULL, "Manuals      : "},
    {"misc",	1, NULL, "Miscellaneous: "},
    {"text",	1, NULL, "Text tools   : "},
#if 0
    {"xbase",	1, NULL, "X11 clients  : "},
    {"xfont",	1, NULL, "X11 fonts    : "},
    {"xfont",	1, NULL, "X11 servers  : "},
    {"xcontrib",1, NULL, "X11 contrib  : "},
    {"xcomp",	1, NULL, "X programming: "},
#endif
    {NULL, 0, NULL, NULL }
}
#endif
;

EXTERN char *fdtype INIT("msdos");
