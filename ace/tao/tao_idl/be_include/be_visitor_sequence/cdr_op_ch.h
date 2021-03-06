/* -*- c++ -*- */
//
// cdr_op_ch.h,v 1.2 1998/10/20 02:32:33 levine Exp
//

// ============================================================================
//
// = LIBRARY
//    TAO IDL
//
// = FILENAME
//    cdr_op_ch.h
//
// = DESCRIPTION
//    Concrete visitor for the Sequence class
//    This one provides code generation for the CDR operators for the sequence
//    in the client header.
//
// = AUTHOR
//    Aniruddha Gokhale
//
// ============================================================================

#ifndef _BE_VISITOR_SEQUENCE_CDR_OP_CH_H_
#define _BE_VISITOR_SEQUENCE_CDR_OP_CH_H_

class be_visitor_sequence_cdr_op_ch : public be_visitor_decl
{
  //
  // = TITLE
  //   be_visitor_sequence_cdr_op_ch
  //
  // = DESCRIPTION
  //   This is a concrete visitor for sequence that generates the CDR operator
  //   declarations
  //

public:
  be_visitor_sequence_cdr_op_ch (be_visitor_context *ctx);
  // constructor

  ~be_visitor_sequence_cdr_op_ch (void);
  // destructor

  virtual int visit_sequence (be_sequence *node);
  // visit sequence

};

#endif /* _BE_VISITOR_SEQUENCE_CDR_OP_CH_H_ */
