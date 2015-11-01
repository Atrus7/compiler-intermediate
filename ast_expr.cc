/* File: ast_expr.cc
 * -----------------
 * Implementation of expression node classes.
 */
#include "ast_expr.h"
#include "ast_type.h"
#include "ast_decl.h"
#include <string.h>
#include "codegen.h"



IntConstant::IntConstant(yyltype loc, int val) : Expr(loc) {
    value = val;
}

DoubleConstant::DoubleConstant(yyltype loc, double val) : Expr(loc) {
    value = val;
}
Location * DoubleConstant::Emit() {
  //error for p5
  ReportError::DoubleInGenP5();
  return NULL;
}

BoolConstant::BoolConstant(yyltype loc, bool val) : Expr(loc) {
    value = val;
}

StringConstant::StringConstant(yyltype loc, const char *val) : Expr(loc) {
    Assert(val != NULL);
    value = strdup(val);
}

void This::Emit() {
  //TBI
  return;
}

Operator::Operator(yyltype loc, const char *tok) : Node(loc) {
    Assert(tok != NULL);
    strncpy(tokenString, tok, sizeof(tokenString));
}


void Operator::Emit() {
  //unfinished
  return;
}

CompoundExpr::CompoundExpr(Expr *l, Operator *o, Expr *r)
  : Expr(Join(l->GetLocation(), r->GetLocation())) {
    Assert(l != NULL && o != NULL && r != NULL);
    (op=o)->SetParent(this);
    (left=l)->SetParent(this);
    (right=r)->SetParent(this);
}

CompoundExpr::CompoundExpr(Operator *o, Expr *r)
  : Expr(Join(o->GetLocation(), r->GetLocation())) {
    Assert(o != NULL && r != NULL);
    left = NULL;
    (op=o)->SetParent(this);
    (right=r)->SetParent(this);
}


/*
  void CompoundExpr::Emit() {
  if(left){
  left->Emit();
  }
  op->Emit();
  right->Emit();
  }
*/
void AssignExpr::Emit() {
  left->Emit();
  op->Emit();
  right->Emit();
  //GENERATOR.GenLabel(GENERATOR.NewLabel());
  //Location * left = GENERATOR.GenLoadLabel();
  //GenAssign(left, right);
}
void LogicalExpr::Emit() {
  //TBI
  return;
}
void EqualityExpr::Emit() {
  //TBI
  return;
}

void ArithmeticExpr::Emit() {
  //TBI
  return;
}

void RelationalExpr::Emit() {
  //TBI
  return;
}

ArrayAccess::ArrayAccess(yyltype loc, Expr *b, Expr *s) : LValue(loc) {
  (base=b)->SetParent(this);
  (subscript=s)->SetParent(this);
}

void ArrayAccess::Emit() {
  base->Emit();
  subscript->Emit();
}

FieldAccess::FieldAccess(Expr *b, Identifier *f)
  : LValue(b? Join(b->GetLocation(), f->GetLocation()) : *f->GetLocation()) {
  Assert(f != NULL); // b can be be NULL (just means no explicit base)
  base = b;
  if (base) base->SetParent(this);
  (field=f)->SetParent(this);
}

void FieldAccess::Emit() {
  if(base){
    base->Emit();
  }
  field->Emit();
}


Call::Call(yyltype loc, Expr *b, Identifier *f, List<Expr*> *a) : Expr(loc)  {
  Assert(f != NULL && a != NULL); // b can be be NULL (just means no explicit base)
  base = b;
  if (base) base->SetParent(this);
  (field=f)->SetParent(this);
  (actuals=a)->SetParentAll(this);
}

void Call::Emit() {
  if(base){
    base->Emit();
  }
  field->Emit();
  actuals->EmitForAll();
}


NewExpr::NewExpr(yyltype loc, NamedType *c) : Expr(loc) {
  Assert(c != NULL);
  (cType=c)->SetParent(this);
}

void NewExpr::Emit() {
  cType->Emit();
}


NewArrayExpr::NewArrayExpr(yyltype loc, Expr *sz, Type *et) : Expr(loc) {
  Assert(sz != NULL && et != NULL);
  (size=sz)->SetParent(this);
  (elemType=et)->SetParent(this);
}

void NewArrayExpr::Emit() {
  size->Emit();
  elemType->Emit();
}


void ReadIntegerExpr::Emit() {
  //TBI
  return;
}

void ReadLineExpr::Emit() {
  //TBI
  return;
}
