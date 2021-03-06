// test_i.h,v 1.1 2001/04/09 21:39:27 mk1 Exp

// ============================================================================
//
// = LIBRARY
//   TAO/tests/MT_Client
//
// = FILENAME
//   test_i.h
//
// = AUTHOR
//   Carlos O'Ryan
//
// ============================================================================

#ifndef TAO_MT_CLIENT_TEST_I_H
#define TAO_MT_CLIENT_TEST_I_H

#include "testS.h"

class Simple_Server_i : public POA_Simple_Server
{
  // = TITLE
  //   Simpler Server implementation
  //
  // = DESCRIPTION
  //   Implements the Simple_Server interface in test.idl
  //
public:
  Simple_Server_i (CORBA::ORB_ptr orb);
  // ctor

  // = The Simple_Server methods.
  void sendCharSeq (const Char_Seq & charSeq, CORBA::Environment&)
    ACE_THROW_SPEC ((CORBA::SystemException));

  void sendOctetSeq (const Octet_Seq & octetSeq, CORBA::Environment&)
    ACE_THROW_SPEC ((CORBA::SystemException));
    
  CORBA::Long get_number (CORBA::Long num, CORBA::Environment&)
    ACE_THROW_SPEC ((CORBA::SystemException));

  void shutdown (CORBA::Environment&)
    ACE_THROW_SPEC ((CORBA::SystemException));
    
private:
  CORBA::ORB_var orb_;
};

#endif /* TAO_MT_CLIENT_TEST_I_H */
