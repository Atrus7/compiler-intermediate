#include "symbol_table.h"
SymbolTable *SymbolTable::active = NULL;

SymbolTable::SymbolTable() : symbols(){
  class_name = NULL;
  if(active){ // assuming we will be able to always track the active
    parent = active;
    active = this;
  }
  else { //first symbol table will be root
    parent = NULL;
    active = this;
  }
}

SymbolTable::SymbolTable(Location *l) : symbols(){
  class_name = l;
  if(active){ // assuming we will be able to always track the active
    parent = active;
    active = this;
  }
  else { //first symbol table will be root
    parent = NULL;
    active = this;
  }

}


//SymbolTable::SymbolTable(Location *l): SymbolTable(){class_name = l;}
Location *SymbolTable::Lookup(const char* label){
  std::list<Location *>::iterator it = symbols.begin();
    for (; it != symbols.end(); ++it){
    Location * symbol = *it;
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
    symbols.push_back(loc);
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

void SymbolTable::SwitchActive(SymbolTable * new_active) {
  active = new_active;
}
