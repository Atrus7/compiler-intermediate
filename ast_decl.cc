/* File: ast_decl.cc
 * -----------------
 * Implementation of Decl node classes.
 */
#include "ast_decl.h"
#include "ast_type.h"
#include "ast_stmt.h"
#include "symbol_table.h"
#include <cstring>


Decl::Decl(Identifier *n) : Node(*n->GetLocation()) {
    Assert(n != NULL);
    (id=n)->SetParent(this);
}


VarDecl::VarDecl(Identifier *n, Type *t) : Decl(n) {
    Assert(n != NULL && t != NULL);
    (type=t)->SetParent(this);
}

Location * VarDecl::Emit() {
  char * var_name = id->GetName();
  GENERATOR.GenLabel(var_name);
  Location * declared_variable = GENERATOR.GenLoadLabel(var_name);
  PrintDebug("dev", "Cmon");
  PrintDebug("dev", SymbolTable::active->GetClassName());

  SymbolTable::active->Add(declared_variable);
  id->Emit();
  type->Emit();
  return NULL;
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

Location * ClassDecl::Emit() {
  char * class_name = id->GetName();
  GENERATOR.GenLabel(class_name);
  Location * declared_variable = GENERATOR.GenLoadLabel(class_name);
  class_table = new SymbolTable(declared_variable);
  SymbolTable::active->Add(declared_variable);
  // Class SymbolTable should be added before we consider the members
  SymbolTable::SwitchActive(class_table);
  id->Emit();
  if(extends) extends->Emit();
  //class_table->AddExtend(ex);
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

Location * FnDecl::Emit() {
  //reset offsets since we'll add a stack frame
  //GENERATOR.ResetStackFrame();
  char * function_name = id->GetName();

  if(std::strcmp(function_name, "main") == 0){
    //do main stuff
  }

  GENERATOR.GenLabel(function_name);
  Location * declared_variable = GENERATOR.GenLoadLabel(function_name);
  SymbolTable::active->Add(declared_variable);
  SymbolTable *fn_table = new SymbolTable(declared_variable);
  SymbolTable::SwitchActive(fn_table);
  id->Emit();
  formals->EmitForAll();
  body->Emit();
  returnType->Emit();
  SymbolTable::SwitchActive(fn_table->GetParent());
  return NULL;
}
void FnDecl::SetFunctionBody(Stmt *b) {
  (body=b)->SetParent(this);
}
