// Logging_Service_i.cpp,v 1.3 2001/04/02 18:12:24 kitty Exp

#include "Logging_Service_i.h"
#include "tao/debug.h"

ACE_RCSID(Logging_Service, Logging_Service_i, "Logging_Service_i.cpp,v 1.3 2001/04/02 18:12:24 kitty Exp")

Logger_Server::Logger_Server (void)
  :service_name_ (ACE_const_cast (char *,"LoggingService"))
{
  // Do nothing
}

int
Logger_Server::parse_args (void)
{
  ACE_Get_Opt get_opts (argc_, argv_, "dn:");
  int c;

  while ((c = get_opts ()) != -1)
    switch (c)
      {
      case 'd':  // debug flag.
        TAO_debug_level++;
        break;
      case 'n':  // Set factory name to cmnd line arg
	service_name_ = get_opts.optarg;
	break;
      case '?':
      default:
        ACE_ERROR_RETURN ((LM_ERROR,
                           "usage:  %s"
                           " [-d]"
			   " [-n service-name]"
                           "\n",
                           argv_ [0]),
                          -1);
      }

  // Indicates successful parsing of command line.
  return 0;
}

int
Logger_Server::init (int argc,
		     char *argv[],
		     CORBA::Environment &ACE_TRY_ENV)
{
  this->argc_ = argc;
  this->argv_ = argv;

  // Call the init of <TAO_ORB_Manager> to initialize the ORB and
  // create a child POA under the root POA.
  if (this->orb_manager_.init_child_poa (argc,
					 argv,
					 "child_poa",
					 ACE_TRY_ENV) == -1)
    ACE_ERROR_RETURN ((LM_ERROR,
		       "%p\n",
		       "init_child_poa"),
		      -1);

  ACE_CHECK_RETURN (-1);

  this->orb_manager_.activate_poa_manager (ACE_TRY_ENV);
  ACE_CHECK_RETURN (-1);

  // Parse the command line arguments.
  if (this->parse_args () != 0)
    ACE_ERROR_RETURN ((LM_ERROR,
		       "%p\n",
		       "parse_args"),
		      -1);

  // Activate the logger_factory.
  CORBA::String_var str =
    this->orb_manager_.activate_under_child_poa ("logger_factory",
						 &this->factory_impl_,
						 ACE_TRY_ENV);
    if (TAO_debug_level > 0)
    ACE_DEBUG ((LM_DEBUG,
		"The IOR is: <%s>\n",
		str.in ()));

    // Initialize the naming service
    int ret = this->init_naming_service (ACE_TRY_ENV);
    ACE_CHECK_RETURN (-1);
    if (ret != 0)
      ACE_ERROR_RETURN ((LM_ERROR,
		       "%p\n",
		       "init_naming_service"),
		      -1);
  else
    return 0;
}


// Initialisation of Naming Service and register IDL_Logger Context
// and logger_factory object.

int
Logger_Server::init_naming_service (CORBA::Environment& ACE_TRY_ENV)
{
  // Get pointers to the ORB and child POA
  CORBA::ORB_var orb = this->orb_manager_.orb ();
  PortableServer::POA_var child_poa = this->orb_manager_.child_poa ();

  // Initialize the naming service
  if (this->my_name_server_.init (orb.in (),
				  child_poa.in ()) == -1)
    return -1;

  // Create an instance of the Logger_Factory
  Logger_Factory_var factory = this->factory_impl_._this (ACE_TRY_ENV);
  ACE_CHECK_RETURN (-1);

  //Register the logger_factory
  CosNaming::Name factory_name (1);
  factory_name.length (1);
  factory_name[0].id = CORBA::string_dup ("Logger_Factory");
  this->my_name_server_->bind (factory_name,
			       factory.in (),
			       ACE_TRY_ENV);
  ACE_CHECK_RETURN (-1);

  return 0;
}

int
Logger_Server::run (CORBA::Environment &ACE_TRY_ENV)
{
  int ret = this->orb_manager_.run (ACE_TRY_ENV);
  ACE_CHECK_RETURN (-1);
  if (ret == -1)
    ACE_ERROR_RETURN ((LM_ERROR,
                       "Logger_Server::run"),
                      -1);
  return 0;
}

Logger_Server::~Logger_Server (void)
{
  // Do nothing
}
