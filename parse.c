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

int is_alnum(char c){
  return ('a' <= c && c <= 'z') ||
         ('A' <= c && c <= 'Z') ||
         ('0' <= c && c <= '9') ||
         (c == '_');
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

    if (strncmp(p, "if", 2) ==0 && !is_alnum(p[2])){
      cur = new_token(TK_IF, cur, p, 2);
      p += 2;
      continue;
    }

    if (strncmp(p, "return", 6) == 0 && !is_alnum(p[6])) {
      cur = new_token(TK_RETURN, cur, p, 6);
      p += 6;
      continue;
    }

    if (check_long_op(p, "==") || check_long_op(p, "!=") ||
        check_long_op(p, "<=") || check_long_op(p, ">=")){
      cur = new_token(TK_RESERVED, cur, p, 2);
      p += 2;
      continue;
    }

    if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' ||
        *p == ')' || *p == '=' || *p == '<' || *p == '>' || *p == ';' ||
        *p == '{' || *p == '}'){
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

    int var_len = 0;
    char *char_head = p;
    while('a' <= *p && *p <= 'z'){
        p++;
        var_len += 1;
    }
    cur = new_token(TK_IDENT, cur, char_head, var_len);
    continue;
    error_at(p, "expected a number");
  }
  new_token(TK_EOF, cur, p, 0);
  return head.next;
}