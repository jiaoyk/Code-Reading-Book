// client.cpp,v 1.6 2001/03/30 20:11:05 bala Exp

#include "ace/Get_Opt.h"
#include "test_i.h"
#include "tao/BiDir_GIOP/BiDirGIOP.h"

ACE_RCSID(BiDirectional, client, "client.cpp,v 1.6 2001/03/30 20:11:05 bala Exp")

const char *ior = "file://test.ior";

void do_nothing (void)
{
}

int
parse_args (int argc, char *argv[])
{
  ACE_Get_Opt get_opts (argc, argv, "k:");
  int c;

  while ((c = get_opts ()) != -1)
    switch (c)
      {
      case 'k':
        ior = get_opts.optarg;
        break;

      case '?':
      default:
        ACE_ERROR_RETURN ((LM_ERROR,
                           "usage:  %s "
                           "-k <ior> "
                           "\n",
                           argv [0]),
                          -1);
      }
  // Indicates sucessful parsing of the command line
  return 0;
}

int
main (int argc, char *argv[])
{
  ACE_TRY_NEW_ENV
    {
      CORBA::ORB_var orb =
        CORBA::ORB_init (argc, argv, "", ACE_TRY_ENV);
      ACE_TRY_CHECK;

      CORBA::Object_var poa_object =
        orb->resolve_initial_references ("RootPOA", ACE_TRY_ENV);
      ACE_TRY_CHECK;

      if (CORBA::is_nil (poa_object.in ()))
        ACE_ERROR_RETURN ((LM_ERROR,
                           " (%P|%t) Unable to initialize the POA.\n"),
                          1);

      PortableServer::POA_var root_poa =
        PortableServer::POA::_narrow (poa_object.in (), ACE_TRY_ENV);
      ACE_TRY_CHECK;

      PortableServer::POAManager_var poa_manager =
        root_poa->the_POAManager (ACE_TRY_ENV);
      ACE_TRY_CHECK;

      // Policies for the childPOA to be created.
      CORBA::PolicyList policies (1);
      policies.length (1);

      CORBA::Any pol;
      pol <<= BiDirPolicy::BOTH;
      policies[0] =
        orb->create_policy (BiDirPolicy::BIDIRECTIONAL_POLICY_TYPE,
                            pol,
                            ACE_TRY_ENV);
      ACE_TRY_CHECK;

      // Create POA as child of RootPOA with the above policies.  This POA
      // will receive request in the same connection in which it sent
      // the request
      PortableServer::POA_var child_poa =
        root_poa->create_POA ("childPOA",
                              poa_manager.in (),
                              policies,
                              ACE_TRY_ENV);
      ACE_TRY_CHECK;

      // Creation of childPOA is over. Destroy the Policy objects.
      for (CORBA::ULong i = 0;
           i < policies.length ();
           ++i)
        {
          policies[i]->destroy (ACE_TRY_ENV);
          ACE_TRY_CHECK;
        }

      poa_manager->activate (ACE_TRY_ENV);
      ACE_TRY_CHECK;

      if (parse_args (argc, argv) != 0)
        return 1;

      CORBA::Object_var object =
        orb->string_to_object (ior, ACE_TRY_ENV);
      ACE_TRY_CHECK;

      Simple_Server_var server =
        Simple_Server::_narrow (object.in (), ACE_TRY_ENV);
      ACE_TRY_CHECK;

      if (CORBA::is_nil (server.in ()))
        {
          ACE_ERROR_RETURN ((LM_ERROR,
                             "Object reference <%s> is nil\n",
                             ior),
                            1);
        }


      Callback_i callback_impl (orb.in ());

      Callback_var callback =
        callback_impl._this (ACE_TRY_ENV);
      ACE_TRY_CHECK;

      // Send the calback object to the server
      server->callback_object (callback.in (),
                               ACE_TRY_ENV);
      ACE_TRY_CHECK;

      // A  method to kickstart callbacks from the server
      CORBA::Long r =
        server->test_method (1, ACE_TRY_ENV);
      ACE_TRY_CHECK;

      if (r != 0)
        {
          ACE_DEBUG ((LM_DEBUG,
                      "(%P|%t) unexpected result = %d ",
                      r));
        }

      orb->run (ACE_TRY_ENV);
      ACE_TRY_CHECK;

      root_poa->destroy (1, 1, ACE_TRY_ENV);
      ACE_TRY_CHECK;

    }
  ACE_CATCHANY
    {
      ACE_PRINT_EXCEPTION (ACE_ANY_EXCEPTION,
                           "Catched exception:");
      return 1;
    }
  ACE_ENDTRY;

  return 0;
}
