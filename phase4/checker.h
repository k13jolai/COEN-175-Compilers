/*
 * File:	checker.h
 *
 * Description:	This file contains the public function declarations for the
 *		semantic checker for Simple C.
 */
#include <map>
# ifndef CHECKER_H
# define CHECKER_H
# include "Scope.h"
Scope *openScope();
Scope *closeScope();

void defineStructure(const std::string&name, Scope *scope);
Symbol *defineFunction(const std::string &name, const Type &type);
Symbol *declareFunction(const std::string &name, const Type &type);
Symbol *declareParameter(const std::string &name, const Type &type);
Symbol *declareVariable(const std::string &name, const Type &type);
Symbol *checkIdentifier(const std::string &name);
# endif /* CHECKER_H */

