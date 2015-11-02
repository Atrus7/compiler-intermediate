/* File: symbol_table.h
 * ---------------
 * The Symbol table class defines a tree data structure where the root
 * will be the global symbol table.
 * A symbol table will be created for all class declarations.
 * The symbol table will serve as a way to look up locations and eventually
 * used for dynamic dispatch.
 * ---------------
 * Author: Christopher Findeisen
 */

#ifndef _H_symbol_table
#define _H_symbol_table

#include <list>
#include "tac.h"

class SymbolTable {
 protected:
  SymbolTable * parent;
  Location * class_name;
  std::list<Location *> symbols;
  int offset;

 public:
  static SymbolTable *active;
  SymbolTable();
  SymbolTable(Location *l);
  static void SwitchActive(SymbolTable * new_active);
  //prints symbols in table
  void DebugSymbolTable();
  Location *Lookup(const char * label);
  void Add(const char * name, bool is_param, Type *type=NULL);
  SymbolTable *FindClassTable(const char * type_name) ;
  Location * GetClass() {return class_name;}
  const char * GetClassName() {if(class_name != NULL) return class_name->GetName(); else return "";}
  SymbolTable *GetParent() {return parent;}
  Segment GetSegment() {if(parent) return fpRelative; else return gpRelative;}
};

#endif
