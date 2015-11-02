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
  Location * lhs = SymbolTable::active->Lookup(text);
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
  return lhs;
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
  Location *rhs = right->Emit();
  char *op_str = op->ToString();

  if(left){
    Location *lhs = left->Emit();
    return GENERATOR.GenBinaryOp(op_str, lhs, rhs);
  }
  else{ //unary minus is the only possibility
    Location * zero = GENERATOR.GenLoadConstant(0);
    return GENERATOR.GenBinaryOp("-", zero, rhs);
  }
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
  return field->GetName();

}

Location * FieldAccess::Emit() {
  if(base){
    base->Emit();
    //SymbolTable::active->Lookup(field);
  }
  field->Emit();
  PrintDebug("dev", "Looking up");
  PrintDebug("dev", field->GetName());
  PrintDebug("dev", "-----------");
  //SymbolTable::active->DebugSymbolTable();
  //PrintDebug("dev", "-----------");
  Location * loc = SymbolTable::active->Lookup(field->GetName());
  if(loc){
    PrintDebug("dev", loc->GetName());
    PrintDebug("dev", "-----------");
    return loc;
  }
  else{
    PrintDebug("dev", "Didn't resolve FieldAccess");
    return NULL;
  }
}


Call::Call(yyltype loc, Expr *b, Identifier *f, List<Expr*> *a) : Expr(loc)  {
  Assert(f != NULL && a != NULL); // b can be be NULL (just means no explicit base)
  base = b;
  if (base) base->SetParent(this);
  (field=f)->SetParent(this);
  (actuals=a)->SetParentAll(this);
}

Location * Call::Emit() {
  //may temporarily swap the active, so need to save the current
  SymbolTable* saved = SymbolTable::active;
  if(base){
    Location * loc = base->Emit();
    SymbolTable* st = SymbolTable::active->FindClassTable(loc->GetName());
    if(st){
      SymbolTable::active = st;
    }
    else{
      PrintDebug("dev", "Error in Call Resolution");
    }
  }
  //prepended name with underscore
  const char * function_name = ('_' + std::string(field->GetName())).c_str();
  Location * loc = SymbolTable::active->Lookup(function_name);
  if(!loc){
      PrintDebug("dev", "Error in Call Resolution");
      SymbolTable::active->DebugSymbolTable();
      PrintDebug("dev", "---------------");
      return NULL;
  }

  Location * ret_loc = GENERATOR.GenLCall(function_name, (loc->GetType() != Type::nullType));
  for (int i = 0; i < actuals->NumElements(); i++){
    Location* param_loc =  actuals->Nth(i)->Emit();
    if(param_loc){
      GENERATOR.GenPushParam(param_loc);
    }
    else{
      PrintDebug("dev", "Error in Call's Parameter Resolution");
    }
  }
  SymbolTable::active = saved;
  return ret_loc;
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
