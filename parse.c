#include "9cc.h"

Token *new_token(TokenKind kind, Token *cur, char *str, int len){
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  tok->len = len;
  cur->next = tok;
  return tok;
}

bool check_long_op(char *p, char *target){
  return memcmp(p, target, strlen(target)) == 0;
}

Token *tokenize(){
  char *p = user_input;
  Token head;
  head.next = NULL;
  Token *cur = &head;

  while (*p) {
    if(isspace(*p)){
      p++;
      continue;
    }

    if (check_long_op(p, "==") || check_long_op(p, "!=") ||
        check_long_op(p, "<=") || check_long_op(p, ">=")){
      cur = new_token(TK_RESERVED, cur, p, 2);
      p += 2;
      continue;
    }

    if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' ||
        *p == ')' || *p == '=' || *p == '<' || *p == '>' || *p == ';'){
      cur = new_token(TK_RESERVED, cur, p++, 1);
      continue;
    }

    if (isdigit(*p)) {
      cur = new_token(TK_NUM, cur, p, 0);
      char *tmp_len = p;
      cur->val = strtol(p, &p, 10);
      cur->len = p - tmp_len;
      continue;
    }

    if ('a' <= *p && *p <= 'z'){
      cur = new_token(TK_IDENT, cur, p++, 1);
      continue;
    }

    error_at(p, "expected a number");
  }
  new_token(TK_EOF, cur, p, 0);
  return head.next;
}