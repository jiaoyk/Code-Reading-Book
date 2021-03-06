// -*- C++ -*-
// Push_Iterator_Handler.cpp,v 1.4 2000/06/21 22:54:25 othman Exp

// Ossama Othman <ossama@uci.edu>

#include "ace/Log_Msg.h"
#include "ace/Process_Manager.h"
#include "Push_Iterator_Handler.h"
#include "Callback_i.h"

ACE_RCSID (AMI_Observer, Push_Iterator_Handler, "Push_Iterator_Handler.cpp,v 1.4 2000/06/21 22:54:25 othman Exp")

Push_Iterator_Handler::Push_Iterator_Handler (void)
  : callback_servant_ (0),
    callback_ (),
    ami_handler_ ()
{
  // Nothing else
}

Push_Iterator_Handler::~Push_Iterator_Handler (void)
{
  // Nothing else
}

void
Push_Iterator_Handler::register_callback
  (const Web_Server::Metadata_Type &metadata,
   CORBA::Environment &ACE_TRY_ENV)
  ACE_THROW_SPEC ((CORBA::SystemException))
{
  this->callback_servant_->metadata (metadata);

  // This handler is no longer needed, so deactivate it.
  this->deactivate (ACE_TRY_ENV);
  ACE_CHECK;
}

void
Push_Iterator_Handler::run (int *request_count,
                            const char *pathname,
                            Web_Server::Iterator_Factory_ptr factory,
                            CORBA::Environment &ACE_TRY_ENV)
  ACE_THROW_SPEC ((CORBA::SystemException,
                   Web_Server::Error_Result))
{
  ACE_NEW_THROW_EX (this->callback_servant_,
                    Callback_i (request_count),
                    CORBA::NO_MEMORY ());
  ACE_CHECK;

  // Pass ownership of the Callback servant to the POA.
  PortableServer::ServantBase_var tmp (this->callback_servant_);

  // Activate the Callback.
  this->callback_ =
    this->callback_servant_->_this (ACE_TRY_ENV);
  ACE_CHECK;

  // Activate this Reply Handler.
  this->ami_handler_ =
    this->_this (ACE_TRY_ENV);
  ACE_CHECK;

  // Register the client callback with the server asynchronously.
  factory->sendc_register_callback (this->ami_handler_.in (),
                                    pathname,
                                    this->callback_.in (),
                                    ACE_TRY_ENV);
  ACE_CHECK;
}

void
Push_Iterator_Handler::deactivate (CORBA::Environment &ACE_TRY_ENV)
  ACE_THROW_SPEC ((CORBA::SystemException))
{
  // Get the POA used when activating the Reply Handler object.
  PortableServer::POA_var poa = this->_default_POA (ACE_TRY_ENV);
  ACE_CHECK;

  // Get the object ID associated with this servant.
  PortableServer::ObjectId_var oid =
    poa->servant_to_id (this,
                        ACE_TRY_ENV);
  ACE_CHECK;

  // Now deactivate the AMI_CallbackHandler object.
  poa->deactivate_object (oid.in (), ACE_TRY_ENV);
  ACE_CHECK;
}
