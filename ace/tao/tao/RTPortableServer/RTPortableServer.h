/* -*- C++ -*- */

// RTPortableServer.h,v 1.2 2001/09/18 00:05:54 irfan Exp

// =========================================================================
//
// = LIBRARY
//    TAO
//
// = FILENAME
//    RTPortableserver.h
//
// = AUTHOR
//    Frank Hunleth <fhunleth@cs.wustl.edu>
//
// =========================================================================

#ifndef TAO_RT_PORTABLESERVER_H
#define TAO_RT_PORTABLESERVER_H
#include "ace/pre.h"

#include "rtportableserver_export.h"
#include "tao/corbafwd.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#include "tao/RTCORBA/RTCORBA.h"

class TAO_RTPortableServer_Export TAO_RTPortableServer_Initializer
{
public:
  // Constructor.
  TAO_RTPortableServer_Initializer (void);
};

static TAO_RTPortableServer_Initializer TAO_RTPortableServer_initializer;

#define TAO_RT_PORTABLESERVER_SAFE_INCLUDE
#include "RTPortableServerC.h"
#undef TAO_RT_PORTABLESERVER_SAFE_INCLUDE

#include "ace/post.h"
#endif /* TAO_RT_PORTABLESERVER_H */
