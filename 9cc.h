#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>

typedef enum {
  TK_RESERVED,
  TK_RETURN,
  TK_IF,
  TK_ELSE,
  TK_WHILE,
  TK_IDENT,
  TK_NUM,
  TK_EOF,
} TokenKind;

typedef struct Token Token;
struct Token {
  TokenKind kind;
  Token *next;
  int val;
  char *str;
  int len;
};

typedef enum {
  ND_ADD,
  ND_SUB,
  ND_MUL,
  ND_DIV,
  ND_ASSIGN,
  ND_LVAR,
  ND_NUM,
  ND_EQU,
  ND_NEQ,
  ND_LTN, // <
  ND_LEQ, // <=
  ND_GTN, // >
  ND_GEQ, // >=
  ND_RETURN,
  ND_IF,
  ND_IF_BRANCH,
  ND_WHILE,
} NodeKind;

typedef struct Node Node;
struct Node {
  NodeKind kind;
  Node *lhs;
  Node *rhs;
  int val;
  int offset;
};

typedef struct LVar LVar;
struct LVar {
    LVar *next;
    char *name;
    int len;
    int offset;
};


extern char *user_input;
extern Token *token;
extern Node *code[100];
extern LVar lvar_head;
extern LVar *cur_lvar;

void error_at(char *loc, char *fmt, ...);
bool consume(char *op);
void expect(char *op);
int expect_number();
bool check_long_op(char *p, char *target);
Token *new_token(TokenKind kind, Token *cur, char *str, int len);
Token *tokenize();

Node *new_node_num(int val);
Node *new_node(NodeKind kind, Node *lhs, Node *rhs);
Node *program();
Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();
void gen(Node *node);

extern int tmpnum;