/*
 * VM assembly bison grammar.
 * ref: http://www.gnu.org/software/bison/manual/bison.html
 */

%{

/* Symbols */

struct vma_symbol {
	struct vma_symbol *next;
	const char *name;
	void *location; /* likely, an insn (TBD) */
};

struct vma_symref {
	union {
		const char *name;
		struct vma_symbol *sym;
	} u;
	int resolved; /* nonzero if this is a valid ref */
}

struct vma_symbol *vma_symtab = NULL;

extern struct vma_symbol *vma_lookup_symbol(const char *name);
extern struct vma_symbol *vma_define_symbol(const char *name);

/* Expressions */

enum vma_expr_type {
	EXPR_CONSTANT,
	EXPR_SYMREF,
	EXPR_OR,
	EXPR_AND,
	EXPR_XOR,
	EXPR_ADD,
	EXPR_SUB,
	EXPR_MUL,
	EXPR_DIV,
	EXPR_NEG,
	EXPR_NOT,
}

struct vma_expr_node {
	struct vma_expr_node *next; /* in expr_list */
	vma_expr_type type;
	union {
		int const_int;
		struct vma_expr_node *child[2]; /* lhs, rhs */
		struct vma_symref symref;
	} u;
};

/* Globals... */

int vma_errors = 0;

/* Helper functions */

extern void *vma_malloc(size_t count);
extern void vma_free(void *ptr);

extern void vma_error(const char *format, ...);
extern void vma_abort(const char *format, ...);

static struct vma_expr_node *vma_build_constant_expr(int value);
static struct vma_expr_node *vma_build_symref_expr(const char *name);
static struct vma_expr_node *vma_build_parent_expr
(
	vma_expr_type type,
	struct vma_expr_node *a,
	struct vma_expr_node *b
);

%}

%define api.value.type union
%token <int> INTEGER
%token <const char *> IDENTIFIER
%token NEWLINE
%token EOF
%token KW_DB
%token KW_DH
%token KW_DW
%token OP_ADD
%token OP_SUB
%token OP_LDC_8_U
%type <struct vma_symbol *> label
%type <struct vma_expr_node *> expr
%type <struct vma_expr_node *> expr_list
%left '|'
%left '&'
%left '^'
%left '+' '-'
%left '*' '/'
%precedence NEG
%precedence NOT

%%

input:
	  /* empty */
	| input line
;

line:
	  NEWLINE
	| stmt NEWLINE
	| stmt EOF
;

stmt:
	  insn
	| label insn
;

insn:
	  kw_insn
	| op_insn
;

kw_insn:
	  KW_DB expr_list
	| KW_DH expr_list
	| KW_DW expr_list
;

op_insn:
	  OP_ADD
	| OP_SUB
	| OP_LDC_8_U expr
;

label:
	  IDENTIFIER ':' { $$->value = vma_define_symbol($1->value); }
;

expr_list:
	  expr { $$->value = vma_build_expr_list($1->value); }
	| expr_list ',' expr { $$->value = vma_append_expr_list($1->value, $3->value); }
;

expr:
	  INTEGER { $$->value = vma_build_constant_expr($1->value); }
	| '@' IDENTIFIER { $$->value = vma_build_symref_expr($2->value); }
	| expr '|' expr { $$->value = vma_build_parent_expr(EXPR_OR, $1->value, $3->value); }
	| expr '&' expr { $$->value = vma_build_parent_expr(EXPR_AND, $1->value, $3->value); }
	| expr '^' expr { $$->value = vma_build_parent_expr(EXPR_XOR, $1->value, $3->value); }
	| expr '+' expr { $$->value = vma_build_parent_expr(EXPR_ADD, $1->value, $3->value); }
	| expr '-' expr { $$->value = vma_build_parent_expr(EXPR_SUB, $1->value, $3->value); }
	| expr '*' expr { $$->value = vma_build_parent_expr(EXPR_MUL, $1->value, $3->value); }
	| expr '/' expr { $$->value = vma_build_parent_expr(EXPR_DIV, $1->value, $3->value); }
	| '-' expr %prec NEG { $$->value = vma_build_parent_expr(EXPR_NEG, $1->value, NULL); }
	| '~' expr %prec NOT { $$->value = vma_build_parent_expr(EXPR_NOT, $1->value, NULL); }
	| '(' expr ')' { $$->value = $2->value; }
;

%%

void *vma_malloc(size_t count)
{
	void *result = malloc(count);
	if (!result) {
		vma_abort("out of memory");
	}
	return result;
}

void vma_free(void *ptr)
{
	free(ptr);
}

struct vma_symbol *vma_lookup_symbol(const char *name)
{
	struct vma_symbol *curr = vma_symtab;
	while (curr) {
		if (!strcmp(name, curr->name)) {
			break;
		}
		curr = curr->next;
	}
	return curr;
}

struct vma_symbol *vma_define_symbol(const char *name)
{
	struct vma_symbol *sym = vma_lookup_symbol(name);
	if (sym) {
		vma_error("symbol '%s' already defined", name);
		return NULL;
	}

	sym = (struct vma_symbol *)malloc(sizeof(*sym));

	sym->next = vma_symtab;
	sym->name = name;
	sym->location = NULL;

	return sym;
}

static struct vma_expr_node *vma_build_constant_expr(int value)
{
	struct vma_expr_node *node = (struct vma_expr_node *)vma_malloc(sizeof(*node));

	node->next = NULL;
	node->type = EXPR_CONSTANT;
	node->u.const_int = value;

	return node;
}

static struct vma_expr_node *vma_build_symref_expr(const char *name)
{
	struct vma_expr_node *node = (struct vma_expr_node *)malloc(sizeof(*node));

	node->next = NULL;
	node->type = EXPR_SYMREF;
	node->u.symref.name = name; /* copy already made by lexer */
	node->u.symref.resolved = 0;

	return node;
}

static struct vma_expr_node *vma_build_parent_expr
(
	vma_expr_type type,
	struct vma_expr_node *a,
	struct vma_expr_node *b
)
{
	struct vma_expr_node *node = (struct vma_expr_node *)malloc(sizeof(*node));

	node->next = NULL;
	node->type = type;
	node->u.child[0] = a;
	node->u.child[1] = b;

	return node;
}

