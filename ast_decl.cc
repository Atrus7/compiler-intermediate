/* File: ast_decl.cc
 * -----------------
 * Implementation of Decl node classes.
 */
#include "ast_decl.h"
#include "ast_type.h"
#include "ast_stmt.h"
#include "symbol_table.h"
#include <cstring>
#include <string>


Decl::Decl(Identifier *n) : Node(*n->GetLocation()) {
    Assert(n != NULL);
    (id=n)->SetParent(this);
}


VarDecl::VarDecl(Identifier *n, Type *t) : Decl(n) {
    Assert(n != NULL && t != NULL);
    (type=t)->SetParent(this);
}

Location * VarDecl::EmitFormal() {
  GENERATOR.GenLabel(id->GetName());
  Location * formal = SymbolTable::active->Lookup(id->GetName());
  GENERATOR.GenLoadLabel(id->GetName());
  //GENERATOR.GenLoad(formal, 0);
  return NULL;
}
Location * VarDecl::Emit() {
  GENERATOR.GenLabel(id->GetName());
  GENERATOR.GenLoadLabel(id->GetName());
  id->Emit();
  type->Emit();
  return NULL;
}

void VarDecl::Declare(){
  SymbolTable::active->Add(id->GetName(), false, type);
}

ClassDecl::ClassDecl(Identifier *n, NamedType *ex, List<NamedType*> *imp, List<Decl*> *m) : Decl(n) {
  // extends can be NULL, impl & mem may be empty lists but cannot be NULL
  Assert(n != NULL && imp != NULL && m != NULL);
  extends = ex;
  if (extends) extends->SetParent(this);
  (implements=imp)->SetParentAll(this);
  (members=m)->SetParentAll(this);
  //GENERATOR
}
void ClassDecl::Declare(){
  char * class_name = id->GetName();
  Location * declared_variable = GENERATOR.GenLoadLabel(class_name);
  class_table = new SymbolTable(declared_variable);
  SymbolTable::active->Add(id->GetName(), false, new NamedType(id));
  SymbolTable::SwitchActive(class_table);
  members->DeclareForAll();
  SymbolTable::SwitchActive(class_table->GetParent());
}

Location * ClassDecl::Emit() {
  SymbolTable::SwitchActive(class_table);
  GENERATOR.GenLabel(SymbolTable::active->GetClassName());
  id->Emit();
  if(extends) extends->Emit();
  implements->EmitForAll();
  members->EmitForAll();
  SymbolTable::SwitchActive(class_table->GetParent());
  return NULL;
}

InterfaceDecl::InterfaceDecl(Identifier *n, List<Decl*> *m) : Decl(n) {
  Assert(n != NULL && m != NULL);
  (members=m)->SetParentAll(this);
}

Location * InterfaceDecl::Emit() {
  id->Emit();
  members->EmitForAll();
  return NULL;
}


FnDecl::FnDecl(Identifier *n, Type *r, List<VarDecl*> *d) : Decl(n) {
  Assert(n != NULL && r!= NULL && d != NULL);
  (returnType=r)->SetParent(this);
  (formals=d)->SetParentAll(this);
  body = NULL;
}

void FnDecl::Declare(){
  std::string function_name = std::string(id->GetName());
  if(function_name != "main"){
    function_name = "_" + function_name;
  }
  SymbolTable::active->Add(function_name.c_str(), false, returnType);
  Location * declared_variable = SymbolTable::active->Lookup(function_name.c_str());
  fn_table = new SymbolTable(declared_variable);
  SymbolTable::SwitchActive(fn_table);

  for (int i = 0; i < formals->NumElements(); i++){
    VarDecl * var= formals->Nth(i);
    SymbolTable::active->Add(var->GetName(), true, var->GetType());
  }

  body->Declare();
  SymbolTable::SwitchActive(fn_table->GetParent());

}
Location * FnDecl::Emit() {
  //reset offsets since we'll add a stack frame
  //GENERATOR.ResetStackFrame();
  CodeGenerator::ResetStackFrame();
  SymbolTable::SwitchActive(fn_table);
  GENERATOR.GenLabel(SymbolTable::active->GetClassName());
  BeginFunc* func = GENERATOR.GenBeginFunc();
  func->SetFrameSize(28);
  id->Emit();
  for (int i = 0; i < formals->NumElements(); i++){
    formals->Nth(i)->EmitFormal();
  }
  body->Emit();
  returnType->Emit();
  GENERATOR.GenEndFunc();
  SymbolTable::SwitchActive(fn_table->GetParent());
  return NULL;
}
void FnDecl::SetFunctionBody(Stmt *b) {
  (body=b)->SetParent(this);
}
