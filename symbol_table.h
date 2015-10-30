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

 public:
  static SymbolTable *active;
  SymbolTable();
  SymbolTable(Location *l);
  static void SwitchActive(SymbolTable * new_active);
  Location *Lookup(const char * label);
  void Add(Location *loc);
  SymbolTable *FindClassTable(char * type_name);
  Location * GetClassName() {return class_name;}
  SymbolTable *GetParent() {return parent;}
};

#endif
