/* -*- c++ -*- */
// be_predefined_type.h,v 1.18 2001/05/24 14:53:21 parsons Exp

// ============================================================================
//
// = LIBRARY
//    TAO IDL
//
// = FILENAME
//    be_predefined_type.h
//
// = DESCRIPTION
//    Extension of class AST_PredefinedType that provides additional means for C++
//    mapping.
//
// = AUTHOR
//    Copyright 1994-1995 by Sun Microsystems, Inc.
//    and
//    Aniruddha Gokhale
//
// ============================================================================

#ifndef _BE_PREDEFINED_TYPE_H
#define _BE_PREDEFINED_TYPE_H

#include "be_type.h"
#include "ast_predefined_type.h"

class UTL_StrList;
class be_visitor;

class be_predefined_type : public virtual AST_PredefinedType,
                           public virtual be_type
{
public:
  be_predefined_type (void);
  // Default constructor.

  be_predefined_type (AST_PredefinedType::PredefinedType t,
                      UTL_ScopedName *n,
                      UTL_StrList *p);
  // Constructor.

  // Visiting.
  virtual int accept (be_visitor* visitor);

  // Narrowing.
  DEF_NARROW_METHODS2 (be_predefined_type, AST_PredefinedType, be_type);
  DEF_NARROW_FROM_DECL (be_predefined_type);

protected:
  virtual int compute_size_type (void);
  // Compute the size type if it is unknown.

  virtual void compute_tc_name (void);
  // Overridden method.

  // Compute and return the value.

  virtual void compute_repoID (void);
  // The object and TypeCode repository IDs have a mandatory omg.org
  // prefix, but the default compute_repoID() uses the #pragma's to
  // obtain prefixes.
};

#endif
