// -*- C++ -*-
//
// Echo_Server_ORBInitializer.cpp,v 1.3 2000/11/20 05:44:57 othman Exp

#include "Echo_Server_ORBInitializer.h"

ACE_RCSID (Dynamic, Echo_Server_ORBInitializer, "Echo_Server_ORBInitializer.cpp,v 1.3 2000/11/20 05:44:57 othman Exp")

#if TAO_HAS_INTERCEPTORS == 1

#include "interceptors.h"

void
Echo_Server_ORBInitializer::pre_init (
    PortableInterceptor::ORBInitInfo_ptr
    TAO_ENV_ARG_DECL_NOT_USED)
  ACE_THROW_SPEC ((CORBA::SystemException))
{
}

void
Echo_Server_ORBInitializer::post_init (
    PortableInterceptor::ORBInitInfo_ptr info
    TAO_ENV_ARG_DECL)
  ACE_THROW_SPEC ((CORBA::SystemException))
{
  TAO_ENV_ARG_DEFN;

  PortableInterceptor::ServerRequestInterceptor_ptr interceptor =
    PortableInterceptor::ServerRequestInterceptor::_nil ();

  // Install the Echo server request interceptor
  ACE_NEW_THROW_EX (interceptor,
                    Echo_Server_Request_Interceptor,
                    CORBA::NO_MEMORY ());
  ACE_CHECK;

  PortableInterceptor::ServerRequestInterceptor_var
    server_interceptor = interceptor;

  info->add_server_request_interceptor (server_interceptor.in ()
                                        TAO_ENV_ARG_PARAMETER);
  ACE_CHECK;

}

#endif  /* TAO_HAS_INTERCEPTORS == 1 */
