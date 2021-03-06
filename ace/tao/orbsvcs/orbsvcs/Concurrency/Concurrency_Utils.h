/* -*- C++ -*- */
// Concurrency_Utils.h,v 1.8 2000/03/23 20:52:18 nanbor Exp

// ============================================================================
//
// = LIBRARY
//    TAO/orbsvcs/Concurrency_Service
//
// = FILENAME
//    Concurrency_Utils.h
//
// = DESCRIPTION
//      This class implements a Concurrency Server wrapper class which
//      holds a number of lock sets.  The server must run in the
//      thread per request concurrency model in order to let the
//      clients block on the semaphores.
//
// = AUTHORS
//    Torben Worm <tworm@cs.wustl.edu>
//
// ============================================================================

#ifndef _CONCURRENCY_SERVER_H
#define _CONCURRENCY_SERVER_H
#include "ace/pre.h"

#include "tao/corba.h"
#include "orbsvcs/CosConcurrencyControlC.h"
#include "CC_LockSetFactory.h"
#include "concurrency_export.h"

class TAO_Concurrency_Export TAO_Concurrency_Server
{
  // = TITLE
  //    Defines a wrapper class for the implementation of the
  //    concurrency server.
  //
  // = DESCRIPTION
  //    This class takes an orb and Poa reference and activates the
  //    concurrency service lock set factory object under them.
public:
  // = Initialization and termination methods.
  TAO_Concurrency_Server (void);
  //Default constructor.

  TAO_Concurrency_Server (CORBA::ORB_var &orb,
                          PortableServer::POA_var &poa);
  // Takes the POA under which to register the Concurrency Service
  // implementation object.

  ~TAO_Concurrency_Server (void);
  // Destructor.

  int init (CORBA::ORB_var &orb,
            PortableServer::POA_var &poa);
  // Initialize the concurrency server under the given ORB and POA.

  CC_LockSetFactory *GetLockSetFactory(void);
  // Get the lock set factory.

private:
  CC_LockSetFactory lock_set_factory_;
  // This is the lock set factory activated under the POA.
};

#include "ace/post.h"
#endif /* _CONCURRENCY_SERVER_H */
