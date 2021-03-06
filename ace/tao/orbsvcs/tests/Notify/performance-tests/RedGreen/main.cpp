// -*- C++ -*-
// main.cpp,v 1.1 2000/09/28 19:35:59 pradeep Exp

#include "RedGreen_Test.h"

int
main (int argc, char *argv [])
{
  ACE_High_Res_Timer::calibrate ();

  RedGreen_Test client;

  client.parse_args (argc, argv);

  ACE_TRY_NEW_ENV
    {
      client.init (argc, argv,
                   ACE_TRY_ENV); //Init the Client
      ACE_TRY_CHECK;

      client.run (ACE_TRY_ENV);
      ACE_TRY_CHECK;
    }
  ACE_CATCH (CORBA::UserException, ue)
    {
      ACE_PRINT_EXCEPTION (ue,
                           "Client user error: ");
      return 1;
    }
  ACE_CATCH (CORBA::SystemException, se)
    {
      ACE_PRINT_EXCEPTION (se,
                           "system error: ");
      return 1;
    }
  ACE_ENDTRY;

  return 0;
}
