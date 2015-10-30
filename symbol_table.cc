#include "symbol_table.h"
static SymbolTable *active_symbol_table = NULL;

SymbolTable::SymbolTable(){
  if(active_symbol_table){ // assuming we will be able to always track the active_symbol_table
    parent = active_symbol_table;
  }
  else { //first symbol table will be root
    active_symbol_table = this;
  }
}

//SymbolTable::SymbolTable(Location *l): SymbolTable(){class_name = l;}
Location *SymbolTable::Lookup(const char* label){
  for (int i = 0; i < symbols.NumElements(); i++){
    Location * symbol = symbols.Nth(i);
    if (label == symbol->GetName()){
      return symbol;
    }
  }
  if(parent != NULL){
    return parent->Lookup(label);
  }
  else{
    return NULL;
  }
}

void SymbolTable::Add(Location *loc){
  Assert(loc);
  if(Lookup(loc->GetName())){
    symbols.Append(loc);
  }
}

SymbolTable *SymbolTable::FindClassTable(char * type_name){
  if (type_name == class_name->GetName()){
    return this;
  }
  if(parent != NULL){
    return parent->FindClassTable(type_name);
  }
  else{
    return NULL;
  }
}
