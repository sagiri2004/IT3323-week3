/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdlib.h>

#include "reader.h"
#include "scanner.h"
#include "parser.h"
#include "error.h"

Token *currentToken;
Token *lookAhead;

void scan(void) {
  Token* tmp = currentToken;
  currentToken = lookAhead;
  lookAhead = getValidToken();
  free(tmp);
}

void eat(TokenType tokenType) {
  if (lookAhead->tokenType == tokenType) {
    printToken(lookAhead);
    scan();
  } else missingToken(tokenType, lookAhead->lineNo, lookAhead->colNo);
}

void compileProgram(void) {
  assert("Parsing a Program ....");
  eat(KW_PROGRAM);
  eat(TK_IDENT);
  eat(SB_SEMICOLON);
  compileBlock();
  eat(SB_PERIOD);
  assert("Program parsed!");
}

void compileBlock(void) {
  assert("Parsing a Block ....");
  if (lookAhead->tokenType == KW_CONST) {
    eat(KW_CONST);
    compileConstDecl();
    compileConstDecls();
    compileBlock2();
  } 
  else compileBlock2();
  assert("Block parsed!");
}

void compileBlock2(void) {
  if (lookAhead->tokenType == KW_TYPE) {
    eat(KW_TYPE);
    compileTypeDecl();
    compileTypeDecls();
    compileBlock3();
  } 
  else compileBlock3();
}

void compileBlock3(void) {
  if (lookAhead->tokenType == KW_VAR) {
    eat(KW_VAR);
    compileVarDecl();
    compileVarDecls();
    compileBlock4();
  } 
  else compileBlock4();
}

void compileBlock4(void) {
  compileSubDecls();
  compileBlock5();
}

void compileBlock5(void) {
  eat(KW_BEGIN);
  compileStatements();
  eat(KW_END);
}

void compileConstDecls(void) {
  if (lookAhead->tokenType == TK_IDENT) {
    compileConstDecl();
    compileConstDecls();
  }
}

void compileConstDecl(void) {
  if (lookAhead->tokenType != TK_IDENT)
    error(ERR_INVALIDCONSTANT, lookAhead->lineNo, lookAhead->colNo);
  eat(TK_IDENT);
  eat(SB_EQ);
  compileConstant();
  eat(SB_SEMICOLON);
}

void compileTypeDecls(void) {
  if (lookAhead->tokenType == TK_IDENT) {
    compileTypeDecl();
    compileTypeDecls();
  }
}

void compileTypeDecl(void) {
  if (lookAhead->tokenType != TK_IDENT)
    error(ERR_INVALIDTYPE, lookAhead->lineNo, lookAhead->colNo);
  eat(TK_IDENT);
  eat(SB_EQ);
  compileType();
  eat(SB_SEMICOLON);
}

void compileVarDecls(void) {
  if (lookAhead->tokenType == TK_IDENT) {
    compileVarDecl();
    compileVarDecls();
  }
}

void compileVarDecl(void) {
  if (lookAhead->tokenType != TK_IDENT)
    error(ERR_INVALIDTYPE, lookAhead->lineNo, lookAhead->colNo);

  /* identifier list */
  eat(TK_IDENT);
  while (lookAhead->tokenType == SB_COMMA) {
    eat(SB_COMMA);
    eat(TK_IDENT);
  }

  eat(SB_COLON);
  compileType();
  eat(SB_SEMICOLON);
}

void compileSubDecls(void) {
  assert("Parsing subtoutines ....");
  while (lookAhead->tokenType == KW_FUNCTION ||
         lookAhead->tokenType == KW_PROCEDURE) {
    if (lookAhead->tokenType == KW_FUNCTION)
      compileFuncDecl();
    else
      compileProcDecl();
  }
  assert("Subtoutines parsed ....");
}

void compileFuncDecl(void) {
  assert("Parsing a function ....");
  eat(KW_FUNCTION);
  eat(TK_IDENT);
  if (lookAhead->tokenType == SB_LPAR)
    compileParams();
  eat(SB_COLON);
  compileBasicType();
  eat(SB_SEMICOLON);
  compileBlock();
  eat(SB_SEMICOLON);
  assert("Function parsed ....");
}

void compileProcDecl(void) {
  assert("Parsing a procedure ....");
  eat(KW_PROCEDURE);
  eat(TK_IDENT);
  if (lookAhead->tokenType == SB_LPAR)
    compileParams();
  eat(SB_SEMICOLON);
  compileBlock();
  eat(SB_SEMICOLON);
  assert("Procedure parsed ....");
}

void compileUnsignedConstant(void) {
  switch (lookAhead->tokenType) {
  case TK_NUMBER:
  case TK_CHAR:
  case TK_IDENT:
    eat(lookAhead->tokenType);
    break;
  default:
    error(ERR_INVALIDCONSTANT, lookAhead->lineNo, lookAhead->colNo);
  }
}

void compileConstant(void) {
  switch (lookAhead->tokenType) {
  case SB_PLUS:
  case SB_MINUS:
    eat(lookAhead->tokenType);
    /* fall through */
  default:
    compileConstant2();
    break;
  }
}

void compileConstant2(void) {
  switch (lookAhead->tokenType) {
  case TK_NUMBER:
  case TK_CHAR:
  case TK_IDENT:
    compileUnsignedConstant();
    break;
  default:
    error(ERR_INVALIDCONSTANT, lookAhead->lineNo, lookAhead->colNo);
  }
}

void compileType(void) {
  if (lookAhead->tokenType == KW_ARRAY) {
    eat(KW_ARRAY);
    eat(SB_LSEL);
    compileUnsignedConstant();
    eat(SB_RSEL);
    eat(KW_OF);
    compileBasicType();
  } else {
    /* simple/basic/alias type */
    compileBasicType();
  }
}

void compileBasicType(void) {
  switch (lookAhead->tokenType) {
  case KW_INTEGER:
  case KW_CHAR:
  case TK_IDENT:
    eat(lookAhead->tokenType);
    break;
  default:
    error(ERR_INVALIDBASICTYPE, lookAhead->lineNo, lookAhead->colNo);
  }
}

void compileParams(void) {
  eat(SB_LPAR);
  compileParam();
  compileParams2();
  eat(SB_RPAR);
}

void compileParams2(void) {
  if (lookAhead->tokenType == SB_SEMICOLON) {
    eat(SB_SEMICOLON);
    compileParam();
    compileParams2();
  }
}

void compileParam(void) {
  if (lookAhead->tokenType != TK_IDENT)
    error(ERR_INVALIDPARAM, lookAhead->lineNo, lookAhead->colNo);
  eat(TK_IDENT);
  eat(SB_COLON);
  compileBasicType();
}

void compileStatements(void) {
  switch (lookAhead->tokenType) {
  case TK_IDENT:
  case KW_CALL:
  case KW_BEGIN:
  case KW_IF:
  case KW_WHILE:
  case KW_FOR:
  case KW_REPEAT:
    compileStatement();
    compileStatements2();
    break;
  default:
    /* epsilon: empty statement sequence */
    break;
  }
}

void compileStatements2(void) {
  if (lookAhead->tokenType == SB_SEMICOLON) {
    eat(SB_SEMICOLON);
    switch (lookAhead->tokenType) {
    case TK_IDENT:
    case KW_CALL:
    case KW_BEGIN:
    case KW_IF:
    case KW_WHILE:
    case KW_FOR:
    case KW_REPEAT:
      compileStatement();
      compileStatements2();
      break;
    default:
      /* possible empty statement after last semicolon */
      break;
    }
  }
}

void compileStatement(void) {
  switch (lookAhead->tokenType) {
  case TK_IDENT:
    compileAssignSt();
    break;
  case KW_CALL:
    compileCallSt();
    break;
  case KW_BEGIN:
    compileGroupSt();
    break;
  case KW_IF:
    compileIfSt();
    break;
  case KW_WHILE:
    compileWhileSt();
    break;
  case KW_FOR:
    compileForSt();
    break;
  case KW_REPEAT:
    compileRepeatSt();
    break;
    // EmptySt needs to check FOLLOW tokens
  case SB_SEMICOLON:
  case KW_END:
  case KW_ELSE:
    break;
    // Error occurs
  default:
    error(ERR_INVALIDSTATEMENT, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

void compileAssignSt(void) {
  assert("Parsing an assign statement ....");
  if (lookAhead->tokenType != TK_IDENT)
    error(ERR_INVALIDSTATEMENT, lookAhead->lineNo, lookAhead->colNo);
  /* left-hand side: one or more variables (optionally indexed) */
  eat(TK_IDENT);
  compileIndexes();
  while (lookAhead->tokenType == SB_COMMA) {
    eat(SB_COMMA);
    if (lookAhead->tokenType != TK_IDENT)
      error(ERR_INVALIDSTATEMENT, lookAhead->lineNo, lookAhead->colNo);
    eat(TK_IDENT);
    compileIndexes();
  }
  eat(SB_ASSIGN);
  /* right-hand side: one or more expressions */
  compileExpression();
  while (lookAhead->tokenType == SB_COMMA) {
    eat(SB_COMMA);
    compileExpression();
  }
  assert("Assign statement parsed ....");
}

void compileCallSt(void) {
  assert("Parsing a call statement ....");
  eat(KW_CALL);
  eat(TK_IDENT);
  if (lookAhead->tokenType == SB_LPAR)
    compileArguments();
  assert("Call statement parsed ....");
}

void compileGroupSt(void) {
  assert("Parsing a group statement ....");
  eat(KW_BEGIN);
  compileStatements();
  eat(KW_END);
  assert("Group statement parsed ....");
}

void compileIfSt(void) {
  assert("Parsing an if statement ....");
  eat(KW_IF);
  compileCondition();
  eat(KW_THEN);
  compileStatement();
  if (lookAhead->tokenType == KW_ELSE) 
    compileElseSt();
  assert("If statement parsed ....");
}

void compileElseSt(void) {
  eat(KW_ELSE);
  compileStatement();
}

void compileWhileSt(void) {
  assert("Parsing a while statement ....");
  eat(KW_WHILE);
  compileCondition();
  eat(KW_DO);
  compileStatement();
  assert("While statement pased ....");
}

void compileForSt(void) {
  assert("Parsing a for statement ....");
  eat(KW_FOR);
  if (lookAhead->tokenType != TK_IDENT)
    error(ERR_INVALIDSTATEMENT, lookAhead->lineNo, lookAhead->colNo);
  eat(TK_IDENT);
  eat(SB_ASSIGN);
  compileExpression();
  eat(KW_TO);
  compileExpression();
  eat(KW_DO);
  compileStatement();
  assert("For statement parsed ....");
}

void compileRepeatSt(void) {
  assert("Parsing a repeat statement ....");
  eat(KW_REPEAT);
  /* at least one statement inside the repeat body */
  compileStatement();
  while (lookAhead->tokenType == SB_SEMICOLON) {
    eat(SB_SEMICOLON);
    if (lookAhead->tokenType == KW_UNTIL)
      break;
    compileStatement();
  }
  eat(KW_UNTIL);
  compileCondition();
  assert("Repeat statement parsed ....");
}

void compileArguments(void) {
  eat(SB_LPAR);
  compileExpression();
  compileArguments2();
  eat(SB_RPAR);
}

void compileArguments2(void) {
  if (lookAhead->tokenType == SB_COMMA) {
    eat(SB_COMMA);
    compileExpression();
    compileArguments2();
  }
}

void compileCondition(void) {
  compileExpression();
  compileCondition2();
}

void compileCondition2(void) {
  switch (lookAhead->tokenType) {
  case SB_EQ:
  case SB_NEQ:
  case SB_LT:
  case SB_LE:
  case SB_GT:
  case SB_GE: {
    TokenType op = lookAhead->tokenType;
    eat(op);
    compileExpression();
    break;
  }
  default:
    /* epsilon */
    break;
  }
}

void compileExpression(void) {
  assert("Parsing an expression");
  compileExpression2();
  assert("Expression parsed");
}

void compileExpression2(void) {
  compileTerm();
  compileExpression3();
}


void compileExpression3(void) {
  switch (lookAhead->tokenType) {
  case SB_PLUS:
  case SB_MINUS: {
    TokenType op = lookAhead->tokenType;
    eat(op);
    compileTerm();
    compileExpression3();
    break;
  }
  default:
    /* epsilon */
    break;
  }
}

void compileTerm(void) {
  compileFactor();
  compileTerm2();
}

void compileTerm2(void) {
  switch (lookAhead->tokenType) {
  case SB_TIMES:
  case SB_SLASH: {
    TokenType op = lookAhead->tokenType;
    eat(op);
    compileFactor();
    compileTerm2();
    break;
  }
  default:
    /* epsilon */
    break;
  }
}

void compileFactor(void) {
  switch (lookAhead->tokenType) {
  case TK_IDENT:
    eat(TK_IDENT);
    compileIndexes();
    if (lookAhead->tokenType == SB_LPAR)
      compileArguments();
    break;
  case TK_NUMBER:
  case TK_CHAR:
    compileUnsignedConstant();
    break;
  case SB_LPAR:
    eat(SB_LPAR);
    compileExpression();
    eat(SB_RPAR);
    break;
  default:
    error(ERR_INVALIDFACTOR, lookAhead->lineNo, lookAhead->colNo);
  }
}

void compileIndexes(void) {
  if (lookAhead->tokenType == SB_LSEL) {
    eat(SB_LSEL);
    compileExpression();
    eat(SB_RSEL);
    compileIndexes();
  }
}

int compile(char *fileName) {
  if (openInputStream(fileName) == IO_ERROR)
    return IO_ERROR;

  currentToken = NULL;
  lookAhead = getValidToken();

  compileProgram();

  free(currentToken);
  free(lookAhead);
  closeInputStream();
  return IO_SUCCESS;

}
