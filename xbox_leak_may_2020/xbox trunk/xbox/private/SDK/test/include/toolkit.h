// -*- Mode:C++ -*-
//
//      Copyright (c) 1997 Microsoft Corporation.  All rights reserved.
//
//    This work is a trade secret of Microsoft Corporation.
//    Unauthorized disclosure is unlawful.
//
// $Header: $
// $NoKeywords: $
//
//
// File:    clients/cookbook/toolkit.h
// Creator: ljo

// Exported Function List: 
#ifndef INC_TOOLKIT_H
#define INC_TOOLKIT_H

#include "AstCL.h"

//  An object of class MyAstDataCollection is used during the traversal of
//  AST to collect data.  The user edits the class to add data members to
//  collect information of interest.  The base class AstDataCollection contains
//  virtual member functions.  These functions are invoked during tree 
//  traversal and do nothing.  To use any of these functions to collect 
//  information, create a MyAstDataCollection member function to override the
//  base class function.
//
//  There's one set of functions referenced during traversal of statements, 
//  and one set referenced during traversal of expressions.  The set for 
//  statements consists of a function to be invoked during a preorder 
//  traversal of the statement level AST, and one for the postorder traversal.
//  (Inorder traversal will be if needed.)  For the expression level AST, 
//  the set contains functions to be invoked during a preorder, inorder, 
//  and postorder traversal.  The Cut methods are for telling the walker
//  to ignore a given subtree.  If true when applied to that node, the preorder
//  and postorder functions won't be called on the node or any of its descendents.
//
//  A quick example -
//  If I want to count the number of 'for' statements, I edit the definition
//  of MyAstDataCollection to add a member to keep the count of 'for'
//  statements.  Add a preorder statement traversal function to 
//  MyAstDataCollection to check the opcode of the AstNode argument, adding 
//  one to the counter if it's a 'for' node (i.e, ocFor).


class AstDataCollection : public Object {
public:
    // Processing control functions
    virtual bool WantToProcessFile(char *fileName);
    virtual bool WantToProcessFunction(SymbolFunction *);

    // Collection functions
    virtual bool PreOrderStatement(AstDeclStat *);  // was (AstStatement *)
    virtual bool PostOrderStatement(AstDeclStat *); // was (AstStatement *)
    virtual bool FCutStatement(AstDeclStat *);
    virtual bool PreOrderExpression(AstExpression *);
    virtual bool InOrderExpression(AstExpression *);
    virtual bool PostOrderExpression(AstExpression *);
    virtual bool FCutExpression(AstExpression *);

    // Traversal functions
    virtual void WalkFunctions (Module *mod);
    virtual bool WalkStatement (AstDeclStat *);
    virtual bool WalkExpression (AstExpression *);

    // Ctors/Dtors
    AstDataCollection () {};
    ~AstDataCollection () {};

};

#endif







