// -*- C++ -*-

//=============================================================================
/**
 * @file SLevel1_Test_i.h
 *
 * SLevel1_Test_i.h,v 1.2 2001/03/30 03:14:06 doccvs Exp
 *
 * Implementation header for the Security Level1test.
 *
 * @author Priyanka Gontla <pgontla@doc.ece.uci.edu>
 */
//=============================================================================

#ifndef TAO_SLEVEL1_TEST_I_H
#define TAO_SLEVEL1_TEST_I_H

#include "SLevel1_TestS.h"
#include "orbsvcs/SecurityLevel1C.h"
#include "openssl/x509.h"

class SLevel1_Server_i : public POA_SLevel1_Server
{
 public:
  /// Constructor
  SLevel1_Server_i (CORBA::ORB_ptr,
                    SecurityLevel1::Current_ptr ss_current);

  /// Simple method invoked to test if the client reached server etc.
  CORBA::Boolean authorize_level1 (CORBA::Environment &)
    ACE_THROW_SPEC ((CORBA::SystemException));

  void shutdown (CORBA::Environment &ACE_TRY_ENV =
                 TAO_default_environment ())
    ACE_THROW_SPEC ((CORBA::SystemException));

  X509 *create_check_cert ();

  static int authorized_1;

 private:

  /// Reference to the ORB.
  CORBA::ORB_var orb_;

  /// Reference to the "SecurityCurrent" object.
  SecurityLevel1::Current_var ss_current_;

};

#endif /* TAO_SLEVEL1_TEST_I_H */
