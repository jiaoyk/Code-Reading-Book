// be_interface_fwd.cpp,v 1.27 2001/04/24 13:44:30 parsons Exp

// ============================================================================
//
// = LIBRARY
//    TAO IDL
//
// = FILENAME
//    be_interface.h
//
// = DESCRIPTION
//    Extension of class AST_Interface_Fwd that provides additional means for C++
//    mapping of an interface.
//
// = AUTHOR
//    Copyright 1994-1995 by Sun Microsystems, Inc.
//    and
//    Aniruddha Gokhale
//
// ============================================================================

#include        "idl.h"
#include        "idl_extern.h"
#include        "be.h"

ACE_RCSID(be, be_interface_fwd, "be_interface_fwd.cpp,v 1.27 2001/04/24 13:44:30 parsons Exp")

be_interface_fwd::be_interface_fwd (void)
{
  // Always the case.
  this->size_type (be_decl::VARIABLE); 
}

be_interface_fwd::be_interface_fwd (AST_Interface *dummy,
                                    UTL_ScopedName *n, 
                                    UTL_StrList *p)
  : AST_InterfaceFwd (dummy, 
                      n, 
                      p),
    AST_Decl (AST_Decl::NT_interface_fwd, 
              n, 
              p)
{
  // Always the case.
  this->size_type (be_decl::VARIABLE); 
}

be_interface_fwd::~be_interface_fwd (void)
{
}

void
be_interface_fwd::destroy (void)
{
  // Do nothing.
}

int
be_interface_fwd::accept (be_visitor *visitor)
{
  return visitor->visit_interface_fwd (this);
}

// Narrowing
IMPL_NARROW_METHODS2 (be_interface_fwd, AST_InterfaceFwd, be_type)
IMPL_NARROW_FROM_DECL (be_interface_fwd)
