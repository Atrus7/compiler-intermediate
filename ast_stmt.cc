/* File: ast_stmt.cc
 * -----------------
 * Implementation of statement node classes.
 */
#include "ast_stmt.h"
#include "ast_type.h"
#include "ast_decl.h"
#include "ast_expr.h"
#include "codegen.h"
#include "symbol_table.h"


Program::Program(List<Decl*> *d) {
    Assert(d != NULL);
    (decls=d)->SetParentAll(this);
}

void Program::Check() {
    /* You can use your pp3 semantic analysis or leave it out if
     * you want to avoid the clutter.  We won't test pp5 against
     * semantically-invalid programs.
     */
}
void Program::Declare() {
  SymbolTable *global = new SymbolTable();
  decls->DeclareForAll();
}
Location * Program::Emit() {
    /* pp5: here is where the code generation is kicked off.
     *      The general idea is perform a tree traversal of the
     *      entire program, generating instructions as you go.
     *      Each node can have its own way of translating itself,
     *      which makes for a great use of inheritance and
     *      polymorphism in the node classes.
     */
  decls->EmitForAll();
  GENERATOR.DoFinalCodeGen();
  return NULL;

}

StmtBlock::StmtBlock(List<VarDecl*> *d, List<Stmt*> *s) {
    Assert(d != NULL && s != NULL);
    (decls=d)->SetParentAll(this);
    (stmts=s)->SetParentAll(this);
}
void StmtBlock::Declare() {
  decls->DeclareForAll();
  stmts->DeclareForAll();
}
Location * StmtBlock::Emit() {
  decls->EmitForAll();
  stmts->EmitForAll();
  return NULL;
}

ConditionalStmt::ConditionalStmt(Expr *t, Stmt *b) {
    Assert(t != NULL && b != NULL);
    (test=t)->SetParent(this);
    (body=b)->SetParent(this);
}

void  ConditionalStmt::Declare() {
  body->Declare();
}

Location * ConditionalStmt::Emit() {
  test->Emit();
  body->Emit();
  return NULL;
}

ForStmt::ForStmt(Expr *i, Expr *t, Expr *s, Stmt *b): LoopStmt(t, b) {
    Assert(i != NULL && t != NULL && s != NULL && b != NULL);
    (init=i)->SetParent(this);
    (step=s)->SetParent(this);
}
Location * LoopStmt::Emit() {
  return NULL;
}

Location * ForStmt::Emit() {
  LoopStmt::Emit();
  init->Emit();
  step->Emit();
  return NULL;
}

IfStmt::IfStmt(Expr *t, Stmt *tb, Stmt *eb): ConditionalStmt(t, tb) {
    Assert(t != NULL && tb != NULL); // else can be NULL
    elseBody = eb;
    if (elseBody) elseBody->SetParent(this);
}

Location * IfStmt::Emit() {
  ConditionalStmt::Emit();
  elseBody->Emit();
  return NULL;
}

ReturnStmt::ReturnStmt(yyltype loc, Expr *e) : Stmt(loc) {
    Assert(e != NULL);
    (expr=e)->SetParent(this);
}
Location * ReturnStmt::Emit() {
  expr->Emit();
  return NULL;
}

PrintStmt::PrintStmt(List<Expr*> *a) {
    Assert(a != NULL);
    (args=a)->SetParentAll(this);
}
Location * PrintStmt::Emit() {
  PrintDebug("dev", "Print should be emitting");
  for (int i = 0; i < args->NumElements(); i++){
    Expr * param_expr = args->Nth(i);
    Location * param_loc = param_expr->Emit();
    Type *type = param_expr->GetType();
    if(!type || type == Type::nullType){
      type = param_loc->GetType();
    }
    if(type == Type::intType || type == Type::boolType){
      GENERATOR.GenBuiltInCall(PrintInt, param_loc, NULL);
    }
    else if(type == Type::stringType){
      GENERATOR.GenBuiltInCall(PrintString, param_loc, NULL);
    }
    else{
      PrintDebug("dev", "Just hitting the else...");
    }

  }

  args->EmitForAll();
  return NULL;
}
