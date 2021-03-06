// test_i.h,v 1.1 1999/07/06 02:38:54 coryan Exp

// ============================================================================
//
// = LIBRARY
//   TAO/tests/Timeout
//
// = FILENAME
//   test_i.h
//
// = AUTHOR
//   Carlos O'Ryan
//
// ============================================================================

#ifndef TAO_TIMEOUT_TEST_I_H
#define TAO_TIMEOUT_TEST_I_H

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
  CORBA::Long echo (CORBA::Long x,
                    CORBA::Long msecs,
                    CORBA::Environment&)
    ACE_THROW_SPEC ((CORBA::SystemException));
  void shutdown (CORBA::Environment&)
    ACE_THROW_SPEC ((CORBA::SystemException));

private:
  CORBA::ORB_var orb_;
  // The ORB
};

#if defined(__ACE_INLINE__)
#include "test_i.i"
#endif /* __ACE_INLINE__ */

#endif /* TAO_TIMEOUT_TEST_I_H */
