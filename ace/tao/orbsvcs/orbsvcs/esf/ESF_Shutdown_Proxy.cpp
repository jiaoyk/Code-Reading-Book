// ESF_Shutdown_Proxy.cpp,v 1.2 2000/04/02 19:22:41 coryan Exp

#ifndef TAO_ESF_SHUTDOWN_PROXY_CPP
#define TAO_ESF_SHUTDOWN_PROXY_CPP

#include "ESF_Shutdown_Proxy.h"

#if ! defined (__ACE_INLINE__)
#include "ESF_Shutdown_Proxy.i"
#endif /* __ACE_INLINE__ */

ACE_RCSID(ESF, ESF_Shutdown_Proxy, "ESF_Shutdown_Proxy.cpp,v 1.2 2000/04/02 19:22:41 coryan Exp")

template<class PROXY> void
TAO_ESF_Shutdown_Proxy<PROXY>::work (PROXY *proxy,
                                     CORBA::Environment &ACE_TRY_ENV)
{
  ACE_TRY
    {
      proxy->shutdown (ACE_TRY_ENV);
      ACE_TRY_CHECK;
    }
  ACE_CATCHANY
    {
      // Do not propagate any exceptions
    }
  ACE_ENDTRY;
}

#endif /* TAO_ESF_SHUTDOWN_PROXY_CPP */
