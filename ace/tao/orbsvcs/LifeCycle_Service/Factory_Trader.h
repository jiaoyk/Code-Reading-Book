// Factory_Trader.h,v 1.7 1999/10/25 05:27:53 coryan Exp
// ============================================================================
//
// = LIBRARY
//
// = FILENAME
//   Factory_Trader.cpp
//
// = DESCRIPTION
//   Factory Trader for the Generic Factory.
//
// = AUTHOR
//   Michael Kircher (mk1@cs.wustl.edu)
//
// ============================================================================

#ifndef FACTORY_TRADER_H
#define FACTORY_TRADER_H

#include "tao/corba.h"
#include "orbsvcs/Trader/Trader.h"
#include "orbsvcs/Trader/Service_Type_Repository.h"
#include "orbsvcs/CosLifeCycleC.h"


class Factory_Trader
{
public:
  Factory_Trader (int debug_level = 1);
  ~Factory_Trader ();

  void add_type ();
  // Add a the Factory type to the repository

  void _cxx_export (const char * name,
                    const char * location,
                    const char * description,
                    const CORBA::Object_ptr object_ptr);
  // export a specific factory

  CORBA::Object_ptr query (const char* constraint);
  // query for a specific factory using a constraint

  static const char * GENERIC_FACTORY_INTERFACE_REPOSITORY_ID;
private:
  TAO_Service_Type_Repository repository_;
  TAO_Trader_Factory::TAO_TRADER *trader_ptr_;
  TAO_Trading_Components_i *trading_Components_ptr_;
  TAO_Support_Attributes_i *support_Attributes_ptr_;

  int debug_level_;
  // debug level (0 = quiet, 1 = default, informative, 2+ = noisy);
};

#endif // FACTORY_TRADER_H
