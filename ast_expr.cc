/* File: ast_expr.cc
 * -----------------
 * Implementation of expression node classes.
 */
#include "ast_expr.h"
#include "ast_type.h"
#include "ast_decl.h"
#include <string.h>
#include "codegen.h"
#include "errors.h"

Expr::Expr(yyltype loc): Stmt(loc), type(Type::nullType) {}
Expr::Expr(): Stmt(), type(Type::nullType) {}

IntConstant::IntConstant(yyltype loc, int val) : Expr(loc) {
    value = val;
    type = Type::intType;
}
Location * IntConstant::Emit() {
  return GENERATOR.GenLoadConstant(value);
}

DoubleConstant::DoubleConstant(yyltype loc, double val) : Expr(loc) {
    value = val;
    type = Type::doubleType;
}
Location * DoubleConstant::Emit() {
  //error for p5
  ReportError::DoubleInGenP5();
  return NULL;
}

BoolConstant::BoolConstant(yyltype loc, bool val) : Expr(loc) {
    value = val;
    type = Type::boolType;
}
Location * BoolConstant::Emit() {
  int bool_casted_to_int = (value != 0)? 1 : 0;
  return GENERATOR.GenLoadConstant(bool_casted_to_int);
}

StringConstant::StringConstant(yyltype loc, const char *val) : Expr(loc) {
    Assert(val != NULL);
    value = strdup(val);
    type = Type::stringType;
}

Location * StringConstant::Emit() {
  return GENERATOR.GenLoadConstant(value);
}

Location * This::Emit() {
  //TBI
  return NULL;
}

Operator::Operator(yyltype loc, const char *tok) : Node(loc) {
    Assert(tok != NULL);
    strncpy(tokenString, tok, sizeof(tokenString));
}

Location * Operator::Emit() {
  //unfinished
  return NULL;
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

Location * AssignExpr::Emit() {
  Location *rhs = right->Emit();
  char * text;
  if(FieldAccess *var = dynamic_cast<FieldAccess*>(left)){
    text = var->Resolve();
  }
  else{
    PrintDebug("dev", "Not FieldAccess");
    return NULL;
  }
  //PrintDebug("dev", "here");
  //PrintDebug("dev", text);
  //SymbolTable::active->DebugSymbolTable();
  //PrintDebug("dev", SymbolTable::active->GetClassName());
  Location * lhs = GENERATOR.GenLoadLabel(text);
  Assert(lhs); //should always be valid in P5...
  if(!rhs){
    PrintDebug("dev", "Warning: wasn't able to resolve rhs of Assign to variable: ");
    PrintDebug("dev", text);
    return NULL;
  }

  //get last temp, it will be the rhs calculated...
  char temp [10];
  sprintf(temp, "_tmp%d", CodeGenerator::nextTempNum-1);
  GENERATOR.GenAssign(lhs, rhs);
  return NULL;
}
Location * LogicalExpr::Emit() {
  //TBI
  return NULL;
}
Location * EqualityExpr::Emit() {
  //TBI
  return NULL;
}

Location * ArithmeticExpr::Emit() {
  //TBI
  return NULL;
}

Location * RelationalExpr::Emit() {
  //TBI
  return NULL;
}

ArrayAccess::ArrayAccess(yyltype loc, Expr *b, Expr *s) : LValue(loc) {
  (base=b)->SetParent(this);
  (subscript=s)->SetParent(this);
}

Location * ArrayAccess::Emit() {
  base->Emit();
  subscript->Emit();
  return NULL;
}

FieldAccess::FieldAccess(Expr *b, Identifier *f)
  : LValue(b? Join(b->GetLocation(), f->GetLocation()) : *f->GetLocation()) {
  Assert(f != NULL); // b can be be NULL (just means no explicit base)
  base = b;
  if (base) base->SetParent(this);
  (field=f)->SetParent(this);
}

//TODO: Resolve bases and arrays
char *FieldAccess::Resolve(){
  //if(base){
  //base->
  //}
  return field->GetName();

}

Location * FieldAccess::Emit() {
  if(base){
    base->Emit();
  }
  field->Emit();
  return NULL;
}


Call::Call(yyltype loc, Expr *b, Identifier *f, List<Expr*> *a) : Expr(loc)  {
  Assert(f != NULL && a != NULL); // b can be be NULL (just means no explicit base)
  base = b;
  if (base) base->SetParent(this);
  (field=f)->SetParent(this);
  (actuals=a)->SetParentAll(this);
}

Location * Call::Emit() {
  if(base){
    base->Emit();
  }
  field->Emit();
  actuals->EmitForAll();
  return NULL;
}


NewExpr::NewExpr(yyltype loc, NamedType *c) : Expr(loc) {
  Assert(c != NULL);
  (cType=c)->SetParent(this);
}

Location * NewExpr::Emit() {
  cType->Emit();
  return NULL;
}


NewArrayExpr::NewArrayExpr(yyltype loc, Expr *sz, Type *et) : Expr(loc) {
  Assert(sz != NULL && et != NULL);
  (size=sz)->SetParent(this);
  (elemType=et)->SetParent(this);
}

Location * NewArrayExpr::Emit() {
  size->Emit();
  elemType->Emit();
  return NULL;
}


Location * ReadIntegerExpr::Emit() {
  GENERATOR.GenBuiltInCall(ReadInteger, NULL, NULL);
  return NULL;
}

Location * ReadLineExpr::Emit() {
  GENERATOR.GenBuiltInCall(ReadLine, NULL, NULL);
  return NULL;
}
