#ifndef FRONTENDINPUT_HPP

#define FRONTENDINPUT_HPP
#include<set>
#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<malloc.h>
#include<ctype.h>
#include<string.h>
#include <cstdarg>
#include<assert.h>
using namespace std;
#define TOKEN_BUF 5
enum
{
	TAC_UNDEF = 0,
	TAC_ADD,
	TAC_SUB,
	TAC_MUL,
	TAC_DIV,
	TAC_EQ,
	TAC_NE,
	TAC_LT,
	TAC_LE,
	TAC_GT,
	TAC_GE,
	TAC_AND,
	TAC_OR,
	TAC_MOD,
	TAC_IFEQ,
	TAC_IFNE,
	TAC_IFLT,
	TAC_IFLE,
	TAC_IFGT,
	TAC_IFGE,
	TAC_NEG,
	TAC_POSI,
	TAC_NOT, 
	TAC_COPY,
	TAC_GOTO,
	TAC_IFZ,
	TAC_BEGINFUNC,
	TAC_ENDFUNC,
	TAC_LABEL,
	TAC_VAR,
	TAC_ARR,
	TAC_FORMAL,
	TAC_ACTUAL,
	TAC_CALL,
	TAC_RETURN,
	TAC_ARR_L,
	TAC_ARR_R,
	TAC_LEA,
	TAC_FUNC,
	TAC_STR,
	TAC_ZERO,
	TAC_INSERT
};
enum
{
	SYM_UNDEF = 0,
	SYM_ARRAY,
	SYM_VAR,
	SYM_FUNC,
	SYM_TEXT,
	SYM_INT,
	SYM_LABEL,
	SYM_CONST_VAR,
	SYM_CONST_ARR
};
enum
{
	FUNC = 256,
	PRINT,
	RETURN,
	CONTINUE,
	BREAK,
	IF,
	ELSE,
	WHILE,
	INT,
	VOID,
	AND,
	OR,
	EQ,
	NE,
	LT,
	LE,
	GT,
	GE,
	UMINUS,
	INTEGER,
	IDENTIFIER,
	TEXT,
	CONST
};
enum
{
	HEAD = 0,
	BODY
};



typedef struct member
{
	int num;
	struct member* arr;
} member;

typedef struct symb
{
	int type;
	int store;
	char* name;
	int offset;
	int value;
	int label;
	int num;
	int eff;//for push_field and pop_field
	int con;//for const
	int formal; //for last_end
	struct member* val;
	struct member* arr;
	struct tac* address;
	struct symb* next;
} SYMB;
typedef struct tac
{
	struct tac* next;
	struct tac* prev;
	int op;
	SYMB* a;
	SYMB* b;
	SYMB* c;
} TAC;
typedef struct enode
{
	struct enode* next;
	TAC* tac;
	SYMB* ret;
} ENODE;
typedef struct regdesc /* Reg descriptor */
{
	struct symb* name; /* Thing in reg */
	int modified;      /* If needs spilling  */
} REGDESC;
typedef struct keyw
{
	int id;
	char name[100];
} KEYW;

typedef struct LHEAD
{
	int type;
	SYMB* symbol;
	TAC* tac;
	struct LHEAD* next_body;
	struct LHEAD* next_head;
} LHEAD;

typedef struct Table
{
	SYMB* symbol;
	Table* next;

}Table;

static int line;
static int lex_char;
static char token[50];
static int tos;
//static SYMB* symb_list;
static ENODE* enode_list;
static Table* global_symbol_table, * local_symbol_table;
static SYMB* field_head, * field_tail,*field_mid;
static int next_temp, next_label;
static int local_scope = 0;
static TAC* tac_head = NULL;
static SYMB* stack_head = NULL, * stack_tail = NULL,*stack_mid=NULL;
static int temp_count = 0;
static int label_count = 0;
static int count_for_rename = 0;
static int count_for_string = 0;
static struct
{
	int val;
	char* str;
} lex;

static struct
{
	int val;
	char* str;
} token_buf[5];

static struct
{
	int buf;
	int flu;
} band;

static KEYW keywords[100] =
{
	{INT, "int"},
	{PRINT, "print"},
	{RETURN, "return"},
	{CONTINUE, "continue"},
	{BREAK, "break"},
	{IF, "if"},
	{ELSE, "else"},
	{WHILE, "while"},
	{VOID, "void"},
	{CONST, "const"},
	{0, ""} };


void make_stirng_to_head(TAC *head);
//前端接口
void lex_and_parser(void);
void print_tac(void);

//递归下降
TAC* do_if(ENODE* cond, TAC* stmt);
TAC* do_while(ENODE* cond, TAC* stmt);
SYMB* declare_func(const char* name);
TAC* comp_unit(void);
TAC* const_decl(void);
TAC* var_decl(void);
TAC* const_def(void);
LHEAD* const_init_val(void);
ENODE* func_ret(void);
ENODE* add_exp(void);
ENODE* mul_exp(void);
ENODE* unary_exp(void);
TAC* var_def(void);
TAC* var_name_decl(int);
TAC* if_stmt(void);
TAC* while_stmt(void);
ENODE* exp(void);
ENODE* cond(void);
ENODE* const_exp(void);
ENODE* l_or_exp(void);
ENODE* l_and_exp(void);
ENODE* eq_exp(void);
ENODE* rel_exp(void);
TAC* comp_piece(void);
TAC* func_def(void);
TAC* func_f_params(void);
TAC* func_f_param(void);
TAC* block(void);
TAC* block_item(void);
TAC* stmt(void);
TAC* continue_stmt(void);
TAC* break_stmt(void);
TAC* return_stmt(void);
TAC* assign_stmt(void);
ENODE* l_val(void);
TAC* decl(void);
ENODE* func_r_params(void);
ENODE* primary_exp(void);

//lex缓冲池适用于LL(K)
void flush_band(void);
void add_band(void);
void get_token_buf(void);
void look_ahead_token();
void look_back_token();
int token_is_keyword();


//作用域标号管理，用于continue和break 适用于任意成对括号
void push_field(void);
void pop_field(void);
void push_two_field(SYMB** stack, SYMB** field);
void pop_two_field(SYMB** stack, SYMB** field);

//get name 
char* get_label_name(void);
char* get_temp_var_name(void);

//语法检查
void expect_val(int value);
void expect_str(const char* name);
int is_have_return(TAC* block);
void make_it_const(SYMB* int_decl);

//构造中间层节点
TAC* find_tac_tail(TAC* head);
ENODE* mknode_for_array(ENODE* pre, ENODE* exp, int num);
TAC* formal_var_tac(SYMB* symbol);
TAC* create_a_node(symb* a, symb* b, symb* c, int type);
TAC* join_tac(TAC* head, TAC* tail);
ENODE* mknode_for_unary(int type, ENODE* node);
ENODE* mknode_for_binary(ENODE* pre, ENODE* now, int type);
TAC* do_assign(const char* name, ENODE* head, ENODE* tail);
TAC* do_if_else(ENODE* cond, TAC* stmt0, TAC* stmt1);
TAC* declare_var_tac(SYMB* decl);
SYMB* make_var_to_arr(SYMB* array, int length);
LHEAD* symb_to_link_head(ENODE* symbol);
LHEAD* join_linker(LHEAD* front, LHEAD* back);
TAC* do_init_for_arr(SYMB* cur, LHEAD* head, int sel);
TAC* do_init_for_var(SYMB* var, LHEAD* head, int sel);
int count_arr_size(SYMB* array);
void make_lhead_std(int spice, LHEAD* head);
TAC* init_tac_every_dim(SYMB* var, int front, int end, LHEAD* head, int sel);
TAC* do_init_tac(TAC* var, LHEAD* head, int sel);
LHEAD* create_link_var(void);
LHEAD* create_link_arr(void);
SYMB* decl_var_name(int type, const char* name);
TAC* make_func(SYMB* head, TAC* parms, TAC* block);
TAC* create_another_node(TAC* head, symb* a, symb* b, symb* c, int type);
TAC* mktac_real_params(ENODE* head);

//常量折叠
int calc_unary_exp(int type, int value);
int calc_binary_exp(int left, int right, int type);
//SYM TO TAC
int lex_val_to_tac(int type);

//malloc相关
TAC* create_a_tac(void);
void all_free(int size, ...);
void* safe_malloc(size_t size, const char* name);
void safe_free(int size, ...);
//词法分析
int yylext(void);
int get_token(void);
int get_an_id(void);
int get_an_eq(void);
int get_a_not(void);
int get_a_string(void);
int get_an_other();
int get_a_great(void);
int get_a_less(void);
int get_an_and(void);
int get_an_or(void);
int get_a_digit(void);
void get_a_hex(void);
void get_an_octal(void);
void get_a_decimal(void);
int get_an_alpha(void);
int get_an_escape(void);
void back_char(void);
void get_out_space(void);
void get_a_bin(void);

//变量重命名
void make_symbol_global(void);
void tac_trans_name(SYMB* head);

//中间代码打印
const char* ts(SYMB* s, char* str);
void tac_node_print(TAC* i);
void tac_print(TAC* node);

//底层符号表构建
//look up symbol
SYMB* look_up_int(int value);
SYMB* look_up_label(const char* name, int create);
SYMB* look_up_var(const char* name, int create);
SYMB* look_up_array(const char* name, int create);
SYMB* look_up_func(const char* name, int create);
SYMB* look_up_symb(int type, const char* name, int value);
SYMB* find_symb(int type, const char* name, int value, SYMB* cur);
SYMB* create_func_symb(const char* name);
//create symbol
SYMB* create_var_symb(const char* name);
SYMB* create_arr_symb(const char* name);
SYMB* create_label_symb(const char* name);
SYMB* create_int_symb(int value);
SYMB* create_symb(int type, const char* name, int value);
ENODE* make_enode(void);

void join_symb(SYMB* symbol);
int is_not_global(SYMB* symbol);


//N维数组初始化
LHEAD* create_lhead_node(void);

//优化接口
SYMB** get_local_symbol_head(void);//未定义
TAC* get_tac_head(void);

//后端接口
void cg(void);


ENODE* create_enode();
ENODE* count_l_arr(SYMB* base, ENODE* dis);
ENODE* count_l_var(SYMB* base);

void we_assert(bool a, const char* name);
void join_val_to_const(SYMB* var, int value);
member* make_member(void);

int get_an_div(void);
int is_two_div(void);
int is_div_and_star(void);

void join_build_in_func(void);

ENODE* l_array(SYMB* symbol);
ENODE* l_variable(SYMB* symbol);

int it_have_lea(TAC* tac);
int lex_val_to_unary(int type);

void init_symbol_table(void);
Table* create_table(void);
void into_new_table(void);
void out_new_table(void);
SYMB** get_local_symbol_head(void);
SYMB** get_global_symbol_head(void);

SYMB* look_up_str(const char* name);
SYMB* create_str_symb(const char* name);
SYMB* symb_deep_copy(SYMB* symbol);
TAC* tac_copy(TAC* cur);
void update_copy_var(TAC* tac);

void make_tac_pre(TAC* ptr);
void tac_format(void);

//void exchange_assign(TAC* as);
void make_tac_format(void);
TAC* tidy_assign_zero(TAC* pre, TAC* end);
void find_assign_zero(TAC* as);
//void tidy_assign(TAC* as);
void insert_tac(TAC* begin, TAC* end);
void format_local(TAC* begin);

int str_to_int(char* name);

char* my_strlwr(char* str);
int bin_to_dec(char Binary[]);
int oct_to_dec(char Octal[]);
int hex_to_dec(char Hexadecimal[]);
void rename_func(TAC* head)
{
	while (head != nullptr)
	{
		if (head->op == TAC_FUNC && strcmp(head->a->name,"main"))
		{
			char a[50];
			sprintf(a, "%s_lyj_", head->a->name);
			sprintf(head->a->name, "%s", a);
		}
		head = head->next;
	}
}
void check_value(TAC* head)
{

	//cout << "check" << endl;
	std::set<SYMB*> HadDecl;
	for (TAC* i = head; i != nullptr; i = i->next)
	{
		if(i->op==TAC_STR)
		{
			exit(10);
		}
		if(i->a && i->a->name && strlen(i->a->name)>=45)
		{
			exit(i->a->type+1);
		}
		if(i->b && i->b->name && strlen(i->b->name)>=45)
		{
			exit(i->b->type+1);
		}
		if(i->c && i->c->name && strlen(i->c->name)>=45)
		{
			exit(i->c->type+1);
		}
		if (i->op == TAC_VAR)
		{
			HadDecl.insert(i->a);
		}
		if (i->op == TAC_ARR)
		{
			HadDecl.insert(i->a);
		}
		if (i->op == TAC_FORMAL)
		{
			HadDecl.insert(i->a);
		}
	}
	for (TAC* i = head; i != nullptr; i = i->next)
	{
		if (i->a && i->a->type == SYM_VAR)
		{
			if (!HadDecl.count(i->a))
			{
				cout << "cannot find decl: " << i->a->name << endl;
				exit(1);
			}
		}
		if (i->b && i->b->type == SYM_VAR)
		{
			if (!HadDecl.count(i->b))
			{
				cout << "cannot find decl: " << i->b->name << endl;
				exit(2);
			}
		}
		if (i->c && i->c->type == SYM_VAR)
		{
			
			if (!HadDecl.count(i->c))
			{
				cout << "cannot find decl: " << i->c->name << endl;
				exit(2);
			}
		}
	}

}
void give_func_return(TAC* head)
{
	while (head != nullptr)
	{
		if (head->op == TAC_ENDFUNC && head->prev->op != TAC_RETURN)
		{
			TAC* temp = create_a_node(look_up_int(0), nullptr, nullptr, TAC_RETURN);
			TAC* prev = head->prev;
			prev->next = temp;
			temp->next = head;
			head->prev = temp;
			temp->prev = prev;

		}
		head = head->next;
	}
}
void check_it_return(TAC* head)
{
	TAC* next_tac = head->next;
	while (head != nullptr && next_tac != nullptr)
	{
		if (head->op == TAC_RETURN && next_tac->op == TAC_RETURN)
		{
			exit(10);
		}
		head = head->next;
		next_tac = next_tac->next;
	}
}
void make_tac_format(void)
{	
	rename_func(tac_head);
	make_tac_pre(tac_head);
	tac_format();
	find_assign_zero(tac_head);	
	make_stirng_to_head(tac_head);
	check_value(tac_head);
	//give_func_return(tac_head);
}
void make_stirng_to_head(TAC *head)
{
	TAC* front_head=create_a_tac();
	front_head->next=head;
	TAC* ptr=head;
	TAC* keep=ptr;
	while(ptr)
	{
		if(ptr->op==TAC_STR)
		{
			keep=ptr->next;
			insert_tac(front_head,ptr);
			ptr=keep;
		}
		else
		{
			ptr=ptr->next;
		}
	}
	tac_head=front_head->next;
}
void find_assign_zero(TAC* as)
{
	SYMB* symbol = NULL;
	TAC* tac = as;
	TAC* cur = as;

	int count = 0;
	int keep = 0;
	while (as != NULL)
	{
		if (as->op == TAC_ARR_L && as->b->type == SYM_INT
			&& as->c->type == SYM_INT && as->c->value == 0)
		{
			if (count == 0)
			{
				symbol = as->a;
				tac = as;
				cur = as;
				count++;
				keep = as->b->value;
			}
			else if (symbol == as->a && as->b->value == keep + 1)
			{
				keep++;
				cur = as;
				count++;
			}
			else
			{
				as = tidy_assign_zero(tac, cur);
				symbol = as->a;
				cur = as;
				tac = as;
				count = 0;
			}
		}
		else if (cur != tac)
		{
			as = tidy_assign_zero(tac, cur);
			cur = NULL;
			tac = NULL;
			count = 0;
		}
		as = as->next;
	}
}

void tac_format(void)
{
	int keep = 0;
	TAC* cur = NULL;
	TAC* virt_head = create_a_tac();
	virt_head->next = get_tac_head();
	TAC* keep_global = virt_head;
	while (keep_global->next->op != TAC_FUNC)
	{
		keep_global = keep_global->next;
	}
	TAC* ptr = keep_global->next;
	while (ptr != NULL)
	{
		if (!keep && (ptr->op == TAC_VAR || ptr->op == TAC_ARR
			|| ptr->op == TAC_COPY || ptr->op == TAC_ARR_L))
		{
			cur = ptr->next;
			insert_tac(keep_global, ptr);
			keep_global = ptr;
			ptr = cur;
			continue;
		}
		if (ptr->op == TAC_FUNC || ptr->op == TAC_FORMAL)
		{
			keep = 1;
		}
		if (ptr->op == TAC_BEGINFUNC)
		{
			format_local(ptr);//TAC_BEGIN
			while (ptr->op != TAC_ENDFUNC)
			{
				ptr = ptr->next;
			}
			keep = 0;
		}
		ptr ? ptr = ptr->next : NULL;
	}
	tac_head = virt_head->next;
	safe_free(1, &virt_head);
}
void insert_tac(TAC* begin, TAC* end)
{

	end->prev->next = end->next;
	end->next	
		? end->next->prev = end->prev 
		: NULL;
	end->next = begin->next;
	end->prev = begin;

	begin->next->prev = end;
	begin->next = end;
}
void format_local(TAC* begin)
{
	if (begin->op != TAC_BEGINFUNC)
	{
		expect_str("TAC_BEGIN_FUNC");
		return;
	}
	while (begin->next->op == TAC_FORMAL)
	{
		begin = begin->next;
	}
	TAC* ptr = begin->next;
	TAC* keep = NULL;
	while (ptr->op != TAC_ENDFUNC)
	{
		if (ptr->op == TAC_VAR || ptr->op == TAC_ARR)
		{
			keep = ptr->next;
			insert_tac(begin, ptr);
			ptr = keep;
			continue;
		}
		ptr = ptr->next;
	}
}

TAC* tidy_assign_zero(TAC* pre, TAC* end)
{
	int v0 = pre->b->value;
	int v1 = end->b->value;
	TAC* ptr = create_a_node(pre->a, pre->b, end->b, TAC_ZERO);
	TAC* temp = ptr;
	pre->prev->next = ptr;
	end->next->prev = ptr;

	ptr->next = end->next;
	ptr->prev = pre->prev;

	ptr = pre;
	while (ptr != end)
	{
		pre = ptr;
		ptr = ptr->next;
		//free
		safe_free(1, &pre);
	}
	//free
	safe_free(1, &ptr);
	return temp;
}

void make_tac_pre(TAC* ptr)
{
	TAC* cur = NULL;
	while (ptr)
	{
		ptr->prev = cur;
		cur = ptr;
		ptr = ptr->next;
	}
}
//lex and prser
void lex_and_parser(void)
{
	tac_head = comp_unit();
}
/*
comp_unit=> { comp_piece }+
{
	tac_last=$
}
comp_piece=>decl | func_def
*/
TAC* comp_unit(void)
{
	TAC* cur = NULL;
	init_symbol_table();
	join_build_in_func();

	get_token();
	while (lex.val != EOF)
	{
		cur = join_tac(cur, comp_piece());
	}
	make_symbol_global();
	return cur;
}
void join_build_in_func(void)
{
	look_up_func("getint", 1);
	look_up_func("putint", 1);
	look_up_func("getin", 1);
	look_up_func("putch", 1);
	look_up_func("getch", 1);
	look_up_func("starttime", 1);
	look_up_func("stoptime", 1);
	look_up_func("putarray", 1);
	look_up_func("putf", 1);
	look_up_func("getarray", 1);
}
TAC* comp_piece(void)
{
	TAC* cur = NULL;
	if (lex.val == VOID)
	{
		cur = func_def();
	}
	else if (lex.val == CONST)
	{
		cur = decl();
	}
	else if (lex.val == INT)
	{
		look_ahead_token();
		look_ahead_token();
		if (lex.val == '(')
		{
			look_back_token();
			cur = func_def();
		}
		else
		{
			look_back_token();
			cur = decl();
		}
	}
	else
	{
		expect_str("VOID | CONST | INT\n");
	}

	return cur;
}
/*
decl=> const_decl | var_decl
<tac><tac><tac>
*/
TAC* decl(void)
{
	if (lex.val == CONST)
	{
		return const_decl();
	}
	else if (lex.val == INT)
	{
		return var_decl();
	}
	else
	{
		expect_str("CONST | INT");
		return NULL;
	}
}
/*
const_decl=> "const" "int" const_def { ","const_def};
<tac> <tac>
*/
TAC* const_decl(void)
{
	expect_val(CONST);
	expect_val(INT);
	TAC* cur = const_def();
	while (lex.val == ',')
	{
		get_token();
		cur = join_tac(cur, const_def());
	}
	expect_val(';');
	return cur;
}
/*
const_def=> var_name_decl "=" const_init_val
<tac> <node> <linker>
*/
TAC* const_def(void)
{
	TAC* ptr_var = var_name_decl(CONST);
	LHEAD* ptr_init = NULL;
	expect_val('=');
	ptr_init = const_init_val();
	ptr_var = do_init_tac(ptr_var, ptr_init, CONST); //do_const_init
	return ptr_var;
}
TAC* do_init_tac(TAC* var, LHEAD* head, int sel)
{
	TAC* temp = NULL;
	if (head == NULL)
	{
		return var;
	}
	if (head->type == BODY)
	{
		temp = do_init_for_var(var->a, head, sel);
	}
	else
	{
		temp = do_init_for_arr(var->a, head, sel);
	}
	return join_tac(var, temp);
}

/*
const_init_val=> const_exp | "{" [const_init_val {","const_init_val} ] "}"
<linker> <linker> <linker>
*/
LHEAD* const_init_val(void)
{
	if (lex.val != '{')
	{
		return create_link_var();
	}
	else
	{
		expect_val('{');
		LHEAD* ptr = create_link_arr();
		expect_val('}');
		return ptr;
	}

}
LHEAD* create_link_var(void)
{
	ENODE* ptr_exp = const_exp();
	return symb_to_link_head(ptr_exp); //type=HEAD;const_exp type=BODY const_init_val
}
LHEAD* create_link_arr(void)
{
	LHEAD* ret = NULL;
	if ('}' == lex.val)
	{
		if (!local_scope)
		{
			return NULL;
		}
		ret = create_lhead_node();
		ret->type = HEAD;
		ret->next_body = create_lhead_node();
		ret->next_body->type = BODY;
		ret->next_body->symbol = look_up_int(0);
		return ret;
	}
	LHEAD* cur0 = NULL, * cur1 = NULL;
	LHEAD* temp0 = NULL, * temp1 = NULL;
	temp1 = const_init_val();
	cur1 = join_linker(cur0, temp1);
	ret = cur1;
	while (lex.val == ',')
	{
		get_token();
		temp1 = const_init_val();
		if (temp0 != NULL && temp0->type == HEAD &&
			temp1 != NULL && temp1->type == BODY)
		{
			cur1 = join_linker(NULL, temp1);
			cur0->next_head = cur1;
		}
		else
		{
			cur1 = join_linker(cur1, temp1);
		}
		cur0 = cur1;
		temp0 = temp1;
	}
	return ret;
}
/*
var_decl=> "int" var_def {","var_def}
<tac> <tac>
*/
TAC* var_decl(void)
{
	expect_val(INT);
	TAC* cur = var_def();
	while (lex.val == ',')
	{
		get_token();
		cur = join_tac(cur, var_def());
	}
	expect_val(';');
	return cur;
}
/*
var_def=> var_name_decl [ "="init_val ]
<tac> <node> <linker>
*/
TAC* var_def(void)
{
	TAC* ptr_var = var_name_decl(INT);
	LHEAD* ptr_init = NULL;
	if (lex.val == '=')
	{
		get_token();
		ptr_init = const_init_val();
	}
	/*else if (!local_scope && ptr_var->a->type==SYM_ARRAY )
	{
		ptr_init = create_lhead_node();
		ptr_init->type = HEAD;
		ptr_init->next_body = create_lhead_node();
		ptr_init->next_body->type = BODY;
		ptr_init->next_body->symbol = look_up_int(0);
	}
	else if (!local_scope && ptr_var->a->type == SYM_VAR)
	{
		ENODE* ptr_exp = make_enode();
		ptr_exp->ret = look_up_int(0);
		ptr_init=symb_to_link_head(ptr_exp);
	}*/
	return do_init_tac(ptr_var, ptr_init, INT);
}
/*
var_name_decl=> ID {"["const_exp"]"}
<symb> <enode>
*/
TAC* var_name_decl(int sel)
{
	SYMB* ptr_var = decl_var_name(SYM_VAR, lex.str);
	ENODE* ptr_exp = NULL;
	if (sel == CONST)
	{
		ptr_var->con = 1;
	}
	else
	{
		ptr_var->con = 0;
	}
	expect_val(IDENTIFIER);
	while (lex.val == '[')
	{
		get_token();
		ptr_exp = const_exp();
		if (ptr_exp->ret->type == SYM_INT)
		{
			ptr_var = make_var_to_arr(ptr_var, ptr_exp->ret->value);
		}
		else
		{
			expect_str("const_exp\n");
		}
		get_token();
		ptr_var->type = SYM_ARRAY;
	}
	return declare_var_tac(ptr_var);
}
SYMB* decl_var_name(int type, const char* name)
{
	SYMB* ptr_var = find_symb(type, lex.str, 0, *get_local_symbol_head());
	if (ptr_var != NULL)
	{
		expect_str("declare twice");
	}
	else
	{
		ptr_var = create_var_symb(lex.str);
		join_symb(ptr_var);
	}
	return ptr_var;
}
/*
func_def=> func_type ID "("[func_f_params]")" block
<tac> <string> <tac> <tac>
*/
TAC* func_def(void)
{
	int type = lex.val;
	get_token();
	SYMB* func_head = declare_func(lex.str);
	local_scope = 1;
	into_new_table();
	get_token();
	expect_val('(');
	TAC* cur0 = func_f_params();
	expect_val(')');
	TAC* cur1 = block();
	// if (type == INT && !is_have_return(cur1))
	// {
	// 	expect_str("func return ,but it have no return");
	// }
	// if (type == VOID && is_have_return(cur1))
	// {
	// 	expect_str("func no return,but it have return");
	// }
	out_new_table();
	local_scope = 0;
	return make_func(func_head, cur0, cur1);
}
/*
func_f_params=> func_f_param {","func_f_param}
<tac> <tac> <tac>
*/
TAC* func_f_params(void)
{
	TAC* cur = NULL;
	if (lex.val == ')')
	{
		return cur;
	}
	cur = join_tac(func_f_param(), cur);
	while (lex.val == ',')
	{
		get_token();
		cur = join_tac(func_f_param(), cur);
	}
	return cur;
}
/*
func_f_param=> "int" ID [ "[" "]" {"["exp"]"}
*/
TAC* func_f_param(void)
{
	expect_val(INT);
	expect_val(IDENTIFIER);
	SYMB* tmp = look_up_var(lex.str, 1);
	TAC* p_tac = formal_var_tac(tmp);
	ENODE* cur0 = NULL;
	if (lex.val == '[')
	{
		get_token();
		expect_val(']');
		make_var_to_arr(tmp, 0);
		while (lex.val == '[')
		{
			get_token();
			cur0 = const_exp();
			if (cur0->ret->type == SYM_INT)
			{
				make_var_to_arr(tmp, cur0->ret->value);
				expect_val(']');
			}
			else
			{
				expect_str("const_exp0\n");
			}
		}
	}
	return p_tac;
}

/*
block=> "{" {block_item} "}"
<tac> <tac>
*/
TAC* block(void)
{
	expect_val('{');
	TAC* cur = NULL;
	TAC* tmp = NULL;
	while (lex.val != '}')
	{
		tmp = block_item();
		cur = join_tac(cur, tmp);
	}
	expect_val('}');
	return cur;
}

/*
block_item=> decl | stmt
<tac> <tac>
*/
TAC* block_item(void)
{

	if (lex.val == CONST || lex.val == INT)
	{
		return decl();
	}
	if (lex.val == '{')
	{
		TAC* blo = NULL;
		into_new_table();
		blo = block();
		out_new_table();
		return blo;
	}
	else
	{
		return stmt();
	}
}
/*
stmt=>  l_val "=" exp ";" | [exp]";" | block   <tac> <node> | nop | <tac>
		|"if" "(" cond ")" stmt ["else" stmt]  <node> <tac> <tac>
		|"while""("cond")" stmt   <node> <tac>
		|"break"";" | "continue" ";"  --需要两个变量来记录当前作用域名的标号
		|"return" [exp] ";" --记录即可
*/
TAC* stmt(void)
{
	if (lex.val == IF)
	{
		return if_stmt();
	}
	else if (lex.val == WHILE)
	{
		return while_stmt();
	}
	else if (lex.val == BREAK)
	{
		return break_stmt();
	}
	else if (lex.val == CONTINUE)
	{
		return continue_stmt();
	}
	else if (lex.val == RETURN)
	{
		return return_stmt();
	}
	else if (lex.val == ';')
	{
		get_token();
		return NULL;
	}
	else
	{
		return assign_stmt();
	}
}
/*
continue_stmt
*/
TAC* continue_stmt(void)
{
	expect_val(CONTINUE);
	expect_val(';');
	field_mid->eff = 1;
	//	field_tail->eff = 1;
	return create_a_node(field_mid, NULL, NULL, TAC_GOTO);
}
/*
break_stmt
*/
TAC* break_stmt(void)
{
	expect_val(BREAK);
	expect_val(';');
	//	field_head->eff = 1;
	field_tail->eff = 1;
	return create_a_node(field_tail, NULL, NULL, TAC_GOTO);
}
/*
return_stmt=>  "return" [exp] ";"
*/
TAC* return_stmt(void)
{
	if (lex.val == RETURN)
	{
		get_token();
		ENODE* node = NULL;
		if (lex.val != ';')
		{
			node = exp();
			TAC* tac = join_tac(node->tac, create_a_node(
				node->ret, NULL, NULL, TAC_RETURN));
			expect_val(';');
			return tac;
		}
		else
		{
			get_token();
			return create_a_node(NULL, NULL, NULL, TAC_RETURN);
		}

	}
	return NULL;
}
/*
assign_stmt=>l_val "=" exp ";"
check [exp];
*/
TAC* assign_stmt(void)
{
	ENODE* cur = NULL;
	TAC* temp = NULL;
	if (lex.val == IDENTIFIER && look_up_func(lex.str, 0) == NULL)
	{
		const char* name = lex.str;
		ENODE* ptr_temp_arr = l_val();
		TAC* cur0 = ptr_temp_arr->tac;
		if (lex.val == '=')
		{
			get_token();
			cur = exp();
			temp = do_assign(name, ptr_temp_arr, cur);
		}
		else if (lex.val == ';')
		{
			temp = cur0;
		}
		else
		{
			get_token();
			cur = exp();
			temp = join_tac(cur0, cur->tac);
		}
	}
	else
	{
		cur = exp();
		temp = cur->tac;
	}
	expect_val(';');
	return temp;
}
/*l_val
l_val=> ID{ "["EXP"]" }  <node> <node>
*/
ENODE* l_variable(SYMB* symbol)
{
	get_token();
	return (ENODE*)safe_malloc(sizeof(ENODE), "l_variable");
}
ENODE* l_array(SYMB* symbol)
{
	int count = 1;
	member* to_len = symbol->arr;
	ENODE* to_exp = NULL;
	ENODE* to_ret = NULL;
	if (to_len == NULL && symbol->type==SYM_ARRAY && lex.val != '[')
	{
		count = 0;
	}
	if (lex.val == '[')
	{
		get_token();
		to_ret = exp();
		expect_val(']');
	}
	while (lex.val == '[' && to_len != NULL)
	{
		get_token();
		to_exp = exp();
		to_ret = mknode_for_array(to_ret, to_exp, to_len->num);
		to_len = to_len->arr;
		expect_val(']');
	}
	if (lex.val == '[' && to_len == NULL)
	{
		expect_str("the arr's dim to short");
		return NULL;
	}

	if (to_ret == NULL)
	{
		to_ret = make_enode();
		to_ret->ret = look_up_int(0);
	}
	if (to_exp == NULL)
	{
		to_exp = make_enode();
		to_exp->ret = look_up_int(0);
	}
	while (to_len != NULL && lex.val != '[')
	{
		to_ret = mknode_for_array(to_ret, to_exp, to_len->num);
		if (count)
		{
			count = 0;
			to_exp->next = NULL;
			to_exp->tac = NULL;
			to_exp->ret = look_up_int(0);
		}
		to_len = to_len->arr;
	}
	if (!count)
	{
		SYMB* t = look_up_var(get_temp_var_name(), 1);
		to_ret->tac = create_another_node(to_ret->tac, t, NULL, NULL, TAC_VAR);
		to_ret->tac = join_tac(to_ret->tac, create_a_node(t, symbol, to_ret->ret, TAC_LEA));
		to_ret->ret = t;
	}
	return to_ret;

}


ENODE* l_val(void) //不是数组，返回空值，如果是ret=偏移量
{
	ENODE* to_exp = NULL;
	ENODE* to_node = NULL;
	SYMB* to_val = NULL;
	const char* name = lex.str;
	SYMB* ptr = look_up_var(name, 0);
	if (ptr != NULL)
	{
		return l_variable(ptr);
	}

	ptr = look_up_array(name, 0);
	if (ptr != NULL)
	{
		get_token();
		return l_array(ptr);
	}
	else
	{
		expect_str("Lvalue: ARRAY || VARIABLE");
		return NULL;
	}
}

/*
if_stmt=> "if" "(" cond ")" stmt ["else" stmt]
*/
TAC* if_stmt(void)
{
	expect_val(IF);
	expect_val('(');
	ENODE* p_cond = cond();
	expect_val(')');
	
	TAC* p_stmt = NULL;
	if (lex.val != '{')
	{
		p_stmt = stmt();
	}
	else
	{
		into_new_table();
		p_stmt = block();
		out_new_table();
	}
	if (lex.val == ELSE)
	{
		get_token();
		if (lex.val == '{')
		{
			into_new_table();
			p_stmt= do_if_else(p_cond, p_stmt, block());
			out_new_table();
		}
		else
		{
			p_stmt= do_if_else(p_cond, p_stmt, stmt());
		}

	}
	else
	{
		p_stmt= do_if(p_cond, p_stmt);
	}
	
	return p_stmt;
}
/*
while_stmt=> "while""("cond")" stmt
*/
TAC* while_stmt(void)
{
	expect_val(WHILE);
	into_new_table();
	expect_val('(');
	ENODE* p_cond = cond();
	expect_val(')');
	push_field();
	TAC* p_stmt = NULL;
	if (lex.val != '{')
	{
		p_stmt = stmt();
	}
	else
	{
		p_stmt = block();
	}
	p_stmt = do_while(p_cond, p_stmt);
	out_new_table();
	return p_stmt;
}

/*
exp=> add_exp <node> <node>
*/
ENODE* exp(void)
{
	return cond();
}
/*
cond=> l_or_exp <node> <node>
*/
ENODE* cond(void)
{
	return l_or_exp();
}
/*
const_exp=> add_exp (ret.type==const)<node> <node>
*/
ENODE* const_exp(void)
{
	return add_exp();
}
/*
l_or_exp=> l_and_exp {"||" l_and_exp}
*/
ENODE* l_or_exp(void)
{
	ENODE* cur = l_and_exp();
	
	while (lex.val == OR)
	{
		int keep = lex.val;
		get_token();
		cur = mknode_for_binary(cur, l_and_exp(), keep);
	}
	return cur;
}
/*
l_and_exp=> eq_exp "&&" eq_exp
*/
ENODE* l_and_exp(void)
{
	ENODE* cur = eq_exp();
	
	while (lex.val == AND)
	{
		int keep = lex.val;
		get_token();
		cur = mknode_for_binary(cur, eq_exp(), keep);
	}
	return cur;
}
/*
eq_exp=> rel_exp { ("==" | "!=") rel_exp }
*/
ENODE* eq_exp(void)
{
	ENODE* cur = rel_exp();
	
	while (lex.val == EQ || lex.val == NE)
	{
		int keep = lex.val;
		get_token();
		cur = mknode_for_binary(cur, rel_exp(), keep);
	}
	return cur;
}
/*
rel_exp=> add_exp { "<" | ">" | "<=" | ">=" add_exp }
*/
ENODE* rel_exp(void)
{
	ENODE* cur = add_exp();
	
	while (lex.val == '>' || lex.val == '<' ||
		lex.val == GE || lex.val == LE)
	{
		int keep = lex.val;
		get_token();
		cur = mknode_for_binary(cur, add_exp(), keep);
	}
	return cur;
}
/*
add_exp=> mul_exp { "+" | "-" mul_exp}
*/
ENODE* add_exp(void)
{
	ENODE* cur = mul_exp();
	while (lex.val == '+' || lex.val == '-')
	{
		int keep = lex.val;
		get_token();
		cur = mknode_for_binary(cur, mul_exp(), keep);
	}
	return cur;
}
/*
mul_exp=> unary_exp { "*" | "/" | "%" unary_exp}
*/
ENODE* mul_exp(void)
{
	ENODE* cur = unary_exp();
	
	while (lex.val == '*' || lex.val == '/' || lex.val == '%')
	{
		int keep = lex.val;
		get_token();
		cur = mknode_for_binary(cur, unary_exp(), keep);
	}
	return cur;
}

/*
unary_exp=> { primary_exp | ID "("[func_r_params]")" | unary_op unary_exp | "(" exp ")" }+
*/
ENODE* unary_exp(void)
{
	ENODE* ptr_unary = NULL;
	if (lex.val == '!' || lex.val == '+' || lex.val == '-')
	{
		int type = lex.val;
		get_token();
		ptr_unary = unary_exp();
		ptr_unary = mknode_for_unary(type, ptr_unary);
	}
	else if (lex.val == IDENTIFIER)
	{
		look_ahead_token();
		if (lex.val == '(')
		{
			look_back_token();
			ptr_unary = func_ret();
		}
		else
		{
			look_back_token();
			ptr_unary = primary_exp();
		}
	}
	else if (lex.val == INTEGER)
	{
		ptr_unary = primary_exp();
	}
	else if (lex.val == '(')
	{
		get_token();
		ptr_unary = exp();
		expect_val(')');
	}
	else
	{
		expect_str("unary_exp\n");
	}
	return ptr_unary;
}
/*
 =>ID "("[func_r_params]")"
*/
ENODE* func_ret(void)
{
	if (lex.val == IDENTIFIER)
	{
		char* name = lex.str;
		get_token();
		expect_val('(');
		ENODE* cur = make_enode();
		TAC* temp = NULL;
		SYMB* str = NULL;
		if (lex.val != ')')
		{
			temp = mktac_real_params(func_r_params());
		}
		if (!strcmp(name, "starttime") || !strcmp(name,"stoptime"))
		{
			temp = join_tac(temp, create_a_node(look_up_int(line), NULL, NULL, TAC_ACTUAL));
		}
		cur->ret = look_up_var(get_temp_var_name(), 1);
		cur->tac = join_tac(temp, create_a_node(cur->ret, NULL, NULL, TAC_VAR));
		
		temp = find_tac_tail(cur->tac);
		if ((str = look_up_func(name, 0)) == NULL)
		{
			expect_str("declare function:");
		}
		temp = create_another_node(temp, cur->ret, str, NULL, TAC_CALL);
		expect_val(')');
		return cur;
	}
	else
	{
		expect_str("ID\n");
		return NULL;
	}
}
/*
func_r_params=> exp {"," exp }
*/
TAC* mktac_real_params(ENODE* head)
{
	TAC* cur = NULL;
	ENODE* node = head;
	while (node != NULL)
	{
		cur = join_tac(cur, node->tac);
		node = node->next;
	}
	node = head;
	while (node != NULL)
	{
		cur = create_another_node(cur, node->ret, NULL, NULL, TAC_ACTUAL);
		//		cur = cur->next;
		node = node->next;
	}
	return cur;


}
inline TAC* create_a_tac(void)
{
	return (TAC*)safe_malloc(sizeof(TAC), "create_a_tac");
}
ENODE* func_r_params(void)
{
	ENODE* cur0 = NULL;
	ENODE* cur1 = NULL;
	ENODE* temp = NULL;
	if (lex.val == TEXT)
	{
		cur0 = make_enode();
		cur0->ret = look_up_str(lex.str);
		cur0->tac = create_a_tac();
		cur0->tac->op = TAC_STR;
		cur0->tac->a = cur0->ret;
		get_token();
	}
	else
	{
		cur0 = exp();
	}
	if (lex.val == ',')
	{
		get_token();
		cur1 = func_r_params();
		temp = cur1;
		while (cur1->next != NULL)
		{
			cur1 = cur1->next;
		}
		cur1->next = cur0;
		return temp;
	}
	else
	{
		return cur0;
	}

}
/*
primary_exp=>  l_val | NUM   exp已经在上一层解决
*/
ENODE* primary_exp(void)
{
	char* name = NULL;
	SYMB* symbol = NULL;
	SYMB* temp = NULL;
	ENODE* p_val = NULL;

	if (lex.val == IDENTIFIER)
	{
		name = lex.str;
		symbol = look_up_var(name, 0);
		p_val = l_val();
		if (symbol != NULL)
		{
			return count_l_var(symbol);
		}
		else
		{
			symbol = look_up_array(name, 0);
			return count_l_arr(symbol, p_val);


		}
	}
	else if (lex.val == INTEGER)
	{
		p_val = make_enode();
		p_val->ret = look_up_int(str_to_int(lex.str));
		get_token();
	}
	else
	{
		expect_str("INT | L_VAL");
	}
	return p_val;
}
ENODE* count_l_var(SYMB* base)
{
	if (base == NULL)
	{
		return NULL;
	}
	ENODE* ptr = create_enode();
	if (base->type == SYM_VAR)
	{
		if (base->con == 1)
		{
			ptr->ret = look_up_int(base->val->num);
		}
		else
		{
			ptr->ret = base;
		}
		return ptr;
	}
	else
	{
		expect_str("SYMB_ARRAY || SYMB_VAR");
		return NULL;
	}
}
ENODE* create_enode()
{
	return (ENODE*)safe_malloc(sizeof(ENODE), "create_enode");
}
ENODE* count_l_arr(SYMB* base, ENODE* dis)
{
	SYMB* symbol = dis->ret;
	SYMB* temp = NULL;
	if (base == NULL || base->type != SYM_ARRAY)
	{
		return NULL;
	}
	if (base->con == 1 && symbol->type == SYM_INT)
	{
		struct member* mem = base->val;
		int count = symbol->value;
		while (count >= 1 && mem != NULL)
		{
			mem = mem->arr;
			count--;
		}
		if (mem == NULL)
		{
			dis->ret = look_up_int(0);
		}
		else
		{
			dis->ret = look_up_int(mem->num);
		}
		return dis;
	}

	temp = look_up_var(get_temp_var_name(), 1);
	TAC* tac = create_a_node(temp, NULL, NULL, TAC_VAR);
	dis->tac = join_tac(dis->tac, tac);
	if (it_have_lea(dis->tac))
	{
		tac = create_a_node(temp, dis->ret, NULL, TAC_COPY);
	}
	else
	{
		tac = create_a_node(temp, base, dis->ret, TAC_ARR_R);
	}
	dis->tac = join_tac(dis->tac, tac);
	dis->ret = temp;
	return dis;
}
int it_have_lea(TAC* tac)
{
	TAC* head = tac;
	while (tac)
	{
		if (tac->op == TAC_CALL)
		{
			break;
		}
		tac = tac->next;
	}
	if (!tac)
	{
		tac = head;
	}
	while (tac != NULL && tac->next != NULL)
	{
		if (tac->op == TAC_LEA)
		{
			return 1;
		}
		else
		{
			tac = tac->next;
		}
	}
	return 0;
}
/*
func_r_params=> exp {"," exp }
*/

//get_token()
void look_ahead_token()
{
	add_band();
	get_token();
}
void look_back_token()
{
	add_band();
	flush_band();
	get_token();
}
int get_token(void)
{
	if (band.flu > 0)
	{
		get_token_buf();
		
	}
	else
	{
		lex.val = yylext();
	}
	return lex.val;
}
void flush_band(void)
{
	band.flu = band.buf;
	band.buf = 0;
}
void get_token_buf(void)
{
	lex.str = token_buf[band.buf].str;
	lex.val = token_buf[band.buf].val;
	band.buf++;
	band.flu--;
	if (band.flu == 0)
	{
		band.buf = 0;
	}
}
void add_band(void)
{
	token_buf[band.buf].str = lex.str;
	token_buf[band.buf].val = lex.val;
	band.buf++;
}


//lex
int yylext(void)
{
	int num;
	get_out_space();

	if (isalpha(lex_char) || lex_char == '_')
	{
		num = get_an_alpha();
	}
	else if (isdigit(lex_char))
	{
		num = get_a_digit();
	}
	else if (lex_char == '"')
	{
		num = get_a_string();
	}
	else if (lex_char == '=')
	{
		num = get_an_eq();
	}
	else if (lex_char == '!')
	{
		num = get_a_not();
	}
	else if (lex_char == '>')
	{
		num = get_a_great();
	}
	else if (lex_char == '<')
	{
		num = get_a_less();
	}
	else if (lex_char == '&')
	{
		num = get_an_and();
	}
	else if (lex_char == '|')
	{
		num = get_an_or();
	}
	else if (lex_char == '/')
	{
		num = get_an_div();
	}
	else
	{
		num = get_an_other();
	}

	return num;
}
int get_an_div(void)
{
	get_out_space();
	if (lex_char == '/')
	{
		return is_two_div();
	}
	if (lex_char == '*')
	{
		return is_div_and_star();
	}
	else
	{
		back_char();
		return '/';
	}

}
int is_div_and_star(void)
{
	char ch0 = 0;
	char ch1 = 0;
	if ((ch0 = getchar()) == EOF)	return EOF;
	if ((ch1 = getchar()) == EOF)	return EOF;
	while (ch0 != '*' || ch1 != '/')
	{
		if ((ch0 = getchar()) == EOF)	return EOF;
		if ((ch1 = getchar()) == EOF)	return EOF;
	}
	return yylext();
}
int is_two_div(void)
{
	char ch = 0;
	ch = getchar();
	while (ch != '\n' && ch != EOF)
	{
		ch = getchar();
	}
	if (ch == EOF)
	{
		return EOF;
	}
	else
	{
		return yylext();
	}
}
int get_an_other()
{
	return lex_char;
}
int get_a_great(void)
{

	get_out_space();
	if (lex_char == '=')
	{
		return GE;
	}
	else
	{
		back_char();
		return '>';
	}
}
int get_a_less(void)
{
	get_out_space();
	if (lex_char == '=')
	{
		return LE;
	}
	else
	{
		back_char();
		return '<';
	}
}
int get_an_and(void)
{
	get_out_space();
	if (lex_char == '&')
	{
		return AND;
	}
	else
	{
		back_char();
		return '&';
	}
}
int get_an_or(void)
{
	get_out_space();
	if (lex_char == '|')
	{
		return OR;
	}
	else
	{
		back_char();
		return '|';
	}
}
int get_an_eq(void)
{
	get_out_space();
	if (lex_char == '=')
	{
		return EQ;
	}
	else
	{
		back_char();
		return '=';
	}
}
int get_a_not(void)
{
	get_out_space();
	if (lex_char == '=')
	{
		return NE;
	}
	else
	{
		back_char();
		return '!';
	}
}
int get_a_string(void)
{
	int timer = 0;
	lex_char = getchar();
	while (lex_char != '"' && lex_char != EOF)
	{
		if (lex_char == '\\')
		{
			token[timer] = get_an_escape();
		}
		else
		{
			token[timer] = lex_char;
		}
		lex_char = getchar();
		timer++;
	}
	if (lex_char == EOF)
	{
		printf("lex error: \" expected\n");
		exit(4);
	}
	token[timer] = 0;
	lex.str = (char*)safe_malloc(50, "get_a_string");
	strcpy(lex.str, token);
	return TEXT;
}
int get_an_escape(void)
{
	lex_char = getchar();
	if (lex_char == '\\')
	{
		return '\\';
	}
	if (lex_char == 'n')
	{
		return '\n';
	}
	if (lex_char == 't')
	{
		return '\t';
	}
	if (lex_char == 'b')
	{
		return '\b';
	}
	return 0;
}
int get_a_digit(void)
{
	token[0] = 0;
	if (lex_char != '0')
	{
		get_a_decimal();
	}
	else
	{
		lex_char = getchar();
		if (lex_char == 'x' || lex_char == 'X')
		{
			lex_char = getchar();
			get_a_hex();
		}
		else if (lex_char == 'b' || lex_char == 'B')
		{
			lex_char = getchar();
			get_a_bin();
		}
		else
		{
			get_an_octal();
		}
	}
	back_char();
	lex.str = (char*)safe_malloc(50, "get_a_digit");
	strcpy(lex.str, token);
	return INTEGER;
}
void get_a_hex(void)
{
	int num = 0;
	while (isxdigit(lex_char))
	{
		token[num] = lex_char;
		lex_char = getchar();
		num++;
	}
	token[num] = 0;
	char* ptr = (char*)malloc(50);
	sprintf(ptr, "0x%s", token);
	sprintf(token, "%s", ptr);
	free(ptr);
}
void get_an_octal(void)
{
	int num = 0;
	while (lex_char >= '0' && lex_char <= '7')
	{
		token[num] = lex_char;
		lex_char = getchar();
		num++;
	}
	token[num] = 0;
	char* ptr = (char*)malloc(50);
	sprintf(ptr, "0%s", token);
	sprintf(token, "%s", ptr);
	free(ptr);

}
void get_a_bin(void)
{
	int num = 0;
	while (lex_char >= '0' && lex_char <= '1')
	{
		token[num] = lex_char;
		lex_char = getchar();
		num++;
	}
	token[num] = 0;
	char* ptr = (char*)malloc(50);
	sprintf(ptr, "0b%s", token);
	sprintf(token, "%s", ptr);
	free(ptr);
}
void get_a_decimal(void)
{
	int num = 0;
	while (isdigit(lex_char))
	{
		token[num] = lex_char;
		lex_char = getchar();
		num++;
	}
	token[num] = 0;
}
int get_an_alpha(void)
{
	int num;
	token[0] = 0;
	while (isalpha(lex_char) || isdigit(lex_char) || lex_char == '_')
	{
		num = strlen(token);
		token[num + 1] = 0;
		token[num] = lex_char;
		lex_char = getchar();
	}

	back_char();

	num = token_is_keyword();
	if (num == 0)
	{
		return get_an_id();
	}
	else
	{
		return num;
	}
}

int get_an_id(void)
{
	lex.str = (char*)safe_malloc(50, "get_an_id");
	if (lex.str)
	{
		strcpy(lex.str, token);
	}
	else
	{
		fprintf(stdout, "error");
		exit(29);
	}
	return IDENTIFIER;
}

int token_is_keyword()
{
	int i = 0;
	while (strcmp(keywords[i].name, ""))
	{
		if (!strcmp(keywords[i].name, token))
		{
			return keywords[i].id;
		}
		i++;
	}
	return 0;
}
void back_char(void)
{
	ungetc(lex_char, stdin);
	lex_char = '\0';
}
void get_out_space(void)
{
	lex_char = getchar();
	while (isspace(lex_char))
	{
		if (lex_char == '\n')
		{
			line++;
		}
		lex_char = getchar();
	}
}

void expect_str(const char* name)
{
	if (!name || !lex.str)
	{
		printf("error expect_str");
		exit(27);
	}
	if (!strcmp(name, lex.str))
	{
		get_token();
	}
	else
	{
		fprintf(stdout, "expect: %s in line: %d\n", name, line);
		fprintf(stdout, "but now is %s", lex.str);
		#define ERRORNAME 24
		const char* errorNameList[ERRORNAME] = {
			"TAC_BEGIN_FUNC",
			"VOID | CONST | INT\n",
			"CONST | INT",
			"const_exp\n",
			"declare twice",
			"func return ,but it have no return",
			"func no return,but it have return",
			"const_exp0\n",
			"the arr's dim to short",
			"Lvalue: ARRAY || VARIABLE",
			"unary_exp\n",
			"declare function:",
			"ID\n",
			"INT | L_VAL",
			"SYMB_ARRAY || SYMB_VAR",
			"SYM_VAR SYM_ARRAY",
			"front: BODY back: HEAD not match",
			"ARR ID || VAR ID",
			"if(cond)",
			"error in look_up_var",
			"repet_declartion",
			"error in look_up_array",
			"repet_declartion",
			"it has declare"
		};
		int errorNameFlag[ERRORNAME] = {2,3,4,5,6,7,
			8,10,11,12,13,14,//在这里面
			15,16,17,18,19,20,
			21,22,23,24,25,26};
		for(int i = 0;i < ERRORNAME;i++)
		{
			if(!strcmp(name, errorNameList[i]))
					exit(errorNameFlag[i]);
		}
		exit(27);
	}

}
void expect_val(int value)
{
	if (value == lex.val)
	{
		get_token();
	}
	else
	{

		fprintf(stdout, "expect: %d in line: %d", value, line);
		fprintf(stdout, "but now is %d", lex.val);
		exit(28);
	}
}

TAC* join_tac(TAC* head, TAC* tail)
{
	TAC* cur = head;
	if (cur == NULL)
	{
		return tail;
	}
	while (cur->next != NULL)
	{
		cur = cur->next;
	}
	cur->next = tail;
	return head;
}

TAC* do_init_for_arr(SYMB* cur, LHEAD* head, int sel)
{
	LHEAD* link = head;
	int count = 0, spice = 0;
	int max = count_arr_size(cur);
	struct member* arr = cur->arr;
	if (arr == NULL)
	{
		spice = cur->num;
	}
	else
	{
		while (arr->arr != NULL)
		{
			arr = arr->arr;
		}
		spice = arr->num;
	}

	while (link != NULL)
	{
		make_lhead_std(spice, link);
		link = link->next_head;
	}
	TAC* temp = NULL, * var = NULL;
	count = 0, link = head;
	LHEAD* tac = create_lhead_node();
	tac->type = HEAD;
	tac->next_body = create_lhead_node();
	tac->next_body->type = BODY;
	tac->next_body->symbol = look_up_int(0);
	for (; count < max; count += spice)
	{
		temp = init_tac_every_dim(cur, count, count + spice - 1, link, sel);
		var = join_tac(var, temp);
		if (link != NULL && link->next_head != NULL)
		{
			link = link->next_head;
		}
		else
		{
			link = tac;
		}

	}

	return var;

}
TAC* do_init_for_var(SYMB* var, LHEAD* head, int sel)
{
	if (sel == CONST)
	{
		we_assert(head->symbol->type == SYM_INT, "SYM_INT");
		join_val_to_const(var, head->symbol->value);
	}
	return create_another_node(head->tac, var, head->symbol, NULL, TAC_COPY);
}
void we_assert(bool a, const char* name)
{
	if (!a)
	{
		expect_str(name);
	}
}
void join_val_to_const(SYMB* var, int value)
{
	if (var->type == SYM_VAR || var->val == NULL)
	{
		var->val = make_member();
		var->val->num = value;
		return;
	}
	if (var->type != SYM_ARRAY)
	{
		expect_str("SYM_VAR SYM_ARRAY");
	}
	member* ptr = var->val;
	while (ptr->arr != NULL)
	{
		ptr = ptr->arr;
	}
	ptr->arr = make_member();
	ptr->arr->num = value;

}
member* make_member(void)
{
	return (member*)safe_malloc(sizeof(member), "make_member");
}
int count_arr_size(SYMB* array)
{
	int count = 0;
	struct member* ptr = array->arr;
	if (ptr == NULL)
	{
		return array->num;
	}
	count = array->num;
	while (ptr != NULL)
	{
		count *= ptr->num;
		ptr = ptr->arr;
	}
	return count;

}
void make_lhead_std(int spice, LHEAD* head)
{
	int count = 1;
	LHEAD* cur = head->next_body;
	LHEAD* cur0 = NULL;
	while (count <= spice && cur != NULL)
	{
		cur0 = cur;
		cur = cur->next_body;
		count++;
	}
	if (spice < count && cur != NULL && cur0 != NULL)
	{
		cur0->next_body = NULL;
		LHEAD* ptr = (LHEAD*)safe_malloc(sizeof(LHEAD), "make_lhead_std");
		ptr->next_head = head->next_head;
		ptr->next_body = cur;
		head->next_head = ptr;
	}
}

TAC* init_tac_every_dim(SYMB* var, int front, int end, LHEAD* head, int sel)
{
	LHEAD* body = head->next_body;
	TAC* ptr = NULL;
	TAC* temp = NULL;
	SYMB* cur0 = NULL;
	while (body != NULL && front <= end)
	{
		cur0 = body->symbol;
		if (sel == CONST)
		{
			we_assert(cur0->type == SYM_INT, "SYM_INT");
			join_val_to_const(var, cur0->value);
		}
		temp = create_a_node(var, look_up_int(front), cur0, TAC_ARR_L);
		ptr = join_tac(ptr, temp);
		front++;
		body = body->next_body;
	}
	cur0 = look_up_int(0);
	while (front <= end)
	{
		if (sel == CONST)
		{
			we_assert(cur0->type == SYM_INT, "SYM_INT");
			join_val_to_const(var, cur0->value);
		}
		temp = create_a_node(var, look_up_int(front), cur0, TAC_ARR_L);
		ptr = join_tac(ptr, temp);
		front++;
	}
	head = head->next_body;
	temp = NULL;
	while (head != NULL)
	{
		temp = join_tac(temp, head->tac);
		head = head->next_body;
	}
	ptr = join_tac(temp, ptr);
	return ptr;
}
TAC* create_a_node(symb* a, symb* b, symb* c, int type)
{
	TAC* cur = create_a_tac();
	cur->next = NULL;
	cur->op = type;
	cur->a = a;
	cur->b = b;
	cur->c = c;
	return cur;
}

SYMB* make_var_to_arr(SYMB* array, int length)
{
	if (array->type == SYM_VAR)
	{
		array->type = SYM_ARRAY;
		array->num = length;
		array->arr = NULL;
		return array;
	}
	if (array->type == SYM_ARRAY)
	{
		struct member* ptr = NULL;
		if (array->arr == NULL)
		{
			array->arr = (member*)safe_malloc(sizeof(member), "make_var_to_arr");
			ptr = array->arr;
		}
		else
		{

			ptr = array->arr;
			while (ptr->arr != NULL)
			{
				ptr = ptr->arr;
			}
			ptr->arr = (member*)safe_malloc(sizeof(member), "make_var_to_arr");
			ptr = ptr->arr;
		}
		ptr->arr = NULL;
		ptr->num = length;
		return array;
	}
	return NULL;


}
LHEAD* symb_to_link_head(ENODE* node)                //type=0;const_exp type=1 const_init_val
{
	LHEAD* ptr = create_lhead_node();
	ptr->type = BODY;
	ptr->symbol = node->ret;
	ptr->tac = node->tac;
	return ptr;
}
LHEAD* create_lhead_node(void)
{
	return (LHEAD*)safe_malloc(sizeof(LHEAD), "create_lhead_node");
}
LHEAD* join_linker(LHEAD* front, LHEAD* back) //判断type=0 or type= 1
{
	if (front == NULL)
	{
		if (back == NULL)
		{
			return NULL;
		}
		if (back->type == HEAD)
		{
			return back;
		}
		else if (back->type == BODY)
		{
			front = (LHEAD*)safe_malloc(sizeof(LHEAD), "join_linker");
			front->type = HEAD;
			front->next_body = back;
			return front;
		}
	}
	else if (front->type == BODY)
	{
		if (back->type == BODY)
		{
			front->next_body = back;
			return back;
		}
		else
		{
			expect_str("front: BODY back: HEAD not match");
			return NULL;
		}
	}
	else if (front->type == HEAD)
	{
		if (back->type == BODY)
		{
			LHEAD* ptr = front;
			while (front->next_body != NULL)
			{
				front = front->next_body;
			}
			front->next_body = back;
			return ptr;
		}
		else
		{
			front->next_head = back;
			return back;
		}

	}
	return NULL;
}
TAC* declare_var_tac(SYMB* decl)
{
	if (decl->type == SYM_VAR)
	{

		return create_a_node(decl, NULL, NULL, TAC_VAR);
	}
	if (decl->type == SYM_ARRAY)
	{

		int size = count_arr_size(decl);
		return create_a_node(decl, look_up_int(size), NULL, TAC_ARR);
	}
	return NULL;
}

void make_it_const(SYMB* int_decl)
{
	if (int_decl == NULL)
	{
		return;
	}
	if (int_decl->type == SYM_VAR)
	{
		int_decl->con = 1;
		return;
	}
	if (int_decl->type == SYM_ARRAY)
	{
		int_decl->con = 1;
		return;
	}
}

TAC* make_func(SYMB* head, TAC* parms, TAC* block)
{
	TAC* cur0 = create_a_node(head, NULL, NULL, TAC_FUNC);
	cur0 = create_another_node(cur0, NULL, NULL, NULL, TAC_BEGINFUNC);
	cur0 = join_tac(cur0, parms);
	cur0 = join_tac(cur0, block);
	cur0 = create_another_node(cur0, NULL, NULL, NULL, TAC_ENDFUNC);
	return cur0;
}

TAC* create_another_node(TAC* head, symb* a, symb* b, symb* c, int type)
{
	TAC* cur = create_a_node(a, b, c, type);
	cur = join_tac(head, cur);
	return cur;

}

void safe_free(int size, ...)
{
	return;
	void** ptr;
	va_list ap;
	va_start(ap, size);
	while (size > 0)
	{
		ptr = va_arg(ap, void**);
		free(*ptr);
		*ptr = NULL;
		size--;
	}
}
void all_free(int size, ...)
{
	return;
	va_list ap;
	va_start(ap, size);
	while (size > 0)
	{
		free(va_arg(ap, void*));
		size--;
	}

}
void* safe_malloc(size_t size, const char* name)
{

	void* ptr = calloc(1, size);
	if (!ptr)
	{
		printf("error malloc");
		fflush(stdout);
		exit(5);
	}
	return ptr;
}
TAC* do_assign(const char* name, ENODE* head, ENODE* tail)
{
	TAC* cur = join_tac(head->tac, tail->tac);
	TAC* keep = cur;
	cur = find_tac_tail(cur);
	SYMB* tmp = look_up_var(name, 0);
	if (tmp == NULL)
	{
		if ((tmp = look_up_array(name, 0)) == NULL)
		{
			expect_str("ARR ID || VAR ID");
			return NULL;
		}
		else
		{
			return join_tac(keep, create_a_node(tmp, head->ret, tail->ret, TAC_ARR_L));
		}
	}
	else
	{
		return join_tac(keep, create_a_node(tmp, tail->ret, NULL, TAC_COPY));
	}
}
TAC* do_if_else(ENODE* cond, TAC* stmt0, TAC* stmt1)
{
	int count = 1;
	TAC* cur = find_tac_tail(cond->tac);
	if (cond == NULL)
	{
		expect_str("if(cond)");
		return NULL;
	}
	SYMB* tmp0 = look_up_label(get_label_name(), 1);
	SYMB* tmp1;
	if (stmt1 && find_tac_tail(stmt1)->op == TAC_LABEL
		&& find_tac_tail(stmt1)->a->type == SYM_LABEL)
	{
		tmp1 = find_tac_tail(stmt1)->a;
		count = 0;
	}
	else
	{
		tmp1 = look_up_label(get_label_name(), 1);
	}

	cur = create_another_node(cur, cond->ret, tmp0, NULL, TAC_IFZ);
	cur = join_tac(cur, stmt0);
	cur = create_another_node(cur, tmp1, NULL, NULL, TAC_GOTO);
	cur = create_another_node(cur, tmp0, NULL, NULL, TAC_LABEL);
	cur = join_tac(cur, stmt1);
	if (count)
	{
		cur = create_another_node(cur, tmp1, NULL, NULL, TAC_LABEL);
	}
//	cur = join_tac(cond->tac, cur);
	all_free(1, cond);
	if (cond->tac)
	{
		return cond->tac;
	}
	else
	{
		return cur;
	}
}

char* get_label_name(void)
{
	char* name = (char*)safe_malloc(sizeof(char) * 50, "get_label_name");
	sprintf(name, "L%d", label_count);
	label_count++;
	return name;
}
TAC* do_if(ENODE* cond, TAC* stmt)
{
	SYMB* sym_cur;
	int count = 1;
	if (stmt && find_tac_tail(stmt)->op == TAC_LABEL
		&& find_tac_tail(stmt)->a->type == SYM_LABEL)
	{
		sym_cur = find_tac_tail(stmt)->a;
		count = 0;
	}
	else
	{
		sym_cur = look_up_label(get_label_name(), 1);
	}
	//	SYMB* sym_cur = look_up_label(get_label_name(), 1);
	TAC* tac_cur = create_a_node(cond->ret, sym_cur, NULL, TAC_IFZ);
	tac_cur = join_tac(cond->tac, tac_cur);
	tac_cur = join_tac(tac_cur, stmt);
	if (count)
	{
		tac_cur = join_tac(tac_cur, create_a_node(sym_cur, NULL, NULL, TAC_LABEL));
	}

	return tac_cur;;
}

TAC* do_while(ENODE* cond, TAC* stmt)
{
	SYMB* head = field_head;
	SYMB* tail = field_tail;
	TAC* cur = NULL;
	head->name = get_label_name();
	tail->name = get_label_name();
	head->type = SYM_LABEL;
	tail->type = SYM_LABEL;
	/*TAC* ptr_0=cur = create_a_node(head, NULL, NULL, TAC_LABEL);
	cur = cond->tac;
	TAC* copy = tac_copy(cur);
	cur = join_tac(ptr_0,cur);
	cur = create_another_node(cur, cond->ret, tail, NULL, TAC_IFZ);*/
	cur = cond->tac;
	TAC* copy = tac_copy(cur);
	cur = create_another_node(cur, cond->ret, tail, NULL, TAC_IFZ);
	cur = create_another_node(cur, head, NULL, NULL, TAC_LABEL);

	cur = join_tac(cur, stmt);
	if(field_mid && field_mid->eff)
	{
		field_mid->type=SYM_LABEL;
		field_mid->name=get_label_name();
		cur=create_another_node(cur,field_mid,NULL,NULL,TAC_LABEL);
	}
	cur = join_tac(cur, copy);
	SYMB* ptr = look_up_var(get_temp_var_name(), 1);
	
	cur = create_another_node(cur, ptr, cond->ret, NULL, TAC_NOT);
	cur = create_another_node(cur, ptr, head, NULL, TAC_IFZ);
	update_copy_var(copy);
	cur = create_another_node(cur, ptr, NULL, NULL, TAC_VAR);
	cur = create_another_node(cur, tail, NULL, NULL, TAC_LABEL);


	head->eff = 1;
	tail->eff = 1;
	pop_field();
	return cur;
}
void update_copy_var(TAC* tac)
{
	SYMB* cur = NULL;
	SYMB* ch = NULL;
	TAC* keep = tac;
	while (tac != NULL)
	{
		if (tac->op == TAC_VAR)
		{
			ch = tac->a;
			cur = look_up_var(get_temp_var_name(), 1);
			tac->a = cur;
			while (keep)
			{
				if (keep->a == ch)
				{
					keep->a = cur;
				}
				if (keep->b == ch)
				{
					keep->b = cur;
				}
				if (keep->c == ch)
				{
					keep->c = cur;
				}
				keep = keep->next;
			}
			keep = tac;
		}
		tac = tac->next;
	}
}
TAC* tac_copy(TAC* cur)
{
	TAC* ptr = NULL;
	ptr = create_a_tac();
	TAC* head = ptr;
	while (cur != NULL)
	{
		ptr = create_a_tac();
		memcpy(ptr, cur, sizeof(TAC));
		ptr->next = NULL;
		join_tac(head, ptr);
		cur = cur->next;
	}
	ptr = head->next;
	//free
	safe_free(1, &head);
	return ptr;
}
void print_tac(void)
{
	tac_print(get_tac_head());
}

SYMB* declare_func(const char* name)
{
	return look_up_func(name, 1);
}


TAC* formal_var_tac(SYMB* symbol)
{
	return create_a_node(symbol, NULL, NULL, TAC_FORMAL);
}


int	is_an_int(SYMB* int_value)
{
	if (int_value->type == SYM_INT)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
int is_have_return(TAC* block)
{
	while (block != NULL)
	{
		if (block->op == TAC_RETURN && block->a != NULL)
		{
			return 1;
		}
		block = block->next;
	}
	return 0;
}
int is_an_array(SYMB* int_value)
{
	if (int_value->type == SYM_ARRAY)
	{
		return 1;
	}
	return 0;
}


ENODE* mknode_for_array(ENODE* pre, ENODE* exp, int num)
{
	TAC* cur = join_tac(pre->tac, exp->tac);
	const char* name = NULL;
	SYMB* t1 = NULL;
	SYMB* t2 = NULL;
	TAC* tail = NULL;
	ENODE* t = (ENODE*)safe_malloc(sizeof(ENODE), "mknode_for_array");
	if (pre->ret->type == SYM_INT)
	{
		t1 = look_up_int(num * pre->ret->value);
	}
	else
	{
		name = get_temp_var_name();
		t1 = look_up_var(name, 1);
		tail = create_another_node(tail, t1, NULL, NULL, TAC_VAR);
		tail = join_tac(tail, create_a_node(t1, pre->ret, look_up_int(num), TAC_MUL));
	}
	//	if (pre->ret->type == SYM_INT && pre->ret->type == SYM_INT)
	if (t1->type == SYM_INT && exp->ret->type == SYM_INT)
	{
		t2 = look_up_int(t1->value + exp->ret->value);
		t->ret = t2;
	}
	else
	{
		name = get_temp_var_name();
		t2 = look_up_var(name, 1);
		tail = create_another_node(tail, t2, NULL, NULL, TAC_VAR);
		tail = join_tac(tail, create_a_node(t2, t1, exp->ret, TAC_ADD));
	}
	cur = join_tac(cur, tail);
	t->tac = cur;
	t->ret = t2;
	return  t;
}
char* get_temp_var_name(void)
{
	char* name = (char*)safe_malloc(sizeof(char) * 50, "get_temp_var_name");
	sprintf(name, "@%d", temp_count);
	temp_count++;
	return name;
}
TAC* find_tac_tail(TAC* head)
{
	if (head == NULL)
	{
		return NULL;
	}
	while (head->next != NULL)
	{
		head = head->next;
	}
	return head;
}
int is_a_function(SYMB* func)
{
	return func->type == SYM_FUNC;
}
void push_field(void)
{
	push_two_field(&stack_head, &field_head);
	push_two_field(&stack_tail, &field_tail);
	push_two_field(&stack_mid,&field_mid);
	field_head->eff = 0;
	field_tail->eff = 0;
}
void pop_field(void)
{
	pop_two_field(&stack_head, &field_head);
	pop_two_field(&stack_tail, &field_tail);
	pop_two_field(&stack_mid,&field_mid);
}
void push_two_field(SYMB** stack, SYMB** field)
{
	if (*stack == NULL)
	{
		*stack = (SYMB*)safe_malloc(sizeof(SYMB), "push_two_field");
		*field = *stack;
	}
	else
	{
		*field = (SYMB*)safe_malloc(sizeof(SYMB), "push_two_field");
		SYMB* cur = *stack;
		while (cur->next != NULL)
		{
			cur = cur->next;
		}
		cur->next = *field;
	}
}
void pop_two_field(SYMB** stack, SYMB** field)
{
	if (*field == NULL)
	{
		return;
	}


	if (*field == *stack)
	{
		if ((*field)->eff)
		{
			*field = NULL;
			*stack = NULL;
		}
		else
		{
			safe_free(1, stack);
		}
		return;
	}
	SYMB* cur = *stack;
	while (cur->next != *field)
	{
		cur = cur->next;
	}
	if ((*field)->eff == 0)
	{
		safe_free(1, &cur->next);
	}
	cur->next = NULL;
	*field = cur;
}

ENODE* mknode_for_binary(ENODE* pre, ENODE* now, int type)
{
	TAC* tac = join_tac(pre->tac, now->tac);
	ENODE* cur = (ENODE*)safe_malloc(sizeof(ENODE), "mknode_for_binary");
	cur->tac = tac;
	tac = NULL;
	int change = lex_val_to_tac(type);
	if (pre->ret->type == SYM_INT && now->ret->type == SYM_INT)
	{

		int value = calc_binary_exp(pre->ret->value,
			now->ret->value, change);
		SYMB* symbol_int = look_up_int(value);
		cur->next = NULL;
		cur->ret = symbol_int;
	}
	else
	{
		SYMB* var = look_up_var(get_temp_var_name(), 1);
		tac = create_another_node(tac, var, NULL, NULL, TAC_VAR);
		tac = create_another_node(tac, var, pre->ret, now->ret, change);
		cur->ret = var;
		cur->tac = join_tac(cur->tac, tac);
	}
	return cur;
}
int calc_binary_exp(int left, int right, int type)
{
	int newval = 0;
	switch (type)
	{
	case TAC_EQ:
		newval = (left == right);
		break;
	case TAC_NE:
		newval = (left != right);
		break;
	case TAC_LT:
		newval = (left < right);
		break;
	case TAC_LE:
		newval = (left <= right);
		break;
	case TAC_GT:
		newval = (left > right);
		break;
	case TAC_GE:
		newval = (left >= right);
		break;
	case TAC_AND:
		newval = (left && right);
		break;
	case TAC_OR:
		newval = (left || right);
		break;
	case TAC_ADD:
		newval = left + right;
		break;
	case TAC_SUB:
		newval = left - right;
		break;
	case TAC_MUL:
		newval = left * right;
		break;
	case TAC_DIV:
		newval = left / right;
		break;
	case TAC_MOD:
		newval = left % right;
		break;
	}
	return newval;
}
int lex_val_to_unary(int type)
{
	int newval = 0;
	switch (type)
	{
	case '!':
		newval = TAC_NOT;
		break;
	case '-':
		newval = TAC_NEG;
		break;
	case '+':
		newval = TAC_POSI;
		break;
	default:
		newval = 0;
		break;
	}
	return newval;

}
int lex_val_to_tac(int type)
{
	int newval = 0;
	switch (type)
	{
	case OR:
		newval = TAC_OR;
		break;
	case AND:
		newval = TAC_AND;
		break;

	case '>':
		newval = TAC_GT;
		break;
	case '<':
		newval = TAC_LT;
		break;
	case EQ:
		newval = TAC_EQ;
		break;
	case GE:
		newval = TAC_GE;
		break;
	case LE:
		newval = TAC_LE;
		break;
	case NE:
		newval = TAC_NE;
		break;
	case '*':
		newval = TAC_MUL;
		break;
	case '/':
		newval = TAC_DIV;
		break;
	case '-':
		newval = TAC_SUB;
		break;
	case '+':
		newval = TAC_ADD;
		break;
	case '%':
		newval = TAC_MOD;
		break;
	default:
		newval = 0;
		break;
	}
	return newval;
}

ENODE* mknode_for_unary(int type, ENODE* node)
{

	int change = lex_val_to_unary(type);
	if (node->ret->type == SYM_INT)
	{
		node->ret = look_up_int(calc_unary_exp(
			change, node->ret->value));
	}
	else
	{
		TAC* ptr = node->tac;
		//ptr = find_tac_tail(ptr);
		SYMB* symbol = look_up_var(get_temp_var_name(), 1);
		ptr = create_another_node(ptr, symbol, NULL, NULL, TAC_VAR);
		ptr = join_tac(ptr, create_a_node(symbol, node->ret, NULL, change));
		node->tac = ptr;
		node->ret = symbol;
	}
	return node;
}
int calc_unary_exp(int type, int value)
{
	int newval = 0;
	switch (type)
	{
	case TAC_NOT:
		newval = !value;
		break;
	case TAC_POSI:
		newval = +value;
		break;
	case TAC_NEG:
		newval = -value;
		break;
	default:
		newval = 0;
		break;
	}
	return newval;

}



//ENODE* mknode_call_ret(const char* func_name, ENODE* args);

ENODE* make_enode(void)
{
	return (ENODE*)safe_malloc(sizeof(ENODE), "make_enode");
}

void cg(void)
{
	fprintf(stdout, "this is cg(void)\n");
}




TAC* get_tac_head(void)
{
	return tac_head;
}

int is_not_global(SYMB* symbol)
{
	if (symbol->name[0] == 'g')
	{
		return 0;
	}
	else
	{
		return 1;
	}
}
void make_symbol_global(void)
{
	int count = 0;
	SYMB* ptr = global_symbol_table->symbol;
	while (ptr != NULL)
	{
		if (ptr->type == SYM_ARRAY || ptr->type == SYM_VAR)
		{
			sprintf(ptr->name, "g%d", count);
			count++;
		}
		if (ptr->type == SYM_FUNC && !strcmp(ptr->name, "putf"))
		{
			sprintf(ptr->name, "printf");
		}
		if (ptr->type == SYM_FUNC && !strcmp(ptr->name, "starttime"))
		{
			sprintf(ptr->name, "_sysy_starttime");
		}
		if (ptr->type == SYM_FUNC && !strcmp(ptr->name, "stoptime"))
		{
			sprintf(ptr->name, "_sysy_stoptime");
		}
		ptr->store = 0;
		ptr = ptr->next;

	}
}
void tac_trans_name(SYMB* head)
{
	char rename[20];
	while (head != NULL)
	{
		if (head->type == SYM_VAR || head->type == SYM_ARRAY)
		{
			if (head->name[0] == '@')
			{
				head->name[0] = 't';
			}
			else
			{
				sprintf(rename, "u%d", count_for_rename);
				sprintf(head->name, "%s", rename);
				count_for_rename++;
			}
		}
		
		head = head->next;
	}
}

void tac_rename_node(SYMB* head, const char* rename)
{

	sprintf(head->name, "%s", rename);
}

void tac_print(TAC* node)
{
	while (node != NULL)
	{
		tac_node_print(node);
		node = node->next;
		fprintf(stdout, "\n");
	}
}

void tac_node_print(TAC* i)
{
	char sa[12]; /* For text of TAC args */
	char sb[12];
	char sc[12];

	switch (i->op)
	{
	case TAC_UNDEF:
		printf("undef");
		break;

	case TAC_ADD:
		printf("%s = %s + %s", ts(i->a, sa), ts(i->b, sb), ts(i->c, sc));
		break;

	case TAC_SUB:
		printf("%s = %s - %s", ts(i->a, sa), ts(i->b, sb), ts(i->c, sc));
		break;

	case TAC_MUL:
		printf("%s = %s * %s", ts(i->a, sa), ts(i->b, sb), ts(i->c, sc));
		break;

	case TAC_DIV:
		printf("%s = %s / %s", ts(i->a, sa), ts(i->b, sb), ts(i->c, sc));
		break;

	case TAC_EQ:
		printf("%s = (%s == %s)", ts(i->a, sa), ts(i->b, sb), ts(i->c, sc));
		break;
	case TAC_MOD:
		printf("%s = (%s %% %s)", ts(i->a, sa), ts(i->b, sb), ts(i->c, sc));
		break;
	case TAC_NE:
		printf("%s = (%s != %s)", ts(i->a, sa), ts(i->b, sb), ts(i->c, sc));
		break;

	case TAC_LT:
		printf("%s = (%s < %s)", ts(i->a, sa), ts(i->b, sb), ts(i->c, sc));
		break;

	case TAC_LE:
		printf("%s = (%s <= %s)", ts(i->a, sa), ts(i->b, sb), ts(i->c, sc));
		break;

	case TAC_GT:
		printf("%s = (%s > %s)", ts(i->a, sa), ts(i->b, sb), ts(i->c, sc));
		break;

	case TAC_GE:
		printf("%s = (%s >= %s)", ts(i->a, sa), ts(i->b, sb), ts(i->c, sc));
		break;
	case TAC_OR:
		printf("%s = (%s || %s)", ts(i->a, sa), ts(i->b, sb), ts(i->c, sc));
		break;
	case TAC_AND:
		printf("%s = (%s && %s)", ts(i->a, sa), ts(i->b, sb), ts(i->c, sc));
		break;
	case TAC_NEG:
		printf("%s = - %s", ts(i->a, sa), ts(i->b, sb));
		break;
	case TAC_POSI:
		printf("%s = + %s", ts(i->a, sa), ts(i->b, sb));
		break;
	case TAC_NOT:
		printf("%s = ! %s", ts(i->a, sa), ts(i->b, sb));
		break;
	case TAC_COPY:
		printf("%s = %s", ts(i->a, sa), ts(i->b, sb));
		break;

	case TAC_GOTO:
		printf("goto %s", ts(i->a, sa));
		break;

	case TAC_IFZ:
		printf("ifz %s goto %s", ts(i->a, sa), ts(i->b, sb));
		break;
	case TAC_IFEQ:
		printf("if %s == %s goto %s", ts(i->a,sa), 
		ts(i->c, sc),ts(i->b, sb));
		break;
	case TAC_IFNE:
		printf("if %s != %s goto %s", ts(i->a,sa),
		ts(i->c, sc),ts(i->b, sb));
		break;
	case TAC_IFLT:
		printf("if %s < %s goto %s", ts(i->a,sa),
		ts(i->c, sc),ts(i->b, sb));
		break;
	case TAC_IFLE:
		printf("if %s <= %s goto %s", ts(i->a,sa),
		ts(i->c, sc),ts(i->b, sb));
		break;
	case TAC_IFGT:
		printf("if %s > %s goto %s", ts(i->a,sa),
		ts(i->c, sc),ts(i->b, sb));
		break;
	case TAC_IFGE:
		printf("if %s >= %s goto %s", ts(i->a,sa),
		ts(i->c, sc),ts(i->b, sb));
		break;
	case TAC_ACTUAL:
		printf("actual %s", ts(i->a, sa));
		break;

	case TAC_FORMAL:
		printf("formal %s", ts(i->a, sa));
		break;

	case TAC_CALL:
		if(i->a == NULL)
			printf("call %s", ts(i->b, sb));
		else printf("%s = call %s", ts(i->a, sa), ts(i->b, sb));
		break;

	case TAC_RETURN:
		if(i->a != NULL)
			printf("return %s", ts(i->a, sa));
		else printf("return");
		break;

	case TAC_LABEL:
		printf("label %s", ts(i->a, sa));
		break;

	case TAC_VAR:
		printf("var %s", ts(i->a, sa));
		break;

	case TAC_BEGINFUNC:
		printf("begin");
		break;

	case TAC_ENDFUNC:
		printf("end");
		break;
	case TAC_ARR_L:
		printf("%s[%s]=%s", ts(i->a, sa), ts(i->b, sb), ts(i->c, sc));
		break;
	case TAC_ARR_R:
		printf("%s=%s[%s]", ts(i->a, sa), ts(i->b, sb), ts(i->c, sc));
		break;
	case TAC_ARR:
		printf("var %s[%s]", ts(i->a, sa), ts(i->b, sb));
		break;
	case TAC_LEA:
		printf("%s=&%s[%s]", ts(i->a, sa), ts(i->b, sb), ts(i->c, sc));
		break;
	case TAC_STR:
		printf("S%d=\"%s\"", i->a->label, i->a->name);
		break;
	case TAC_FUNC:
		printf("\nFunc %s", ts(i->a, sa));
		break;
	case TAC_ZERO:
		printf("%s[%s...%s]=0", ts(i->a, sa), ts(i->b, sb), ts(i->c, sc));
		break;
	default:
		printf("unknown TAC opcode");
		break;
	}

	fflush(stdout);
}

const char* ts(SYMB* s, char* str)
{
	if (s == NULL)
	{
		return "(nullptr)";
	}

	switch (s->type)
	{
	case SYM_FUNC:
	case SYM_VAR:
	case SYM_ARRAY:
	case SYM_LABEL:
		return s->name;

	case SYM_TEXT:
		sprintf(str, "S%d", s->label);
		return str;

	case SYM_INT:
		sprintf(str, "%d", s->value);
		return str;

	default:
		printf("unknown TAC arg type: %s\n", s->name);
		return NULL;
	}
}


SYMB* look_up_int(int value)
{
	SYMB* cur = look_up_symb(SYM_INT, NULL, value);
	if (cur == NULL)
	{
		cur = create_int_symb(value);
		join_symb(cur);
		return cur;
	}

	return cur;
}

SYMB* look_up_label(const char* name, int create)
{
	SYMB* cur = look_up_symb(SYM_LABEL, name, 0);
	if (cur != NULL && create == 1)
	{
		expect_str(name);
		return NULL;
	}
	if (cur == NULL && create == 1)
	{
		cur = create_label_symb(name);
		Table* ptr = local_symbol_table;
		local_symbol_table = global_symbol_table;
		join_symb(cur);
		local_symbol_table = ptr;
		return cur;
	}
	return cur;
}

SYMB* look_up_str(const char* name)
{
	SYMB* cur = look_up_symb(SYM_TEXT, name, 0);
	if (cur == NULL)
	{
		cur = create_str_symb(name);
		cur->label = count_for_string++;
		Table* ptr = local_symbol_table;
		local_symbol_table = global_symbol_table;
		join_symb(cur);
		local_symbol_table = ptr;
		return cur;
	}
	return cur;
}

SYMB* look_up_var(const char* name, int create)
{
	if (!create)
	{
		return look_up_symb(SYM_VAR, name, 0);
	}
	if (create != 1)
	{
		expect_str("error in look_up_var");
	}

	SYMB* cur = find_symb(SYM_VAR, name, 0, local_symbol_table->symbol);
	if (cur != NULL)
	{
		expect_str("repet_declartion");
		return NULL;
	}
	if (cur == NULL && create == 1)
	{
		cur = create_var_symb(name);
		join_symb(cur);
		return cur;
	}
	return cur;

}

SYMB* look_up_array(const char* name, int create)
{
	if (!create)
	{
		return look_up_symb(SYM_ARRAY, name, 0);
	}
	if (create != 1)
	{
		expect_str("error in look_up_array");
	}

	SYMB* cur = find_symb(SYM_ARRAY, name, 0, local_symbol_table->symbol);
	if (cur != NULL)
	{
		expect_str("repet_declartion");
		return NULL;
	}
	if (cur == NULL && create == 1)
	{
		cur = create_var_symb(name);
		join_symb(cur);
		return cur;
	}
	return cur;
}
SYMB* look_up_func(const char* name, int create)
{
	SYMB* cur = look_up_symb(SYM_FUNC, name, 0);
	if (cur != NULL && create == 1)
	{
		expect_str("it has declare");
		return NULL;
	}
	if (cur == NULL && create == 1)
	{
		cur = create_func_symb(name);
		join_symb(cur);
	}
	return cur;
}
void join_symb(SYMB* symbol)
{
	SYMB* cur = *get_local_symbol_head();
	SYMB** cur0 = get_local_symbol_head();
	if (cur == NULL)
	{
		*cur0 = symbol;
		return;
	}
	while (cur->next != NULL)
	{
		cur = cur->next;
	}
	cur->next = symbol;
	symbol->next = NULL;

}

SYMB* look_up_symb(int type, const char* name, int value)
{
	SYMB* cur = *get_local_symbol_head();
	cur = find_symb(type, name, value, cur);
	if (cur == NULL && local_scope == 1)
	{
		Table* sym = global_symbol_table;
		SYMB* t = NULL;
		while (sym != local_symbol_table)
		{
			t = find_symb(type, name, value, sym->symbol);
			if (t != NULL)
			{
				cur = t;
			}
			sym = sym->next;
		}
	}
	return cur;
}
SYMB* find_symb(int type, const char* name, int value, SYMB* cur)
{
	while (cur != NULL)
	{
		if (cur->type == type)
		{
			if (type == SYM_INT && cur->value == value)
			{
				break;
			}
			else if (name != NULL && strcmp(name, cur->name) == 0)
			{
				break;
			}
		}
		cur = cur->next;
	}
	return cur;
}
SYMB* create_func_symb(const char* name)
{
	return create_symb(SYM_FUNC, name, 0);
}
SYMB* create_var_symb(const char* name)
{
	return create_symb(SYM_VAR, name, 0);
}
SYMB* create_arr_symb(const char* name)
{
	return create_symb(SYM_ARRAY, name, 0);
}
SYMB* create_label_symb(const char* name)
{
	return create_symb(SYM_LABEL, name, 0);
}
SYMB* create_str_symb(const char* name)
{
	return create_symb(SYM_TEXT, name, 0);
}
SYMB* create_int_symb(int value)
{
	return create_symb(SYM_INT, NULL, value);
}

SYMB* create_symb(int type, const char* name, int value)
{
	SYMB* cur = (SYMB*)safe_malloc(sizeof(SYMB), "create_symb");
	cur->type = type;
	if (type == SYM_INT)
	{
		cur->name = NULL;
		cur->value = value;
	}
	else
	{
		cur->name = (char*)safe_malloc(sizeof(char) * 50, "create_symb");
		strcpy(cur->name, name);
	}
	cur->next = NULL;
	return cur;
}



//重开符号表

void init_symbol_table(void)
{
	global_symbol_table = create_table();
	local_symbol_table = global_symbol_table;
}
Table* create_table(void)
{
	return (Table*)safe_malloc(sizeof(Table), "create_table");
}
void into_new_table(void)
{
	local_symbol_table->next = create_table();
	local_symbol_table = local_symbol_table->next;
}

void out_new_table(void)
{
	tac_trans_name(local_symbol_table->symbol);
	Table* ptr = global_symbol_table;
	while (ptr->next != local_symbol_table)
	{
		ptr = ptr->next;
	}
	local_symbol_table = ptr;
	safe_free(1, &ptr->next);
}

SYMB** get_local_symbol_head(void)
{
	return &local_symbol_table->symbol;
}
SYMB** get_global_symbol_head(void)
{
	return &global_symbol_table->symbol;
}

SYMB* symb_deep_copy(SYMB* symbol)
{
	SYMB* result = (SYMB*)safe_malloc(sizeof(SYMB), "symb_deep_copy");
	result->type = symbol->type;
	result->store = symbol->store;
	result->value = symbol->value;
	result->label = symbol->label;
	result->arr = symbol->arr;
	result->address = symbol->address;
	result->next = NULL;
	result->name = (char*)safe_malloc(sizeof(char) * 50, "symb_deep_copy");
	strcpy(result->name, symbol->name);
	return result;
}

int str_to_int( char* name)
{
	my_strlwr(name);
	if (name[0] != '0')
	{
		return atoi(name);
	}
	if (name[1]=='x')
	{
		return hex_to_dec(name);
	}
	if (name[1] == 'b')
	{
		return bin_to_dec(name);
	}
	return oct_to_dec(name);
	
}

char* my_strlwr(char* str)     
{
	assert(str);                  
	char* ret = str;              
	while (*str != '\0')          
	{
		if ((*str >= 'A') && (*str <= 'Z'))  
		{
			*str = *str + 32;         
			str++;
		}
		else
			str++;
	}
	return ret;             
}

int hex_to_dec(char hex[]) 
{

	int len = strlen(hex);
	int i=2;
	int dec = 0;
	while(i<len)
	{
		dec = dec * 16;
		dec += (hex[i] >= '0' && hex[i] <= '9'
			? hex[i] - '0'
			: hex[i] >= 'a' && hex[i] <= 'f'
			? hex[i] - 'a'
			: 0);
		i++;
	}
	return dec;
}

int oct_to_dec(char oct[])
{
	int len = strlen(oct);
	int i = 1;
	int dec = 0;
	while( i < len)
	{
		dec = dec * 8;
		dec += (oct[i] >= '0' && oct[i] <= '9'
			? oct[i] - '0'
			: 0);
		i++;
	}
	return dec;
}
int bin_to_dec(char bin[])
{
	int len = strlen(bin);
	int i = 2;
	int dec = 0;
	while ( i < len )
	{
		dec = dec * 2;
		dec += bin[i] - '0';
		i++;
	}
	return dec;
}


#endif
