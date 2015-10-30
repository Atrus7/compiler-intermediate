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
  static SymbolTable *active_symbol_table;
  SymbolTable * parent = NULL;
  Location * class_name = NULL;
  List<Location *> symbols;

 public:
  SymbolTable();
  SymbolTable(Location *l): SymbolTable(){class_name = l;}
  Location *Lookup(const char * label);
  void Add(Location *loc);
  SymbolTable *FindClassTable(char * type_name);
}

#endif
