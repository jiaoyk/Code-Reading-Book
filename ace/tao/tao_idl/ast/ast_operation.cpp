// ast_operation.cpp,v 1.19 2000/10/14 22:21:53 parsons Exp

/*

COPYRIGHT

Copyright 1992, 1993, 1994 Sun Microsystems, Inc.  Printed in the United
States of America.  All Rights Reserved.

This product is protected by copyright and distributed under the following
license restricting its use.

The Interface Definition Language Compiler Front End (CFE) is made
available for your use provided that you include this license and copyright
notice on all media and documentation and the software program in which
this product is incorporated in whole or part. You may copy and extend
functionality (but may not remove functionality) of the Interface
Definition Language CFE without charge, but you are not authorized to
license or distribute it to anyone else except as part of a product or
program developed by you or with the express written consent of Sun
Microsystems, Inc. ("Sun").

The names of Sun Microsystems, Inc. and any of its subsidiaries or
affiliates may not be used in advertising or publicity pertaining to
distribution of Interface Definition Language CFE as permitted herein.

This license is effective until terminated by Sun for failure to comply
with this license.  Upon termination, you shall destroy or return all code
and documentation for the Interface Definition Language CFE.

INTERFACE DEFINITION LANGUAGE CFE IS PROVIDED AS IS WITH NO WARRANTIES OF
ANY KIND INCLUDING THE WARRANTIES OF DESIGN, MERCHANTIBILITY AND FITNESS
FOR A PARTICULAR PURPOSE, NONINFRINGEMENT, OR ARISING FROM A COURSE OF
DEALING, USAGE OR TRADE PRACTICE.

INTERFACE DEFINITION LANGUAGE CFE IS PROVIDED WITH NO SUPPORT AND WITHOUT
ANY OBLIGATION ON THE PART OF Sun OR ANY OF ITS SUBSIDIARIES OR AFFILIATES
TO ASSIST IN ITS USE, CORRECTION, MODIFICATION OR ENHANCEMENT.

SUN OR ANY OF ITS SUBSIDIARIES OR AFFILIATES SHALL HAVE NO LIABILITY WITH
RESPECT TO THE INFRINGEMENT OF COPYRIGHTS, TRADE SECRETS OR ANY PATENTS BY
INTERFACE DEFINITION LANGUAGE CFE OR ANY PART THEREOF.

IN NO EVENT WILL SUN OR ANY OF ITS SUBSIDIARIES OR AFFILIATES BE LIABLE FOR
ANY LOST REVENUE OR PROFITS OR OTHER SPECIAL, INDIRECT AND CONSEQUENTIAL
DAMAGES, EVEN IF SUN HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.

Use, duplication, or disclosure by the government is subject to
restrictions as set forth in subparagraph (c)(1)(ii) of the Rights in
Technical Data and Computer Software clause at DFARS 252.227-7013 and FAR
52.227-19.

Sun, Sun Microsystems and the Sun logo are trademarks or registered
trademarks of Sun Microsystems, Inc.

SunSoft, Inc.
2550 Garcia Avenue
Mountain View, California  94043

NOTE:

SunOS, SunSoft, Sun, Solaris, Sun Microsystems or the Sun logo are
trademarks or registered trademarks of Sun Microsystems, Inc.

*/

// AST_Operation nodes denote IDL operation declarations
// AST_Operations are a subclass of AST_Decl (they are not a type!)
// and of UTL_Scope (the arguments are managed in a scope).
// AST_Operations have a return type (a subclass of AST_Type),
// a bitfield for denoting various properties of the operation (the
// values are ORed together from constants defined in the enum
// AST_Operation::FLags), a name (a UTL_ScopedName), a context
// (implemented as a list of Strings, a UTL_StrList), and a raises
// clause (implemented as an array of AST_Exceptions).

#include "idl.h"
#include "idl_extern.h"

ACE_RCSID(ast, ast_operation, "ast_operation.cpp,v 1.19 2000/10/14 22:21:53 parsons Exp")

// Constructor(s) and destructor.

AST_Operation::AST_Operation (void)
  : pd_return_type (0),
    pd_flags (OP_noflags),
    pd_context (0),
    pd_exceptions( 0),
    argument_count_ (-1),
    has_native_ (0)
{
}

AST_Operation::AST_Operation (AST_Type *rt,
                              Flags fl,
                              UTL_ScopedName *n,
                              UTL_StrList *p,
                              idl_bool local,
                              idl_bool abstract)
  : AST_Decl(AST_Decl::NT_op, 
             n, 
             p),
    UTL_Scope(AST_Decl::NT_op),
    COMMON_Base (local, 
                 abstract),
    pd_return_type (rt),
    pd_flags (fl),
    pd_context (0),
    pd_exceptions (0),
    argument_count_ (-1),
    has_native_ (0)
{
  AST_PredefinedType *pdt = 0;

  // Check that if the operation is oneway, the return type must be void.
  if (rt != 0 && pd_flags == OP_oneway) 
    {
      if (rt->node_type () != AST_Decl::NT_pre_defined)
        {
          idl_global->err ()->error1 (UTL_Error::EIDL_NONVOID_ONEWAY, 
                                      this);
        }
      else 
        {
          pdt = AST_PredefinedType::narrow_from_decl (rt);

          if (pdt == 0 || pdt->pt () != AST_PredefinedType::PT_void)
            {
              idl_global->err ()->error1 (UTL_Error::EIDL_NONVOID_ONEWAY, 
                                          this);
            }
        }
    }
}

AST_Operation::~AST_Operation (void)
{
}

// Public operations.

// Return the member count.
int
AST_Operation::argument_count (void)
{
  this->compute_argument_attr ();

  return this->argument_count_;
}

// Return if any argument or the return type is a <native> type.
int
AST_Operation::has_native (void)
{
  this->compute_argument_attr ();

  return this->has_native_;
}

void
AST_Operation::destroy (void)
{
}

// Private operations.

// Compute total number of members.
int
AST_Operation::compute_argument_attr (void)
{
  if (this->argument_count_ != -1)
    {
      return 0;
    }

  AST_Decl *d = 0;
  AST_Type *type = 0;
  AST_Argument *arg = 0;

  this->argument_count_ = 0;

  // If there are elements in this scope.
  if (this->nmembers () > 0)
    {
      // Instantiate a scope iterator.
      UTL_ScopeActiveIterator *si = 0;
      ACE_NEW_RETURN (si,
                      UTL_ScopeActiveIterator (this, 
                                               UTL_Scope::IK_decls),
                      -1);

      while (!si->is_done ())
        {
          // Get the next AST decl node.
          d = si->item ();

          if (d->node_type () == AST_Decl::NT_argument)
            {
              this->argument_count_++;

              arg = AST_Argument::narrow_from_decl (d);

              type = AST_Type::narrow_from_decl (arg->field_type ());

              if (type->node_type () == AST_Decl::NT_native)
                {
                  this->has_native_ = 1;
                }
            }

          si->next ();
        }

      delete si;
    }

  type = AST_Type::narrow_from_decl (this->return_type ());

  if (type->node_type () == AST_Decl::NT_native)
    {
      this->has_native_ = 1;
    }

  return 0;
}

// Add this context (a UTL_StrList) to this scope.
UTL_StrList *
AST_Operation::fe_add_context (UTL_StrList *t)
{
  this->pd_context = t;

  return t;
}

UTL_ExceptList *
AST_Operation::be_add_exceptions (UTL_ExceptList *t)
{
  if (this->pd_exceptions != 0)
    {
      idl_global->err ()->error1 (UTL_Error::EIDL_ILLEGAL_RAISES, 
                                  this);
    }
  else
    {
      this->pd_exceptions = t;
    }

  return this->pd_exceptions;
}

// Add these exceptions (identified by name) to this scope.
// This looks up each name to resolve it to the name of a known
// exception, and then adds the referenced exception to the list
//  exceptions that this operation can raise.

// NOTE: No attempt is made to ensure that exceptions are mentioned
//       only once..
UTL_NameList *
AST_Operation::fe_add_exceptions (UTL_NameList *t)
{
  UTL_NamelistActiveIterator *nl_i = 0;
  UTL_ScopedName *nl_n = 0;
  AST_Exception *fe = 0;
  AST_Decl *d = 0;

  this->pd_exceptions = 0;

  ACE_NEW_RETURN (nl_i,
                  UTL_NamelistActiveIterator(t),
                  0);

  while (!nl_i->is_done()) 
    {
      nl_n = nl_i->item ();

      d = lookup_by_name (nl_n, 
                          I_TRUE);

      if (d == 0 || d->node_type() != AST_Decl::NT_except) 
        {
          idl_global->err ()->lookup_error (nl_n);
          delete nl_i;
          return 0;
        }

      fe = AST_Exception::narrow_from_decl (d);

      if ((this->flags () == AST_Operation::OP_oneway) && fe)
        {
          idl_global->err ()->error1 (UTL_Error::EIDL_ILLEGAL_RAISES, 
                                      this);
        }

      if (fe == 0) 
        {
          idl_global->err ()->error1 (UTL_Error::EIDL_ILLEGAL_RAISES, 
                                       this);
          return 0;
        }

      if (this->pd_exceptions == 0)
        {
          ACE_NEW_RETURN (this->pd_exceptions,
                          UTL_ExceptList (fe, 
                                          0),
                          0);
        }
      else
        {
          UTL_ExceptList *el = 0;
          ACE_NEW_RETURN (el,
                          UTL_ExceptList (fe,
                                          0),
                          0);

          this->pd_exceptions->nconc (el);
        }

      nl_i->next ();
    }

  delete nl_i;
  return t;
}

// Add this AST_Argument node (an operation argument declaration)
// to this scope.
AST_Argument *
AST_Operation::fe_add_argument (AST_Argument *t)
{
  AST_Decl *d = 0;

  // Already defined and cannot be redefined? Or already used?
  if ((d = lookup_by_name_local (t->local_name(), 0)) != 0) 
    {
      if (!can_be_redefined (d)) 
        {
          idl_global->err ()->error3 (UTL_Error::EIDL_REDEF, 
                                      t, 
                                      this, 
                                      d);
          return 0;
        }

      if (this->referenced (d, t->local_name ())) 
        {
          idl_global->err ()->error3 (UTL_Error::EIDL_DEF_USE, 
                                      t, 
                                      this, 
                                      d);
          return 0;
        }

      if (t->has_ancestor (d)) 
        {
          idl_global->err ()->redefinition_in_scope (t, 
                                                     d);
          return 0;
        }
    }

  // Cannot add OUT or INOUT argument to oneway operation.
  if ((t->direction () == AST_Argument::dir_OUT 
       || t->direction() == AST_Argument::dir_INOUT) 
      && pd_flags == OP_oneway) 
    {
      idl_global->err ()->error2 (UTL_Error::EIDL_ONEWAY_CONFLICT, 
                                  t, 
                                  this);
      return 0;
    }

  // Add it to scope.
  this->add_to_scope (t);

  // Add it to set of locally referenced symbols.
  this->add_to_referenced (t, 
                           I_FALSE, 
                           t->local_name ());

  return t;
}

// Dump this AST_Operation node (an operation) to the ostream o.
void
AST_Operation::dump (ostream &o)
{
  UTL_ScopeActiveIterator *i = 0;
  UTL_StrlistActiveIterator *si = 0;
  UTL_ExceptlistActiveIterator *ei = 0;
  AST_Decl *d = 0;
  AST_Exception *e = 0;
  UTL_String *s = 0;

  if (this->pd_flags == OP_oneway)
    {
      o << "oneway ";
    }
  else if (this->pd_flags == OP_idempotent)
    {
      o << "idempotent ";
    }

  ACE_NEW (i,
           UTL_ScopeActiveIterator (this, 
                                    IK_decls));

  this->pd_return_type->name ()->dump (o);
  o << " ";
  this->local_name ()->dump (o);
  o << "(";

  while (!i->is_done()) 
    {
      d = i->item ();
      d->dump (o);
      i->next ();

      if (!i->is_done())
        {
          o << ", ";
        }
    }

  delete i;
  o << ")";

  if (this->pd_exceptions != 0) 
    {
      o << " raises(";

      ACE_NEW (ei,
               UTL_ExceptlistActiveIterator (pd_exceptions));

      while (!ei->is_done()) 
        {
          e = ei->item ();
          ei->next ();
          e->local_name ()->dump (o);

          if (!ei->is_done())
            {
             o << ", ";
            }
        }

      delete ei;
      o << ")";
    }

  if (this->pd_context != 0) 
    {
      o << " context(";

      ACE_NEW (si,
               UTL_StrlistActiveIterator (pd_context));

      while (!si->is_done()) 
        {
          s = si->item ();
          si->next ();
          o << s->get_string ();

          if (!si->is_done())
            {
              o << ", ";
            }
        }

      delete si;
      o << ")";
    }
}

int
AST_Operation::ast_accept (ast_visitor *visitor)
{
  return visitor->visit_operation (this);
}

// Data accessors

AST_Type *
AST_Operation::return_type (void)
{
  return this->pd_return_type;
}

AST_Operation::Flags
AST_Operation::flags (void)
{
  return this->pd_flags;
}

UTL_StrList *
AST_Operation::context (void)
{
  return this->pd_context;
}

UTL_ExceptList *
AST_Operation::exceptions (void)
{
  return this->pd_exceptions;
}

// Narrowing.
IMPL_NARROW_METHODS2(AST_Operation, AST_Decl, UTL_Scope)
IMPL_NARROW_FROM_DECL(AST_Operation)
IMPL_NARROW_FROM_SCOPE(AST_Operation)
