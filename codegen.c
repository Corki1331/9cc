#include "9cc.h"

Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_node_num(int val){
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->val = val;
  return node;
}

void error_at(char *loc, char *fmt, ...) {
  va_list ap;
  va_start(ap,fmt);

  int pos = loc - user_input;
  fprintf(stderr, "%s\n", user_input);
  fprintf(stderr, "%*s", pos, "");
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

bool consume(char *op) {
  if (token->kind != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len)){
    return false;
  }
  token = token->next;
  return true;
}

void expect(char *op){
  if (token->kind != TK_RESERVED || token->str[0] != *op){
    error_at(token->str, "expected '%c'", *op);
  }
  token = token->next;
}

int expect_number() {
  if(token->kind != TK_NUM){
    error_at(token->str, "expected a number");
  }
  int val = token->val;
  token = token->next;
  return val;
}

bool expect_char(){
  if (token->kind != TK_IDENT){
    return false;
  }
  return true;
}

bool at_eof() {
  if(token->kind != TK_EOF){
    return false;
  }
  return true;
}

LVar *find_lvar(char *target_name) {
  for (LVar *var = lvar_head.next; var; var = var->next){
    if (!memcmp(target_name, var->name, var->len))
      return var;
  }
  return NULL;
}

Node *program() {
  int i = 0;
  while (!at_eof()){
    code[i++] = stmt();
  }
  code[i] = NULL;
  //return stmt();
}

Node *stmt() {
  Node *node = expr();
  expect(";");
  return node;
}

Node *expr(){
  return assign();
}

Node *assign() {
  Node *node = equality();
  if (consume("=")){
    node = new_node(ND_ASSIGN, node, assign());
  }
  return node;
}

Node *equality(){
  Node *node = relational();

  for (;;){
    if (consume("==")){
      node = new_node(ND_EQU, node, relational());
    } else if(consume("!=")){
      node = new_node(ND_NEQ, node, relational());
    } else {
      return node;
    }
  }
}

Node *relational(){
  Node *node = add();

  for(;;){
    if (consume("<")){
      node = new_node(ND_LTN, node, add());
    } else if (consume("<=")){
      node = new_node(ND_LEQ, node, add());
    } else if (consume(">")){
      node = new_node(ND_GTN, node, add());
    } else if (consume(">=")){
      node = new_node(ND_GEQ, node, add());
    } else {
      return node;
    }
  }
}

Node *add(){
  Node *node = mul();

  for(;;){
    if (consume("+")){
      node = new_node(ND_ADD, node, mul());
    } else if (consume("-")){
      node = new_node(ND_SUB, node, mul());
    } else {
      return node;
    }
  }
}

Node *mul() {
  Node *node = unary();

  for (;;){
    if (consume("*"))
      node = new_node(ND_MUL, node, unary());
    else if (consume("/"))
      node = new_node(ND_DIV, node, unary());
    else
      return node;
  }
}

Node *unary(){
  if (consume("+")){
    return unary();
  } else if(consume("-")) {
    return new_node(ND_SUB, new_node_num(0), unary()); 
  }
  return primary();
}

Node *primary() {
  if (consume("(")){
    Node *node = expr();
    expect(")");
    return node;
  }

  bool is_char = expect_char();
  if (is_char) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_LVAR;

    LVar *lvar = find_lvar(token->str);
    if (lvar) {
      node->offset = lvar->offset;
      token = token->next;
    }else {
      lvar = calloc(1, sizeof(LVar));
      lvar->next = NULL;
      cur_lvar->next = lvar;
      lvar->name = token->str;
      lvar->len = token->len;
      lvar->offset = cur_lvar->offset + 8;
      node->offset = lvar->offset;
      cur_lvar = lvar;
      token = token->next;
    }
    return node;
  }

  return new_node_num(expect_number());
}

void gen_lval(Node *node) {
  if (node->kind != ND_LVAR)
    fprintf(stderr,"left value is not val");
  printf("  mov rax, rbp\n");
  printf("  sub rax, %d\n", node->offset);
  printf("  push rax\n");
}

void gen(Node *node) {
  switch (node->kind)
  {
  case ND_NUM:
    printf("  push %d\n", node->val);
    return;
  case ND_LVAR:
    gen_lval(node);
    printf("  pop rax\n");
    printf("  mov rax, [rax]\n");
    printf("  push rax\n");
    return;
  case ND_ASSIGN:
    gen_lval(node->lhs);
    gen(node->rhs);
    printf("  pop rdi\n");
    printf("  pop rax\n");
    printf("  mov [rax], rdi\n");
    printf("  push rdi\n");
    return;
  }

  gen(node->lhs);
  gen(node->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  switch (node->kind) {
    case ND_ADD:
      printf("  add rax, rdi\n");
      break;
    case ND_SUB:
      printf("  sub rax, rdi\n");
      break;
    case ND_MUL:
      printf("  imul rax, rdi\n");
      break;
    case ND_DIV:
      printf("  cqo\n");
      printf("  idiv rdi\n");
      break;
    case ND_EQU:
      printf("  cmp rax, rdi\n");
      printf("  sete al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_NEQ:
      printf("  cmp rax, rdi\n");
      printf("  setne al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_LTN:
      printf("  cmp rax, rdi\n");
      printf("  setl al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_LEQ:
      printf("  cmp rax, rdi\n");
      printf("  setle al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_GTN:
      printf("  cmp rax, rdi\n");
      printf("  setg al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_GEQ:
      printf("  cmp rax, rdi\n");
      printf("  setge al\n");
      printf("  movzb rax, al\n");
      break;
  }

  printf("  push rax\n");
}

