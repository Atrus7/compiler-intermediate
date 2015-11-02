#include "symbol_table.h"
#include "codegen.h"
#include <string>
#include <cstring>
SymbolTable *SymbolTable::active = NULL;

SymbolTable::SymbolTable() : symbols(), offset(CodeGenerator::OffsetToFirstGlobal){
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

SymbolTable::SymbolTable(Location *l) : symbols(), offset(CodeGenerator::OffsetToFirstLocal){
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
    if (std::strcmp(label, symbol->GetName()) ==0 ){
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


void SymbolTable::Add(const char * name, bool is_param, Type *type){
  static int param_offset = CodeGenerator::OffsetToFirstParam;

  Location *loc  = new Location(GetSegment(), ((is_param)?param_offset : offset), name);
  //will need to modify this for arrays
  if(GetSegment() == fpRelative){
    if(!is_param) offset -=4;
    if(is_param) param_offset +=4;
  }
  else{
    offset +=4;
  }
  if(type){
    loc->SetType(type);
  }
  symbols.push_back(loc);
}

SymbolTable *SymbolTable::FindClassTable(const char * type_name){
  if (std::strcmp(type_name, class_name->GetName()) == 0 ){
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

void SymbolTable::DebugSymbolTable(){
  std::list<Location *>::iterator it = symbols.begin();
  for (; it != symbols.end(); ++it){
    Location * symbol = *it;
    PrintDebug("dev", symbol->GetName());
  }
  if(parent != NULL){
    parent->DebugSymbolTable();
  }
}
