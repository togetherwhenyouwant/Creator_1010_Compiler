#ifndef ARM_HPP
#define ARM_HPP
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <cstdint>
#include <map>
#include "frontEndInput.hpp"

using namespace std;


#define REG_ZERO 0
#define REG_NUM 16

typedef struct functag
{
    char *name;
    char *returnName;
    int numVar;
    int numFormal;
    int maxActual;
    int totalSpace;
    int begin;
    int end;
    int numReturn;
    int rmReturn;
    int reg2Offset;
    int reg3Offset;
    struct functag *next;
} FUNCTAG;
typedef struct armCode
{
    int opCode;
    int reg[16];
    int immOne;
    int imm;
    char *label;
    char *name;
    int lr;
    struct armCode *next;
    struct armCode *prev;
} ARMCODE;
typedef struct division{
	int multiplication;
	int factorial;
	int positive;
}DIVISION;
enum
{
    ARM_UNDEF = 0,
    ARM_STORE_ACTUAL,
    ARM_ADD,
    ARM_SUB,
    ARM_MUL,
    ARM_DIV,
    ARM_EQ,
    ARM_NE,
    ARM_LT,
    ARM_LE,
    ARM_GT,
    ARM_GE,
    ARM_IFEQ,
    ARM_IFNE,
    ARM_IFLT,
    ARM_IFLE,
    ARM_IFGT,
    ARM_IFGE,
    ARM_AND,
    ARM_OR,
    ARM_MOD,
    ARM_NEG,
    ARM_MOV_IMM,
    ARM_MOV_REG,
    ARM_GOTO,
    ARM_LABEL,
    ARM_FUNC,
    ARM_PUSH,
    ARM_POP,
    ARM_LEA_GLOBAL,
    ARM_LEA_ARR,
    ARM_STORE,
    ARM_STORE_ARR,
    ARM_LOAD,
    ARM_LOAD_ARR,
    ARM_CALL,
    ARM_RETURN,
    ARM_CMP,
    ARM_GLOBAL_VAR,
    ARM_GLOBAL_STR,
    ARM_GLOBAL_ARR,
    ARM_WORD,
    ARM_ZERO,
    ARM_LSL,
    ARM_DIV2,
    ARM_LSR,
    ARM_END,
    ARM_LEA_STR,
    ARM_LOAD_THREE,
    ARM_STORE_THREE,
    ARM_ASR,
    ARM_MOVLT,
    ARM_SMULL,
    ARM_RSBLT,
    ARM_RSBS,
    ARM_RSBPL
};

DIVISION *findDivision(int n);
void peepIfz(ARMCODE *head,ARMCODE *aim);
void peepSub(ARMCODE *head,ARMCODE *aim,int i);
void peepMovImm(ARMCODE *head,ARMCODE *aim);
void peepStore(ARMCODE *head,ARMCODE *aim);
void peepLoad(ARMCODE *head,ARMCODE *aim);
void peepGoto(ARMCODE *head,ARMCODE *aim);
void peepHole(ARMCODE *head);
void cg(TAC *tacFirst);
void loadFormal(FUNCTAG* fun,ARMCODE* head);
int cgCode(TAC *tacCode, FUNCTAG *fun, ARMCODE *head);
void cgCall(int opCode, TAC *tacCode, FUNCTAG *fun, ARMCODE *head);
void cgActual(int opCode, TAC *tacCode, FUNCTAG *fun, ARMCODE *head, int r, int off);
int findOffset(TAC *a);
void cgArrRight(int opCode, TAC *tacCode, FUNCTAG *fun, ARMCODE *head);
void cgArrLeft(int opCode, TAC *tacCode, FUNCTAG *fun, ARMCODE *head);
void cgLea(int opCode, TAC *tacCode, FUNCTAG *fun, ARMCODE *head);
void cgZero(int opCode, TAC *tacCode, FUNCTAG *fun, ARMCODE *head);
void cgEnd(int opCode, TAC *tacCode, FUNCTAG *fun, ARMCODE *head);
void cgReturn(int opCode, TAC *tacCode, FUNCTAG *fun, ARMCODE *head);
void cgStoreFormal(int opCode, TAC *tacCode, FUNCTAG *fun, ARMCODE *head);
void cgPush(int opCode, TAC *tacCode, FUNCTAG *fun, ARMCODE *head);
void functionInit(FUNCTAG *fun);
void cgIfZero(int opCode, TAC *tacCode, FUNCTAG *fun, ARMCODE *head);
void cgCmpGoto(int opCode, TAC *tacCode, FUNCTAG *fun, ARMCODE *head);
void cgFunc(int opCode, TAC *tacCode, FUNCTAG *fun, ARMCODE *head);
void cgGotoLabel(int opCode, TAC *tacCode, FUNCTAG *fun, ARMCODE *head);
void cgCopy(int opCode, TAC *tacCode, FUNCTAG *fun, ARMCODE *head);
void cgCmp(int opCode, TAC *tacCode, FUNCTAG *fun, ARMCODE *head);
void cgNot(int opCode, TAC *tacCode, FUNCTAG *fun, ARMCODE *head);
void cgNeg(int opCode, TAC *tacCode, FUNCTAG *fun, ARMCODE *head);
int findPower(int num); //乘法除法变简单移位
void cgDivMod(int opCode, TAC *tacCode, FUNCTAG *fun, ARMCODE *head);
void cgAndOr(int opCode, TAC *tacCode, FUNCTAG *fun, ARMCODE *head);
void cgAddSubMul(int opCode, TAC *tacCode, FUNCTAG *fun, ARMCODE *head);
void loadToReg(int reg, SYMB *n, ARMCODE *head);
void error(char *str);
void cgLast();
int findPushNumber(FUNCTAG *fun);
vector<int> getRegisterUsed(string sFuncName);
void makeVector2Array(int arr[], const std:: vector<int>& vRegister);
void cgPre(TAC *tacCode, FUNCTAG *fun, ARMCODE *head);
void beginToFunc(TAC *pre, TAC *now, TAC *nex, ARMCODE *head);
TAC *funcToEnd(TAC *pre, TAC *now, TAC *nex, FUNCTAG *fun); //func到end间的函数变量ok
void printVar(SYMB *a, SYMB *b, ARMCODE *head);             //输出var定义ok
void printArr(SYMB *a, SYMB *b, ARMCODE *head);             //输出全局数组ok
void printStr(SYMB *a, ARMCODE *head);                      //输出全局字符串ok
void cgHead();
SYMB *SYMBConst(int n); //创建新的整数节点（符号表）ok
void registerInit(int reg[]);
void addToReg(int reg[], int i);
void preserveScene(FUNCTAG *fun, ARMCODE *head);
void restoreScene(FUNCTAG *fun, ARMCODE *head);
void loadFromStack(int start, int aim, FUNCTAG *fun, ARMCODE *head);

int register_malloc(string func_name, char *var_name);

void addNodeToChain(ARMCODE *head, ARMCODE *node);
void printArmCode(ARMCODE *i);
void movImm(int r, int num);
void printFunc(char *name);
ARMCODE *createArmCode(void);
ARMCODE *createArmCodeFunc(int opCode, int lr);
ARMCODE *createArmCodeLabel(int opCode, char *label);
ARMCODE *createArmCodeOneReg(int opCode, int r1, SYMB *a);
ARMCODE *createArmCodeOneRegOneName(int opCode, int r1, char *name);
ARMCODE *createArmCodeTwoReg(int opCode, int r1, int r2, SYMB *a);
ARMCODE *createArmCodeTwoRegOneImm(int opCode, int r1, int r2, int imm1, SYMB *a);
ARMCODE *createArmCodeThreeReg(int opCode, int r1, int r2, int r3, SYMB *a);
ARMCODE *createArmCodeNothing(int opCode);
ARMCODE *createArmCodeOneRegOneImm(int opCode, int r1, int imm1);
ARMCODE *createArmCodeNameOneImm(int opCode, char *name, int imm1);
ARMCODE *createArmCodeString(int opCode, int name, char *label);
ARMCODE *createArmCodeName(int opCode, char *name);
ARMCODE *createArmCodeOneImm(int opCode, int imm1);
ARMCODE *createArmCodeTwoRegTwoImm(int opCode, int r1, int r2, int imm1,int imm2, SYMB *a);
ARMCODE *findPush(ARMCODE *pop);
void addRegToPushPop(ARMCODE *pop, int reg[]);
const char* ts_r(SYMB* s, char* str);
static int reg[16] = {0};
static int formalOffset;
static int varOffset;
static int actualOffset;
static int off = 0;
static int firstActual = 1;
static SYMB *symb_list;
static vector<TAC*> formalList;

void tac_node_print_r(TAC* i)
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
        printf("%s = %s + %s", ts_r(i->a, sa), ts_r(i->b, sb), ts_r(i->c, sc));
        break;

    case TAC_SUB:
        printf("%s = %s - %s", ts_r(i->a, sa), ts_r(i->b, sb), ts_r(i->c, sc));
        break;

    case TAC_MUL:
        printf("%s = %s * %s", ts_r(i->a, sa), ts_r(i->b, sb), ts_r(i->c, sc));
        break;

    case TAC_DIV:
        printf("%s = %s / %s", ts_r(i->a, sa), ts_r(i->b, sb), ts_r(i->c, sc));
        break;

    case TAC_EQ:
        printf("%s = (%s == %s)", ts_r(i->a, sa), ts_r(i->b, sb), ts_r(i->c, sc));
        break;
    case TAC_MOD:
        printf("%s = (%s %% %s)", ts_r(i->a, sa), ts_r(i->b, sb), ts_r(i->c, sc));
        break;
    case TAC_NE:
        printf("%s = (%s != %s)", ts_r(i->a, sa), ts_r(i->b, sb), ts_r(i->c, sc));
        break;

    case TAC_LT:
        printf("%s = (%s < %s)", ts_r(i->a, sa), ts_r(i->b, sb), ts_r(i->c, sc));
        break;

    case TAC_LE:
        printf("%s = (%s <= %s)", ts_r(i->a, sa), ts_r(i->b, sb), ts_r(i->c, sc));
        break;

    case TAC_GT:
        printf("%s = (%s > %s)", ts_r(i->a, sa), ts_r(i->b, sb), ts_r(i->c, sc));
        break;

    case TAC_GE:
        printf("%s = (%s >= %s)", ts_r(i->a, sa), ts_r(i->b, sb), ts_r(i->c, sc));
        break;
    case TAC_OR:
        printf("%s = (%s || %s)", ts_r(i->a, sa), ts_r(i->b, sb), ts_r(i->c, sc));
        break;
    case TAC_AND:
        printf("%s = (%s && %s)", ts_r(i->a, sa), ts_r(i->b, sb), ts_r(i->c, sc));
        break;
    case TAC_NEG:
        printf("%s = - %s", ts_r(i->a, sa), ts_r(i->b, sb));
        break;
    case TAC_POSI:
        printf("%s = + %s", ts_r(i->a, sa), ts_r(i->b, sb));
        break;
    case TAC_NOT:
        printf("%s = ! %s", ts_r(i->a, sa), ts_r(i->b, sb));
        break;
    case TAC_COPY:
        printf("%s = %s", ts_r(i->a, sa), ts_r(i->b, sb));
        break;

    case TAC_GOTO:
        printf("goto %s", ts_r(i->a, sa));
        break;

    case TAC_IFZ:
        printf("ifz %s goto %s", ts_r(i->a, sa), ts_r(i->b, sb));
        break;
    case TAC_IFEQ:
        printf("if %s == %s goto %s", ts_r(i->a, sa), ts_r(i->c, sc), ts_r(i->b, sb));
        break;
    case TAC_IFNE:
        printf("if %s != %s goto %s", ts_r(i->a, sa), ts_r(i->c, sc), ts_r(i->b, sb));
        break;
    case TAC_IFLT:
        printf("if %s  < %s goto %s", ts_r(i->a, sa), ts_r(i->c, sc), ts_r(i->b, sb));
        break;

    case TAC_IFLE:
        printf("if %s <= %s goto %s", ts_r(i->a, sa), ts_r(i->c, sc), ts_r(i->b, sb));
        break;
    case TAC_IFGT:
        printf("if %s  > %s goto %s", ts_r(i->a, sa), ts_r(i->c, sc), ts_r(i->b, sb));
        break;
    case TAC_IFGE:
        printf("if %s >= %s goto %s", ts_r(i->a, sa), ts_r(i->c, sc), ts_r(i->b, sb));
        break;
    case TAC_ACTUAL:
        printf("actual %s", ts_r(i->a, sa));
        break;

    case TAC_FORMAL:
        printf("formal %s", ts_r(i->a, sa));
        break;

    case TAC_CALL:
        printf("%s = call %s", ts_r(i->a, sa), ts_r(i->b, sb));
        break;

    case TAC_RETURN:
        if (i->a != NULL)
            printf("return %s", ts_r(i->a, sa));
        else printf("return");
        break;

    case TAC_LABEL:
        printf("label %s", ts_r(i->a, sa));
        break;

    case TAC_VAR:
        printf("var %s", ts_r(i->a, sa));
        break;

    case TAC_BEGINFUNC:
        printf("begin");
        break;

    case TAC_ENDFUNC:
        printf("end");
        break;
    case TAC_ARR_L:
        printf("%s[%s]=%s", ts_r(i->a, sa), ts_r(i->b, sb), ts_r(i->c, sc));
        break;
    case TAC_ARR_R:
        printf("%s=%s[%s]", ts_r(i->a, sa), ts_r(i->b, sb), ts_r(i->c, sc));
        break;
    case TAC_ARR:
        printf("var %s[%s]", ts_r(i->a, sa), ts_r(i->b, sb));
        break;
    case TAC_LEA:
        printf("%s=&%s[%s]", ts_r(i->a, sa), ts_r(i->b, sb), ts_r(i->c, sc));
        break;
    case TAC_STR:
        printf("S%d=\"%s\"", i->a->label, i->a->name);
        break;
    case TAC_FUNC:
        printf("\nFunc %s", ts_r(i->a, sa));
        break;
    case TAC_ZERO:
        printf("%s[%s...%s]=0", ts_r(i->a, sa), ts_r(i->b, sb), ts_r(i->c, sc));
        break;
    default:
        printf("unknown TAC opcode");
        break;
    }

    fflush(stdout);
}

const char* ts_r(SYMB* s, char* str)
{
    if (s == NULL)
    {
        return "(nullptr)";
    }
    int a ;
    switch (s->type)
    {
    case SYM_VAR:
        a = register_malloc("main", s->name);
        if (a == -1)
        {
            sprintf(str, "mem(%s)", s->name);
        }
        else
        {
            sprintf(str, "R%d", a);
        }
        return str;
    case SYM_FUNC:
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


void cg(TAC* tacFirst)
{
    TAC* ptr_tac = tacFirst;
    #ifndef NO_PRINT
    while ((ptr_tac=ptr_tac->next) != nullptr)
    {
        tac_node_print_r(ptr_tac);
        cout << endl;

    }
    #endif

    check_value(tacFirst->next);
    int tag = 0;
    TAC *tacCode;
    FUNCTAG *funcHead = (FUNCTAG *)safe_malloc(sizeof(FUNCTAG), "Function tag head");
    registerInit(reg);
    ARMCODE *emptyHead = createArmCode();
    ARMCODE *chain = emptyHead;
    ARMCODE *node;
    //cgHead();
    cgPre(tacFirst, funcHead, chain);
    FUNCTAG *fun = funcHead->next;

    for (tacCode = tacFirst->next; tacCode != NULL; tacCode = tacCode->next) //遍历三地址码
    {
        tag = cgCode(tacCode, fun, chain);
        if (tag == 1)
        {
            fun = fun->next;
            tag = 0;
        }
        if(tag == 2){
            loadFormal(fun,chain);
            formalList.clear();
        }
        if(tag == -1){
            exit(11);
        }
    }
    node = emptyHead->next;
    peepHole(node);
    
    for (node = emptyHead->next; node != NULL; node = node->next)
    {
        printArmCode(node);
    }
    KeepRegisterMap.clear();
    //cgLast();
}

void peepHole(ARMCODE *head){
    ARMCODE *node = head;
    ARMCODE *now = head;
    ARMCODE *pre = head->prev;
    ARMCODE *next = head->next;
    char *returnname=(char *)safe_malloc(sizeof(char)*50,"func");
    while(now->opCode!=ARM_FUNC){
        now=now->next;
    }
    for(int i=0;i<3;i++){
    while(now->next!=NULL){
        switch (now->opCode)
        {
            case ARM_FUNC:
                sprintf(returnname,".%s_rm",now->name);
                break;

            case ARM_GOTO:
                if(strcmp(now->label,returnname)!=0)
                peepGoto(node,now);
                break;

            case ARM_LOAD:
                peepLoad(node,now);
                break;

            case ARM_STORE:
                peepStore(node,now);
                break;

            case ARM_MOV_IMM:
                peepMovImm(node,now);
                break;

            case ARM_SUB:
                if(now->reg[1]==11&&now->imm==1){
                    peepSub(node,now,i);
                }
                break;

            case ARM_IFEQ:
            case ARM_IFNE:
            case ARM_IFLE:
            case ARM_IFLT:
            case ARM_IFGE:
            case ARM_IFGT:
                peepIfz(node,now);
                break;
        }
        now=now->next;
    }
    now=node;
    while(now->opCode!=ARM_FUNC){
        now=now->next;
    }
    }

    
}

void peepIfz(ARMCODE *head,ARMCODE *aim){
    ARMCODE *node = head;
    ARMCODE *now = aim;
    char *label = now->label;
    ARMCODE *next=aim->next;
    ARMCODE *ptr;

    ptr=node;
    while(ptr!=NULL){
        if(ptr->opCode==ARM_LABEL && strcmp(ptr->label,label)==0){
            break;
        }
        ptr = ptr->next;
    }

    if(ptr==NULL){
        return;
    }else{
        next = ptr->next;
        if(next->opCode==ARM_GOTO){
            now->label=next->label;
        }else if(next->opCode==ARM_LABEL){
            now->label=next->label;
        }else{
            return;
        }
    }

}

void peepSub(ARMCODE *head,ARMCODE *aim,int i){
    ARMCODE *node = head;
    ARMCODE *now = aim;
    ARMCODE *next=aim->next;
    ARMCODE *ptr;

    if(next->opCode==ARM_ADD && next->imm==1 && now->reg[0]==next->reg[1]){
        now->reg[0]=next->reg[0];
        now->immOne=now->immOne-next->immOne;
        ptr=next->next;
        now->next=ptr;
        ptr->prev=now;
    }

    if(i>=1&&next->opCode==ARM_MOV_IMM && next->reg[0]!=now->reg[0]){
        ptr=now->prev;
        ptr->next=next;
        next->prev=ptr;
        ptr=next->next;
        next->next=now;
        now->next=ptr;
        now->prev=next;
        ptr->prev=now;
    }
}

void peepMovImm(ARMCODE *head,ARMCODE *aim){
    ARMCODE *node = head;
    ARMCODE *now = aim;
    ARMCODE *next=aim->next;
    ARMCODE *ptr;
    if(next->opCode==ARM_MOV_REG&&next->reg[1]==now->reg[0] && now->reg[0]==0){
        now->reg[0]=next->reg[0];
        ptr = next->next;
        now->next=ptr;
        ptr->prev=now;
        return ;
    }

    if(next->opCode==ARM_MOV_REG&&next->reg[1]==now->reg[0] && now->opCode==ARM_MOV_IMM){
        next->opCode= ARM_MOV_IMM;
        next->immOne= now->immOne;
        next->imm=1;
        return ;
    }

    if(next->opCode==ARM_LEA_ARR&&next->reg[2]==now->reg[0]){
        if(now->immOne*4<4000 && now->immOne*4>-1000){
            now->opCode=ARM_ADD;
            now->reg[0]=next->reg[0];
            now->reg[1]=next->reg[1];
            now->immOne=4*now->immOne;
            now->imm=1;
            ptr=next->next;
            now->next=ptr;
            ptr->prev=now;
            return ;
        }
    }
}

void peepStore(ARMCODE *head,ARMCODE *aim){
    ARMCODE *node = head;
    ARMCODE *now = aim;
    ARMCODE *next=aim->next;
    ARMCODE *ptr;
    if(now->imm==1){
        if(next->opCode==ARM_LOAD && next->imm==1 && now->immOne==next->immOne){
            next->opCode=ARM_MOV_REG;
            next->reg[1]=now->reg[0];
        }
    }

    if(now->imm==0){
        ptr = now->prev;
        if(ptr->opCode==ARM_SUB&&ptr->reg[1]==11 && ptr->reg[0]==now->reg[1])
        {
            if(ptr->imm==1){
                ptr->opCode=ARM_STORE;
                ptr->reg[0]=now->reg[0];
                ptr->immOne=-ptr->immOne;
            }else{
                ptr->opCode=ARM_STORE_THREE;
                ptr->reg[0]=now->reg[0];
                ptr->reg[1]=ptr->reg[2];
                ptr->immOne=0;
            }
                ptr->next=next;
                next->prev=ptr;
                return ;
        }else if(ptr->opCode==ARM_ADD&&ptr->reg[1]==11 && ptr->reg[0]==now->reg[1])
        {
            if(ptr->imm==1){
                ptr->opCode=ARM_STORE;
                ptr->reg[0]=now->reg[0];
            }else{
                ptr->opCode=ARM_STORE_THREE;
                ptr->reg[0]=now->reg[0];
                ptr->reg[1]=ptr->reg[2];
                ptr->immOne=1;
            }
                ptr->next=next;
                next->prev=ptr;
                return ;
        }
    }

    next=now->next;
    

}

void peepLoad(ARMCODE *head,ARMCODE *aim){
    ARMCODE *node = head;
    ARMCODE *now = aim;
    ARMCODE *next=aim->next;
    ARMCODE *ptr;
    if(now->imm==1){
        if(next->opCode==ARM_STORE && next->imm==1 && now->immOne==next->immOne &&now->reg[0]==next->reg[0]){
            next=next->next;
            now->next=next;
            next->prev=now;            
        }
    }else{
        if(next->opCode==ARM_STORE && next->imm==0 && now->reg[1]==next->reg[1] &&now->reg[0]==next->reg[0]){
            next=next->next;
            now->next=next;
            next->prev=now;            
        } 
    }

    next=aim->next;
    if(next->opCode==ARM_MOV_REG && next->reg[1]==now->reg[0]){
        now->reg[0]=next->reg[0];
        ptr = next->next;
        now->next=ptr;
        ptr->prev=now;
    }

    // next=now->next;
    // if(now->imm==0){
    //     ptr = now->prev;
    //     if(ptr->opCode==ARM_SUB&&ptr->reg[1]==11 && ptr->reg[0]==now->reg[1])
    //     {
    //         if(ptr->imm==1){
    //             ptr->opCode=ARM_LOAD;
    //             ptr->reg[0]=now->reg[0];
    //             ptr->immOne=-ptr->immOne;
    //         }else{
    //             ptr->opCode=ARM_LOAD_THREE;
    //             ptr->reg[0]=now->reg[0];
    //             ptr->reg[1]=ptr->reg[2];
    //             ptr->immOne=0;
    //         }
    //             ptr->next=next;
    //             next->prev=ptr;
    //             return ;
    //     }else if(ptr->opCode==ARM_ADD&&ptr->reg[1]==11 && ptr->reg[0]==now->reg[1])
    //     {
    //         if(ptr->imm==1){
    //             ptr->opCode=ARM_LOAD;
    //             ptr->reg[0]=now->reg[0];
    //         }else{
    //             ptr->opCode=ARM_LOAD_THREE;
    //             ptr->reg[0]=now->reg[0];
    //             ptr->reg[1]=ptr->reg[2];
    //             ptr->immOne=1;
    //         }
    //             ptr->next=next;
    //             next->prev=ptr;
    //             return ;
    //     }
    // }
}

void peepGoto(ARMCODE *head,ARMCODE *aim){
    ARMCODE *node = head;
    ARMCODE *now = aim;
    char *label = now->label;
    ARMCODE *next=aim->next;
    ARMCODE *ptr;

    if(next->opCode==ARM_LABEL&& strcmp(now->label,next->label)==0){
        ptr=now->prev;
        ptr->next=next;
        next->prev=ptr;
        return ;
    }


    if(next->opCode==ARM_GOTO){
        ptr = aim->next->next;
        now->next = ptr;
        ptr->prev=now;
    }

    ptr=node;
    while(ptr!=NULL){
        if(ptr->opCode==ARM_LABEL && strcmp(ptr->label,label)==0){
            break;
        }
        ptr = ptr->next;
    }

    if(ptr==NULL){
        return;
    }else{
        next = ptr->next;
        if(next->opCode==ARM_GOTO){
            now->label=next->label;
        }else if(next->opCode==ARM_LABEL){
            now->label=next->label;
        }else{
            return;
        }
    }
}


int cgCode(TAC *tacCode, FUNCTAG *fun, ARMCODE *head)
{
    ARMCODE *chain = head;
    switch (tacCode->op)
    {
    case TAC_UNDEF:
        error((char *)"cannot translate TAC_UNDEF");
        return -1;

    case TAC_ADD:
        cgAddSubMul(ARM_ADD, tacCode, fun, chain);
        return 0;

    case TAC_SUB:
        cgAddSubMul(ARM_SUB, tacCode, fun, chain);
        return 0;

    case TAC_MUL:
        cgAddSubMul(ARM_MUL, tacCode, fun, chain);
        return 0;

    case TAC_AND:
        cgAndOr(ARM_AND, tacCode, fun, chain);
        return 0;

    case TAC_OR:
        cgAndOr(ARM_OR, tacCode, fun, chain);
        return 0;

    case TAC_DIV:
        cgDivMod(ARM_DIV, tacCode, fun, chain);
        return 0;

    case TAC_MOD:
        cgDivMod(ARM_MOD, tacCode, fun, chain);
        return 0;

    case TAC_NEG:
        cgNeg(ARM_NEG, tacCode, fun, chain);
        return 0;

    case TAC_EQ:
        cgCmp(ARM_EQ, tacCode, fun, chain);
        return 0;
    case TAC_NE:
        cgCmp(ARM_NE, tacCode, fun, chain);
        return 0;
    case TAC_LT:
        cgCmp(ARM_LT, tacCode, fun, chain);
        return 0;
    case TAC_LE:
        cgCmp(ARM_LE, tacCode, fun, chain);
        return 0;
    case TAC_GT:
        cgCmp(ARM_GT, tacCode, fun, chain);
        return 0;
    case TAC_GE:
        cgCmp(ARM_GE, tacCode, fun, chain);
        return 0;

    case TAC_COPY:
    case TAC_POSI:
        cgCopy(ARM_MOV_REG, tacCode, fun, chain);
        return 0;

    case TAC_NOT:
        cgNot(ARM_UNDEF, tacCode, fun, chain);
        return 0;

    case TAC_GOTO:
        cgGotoLabel(ARM_GOTO, tacCode, fun, chain);
        return 0;

    case TAC_IFZ:
        cgIfZero(ARM_IFEQ,tacCode, fun, chain);
        return 0;

    case TAC_IFEQ:
        cgCmpGoto(ARM_IFEQ, tacCode, fun, chain);
        return 0;

    case TAC_IFNE:
        cgCmpGoto(ARM_IFNE, tacCode, fun, chain);
        return 0;

    case TAC_IFLT:
        cgCmpGoto(ARM_IFLT, tacCode, fun, chain);
        return 0;

    case TAC_IFLE:
        cgCmpGoto(ARM_IFLE, tacCode, fun, chain);
        return 0;

    case TAC_IFGT:
        cgCmpGoto(ARM_IFGT, tacCode, fun, chain);
        return 0;

    case TAC_IFGE:
        cgCmpGoto(ARM_IFGE, tacCode, fun, chain);
        return 0;

    case TAC_LABEL:
        cgGotoLabel(ARM_LABEL, tacCode, fun, chain);
        return 0;

    case TAC_FUNC:
        cgFunc(ARM_FUNC, tacCode, fun, chain);
        return 0;

    case TAC_BEGINFUNC: //函数跳转，重置栈
        functionInit(fun);
        cgPush(ARM_PUSH, tacCode, fun, chain);
        return 0;

    case TAC_FORMAL: //形参赋值给对应地址
        tacCode->a->store = 1;
        tacCode->a->formal = 1;

        if (fun->numFormal > 4)
        {
            tacCode->a->offset = findPushNumber(fun)+(fun->numFormal - 4) * 4;
        }
        else
        {
            tacCode->a->offset = -formalOffset;
            formalOffset = formalOffset - 4;
            cgStoreFormal(ARM_STORE, tacCode, fun, chain);
        }
        fun->numFormal = fun->numFormal - 1;
        formalList.push_back(tacCode);
        if(fun->numFormal==0){
            return 2;
        }else{
            return 0;           
        }


    case TAC_VAR:              //局部变量赋值给对应地址
        tacCode->a->store = 1; // local var
        varOffset += 4;
        tacCode->a->offset = -varOffset;
        return 0;

    case TAC_ARR:
        tacCode->a->store = 1;
        varOffset += tacCode->b->value * 4;
        tacCode->a->offset = -varOffset;
        return 0;

    case TAC_ACTUAL: //实参
        off = findOffset(tacCode);
        if (off > 3)
        {
            actualOffset = (off - 4) * 4;
            cgActual(ARM_UNDEF, tacCode, fun, chain, 4, actualOffset);
        }
        else
        {
            cgActual(ARM_UNDEF, tacCode, fun, chain, off, actualOffset);
        }
        firstActual = 0;
        return 0;

    case TAC_RETURN:
        cgReturn(ARM_POP, tacCode, fun, chain);
        return 0;

    case TAC_ENDFUNC:
        cgEnd(ARM_POP, tacCode, fun, chain);
        return 1;

    case TAC_CALL:
        cgCall(ARM_CALL, tacCode, fun, chain);
        firstActual = 1;
        return 0;

    case TAC_ARR_L:
        cgArrLeft(ARM_UNDEF, tacCode, fun, chain);
        return 0;

    case TAC_ARR_R:
        cgArrRight(ARM_UNDEF, tacCode, fun, chain);
        return 0;

    case TAC_LEA:
        cgLea(ARM_UNDEF, tacCode, fun, chain);
        return 0;

    case TAC_ZERO:
        cgZero(ARM_ZERO, tacCode, fun, chain);
        return 0;

    default:
        error((char *)"unknown TAC opcode to translate");
        return -1;
    }
    return -1;
}

void loadFormal(FUNCTAG* fun,ARMCODE* head){
    ARMCODE* node;
    ARMCODE* chain = head;
    int reg0;
    string funcName = fun->name;
    for(int i=formalList.size()-1;i>=0;i--){
        reg0=register_malloc(funcName,formalList[i]->a->name);
        if(reg0!=-1){
            loadToReg(reg0,formalList[i]->a,chain);
        }
    }
}

vector<int> getRegisterUsed(string sFuncName){
    vector<int> keepRegister = { 4,5,6,7,8,9,10 };
	return keepRegister;
}

int findPushNumber(FUNCTAG *fun){
    int num=0;
    for(int i=0;i<16;i++){
        if(reg[i]==1){
            num=num+4;
        }
    }
    return num;
}

void cgCall(int opCode, TAC* tacCode, FUNCTAG* fun, ARMCODE* head)
{
    ARMCODE* node;
    ARMCODE* chain = head;
    int reg0;
    string funcName = fun->name;

    if (firstActual == 1)
    {
        preserveScene(fun, chain);
    }

    node = createArmCodeLabel(opCode, tacCode->b->name);
    addNodeToChain(chain, node);

    restoreScene(fun, chain);


    if (tacCode->a != NULL)
    {
        reg0 = register_malloc(funcName, tacCode->a->name);
        if (reg0 != -1) {
            node = createArmCodeTwoReg(ARM_MOV_REG, reg0, 0, tacCode->a);
            addNodeToChain(chain, node);
        }
        else {
            if (tacCode->a->store == 1)
            {
                if (tacCode->a->offset > 255 || tacCode->a->offset < -4000)
                {
                    node = createArmCodeOneRegOneImm(ARM_MOV_IMM, 1, tacCode->a->offset);
                    addNodeToChain(chain, node);
                    node = createArmCodeThreeReg(ARM_ADD, 1, 11, 1, tacCode->a);
                    addNodeToChain(chain, node);
                    node = createArmCodeTwoReg(ARM_STORE, 0, 1, tacCode->a);
                    addNodeToChain(chain, node);
                }
                else
                {
                    node = createArmCodeOneRegOneImm(ARM_STORE, 0, tacCode->a->offset);
                    addNodeToChain(chain, node);
                }
            }
            else
            {
                node = createArmCodeOneRegOneName(ARM_LEA_GLOBAL, 1, tacCode->a->name);
                addNodeToChain(chain, node);
                node = createArmCodeTwoReg(ARM_STORE, 0, 1, tacCode->a);
                addNodeToChain(chain, node);
            }
        }
    }

}
void cgActual(int opCode, TAC *tacCode, FUNCTAG *fun, ARMCODE *head, int r, int off)
{
    ARMCODE *node;
    ARMCODE *chain = head;
    string funcName = fun->name;
    int reg0;
    int offset = off;
    if (firstActual == 1)
    {
        preserveScene(fun, chain);
    }

    if (r > 3)
    {
        reg0 = register_malloc(funcName, tacCode->a->name);
        if (reg0 == -1)
        {
            reg0 = 0;
            loadToReg(0, tacCode->a, chain);
        }
        else if (reg0 == 2)
        {
            reg0 = 0;
            loadFromStack(2, 0, fun, chain);
        }
        else if (reg0 == 3)
        {
            reg0 = 0;
            loadFromStack(3, 0, fun, chain);
        }

        if (offset > 4000)
        {
            loadToReg(1, SYMBConst(offset), chain);
            node = createArmCodeThreeReg(ARM_ADD, 1, 13, 1, tacCode->a);
            addNodeToChain(chain, node);
            node = createArmCodeTwoReg(ARM_STORE, reg0, 1, tacCode->a);
            addNodeToChain(chain, node);
        }
        else
        {
            node = createArmCodeOneRegOneImm(ARM_STORE_ACTUAL, reg0, offset);
            addNodeToChain(chain, node);
        }
    }
    else
    {
        reg0 = register_malloc(funcName, tacCode->a->name);
        if (reg0 == -1)
        {
            reg0 = r;
            loadToReg(reg0, tacCode->a, chain);
        }
        else if (reg0 == 2)
        {
            loadFromStack(2, r, fun, chain);
        }
        else if (reg0 == 3)
        {
            loadFromStack(3, r, fun, chain);
        }
        else
        {
            node = createArmCodeTwoReg(ARM_MOV_REG, r, reg0, tacCode->a);
            addNodeToChain(chain, node);
        }
    }

}

int findOffset(TAC *a)
{
    TAC *b = a->next;
    int num = 0;
    while (b->op != TAC_CALL)
    {
        if (b->op == TAC_ACTUAL)
            num++;
        b = b->next;
    }
    return num;
}

void cgArrRight(int opCode, TAC *tacCode, FUNCTAG *fun, ARMCODE *head)
{
    ARMCODE *node;
    ARMCODE *chain = head;
    string funcName = fun->name;
    int reg0 = register_malloc(funcName, tacCode->a->name);
    int reg1 = register_malloc(funcName, tacCode->b->name);
    int reg2 = register_malloc(funcName, tacCode->c->name);
    if (reg1 == -1)
    {
        reg1 = 0;
        loadToReg(0, tacCode->b, chain);
    }
    if(reg2 == -1){
        reg2 = 1;
        loadToReg(1,tacCode->c,chain);
    }

    node = createArmCodeThreeReg(ARM_LOAD_ARR, reg0, reg1, reg2, tacCode->a);
    addNodeToChain(chain, node);
}

void cgArrLeft(int opCode, TAC *tacCode, FUNCTAG *fun, ARMCODE *head)
{
    ARMCODE *node;
    ARMCODE *chain = head;
    string funcName = fun->name;
    int reg0 = register_malloc(funcName, tacCode->a->name);
    if (reg0 == -1)
    {
        reg0 = 0;
        loadToReg(0, tacCode->a, chain);
    }
    int reg1 = register_malloc(funcName, tacCode->b->name);
    if (reg1 == -1)
    {
        reg1 = 1;
        loadToReg(1, tacCode->b, chain);
    }

    node = createArmCodeThreeReg(ARM_LEA_ARR, 1, reg0, reg1, tacCode->a);
    addNodeToChain(chain, node);

    int reg2 = register_malloc(funcName, tacCode->c->name);
    if (reg2 == -1)
    {
        reg2 = 0;
        loadToReg(0, tacCode->c, chain);
    }

    node = createArmCodeTwoReg(ARM_STORE, reg2, 1, tacCode->a);
    addNodeToChain(chain, node);
}

void cgLea(int opCode, TAC *tacCode, FUNCTAG *fun, ARMCODE *head)
{
    ARMCODE *node;
    ARMCODE *chain = head;
    string funcName = fun->name;
    int reg0 = register_malloc(funcName, tacCode->a->name);
    int reg1;
    int reg2;
    if (tacCode->c == NULL)
    {
        if (tacCode->b->store == 1)
        {
            if (tacCode->b->offset > 4000 || tacCode->b->offset < -4000)
            {
                node = createArmCodeOneRegOneImm(ARM_MOV_IMM, 0, tacCode->b->offset);
                addNodeToChain(chain, node);
                node = createArmCodeThreeReg(ARM_ADD, reg0, 11, 0, tacCode->a);
                addNodeToChain(chain, node);
            }
            else
            {
                node = createArmCodeTwoRegOneImm(ARM_ADD, reg0, 11, tacCode->b->offset, tacCode->a);
                addNodeToChain(chain, node);
            }
        }
        else
        {
            node = createArmCodeOneRegOneName(ARM_LEA_GLOBAL, reg0, tacCode->b->name);
            addNodeToChain(chain, node);
        }
    }
    else
    {
        reg1 = register_malloc(funcName, tacCode->b->name);
        if (reg1 == -1)
        {
            reg1 = 0;
            loadToReg(0, tacCode->b, chain);
        }
        reg2 = register_malloc(funcName, tacCode->c->name);
        if (reg2 == -1)
        {
            reg2 = 1;
            loadToReg(1, tacCode->c, chain);
        }
        node = createArmCodeThreeReg(ARM_LEA_ARR, reg0, reg1, reg2, tacCode->a);
        addNodeToChain(chain, node);
    }
}

void cgZero(int opCode, TAC *tacCode, FUNCTAG *fun, ARMCODE *head)
{
    ARMCODE *node;
    ARMCODE *chain = head;
    string funcName = fun->name;
    preserveScene(fun, chain);
    if (tacCode->a->store == 0)
    {
        node = createArmCodeOneRegOneName(ARM_LEA_GLOBAL, 0, tacCode->a->name);
        addNodeToChain(chain, node);
        if (tacCode->b->value * 4 > 4000 || tacCode->b->value < -4000)
        {
            node = createArmCodeOneRegOneImm(ARM_MOV_IMM, 3, tacCode->b->value * 4);
            addNodeToChain(chain, node);
            node = createArmCodeThreeReg(ARM_ADD, 0, 0, 3, tacCode->a);
            addNodeToChain(chain, node);
        }
        else
        {
            node = createArmCodeTwoRegOneImm(ARM_ADD, 0, 0, tacCode->b->value * 4, tacCode->a);
            addNodeToChain(chain, node);
        }
    }
    else
    {
        if ((tacCode->a->offset + tacCode->b->value * 4) > 4000 || (tacCode->a->offset + tacCode->b->value * 4) < -4000)
        {
            node = createArmCodeOneRegOneImm(ARM_MOV_IMM, 0, tacCode->a->offset + tacCode->b->value * 4);
            addNodeToChain(chain, node);
            node = createArmCodeThreeReg(ARM_ADD, 0, 11, 0, tacCode->a);
            addNodeToChain(chain, node);
        }
        else
        {
            node = createArmCodeTwoRegOneImm(ARM_ADD, 0, 11, tacCode->a->offset + tacCode->b->value * 4, tacCode->a);
            addNodeToChain(chain, node);
        }
    }

    node = createArmCodeOneRegOneImm(ARM_MOV_IMM, 2, (tacCode->c->value - tacCode->b->value + 1) * 4);
    addNodeToChain(chain, node);
    node = createArmCodeOneRegOneImm(ARM_MOV_IMM, 1, 0);
    addNodeToChain(chain, node);
    node = createArmCodeLabel(ARM_CALL, (char *)"memset");
    addNodeToChain(chain, node);

    restoreScene(fun, chain);
}

void cgEnd(int opCode, TAC *tacCode, FUNCTAG *fun, ARMCODE *head)
{
    ARMCODE *node;
    ARMCODE *chain = head;    
    string funcName = fun->name;
    if (fun->numReturn == 0)
    {
        if (fun->end == 1)
        {
            node = createArmCodeTwoRegOneImm(ARM_SUB, 13, 11, 4, tacCode->a);
            addNodeToChain(chain, node);
            node = createArmCodeFunc(ARM_POP, 1);
            addNodeToChain(chain, node);
        }
        else
        {
            node = createArmCodeTwoRegOneImm(ARM_SUB, 13, 11, 0, tacCode->a);
            addNodeToChain(chain, node);
            node = createArmCodeFunc(ARM_POP, 0);
            addNodeToChain(chain, node);
        }
        node = createArmCodeNothing(ARM_RETURN);
        addNodeToChain(chain, node);
    }
    node = createArmCodeName(ARM_END, fun->name);
    addNodeToChain(chain, node);
    registerInit(reg);
}

void cgReturn(int opCode, TAC *tacCode, FUNCTAG *fun, ARMCODE *head)
{
    ARMCODE *node;
    ARMCODE *chain = head;
    string funcName = fun->name;
    int reg0;
    if (tacCode->a != NULL)
    {
        reg0 = register_malloc(funcName, tacCode->a->name);
        if (reg0 == -1)
        {
            reg0 = 0;
            loadToReg(0, tacCode->a, chain);
        }
        else
        {
            node = createArmCodeTwoReg(ARM_MOV_REG, 0, reg0, tacCode->a);
            addNodeToChain(chain, node);
        }
    }
    if (fun->numReturn == 1 && fun->end == 0) /* return value */
    {
        if (fun->rmReturn)
        {
            node = createArmCodeLabel(ARM_LABEL, fun->returnName);
            addNodeToChain(chain, node);
        }
        #ifdef PRINTF_RETURN 
        if (strcmp(fun->name, "main") == 0){
            node = createArmCodeLabel(ARM_CALL,(char *)"putint");
                        addNodeToChain(chain, node);
            node = createArmCodeOneRegOneImm(ARM_MOV_IMM,0,10);
                        addNodeToChain(chain, node);
            node = createArmCodeLabel(ARM_CALL,(char *)"putch");
                        addNodeToChain(chain, node);
        }
        #endif
        // if (strcmp(fun->name, "main") == 0)
        // {
        //     printf("    bl putint\n");
        //     printf("    mov r0,#10\n");
        //     printf("    bl putch\n");
        // }
        node = createArmCodeTwoRegOneImm(ARM_SUB, 13, 11, 0, tacCode->a);
        addNodeToChain(chain, node);
        node = createArmCodeFunc(ARM_POP, 0);
        addNodeToChain(chain, node);
        node = createArmCodeNothing(ARM_RETURN);
        addNodeToChain(chain, node);
    }
    else if (fun->numReturn == 1 && fun->end == 1)
    {
        if (fun->rmReturn)
        {
            node = createArmCodeLabel(ARM_LABEL, fun->returnName);
            addNodeToChain(chain, node);
        }
        #ifdef PRINTF_RETURN 
        if (strcmp(fun->name, "main") == 0){
            node = createArmCodeLabel(ARM_CALL,(char *)"putint");
                        addNodeToChain(chain, node);
            node = createArmCodeOneRegOneImm(ARM_MOV_IMM,0,10);
                        addNodeToChain(chain, node);
            node = createArmCodeLabel(ARM_CALL,(char *)"putch");
                        addNodeToChain(chain, node);
        }
        #endif
        // if (strcmp(fun->name, "main") == 0)
        // {
        //     printf("    bl putint\n");
        //     printf("    mov r0,#10\n");
        //     printf("    bl putch\n");
        // }
        node = createArmCodeTwoRegOneImm(ARM_SUB, 13, 11, 4, tacCode->a);
        addNodeToChain(chain, node);
        node = createArmCodeFunc(ARM_POP, 1);
        addNodeToChain(chain, node);
        node = createArmCodeNothing(ARM_RETURN);
        addNodeToChain(chain, node);
    }
    else if (fun->numReturn > 1)
    {
        node = createArmCodeLabel(ARM_GOTO, fun->returnName);
        addNodeToChain(chain, node);
        fun->numReturn -= 1;
    }
}

void cgStoreFormal(int opCode, TAC *tacCode, FUNCTAG *fun, ARMCODE *head)
{
    ARMCODE *node;
    ARMCODE *chain = head;
    string funcName = fun->name;
    int reg = fun->numFormal - 1;
    if (tacCode->a->offset > 4000 || tacCode->a->offset < -4000)
    {
        node = createArmCodeOneRegOneImm(ARM_MOV_IMM, 4, tacCode->a->offset);
        addNodeToChain(chain, node);
        node = createArmCodeThreeReg(ARM_ADD, 4, 11, 4, tacCode->a);
        addNodeToChain(chain, node);
        node = createArmCodeTwoReg(opCode, reg, 4, tacCode->a);
        addNodeToChain(chain, node);
    }
    else
    {
        node = createArmCodeOneRegOneImm(opCode, reg, tacCode->a->offset);
        addNodeToChain(chain, node);
    }
}

void cgPush(int opCode, TAC *tacCode, FUNCTAG *fun, ARMCODE *head)
{
    ARMCODE *node;
    ARMCODE *chain = head;
    string funcName = fun->name;
    vector<int> registerGet=getRegisterUsed(funcName);
    for(int i=0;i<registerGet.size();i++){
        reg[registerGet[i]]=1;
    }
    if (fun->begin == 1)
    {
        node = createArmCodeFunc(ARM_PUSH, fun->begin);
        addNodeToChain(chain, node);
        node = createArmCodeTwoRegOneImm(ARM_ADD, 11, 13, 4*fun->begin, tacCode->a);
        addNodeToChain(chain, node);
        if (fun->totalSpace * 4 > 1000)
        {
            node = createArmCodeOneRegOneImm(ARM_MOV_IMM, 4, fun->totalSpace * 4);
            addNodeToChain(chain, node);
            node = createArmCodeThreeReg(ARM_SUB, 13, 13, 4, tacCode->a);
            addNodeToChain(chain, node);
        }
        else
        {
            node = createArmCodeTwoRegOneImm(ARM_SUB, 13, 13, fun->totalSpace * 4, tacCode->a);
            addNodeToChain(chain, node);
        }
    }
    else
    {
        node = createArmCodeFunc(ARM_PUSH, fun->begin);
        addNodeToChain(chain, node);
        node = createArmCodeTwoRegOneImm(ARM_ADD, 11, 13, 4*fun->begin, tacCode->a);
        addNodeToChain(chain, node);
        if ((fun->totalSpace + 1) * 4 > 1000)
        {
            node = createArmCodeOneRegOneImm(ARM_MOV_IMM, 4, (fun->totalSpace + 1) * 4);
            addNodeToChain(chain, node);
            node = createArmCodeThreeReg(ARM_SUB, 13, 13, 4, tacCode->a);
            addNodeToChain(chain, node);
        }
        else
        {
            node = createArmCodeTwoRegOneImm(ARM_SUB, 13, 13, (fun->totalSpace + 1) * 4, tacCode->a);
            addNodeToChain(chain, node);
        }
    }
}

void functionInit(FUNCTAG *fun)
{
    if (fun->numFormal > 0)
    {
        formalOffset = fun->numVar * 4 + 8;
        if (fun->numFormal > 3)
            formalOffset += 16;
        else
            formalOffset = 4 * fun->numFormal + formalOffset;
        fun->reg2Offset = -(formalOffset + 4);
        fun->reg3Offset = -(formalOffset + 8);
    }
    else
    {
        formalOffset = 0;
        fun->reg2Offset = -(fun->numVar * 4 + 12);
        fun->reg3Offset = -(fun->numVar * 4 + 16);
    }

    varOffset = 4;
    actualOffset = 0;
}

void cgIfZero(int opCode, TAC *tacCode, FUNCTAG *fun, ARMCODE *head){
    ARMCODE *node;
    ARMCODE *chain = head;
    string funcName = fun->name;
    int reg0 = register_malloc(funcName, tacCode->a->name);
    if (reg0 == -1)
    {
        reg0 = 0;
        loadToReg(0, tacCode->a, chain);
    }
    node = createArmCodeOneRegOneImm(ARM_CMP,reg0,0);
    addNodeToChain(chain, node);
    node = createArmCodeLabel(opCode,tacCode->b->name);
    addNodeToChain(chain, node);
}

void cgCmpGoto(int opCode, TAC *tacCode, FUNCTAG *fun, ARMCODE *head)
{
    ARMCODE *node;
    ARMCODE *chain = head;
    string funcName = fun->name;
    int reg0 = register_malloc(funcName, tacCode->a->name);
    if (reg0 == -1)
    {
        reg0 = 0;
        loadToReg(0, tacCode->a, chain);
    }
    int reg1;
    if (tacCode->c->type == SYM_VAR)
    {
        reg1 = register_malloc(funcName, tacCode->c->name);
        if (reg1 == -1)
        {
            reg1 = 1;
            loadToReg(1, tacCode->c, chain);
        }
        node = createArmCodeTwoReg(ARM_CMP, reg0, reg1, tacCode->a);
        addNodeToChain(chain, node);
    }
    else if (tacCode->c->type == SYM_INT)
    {
        if (tacCode->c->value > 255 || tacCode->c->value < -255)
        {
            node = createArmCodeOneRegOneImm(ARM_MOV_IMM, 1, tacCode->c->value);
            addNodeToChain(chain, node);
            node = createArmCodeTwoReg(ARM_CMP, reg0, 1, tacCode->a);
            addNodeToChain(chain, node);
        }
        else
        {
            node = createArmCodeOneRegOneImm(ARM_CMP, reg0, tacCode->c->value);
            addNodeToChain(chain, node);
        }
    }
    node = createArmCodeLabel(opCode, tacCode->b->name);
    addNodeToChain(chain, node);
}

void cgFunc(int opCode, TAC *tacCode, FUNCTAG *fun, ARMCODE *head)
{
    ARMCODE *node;
    ARMCODE *chain = head;
    node = createArmCodeName(opCode, tacCode->a->name);
    addNodeToChain(chain, node);
}

void cgGotoLabel(int opCode, TAC *tacCode, FUNCTAG *fun, ARMCODE *head)
{
    ARMCODE *node;
    ARMCODE *chain = head;
    node = createArmCodeLabel(opCode, tacCode->a->name);
    addNodeToChain(chain, node);
}

void cgCopy(int opCode, TAC *tacCode, FUNCTAG *fun, ARMCODE *head)
{
    ARMCODE *node;
    ARMCODE *chain = head;
    string funcName = fun->name;

    int reg0 = register_malloc(funcName, tacCode->a->name);
    int reg1 = register_malloc(funcName, tacCode->b->name);
    if (reg1 == -1)
    {
        reg1 = 0;
        loadToReg(0, tacCode->b, chain);
    }
    node = createArmCodeTwoReg(opCode, reg0, reg1, tacCode->a);
    addNodeToChain(chain, node);
}

void cgCmp(int opCode, TAC *tacCode, FUNCTAG *fun, ARMCODE *head)
{
    ARMCODE *node;
    ARMCODE *chain = head;
    string funcName = fun->name;
    int reg0 = register_malloc(funcName, tacCode->a->name);
    int reg1 = register_malloc(funcName, tacCode->b->name);
    if (reg1 == -1)
    {
        reg1 = 0;
        loadToReg(0, tacCode->b, chain);
    }
    int reg2;

    if (tacCode->c->type == SYM_VAR)
    {
        reg2 = register_malloc(funcName, tacCode->c->name);
        if (reg2 == -1)
        {
            reg2 = 1;
            loadToReg(1, tacCode->c, chain);
        }
        node = createArmCodeTwoReg(ARM_CMP, reg1, reg2, tacCode->a);
        addNodeToChain(chain, node);
    }
    else if (tacCode->c->type == SYM_INT)
    {
        if (tacCode->c->value > 255 || tacCode->c->value < -255)
        {
            node = createArmCodeOneRegOneImm(ARM_MOV_IMM, 1, tacCode->c->value);
            addNodeToChain(chain, node);
            node = createArmCodeTwoReg(ARM_CMP, reg1, 1, tacCode->a);
            addNodeToChain(chain, node);
        }
        else
        {
            node = createArmCodeOneRegOneImm(ARM_CMP, reg1, tacCode->c->value);
            addNodeToChain(chain, node);
        }
    }
    node = createArmCodeOneReg(opCode, reg0, tacCode->a);
    addNodeToChain(chain, node);
}

void cgNot(int opCode, TAC *tacCode, FUNCTAG *fun, ARMCODE *head)
{
    ARMCODE *node;
    ARMCODE *chain = head;
    string funcName = fun->name;
    int reg0 = register_malloc(funcName, tacCode->a->name);
    int reg1 = register_malloc(funcName, tacCode->b->name);
    if (reg1 == -1)
    {
        reg1 = 0;
        loadToReg(0, tacCode->b, chain);
    }
    node = createArmCodeOneRegOneImm(ARM_CMP, reg1, 0);
    addNodeToChain(chain, node);
    node = createArmCodeOneReg(ARM_EQ, reg0, tacCode->a);
    addNodeToChain(chain, node);
}

void cgNeg(int opCode, TAC *tacCode, FUNCTAG *fun, ARMCODE *head)
{
    ARMCODE *node;
    ARMCODE *chain = head;
    string funcName = fun->name;
    int reg0 = register_malloc(funcName, tacCode->a->name);
    int reg1 = register_malloc(funcName, tacCode->b->name);
    if (reg1 == -1)
    {
        reg1 = 0;
        loadToReg(0, tacCode->b, chain);
    }
    node = createArmCodeTwoRegOneImm(opCode, reg0, reg1,0, tacCode->a);
    addNodeToChain(chain, node);
}

int findPower(int num) //乘法除法变简单移位
{
    int n = 1;
    int value = 2;
    for (n = 1; n <= 10;)
    {
        if (num != value)
        {
            value = value * 2;
            n++;
        }
        else
        {
            break;
        }
    }
    if (n > 10)
    {
        return 0;
    }
    else
    {
        return n;
    }
}

DIVISION *findDivision(int n){
    unsigned int i,j,k;
	double m;
	double o;
	int num=n;
    DIVISION *division=(DIVISION *)calloc(1,sizeof(DIVISION));
	if(num<0){
		num=-num;
		division->positive=0;
	}else{
        division->positive=1;
    }
	
	if(num==2){
		division->factorial=1;
		division->multiplication=0;
		return division;
	}
	
	i=2;
	j=0;
	
	while(num>i){
		j++;
		i=i*2;
        if(j == 31) break;
	}

    if(j==31){
        division->factorial=-1;
        division->multiplication=0;
        return division;
    }
	
	if(num==i){
		division->factorial=j+1;
		division->multiplication=0;
		return division;
	}
	
	division->factorial=j;
    uint64_t temp = (uint64_t)1 << (31+j);
	division->multiplication=(temp+(uint64_t)num-1)/(uint64_t)num;
    //uint64_t oTemp =temp % num;	
    uint64_t oTemp = (uint64_t)num - temp % (uint64_t)num;
    uint64_t mTemp = (uint64_t)1 << (j);
    // if(oTemp >= mTemp)
    // {
    //     division->multiplication -= 1;
    // }
	return division;	
}

void cgDivMod(int opCode, TAC *tacCode, FUNCTAG *fun, ARMCODE *head)
{
    ARMCODE *node;
    ARMCODE *chain = head;
    string funcName = fun->name;
    int reg0 = register_malloc(funcName, tacCode->a->name);
    int reg1 = register_malloc(funcName, tacCode->b->name);
    int reg2;

    // if (tacCode->c->type == SYM_INT && opCode == ARM_DIV && (tacCode->c->value ==2 ||tacCode->c->value ==-2))
    // {
    //     if (reg1 == -1)
    //     {
    //     reg1 = 0;
    //     loadToReg(0, tacCode->b, chain);
    //     }
    //     int tag;
    //     if(tacCode->c->value==2){
    //         tag=1;
    //     }else{
    //         tag=0;
    //     }
    //     node = createArmCodeTwoRegOneImm(ARM_DIV2,reg0,reg1,tag,tacCode->a);
    //     addNodeToChain(chain,node);
    //     return ;
    // }
    // if (tacCode->c->type == SYM_INT && opCode == ARM_DIV){
    //     int num;
    //     if(tacCode->c->value<0){
    //         num=findPower(-tacCode->c->value);
    //     }else{
    //         num=findPower(tacCode->c->value);
    //     }
    //     if(num!=0 && tacCode->c->value>-257 && tacCode->c->value <257){
    //         if (reg1 == -1)
    //         {
    //             reg1 = 0;
    //             loadToReg(reg1, tacCode->b, chain);
    //         }
    //         reg2=1;
    //         if(tacCode->c->value<0){
    //             node = createArmCodeTwoRegOneImm(ARM_ADD,reg2,reg1,-(tacCode->c->value+1),SYMBConst(0));
    //         }else{
    //             node = createArmCodeTwoRegOneImm(ARM_ADD,reg2,reg1,tacCode->c->value-1,SYMBConst(0));
    //         }
    //         addNodeToChain(chain,node);
    //         node = createArmCodeOneRegOneImm(ARM_CMP,reg1,0);
    //         addNodeToChain(chain,node);
    //         node = createArmCodeTwoReg(ARM_MOVLT,reg1,reg2,SYMBConst(0));
    //         addNodeToChain(chain,node);
    //         if(tacCode->c->value<0){
    //             node = createArmCodeTwoRegTwoImm(ARM_ASR,reg0,reg1,num,0,SYMBConst(0));
    //         }else{
    //             node = createArmCodeTwoRegTwoImm(ARM_ASR,reg0,reg1,num,1,SYMBConst(0));
    //         }
    //         addNodeToChain(chain,node);
    //         return ;
    //     }
    // }


    if(tacCode->c->type==SYM_INT && opCode==ARM_DIV){
        DIVISION *division=findDivision(tacCode->c->value);
        if(division->factorial==1 && division->multiplication==0){
            if (reg1 == -1)
            {
                reg1 = 0;
                loadToReg(reg1, tacCode->b, chain);
            }
            node = createArmCodeTwoRegOneImm(ARM_DIV2,reg0,reg1,division->positive,tacCode->a);
            addNodeToChain(chain,node);
            return ;
        }else if(division->multiplication==0 && division->factorial>0){
            if (reg1 == -1)
            {
                reg1 = 0;
                loadToReg(reg1, tacCode->b, chain);
            }
            reg2=1;
            if(division->factorial<=8){
                node = createArmCodeTwoRegOneImm(ARM_ADD,reg2,reg1,pow(2,division->factorial)-1,SYMBConst(0));
                addNodeToChain(chain,node);                
            }else{
                node = createArmCodeOneRegOneImm(ARM_MOV_IMM,reg2,pow(2,division->factorial)-1);
                addNodeToChain(chain,node);
                node = createArmCodeThreeReg(ARM_ADD,reg2,reg1,reg2,SYMBConst(0));
                addNodeToChain(chain,node);
            }
            node = createArmCodeOneRegOneImm(ARM_CMP,reg1,0);
            addNodeToChain(chain,node);
            node = createArmCodeTwoReg(ARM_MOVLT,reg1,reg2,SYMBConst(0));
            addNodeToChain(chain,node); 
            node = createArmCodeTwoRegTwoImm(ARM_ASR,reg0,reg1,division->factorial,division->positive,tacCode->a);
            addNodeToChain(chain,node);      
            return ;                   
        }else if(division->multiplication>0){
            if (reg1 == -1)
            {
                reg1 = 0;
                loadToReg(reg1, tacCode->b, chain);
            }
            else 
            {
                node = createArmCodeTwoReg(ARM_MOV_REG,0,reg1,SYMBConst(0));
                addNodeToChain(chain,node);
                reg1 = 0;
            }
            reg2=1;
            loadToReg(reg2,SYMBConst(division->multiplication),chain);
            node = createArmCodeTwoReg(ARM_SMULL,reg2,reg1,SYMBConst(0));
            addNodeToChain(chain,node);
            node = createArmCodeTwoRegOneImm(ARM_ASR,reg1,reg2,division->factorial-1,SYMBConst(0));
            addNodeToChain(chain,node);
            node = createArmCodeTwoRegOneImm(ARM_LSR,reg2,reg2,31,SYMBConst(0));
            addNodeToChain(chain,node);
            if(division->positive==1){
                node = createArmCodeThreeReg(ARM_ADD,reg0,reg1,reg2,tacCode->a);
                addNodeToChain(chain,node);
            }else{
                node = createArmCodeTwoRegOneImm(ARM_NEG,reg2,reg2,0,SYMBConst(0));
                addNodeToChain(chain,node);
                node = createArmCodeThreeReg(ARM_NEG,reg0,reg1,reg2,tacCode->a);
                addNodeToChain(chain,node);
            }
            return ;
        }else{
        }
    }

    if(tacCode->c->type==SYM_INT && opCode==ARM_MOD){
        DIVISION *division = findDivision(tacCode->c->value);
        if(division->factorial==1 && division->multiplication==0){
            if(reg1 == -1){
                reg1=0;
                loadToReg(reg1,tacCode->b,chain);
            }else{
                node = createArmCodeTwoReg(ARM_MOV_REG,0,reg1,SYMBConst(0));
                addNodeToChain(chain,node);
                reg1=0;
            }
            node = createArmCodeOneRegOneImm(ARM_CMP,reg1,0);
            addNodeToChain(chain,node);
            node = createArmCodeTwoRegOneImm(ARM_AND,reg1,reg1,1,SYMBConst(0));
            addNodeToChain(chain,node);
            node = createArmCodeTwoRegOneImm(ARM_RSBLT,reg1,reg1,0,SYMBConst(0));
            addNodeToChain(chain,node);
            node = createArmCodeTwoReg(ARM_MOV_REG,reg0,reg1,tacCode->a);
            addNodeToChain(chain,node);
            return ;
        }else if(division->multiplication==0 && division->factorial>0 && division->factorial<=8){
            if(reg1 == -1){
                reg1=0;
                loadToReg(reg1,tacCode->b,chain);
            }else{
                node = createArmCodeTwoReg(ARM_MOV_REG,0,reg1,SYMBConst(0));
                addNodeToChain(chain,node);
                reg1=0;
            }
            reg2 = 1;
            node = createArmCodeTwoRegOneImm(ARM_RSBS,reg2,reg1,0,SYMBConst(0));
            addNodeToChain(chain,node);
            node = createArmCodeTwoRegOneImm(ARM_AND,reg2,reg2,pow(2,division->factorial)-1,SYMBConst(0));
            addNodeToChain(chain,node);
            node = createArmCodeTwoRegOneImm(ARM_AND,reg1,reg1,pow(2,division->factorial)-1,SYMBConst(0));
            addNodeToChain(chain,node);
            node = createArmCodeTwoRegOneImm(ARM_RSBPL,reg1,reg2,0,SYMBConst(0));
            addNodeToChain(chain,node);
            node = createArmCodeTwoReg(ARM_MOV_REG,reg0,reg1,tacCode->a);
            addNodeToChain(chain,node);
        }else if(division->multiplication>0){
            if (reg1 == -1)
            {
                reg1 = 0;
                loadToReg(reg1, tacCode->b, chain);
            }
            else 
            {
                node = createArmCodeTwoReg(ARM_MOV_REG,0,reg1,SYMBConst(0));
                addNodeToChain(chain,node);
                reg1 = 0;
            }
            reg2=1;
            loadToReg(reg2,SYMBConst(division->multiplication),chain);
            node = createArmCodeTwoReg(ARM_SMULL,reg2,reg1,SYMBConst(0));
            addNodeToChain(chain,node);
            node = createArmCodeTwoRegOneImm(ARM_ASR,reg1,reg2,division->factorial-1,SYMBConst(0));
            addNodeToChain(chain,node);
            node = createArmCodeTwoRegOneImm(ARM_LSR,reg2,reg2,31,SYMBConst(0));
            addNodeToChain(chain,node);
            if(division->positive==1){
                node = createArmCodeThreeReg(ARM_ADD,reg2,reg1,reg2,SYMBConst(0));
                addNodeToChain(chain,node);
            }else{
                node = createArmCodeTwoRegOneImm(ARM_NEG,reg2,reg2,0,SYMBConst(0));
                addNodeToChain(chain,node);
                node = createArmCodeThreeReg(ARM_NEG,reg2,reg1,reg2,SYMBConst(0));
                addNodeToChain(chain,node);
            }
            node = createArmCodeOneRegOneImm(ARM_MOV_IMM,reg1,tacCode->c->value);
            addNodeToChain(chain,node);
            node = createArmCodeThreeReg(ARM_MUL,reg1,reg1,reg2,SYMBConst(0));
            addNodeToChain(chain,node);
            reg2 = register_malloc(funcName, tacCode->b->name);
            if (reg2 == -1)
            {
                reg2 = 1;
                loadToReg(reg2, tacCode->b, chain);
            }
            node = createArmCodeThreeReg(ARM_SUB,reg0,reg2,reg1,tacCode->a);
            addNodeToChain(chain,node);
            return ;
        }else{
        }
    }

    preserveScene(fun, chain);
    if (reg1 == -1)
    {
        reg1 = 0;
        loadToReg(0, tacCode->b, chain);
    }
    if (tacCode->c->type == SYM_VAR)
    {
        reg2 = register_malloc(funcName, tacCode->c->name);
        if (reg2 == -1)
        {
            reg2 = 1;
            loadToReg(1, tacCode->c, chain);
        }
        node = createArmCodeTwoReg(ARM_MOV_REG, 1, reg2, tacCode->a);
        addNodeToChain(chain, node);
    }
    else if (tacCode->c->type == SYM_INT)
    {
        node = createArmCodeOneRegOneImm(ARM_MOV_IMM, 1, tacCode->c->value);
        addNodeToChain(chain, node);
    }
    node = createArmCodeTwoReg(ARM_MOV_REG, 0, reg1, tacCode->a);
    addNodeToChain(chain, node);

    node = createArmCodeNothing(opCode);
    addNodeToChain(chain, node);

    restoreScene(fun, chain);

    switch (opCode)
    {
    case ARM_DIV:
        node = createArmCodeTwoReg(ARM_MOV_REG, reg0, 0, tacCode->a);
        addNodeToChain(chain, node);
        break;

    case ARM_MOD:
        node = createArmCodeTwoReg(ARM_MOV_REG, reg0, 1, tacCode->a);
        addNodeToChain(chain, node);
        break;
    }
}

void cgAndOr(int opCode, TAC *tacCode, FUNCTAG *fun, ARMCODE *head)
{
    ARMCODE *node;
    ARMCODE *chain = head;
    string funcName = fun->name;
    int reg0 = register_malloc(funcName, tacCode->a->name);
    int reg1 = register_malloc(funcName, tacCode->b->name);
    if (reg1 == -1)
    {
        reg1 = 0;
        loadToReg(0, tacCode->b, chain);
    }
    int reg2 = register_malloc(funcName, tacCode->c->name);
    if (reg2 == -1)
    {
        reg2 = 1;
        loadToReg(1, tacCode->c, chain);
    }
    node = createArmCodeThreeReg(opCode, reg0, reg1, reg2, tacCode->a);
    addNodeToChain(chain, node);
}

void cgAddSubMul(int opCode, TAC *tacCode, FUNCTAG *fun, ARMCODE *head)
{
    ARMCODE *node;
    ARMCODE *chain = head;
    string funcName = fun->name;
    int reg0 = register_malloc(funcName, tacCode->a->name);
    int reg1 = register_malloc(funcName, tacCode->b->name);
    int reg2;
    if (reg1 == -1)
    {
        reg1 = 0;
        loadToReg(0, tacCode->b, chain);
    }

    if (tacCode->c->type == SYM_INT && opCode == ARM_MUL)
    {
        int tag = findPower(tacCode->c->value);
        if (tag != 0)
        {
            node = createArmCodeTwoRegOneImm(ARM_LSL, reg0, reg1, tag, tacCode->a);
            addNodeToChain(chain, node);
            return;
        }
    }

    if (tacCode->c->type == SYM_INT&&opCode!=ARM_MUL)
    {
        if (tacCode->c->value > 255 || tacCode->c->value < -255)
        {
            node = createArmCodeOneRegOneImm(ARM_MOV_IMM, 1, tacCode->c->value);
            addNodeToChain(chain, node);
            node = createArmCodeThreeReg(opCode, reg0, reg1, 1, tacCode->a);
            addNodeToChain(chain, node);
        }
        else
        {
            node = createArmCodeTwoRegOneImm(opCode, reg0, reg1, tacCode->c->value, tacCode->a);
            addNodeToChain(chain, node);
        }
    }
    else
    {
        reg2 = register_malloc(funcName, tacCode->c->name);
        if (reg2 == -1)
        {
            reg2 = 1;
            loadToReg(1, tacCode->c, chain);
        }
        node = createArmCodeThreeReg(opCode, reg0, reg1, reg2, tacCode->a);
        addNodeToChain(chain, node);
    }
}

void loadToReg(int reg, SYMB *n, ARMCODE *head)
{
    ARMCODE *node;
    ARMCODE *chain = head;
    
    switch (n->type) //针对不同数据类型的加载操作
    {
    case SYM_INT:
        node = createArmCodeOneRegOneImm(ARM_MOV_IMM, reg, n->value);
        addNodeToChain(chain, node);
        break;

    case SYM_VAR:
        if (n->store == 1) /* local var */
        {
            if (n->offset > 4000 || n->offset < -1000)
            {
                node = createArmCodeOneRegOneImm(ARM_MOV_IMM, reg, n->offset);
                addNodeToChain(chain, node);
                node = createArmCodeThreeReg(ARM_ADD, reg, 11, reg, n);
                addNodeToChain(chain, node);
                node = createArmCodeTwoReg(ARM_LOAD, reg, reg, n);
                addNodeToChain(chain, node);
                // node = createArmCodeTwoRegOneImm(ARM_LOAD_THREE,reg,reg,1,n);
                // addNodeToChain(chain,node);
            }
            else
            {
                node = createArmCodeOneRegOneImm(ARM_LOAD, reg, n->offset);
                addNodeToChain(chain, node);
            }
        }
        else /* global var */
        {
            if(n->name[0]!='g'){
                exit(13);
            }
            node = createArmCodeOneRegOneName(ARM_LEA_GLOBAL, reg, n->name);
            addNodeToChain(chain, node);
            node = createArmCodeTwoReg(ARM_LOAD, reg, reg, n);
            addNodeToChain(chain, node);
        }
        break;

    case SYM_TEXT:
        node = createArmCodeOneRegOneImm(ARM_LEA_STR, reg, n->label);
        addNodeToChain(chain, node);
        break;

    case SYM_ARRAY:
        if (n->store == 1) /* local var */
        {
            if (n->offset > 1000 || n->offset < -1000)
            {
                node = createArmCodeOneRegOneImm(ARM_MOV_IMM, reg, n->offset);
                addNodeToChain(chain, node);
                node = createArmCodeThreeReg(ARM_ADD, reg, 11, reg, n);
                addNodeToChain(chain, node);
                if (n->formal == 1)
                {
                    node = createArmCodeTwoReg(ARM_LOAD, reg, reg, n);
                    addNodeToChain(chain, node);
                }
            }
            else
            {
                node = createArmCodeTwoRegOneImm(ARM_ADD, reg, 11, n->offset, n);
                addNodeToChain(chain, node);
                if (n->formal == 1)
                {
                    node = createArmCodeTwoReg(ARM_LOAD, reg, reg, n);
                    addNodeToChain(chain, node);
                }
            }
        }
        else /* global var */
        {
            if(n->name[0]!='g'){
                exit(14);
            }
            node = createArmCodeOneRegOneName(ARM_LEA_GLOBAL, reg, n->name);
            addNodeToChain(chain, node);
        }
        break;
    }
}

void error(char *str)
{
    printf("%s\n", str);
    exit(12);
}

void cgLast()
{
    // printf("	.ident	\"GCC: (Raspbian 8.3.0-6+rpi1) 8.3.0\"\n");
    // printf("	.section	.note.GNU-stack,\"\",%%progbits\n");
}

void cgPre(TAC *tacCode, FUNCTAG *fun, ARMCODE *head)
{
    TAC *first = tacCode;
    TAC *pre = tacCode;
    TAC *now = pre->next;
    TAC *nex = now->next;
    FUNCTAG *function = fun;
    ARMCODE *chain = head;
    int tag = 0;

    while (nex != NULL)
    {
        switch (tag)
        {
        case 0:
            beginToFunc(pre, now, nex, chain); //删除全局变量
            tag = 1;
            break;

        case 1:
            pre = funcToEnd(pre, now, nex, function); //给函数加定义
            function = function->next;
            tag = 0;
            break;
        }
        pre = pre->next;
        now = pre->next;
        if (now != NULL)
        {
            nex = now->next;
        }
        else
        {
            nex = now;
        }
    }
}

void beginToFunc(TAC *pre, TAC *now, TAC *nex, ARMCODE *head)
{
    ARMCODE *chain = head;
    while (now->op != TAC_FUNC)
    {
        now->a->store = 0;
        if (now->op == TAC_VAR && now->a->type == SYM_VAR)
        {
            if (nex->a == now->a)
            {
                printVar(now->a, nex->b, chain);
                pre->next = nex->next;
                pre->next->prev = pre;
                now = pre->next;
                nex = now->next;
            }
            else
            {
                printVar(now->a, SYMBConst(0), chain);
                pre->next = nex;
                nex->prev = pre;
                now = pre->next;
                nex = now->next;
            }
        }
        else if (now->op == TAC_ARR && now->a->type == SYM_ARRAY)
        {
            printArr(now->a, now->b, chain);
            if (nex->op != TAC_ARR_L && nex->op != TAC_ZERO)
            {
                ARMCODE *node = createArmCodeOneImm(ARM_ZERO, now->b->value);
                addNodeToChain(chain, node);
                pre->next = nex;
                nex->prev = pre;
                now = pre->next;
                nex = now->next;
                // printf("    .zero %d\n", now->b->value * 4);
            }
            else
            {
                while (nex->op == TAC_ARR_L || nex->op == TAC_ZERO)
                {
                    now = nex;
                    nex = nex->next;
                    if (now->op == TAC_ARR_L)
                    {
                        ARMCODE *node = createArmCodeOneImm(ARM_WORD, now->c->value);
                        addNodeToChain(chain, node);
                        // printf("	.word %d\n", now->c->value);
                    }
                    else if (now->op == TAC_ZERO)
                    {
                        ARMCODE *node = createArmCodeOneImm(ARM_ZERO, now->c->value - now->b->value + 1);
                        addNodeToChain(chain, node);
                        // printf("	.zero %d\n", (now->c->value - now->b->value + 1) * 4);
                    }
                }
                pre->next = nex;
                nex->prev = pre;
                now = pre->next;
                nex = now->next;
            }
        }
        else if (now->op == TAC_STR && now->a->type == SYM_TEXT)
        {
            printStr(now->a, chain);
            pre->next = nex;
            nex->prev = pre;
            now = pre->next;
            nex = now->next;
        }
        else
        {
            error((char *)"wrong type!\n");
            pre->next = nex;
            nex->prev = pre;
            now = pre->next;
            nex = now->next;            
        }

    }
}

TAC *funcToEnd(TAC *pre, TAC *now, TAC *nex, FUNCTAG *fun) //func到end间的函数变量ok
{
    FUNCTAG *function = (FUNCTAG *)safe_malloc(sizeof(FUNCTAG), pre->a->name);
    function->returnName=(char *)safe_malloc(sizeof(char)*50,pre->a->name);
    FUNCTAG *func = fun;
    function->name = pre->a->name;
    int actual = 0;
    #ifdef PRINTF_RETURN
    function->begin=1;
    function->end=1;
    #endif
    while (now->op != TAC_ENDFUNC)
    {
        switch (now->op)
        {
        case TAC_VAR:
            function->numVar += 1;
            break;

        case TAC_ARR:
            function->numVar += now->b->value;
            break;

        case TAC_FORMAL:
            function->numFormal += 1;
            break;

        case TAC_ZERO:
            function->begin = 1;
            function->end = 1;
            break;

        case TAC_ACTUAL:
            actual += 1;
            break;

        case TAC_DIV:
            function->begin = 1;
            function->end = 1;
            break;

        case TAC_MOD:
            function->begin = 1;
            function->end = 1;
            break;

        case TAC_CALL:
            function->begin = 1;
            function->end = 1;
            if (actual > function->maxActual)
                function->maxActual = actual;
            actual = 0;
            break;

        case TAC_RETURN:
            function->numReturn += 1;
            sprintf(function->returnName,".%s_rm",function->name);
            if (function->numReturn > 1)
                function->rmReturn = 1;
            break;
        }
        pre = now;
        now = nex;
        nex = nex->next;
    }
    function->totalSpace = (function->numVar + 1) / 2 * 2;
    if (function->numFormal > 2)
    {
        function->totalSpace += 4;
    }
    else if (function->numFormal > 0)
    {
        function->totalSpace += 2;
    }

    if (function->maxActual > 4)
    {
        function->totalSpace = function->totalSpace + (function->maxActual - 3) / 2 * 2;
    }
    function->totalSpace += 4;
    func->next = function;
    return pre;
}

void printVar(SYMB *a, SYMB *b, ARMCODE *head) //输出var定义ok
{
    ARMCODE *chain = head;
    ARMCODE *node = createArmCodeNameOneImm(ARM_GLOBAL_VAR, a->name, b->value);
    addNodeToChain(chain, node);
}

void printArr(SYMB *a, SYMB *b, ARMCODE *head) //输出全局数组ok
{
    ARMCODE *chain = head;
    ARMCODE *node = createArmCodeNameOneImm(ARM_GLOBAL_ARR, a->name, b->value);
    addNodeToChain(chain, node);
}

void printStr(SYMB *a, ARMCODE *head) //输出全局字符串ok
{
    ARMCODE *chain = head;
    ARMCODE *node = createArmCodeString(ARM_GLOBAL_STR,a->label, a->name);
    addNodeToChain(chain, node);
}

void cgHead()
{
    // printf("	.arch armv7-a\n");
    // printf("	.eabi_attribute 28, 1\n");
    // printf("	.eabi_attribute 20, 1\n");
    // printf("	.eabi_attribute 21, 1\n");
    // printf("	.eabi_attribute 23, 3\n");
    // printf("	.eabi_attribute 24, 1\n");
    // printf("	.eabi_attribute 25, 1\n");
    // printf("	.eabi_attribute 26, 2\n");
    // printf("	.eabi_attribute 30, 6\n");
    // printf("	.eabi_attribute 34, 1\n");
    // printf("	.eabi_attribute 18, 4\n");
    // printf("	.text\n");
}

SYMB *SYMBConst(int n) //创建新的整数节点（符号表）ok
{
    SYMB *symbConst = (SYMB *)safe_malloc(sizeof(SYMB), "new SYMB");
    symbConst->type = SYM_INT;
    symbConst->value = n;
    return symbConst;
}

void registerInit(int reg[])
{
    int i = 0;
    for (i = REG_ZERO; i < REG_NUM ; i++)
    {
        reg[i] = 0;
    }
    reg[4]=1;
}

void addToReg(int reg[], int i)
{
    reg[i] = 1;
}

void preserveScene(FUNCTAG *fun, ARMCODE *head)
{
    ARMCODE *node;
    ARMCODE *chain = head;
    int offsetR2 = fun->reg2Offset;
    int offsetR3 = fun->reg3Offset;
    int reg = 0;
    if (offsetR2 > 255 || offsetR2 < -4000)
    {
        node = createArmCodeOneRegOneImm(ARM_MOV_IMM, reg, offsetR2);
        addNodeToChain(chain, node);
        node = createArmCodeThreeReg(ARM_ADD, reg, 11, reg, SYMBConst(0));
        addNodeToChain(chain, node);
        node = createArmCodeTwoReg(ARM_STORE, 2, reg, SYMBConst(0));
        addNodeToChain(chain, node);
    }
    else
    {
        node = createArmCodeOneRegOneImm(ARM_STORE, 2, offsetR2);
        addNodeToChain(chain, node);
    }

    if (offsetR3 > 255 || offsetR3 < -4000)
    {
        node = createArmCodeOneRegOneImm(ARM_MOV_IMM, reg, offsetR3);
        addNodeToChain(chain, node);
        node = createArmCodeThreeReg(ARM_ADD, reg, 11, reg, SYMBConst(0));
        addNodeToChain(chain, node);
        node = createArmCodeTwoReg(ARM_STORE, 3, reg, SYMBConst(0));
        addNodeToChain(chain, node);
    }
    else
    {
        node = createArmCodeOneRegOneImm(ARM_STORE, 3, offsetR3);
        addNodeToChain(chain, node);
    }
}

void restoreScene(FUNCTAG *fun, ARMCODE *head)
{
    ARMCODE *node;
    ARMCODE *chain = head;
    int offsetR2 = fun->reg2Offset;
    int offsetR3 = fun->reg3Offset;
    int reg = 2;
    if (offsetR2 > 255 || offsetR2 < -4000)
    {
        node = createArmCodeOneRegOneImm(ARM_MOV_IMM, reg, offsetR2);
        addNodeToChain(chain, node);
        node = createArmCodeThreeReg(ARM_ADD, reg, 11, reg, SYMBConst(0));
        addNodeToChain(chain, node);
        node = createArmCodeTwoReg(ARM_LOAD, 2, reg, SYMBConst(0));
        addNodeToChain(chain, node);
    }
    else
    {
        node = createArmCodeOneRegOneImm(ARM_LOAD, 2, offsetR2);
        addNodeToChain(chain, node);
    }
    reg = 3;
    if (offsetR3 > 255 || offsetR3 < -4000)
    {
        node = createArmCodeOneRegOneImm(ARM_MOV_IMM, reg, offsetR3);
        addNodeToChain(chain, node);
        node = createArmCodeThreeReg(ARM_ADD, reg, 11, reg, SYMBConst(0));
        addNodeToChain(chain, node);
        node = createArmCodeTwoReg(ARM_LOAD, 3, reg, SYMBConst(0));
        addNodeToChain(chain, node);
    }
    else
    {
        node = createArmCodeOneRegOneImm(ARM_LOAD, 3, offsetR3);
        addNodeToChain(chain, node);
    }
}

void loadFromStack(int start, int aim, FUNCTAG *fun, ARMCODE *head)
{
    ARMCODE *node;
    ARMCODE *chain = head;
    int offset;
    switch (start)
    {
    case 2:
        offset = fun->reg2Offset;
        break;

    case 3:
        offset = fun->reg3Offset;
        break;
    default:
        break;
    }
    if (offset > 1000 || offset < -4000)
    {
        node = createArmCodeOneRegOneImm(ARM_MOV_IMM, 0, offset);
        addNodeToChain(chain, node);
        node = createArmCodeThreeReg(ARM_ADD, 0, 11, 0, SYMBConst(0));
        addNodeToChain(chain, node);
        node = createArmCodeTwoReg(ARM_LOAD, aim, 0, SYMBConst(0));
        addNodeToChain(chain, node);
    }
    else
    {
        node = createArmCodeOneRegOneImm(ARM_LOAD, aim, offset);
        addNodeToChain(chain, node);
    }
}

int lyj_register_malloc(string func_name, char *var_name)
{
    #ifdef NO_REG
    return -1;
    #else

    if(func_name.size() == 0)
    {
        exit(10);
    }
    if (!var_name)
    {
        return -1;
    }
    if (var_name[0] == 'g')
    {
        return -1;
    }
    const string sVarName = var_name;
    if (!KeepRegisterMap.count(sVarName))
    {
        return -1;
    }
    int RegisterList = KeepRegisterMap[sVarName];
    if (RegisterList > 12)
    {
        return -1;
    }
    if (RegisterList >= 4 && RegisterList <= 10)
    {
        return RegisterList;
    }
    if (RegisterList == 11 || RegisterList == 12)
    {
        return RegisterList - 9;
    }
    cout << "error in register_malloc(string,const char*)" << endl;
    exit(10);
    #endif
}
int register_malloc(string func_name, char *var_name)
{
    int a=lyj_register_malloc(func_name,var_name);
    if(2<=a && a<=10 || a==-1)
    {
        return a;
    }
    exit(11);

}
void addRegToPushPop(ARMCODE *pop, int reg[])
{
    ARMCODE *push = findPush(pop);
    int i = 4;
    int j = 0;
    for (i = 4; i < 11; i++)
    {
        if (reg[i] == 1)
        {
            push->reg[j] = i;
            pop->reg[j] = i;
            j++;
        }
        i++;
    }
}

ARMCODE *findPush(ARMCODE *pop)
{
    ARMCODE *n = pop->prev;
    while (n->opCode != ARM_PUSH)
    {
        n = n->prev;
    }
    return n;
}

ARMCODE *createArmCode(void)
{
    return (ARMCODE *)calloc(1,sizeof(ARMCODE));
    //(ARMCODE *)safe_malloc(sizeof(ARMCODE), "create arm code");
}

ARMCODE *createArmCodeFunc(int opCode, int lr) //push pop使用 无拓展
{
    ARMCODE *node = createArmCode();
    node->opCode = opCode;
    node->lr = lr;
    for(int i=0;i<16;i++){
        node->reg[i]=reg[i];
    }
    return node;
}

ARMCODE *createArmCodeLabel(int opCode, char *label) //goto label使用
{
    ARMCODE *node = createArmCode();
    node->opCode = opCode;
    node->label = label;
    return node;
}

ARMCODE *createArmCodeOneRegOneName(int opCode, int r1, char *name) //加载全局数据
{
    ARMCODE *node = createArmCode();
    node->opCode = opCode;
    node->reg[0] = r1;
    node->name = name;
    return node;
}

ARMCODE *createArmCodeNothing(int opCode) //跳转固定
{
    ARMCODE *node = createArmCode();
    node->opCode = opCode;
    return node;
}

ARMCODE *createArmCodeOneRegOneImm(int opCode, int r1, int imm1) //比较、存取栈上偏移，取立即数
{
    ARMCODE *node = createArmCode();
    node->opCode = opCode;
    node->reg[0] = r1;
    node->immOne = imm1;
    node->imm = 1;
    return node;
}

ARMCODE *createArmCodeTwoRegTwoImm(int opCode, int r1, int r2, int imm1,int imm2, SYMB *a)
{
    ARMCODE *node = createArmCode();
    ARMCODE *node2;
    ARMCODE *node3;
    ARMCODE *node4;
    node->opCode = opCode;
    if (r1 == -1)
    {
        node->reg[0] = 1;
        if (a->store == 1)
        {
            if (a->offset > 255 || a->offset < -255)
            {
                node2 = createArmCodeOneRegOneImm(ARM_MOV_IMM, 0, -a->offset);
                node->next = node2;
                node2->prev = node;
                node3 = createArmCodeThreeReg(ARM_SUB, 0, 11, 0, SYMBConst(0));
                node2->next = node3;
                node3->prev = node2;
                node4 = createArmCodeTwoReg(ARM_STORE, 1, 0, SYMBConst(0));
                node3->next = node4;
                node4->prev = node3;
            }
            else
            {
                node2 = createArmCodeOneRegOneImm(ARM_STORE, 1, a->offset);
                node->next = node2;
                node2->prev = node;
            }
        }
        else
        {
            node2 = createArmCodeOneRegOneName(ARM_LEA_GLOBAL, 0, a->name);
            node->next = node2;
            node2->prev = node;
            node3 = createArmCodeTwoReg(ARM_STORE, 1, 0, SYMBConst(0));
            node2->next = node3;
            node3->prev = node2;
        }
    }
    else
    {
        node->reg[0] = r1;
    }
    node->reg[1] = r2;
    node->immOne = imm1;
    node->imm = imm2;
    return node;
}

ARMCODE *createArmCodeString(int opCode, int name, char *label) //全局字符串
{
    ARMCODE *node = createArmCode();
    node->opCode = opCode;
    node->immOne = name;
    node->imm = 1;
    node->label = label;
    return node;
}

ARMCODE *createArmCodeNameOneImm(int opCode, char *name, int imm1) //全局变量
{
    ARMCODE *node = createArmCode();
    node->opCode = opCode;
    node->name = name;
    node->immOne = imm1;
    node->imm = 1;
    return node;
}

ARMCODE *createArmCodeName(int opCode, char *name) //func和全局数组
{
    ARMCODE *node = createArmCode();
    node->opCode = opCode;
    node->name = name;
    return node;
}

ARMCODE *createArmCodeOneImm(int opCode, int imm1) //zero word赋值时使用
{
    ARMCODE *node = createArmCode();
    node->opCode = opCode;
    node->immOne = imm1;
    node->imm = 1;
    return node;
}

ARMCODE *createArmCodeTwoReg(int opCode, int r1, int r2, SYMB *a)
{
    ARMCODE *node = createArmCode();
    ARMCODE *node2;
    ARMCODE *node3;
    ARMCODE *node4;
    node->opCode = opCode;
    if (r1 == -1)
    {
        node->reg[0] = 1;
        if (a->store == 1)
        {
            if (a->offset > 255 || a->offset < -255)
            {
                node2 = createArmCodeOneRegOneImm(ARM_MOV_IMM, 0, -a->offset);
                node->next = node2;
                node2->prev = node;
                node3 = createArmCodeThreeReg(ARM_SUB, 0, 11, 0, SYMBConst(0));
                node2->next = node3;
                node3->prev = node2;
                node4 = createArmCodeTwoReg(ARM_STORE, 1, 0, SYMBConst(0));
                node3->next = node4;
                node4->prev = node3;
            }
            else
            {
                node2 = createArmCodeOneRegOneImm(ARM_STORE, 1, a->offset);
                node->next = node2;
                node2->prev = node;
            }
        }
        else
        {
            node2 = createArmCodeOneRegOneName(ARM_LEA_GLOBAL, 0, a->name);
            node->next = node2;
            node2->prev = node;
            node3 = createArmCodeTwoReg(ARM_STORE, 1, 0, SYMBConst(0));
            node2->next = node3;
            node3->prev = node2;
        }
    }
    else
    {
        node->reg[0] = r1;
    }
    node->reg[1] = r2;
    return node;
}

ARMCODE *createArmCodeOneReg(int opCode, int r1, SYMB *a)
{
    ARMCODE *node = createArmCode();
    ARMCODE *node2;
    ARMCODE *node3;
    ARMCODE *node4;
    node->opCode = opCode;
    if (r1 == -1)
    {
        node->reg[0] = 1;
        if (a->store == 1)
        {
            if (a->offset > 255 || a->offset < -255)
            {
                node2 = createArmCodeOneRegOneImm(ARM_MOV_IMM, 0, -a->offset);
                node->next = node2;
                node2->prev = node;
                node3 = createArmCodeThreeReg(ARM_SUB, 0, 11, 0, SYMBConst(0));
                node2->next = node3;
                node3->prev = node2;
                node4 = createArmCodeTwoReg(ARM_STORE, 1, 0, SYMBConst(0));
                node3->next = node4;
                node4->prev = node3;
            }
            else
            {
                node2 = createArmCodeOneRegOneImm(ARM_STORE, 1, a->offset);
                node->next = node2;
                node2->prev = node;
            }
        }
        else
        {
            node2 = createArmCodeOneRegOneName(ARM_LEA_GLOBAL, 0, a->name);
            node->next = node2;
            node2->prev = node;
            node3 = createArmCodeTwoReg(ARM_STORE, 1, 0, SYMBConst(0));
            node2->next = node3;
            node3->prev = node2;
        }
    }
    else
    {
        node->reg[0] = r1;
    }
    return node;
}

ARMCODE *createArmCodeTwoRegOneImm(int opCode, int r1, int r2, int imm1, SYMB *a)
{
    ARMCODE *node = createArmCode();
    ARMCODE *node2;
    ARMCODE *node3;
    ARMCODE *node4;
    node->opCode = opCode;
    if (r1 == -1)
    {
        node->reg[0] = 1;
        if (a->store == 1)
        {
            if (a->offset > 255 || a->offset < -255)
            {
                node2 = createArmCodeOneRegOneImm(ARM_MOV_IMM, 0, -a->offset);
                node->next = node2;
                node2->prev = node;
                node3 = createArmCodeThreeReg(ARM_SUB, 0, 11, 0, SYMBConst(0));
                node2->next = node3;
                node3->prev = node2;
                node4 = createArmCodeTwoReg(ARM_STORE, 1, 0, SYMBConst(0));
                node3->next = node4;
                node4->prev = node3;
            }
            else
            {
                node2 = createArmCodeOneRegOneImm(ARM_STORE, 1, a->offset);
                node->next = node2;
                node2->prev = node;
            }
        }
        else
        {
            node2 = createArmCodeOneRegOneName(ARM_LEA_GLOBAL, 0, a->name);
            node->next = node2;
            node2->prev = node;
            node3 = createArmCodeTwoReg(ARM_STORE, 1, 0, SYMBConst(0));
            node2->next = node3;
            node3->prev = node2;
        }
    }
    else
    {
        node->reg[0] = r1;
    }
    node->reg[1] = r2;
    node->immOne = imm1;
    node->imm = 1;
    if(opCode==ARM_ADD && node->immOne<0){
        node->opCode=ARM_SUB;
        node->immOne=-node->immOne;
    }
    if(opCode==ARM_SUB && node->immOne<0){
        node->opCode=ARM_ADD;
        node->immOne=-node->immOne;
    }
    return node;
}

ARMCODE *createArmCodeThreeReg(int opCode, int r1, int r2, int r3, SYMB *a)
{
    ARMCODE *node = createArmCode();
    ARMCODE *node2;
    ARMCODE *node3;
    ARMCODE *node4;
    node->opCode = opCode;
    if (r1 == -1)
    {
        node->reg[0] = 1;
        if (a->store == 1)
        {
            if (a->offset > 255 || a->offset < -255)
            {
                node2 = createArmCodeOneRegOneImm(ARM_MOV_IMM, 0, -a->offset);
                node->next = node2;
                node2->prev = node;
                node3 = createArmCodeThreeReg(ARM_SUB, 0, 11, 0, SYMBConst(0));
                node2->next = node3;
                node3->prev = node2;
                node4 = createArmCodeTwoReg(ARM_STORE, 1, 0, SYMBConst(0));
                node3->next = node4;
                node4->prev = node3;
            }
            else
            {
                node2 = createArmCodeOneRegOneImm(ARM_STORE, 1, a->offset);
                node->next = node2;
                node2->prev = node;
            }
        }
        else
        {
            node2 = createArmCodeOneRegOneName(ARM_LEA_GLOBAL, 0, a->name);
            node->next = node2;
            node2->prev = node;
            node3 = createArmCodeTwoReg(ARM_STORE, 1, 0, SYMBConst(0));
            node2->next = node3;
            node3->prev = node2;
        }
    }
    else
    {
        node->reg[0] = r1;
    }
    node->reg[1] = r2;
    node->reg[2] = r3;
    return node;
}

void movImm(int r, int num)
{
    unsigned short int t1, t2;
    t1 = num & 0x0000FFFF;
    t2 = (num & 0xFFFF0000) >> 16;
    printf("    movw r%d,0x%x\n", r, t1);
    printf("    movt r%d,0x%x\n", r, t2);
}

void printFunc(char *name)
{
    printf("	.text\n");
    printf("	.align 2\n");
    printf("	.global %s\n", name);
    // printf("	.arch armv7-a\n");
    // printf("	.syntax unified\n");
    // printf("	.arm\n");
    // printf("	.type	%s, %%function\n", name);
    printf("%s:\n", name);
}

void addNodeToChain(ARMCODE *head, ARMCODE *node)
{
    ARMCODE *p = head;
    while (p->next != NULL)
    {
        p = p->next;
    }
    p->next = node;
    node->prev = p;
}

void printArmCode(ARMCODE *i)
{
    int j = 0;
    switch (i->opCode)
    {
    case ARM_UNDEF:
        printf("undef");
        exit(17);
        break;

    case ARM_ADD:
        if (i->imm == 1)
        {   
            if (i->reg[0] == 11 && i->reg[1] == 13)
            {
                printf("    add fp,sp,#%d\n", i->immOne);
            }
            else if (i->reg[0] == 13 && i->reg[1] == 13)
            {
                printf("    add sp,sp,#%d\n", i->immOne);
            }
            else if (i->reg[1] == 11)
            {
                printf("    add r%d,fp,#%d\n", i->reg[0], i->immOne);
            }
            else if (i->reg[1] == 13)
            {
                printf("    add r%d,sp,#%d\n", i->reg[0], i->immOne);
            }
            else
            {
                printf("    add r%d,r%d,#%d\n", i->reg[0], i->reg[1], i->immOne);
            }
        }
        else
        {   if (i->reg[0] == 11 && i->reg[1] == 13)
            {
                printf("    add fp,sp,r%d\n", i->reg[2]);
            }
            else if (i->reg[0] == 13 && i->reg[1] == 13)
            {
                printf("    add sp,sp,r%d\n", i->reg[2]);
            }
            else if (i->reg[1] == 11)
            {
                printf("    add r%d,fp,r%d\n", i->reg[0], i->reg[2]);
            }
            else if (i->reg[1] == 13)
            {
                printf("    add r%d,sp,r%d\n", i->reg[0], i->reg[2]);
            }

            else
            {
                printf("    add r%d,r%d,r%d\n", i->reg[0], i->reg[1], i->reg[2]);
            }
        }
        break;

    case ARM_SUB:
        if (i->imm ==1)
        {   
            if (i->reg[0] == 13 && i->reg[1] == 11)
            {
                printf("    sub sp,fp,#%d\n", i->immOne);
            }
            else if (i->reg[0] == 13 && i->reg[1] == 13)
            {
                printf("    sub sp,sp,#%d\n", i->immOne);
            }
            else if (i->reg[1] == 11)
            {
                printf("    sub r%d,fp,#%d\n", i->reg[0], i->immOne);
            }
            else if (i->reg[1] == 13)
            {
                printf("    sub r%d,sp,#%d\n", i->reg[0], i->immOne);
            }

            else
            {
                printf("    sub r%d,r%d,#%d\n", i->reg[0], i->reg[1], i->immOne);
            }
        }
        else
        {   if (i->reg[0] == 13 && i->reg[1] == 11)
            {
                printf("    sub sp,fp,r%d\n", i->reg[2]);
            }
            else if (i->reg[0] == 13 && i->reg[1] == 13)
            {
                printf("    sub sp,sp,r%d\n", i->reg[2]);
            }
            else if (i->reg[1] == 11)
            {
                printf("    sub r%d,fp,r%d\n", i->reg[0], i->reg[2]);
            }
            else if (i->reg[1] == 13)
            {
                printf("    sub r%d,sp,r%d\n", i->reg[0], i->reg[2]);
            }

            else
            {
                printf("    sub r%d,r%d,r%d\n", i->reg[0], i->reg[1], i->reg[2]);
            }
        }
        break;

    case ARM_MUL:
        printf("    mul r%d,r%d,r%d\n", i->reg[0], i->reg[1], i->reg[2]);
        break;

    case ARM_DIV:
        printf("    bl __aeabi_idiv\n");
        break;

    case ARM_MOD:
        printf("    bl __aeabi_idivmod\n");
        break;

    case ARM_CMP:
        if (i->imm == 1)
        {
            printf("    cmp r%d,#%d\n", i->reg[0], i->immOne);
        }
        else
        {
            printf("    cmp r%d,r%d\n", i->reg[0], i->reg[1]);
        }
        break;

    case ARM_EQ:
        printf("    moveq r%d,#1\n", i->reg[0]);
        printf("    movne r%d,#0\n", i->reg[0]);
        break;

    case ARM_NE:
        printf("    movne r%d,#1\n", i->reg[0]);
        printf("    moveq r%d,#0\n", i->reg[0]);
        break;

    case ARM_LT:
        printf("    movlt r%d,#1\n", i->reg[0]);
        printf("    movge r%d,#0\n", i->reg[0]);
        break;

    case ARM_LE:
        printf("    movle r%d,#1\n", i->reg[0]);
        printf("    movgt r%d,#0\n", i->reg[0]);
        break;

    case ARM_GT:
        printf("    movgt r%d,#1\n", i->reg[0]);
        printf("    movle r%d,#0\n", i->reg[0]);
        break;

    case ARM_GE:
        printf("    movge r%d,#1\n", i->reg[0]);
        printf("    movlt r%d,#0\n", i->reg[0]);
        break;

    case ARM_IFEQ:
        printf("    beq %s\n", i->label);
        break;

    case ARM_IFNE:
        printf("    bne %s\n", i->label);
        break;

    case ARM_IFLT:
        printf("    blt %s\n", i->label);
        break;

    case ARM_IFLE:
        printf("    ble %s\n", i->label);
        break;

    case ARM_IFGT:
        printf("    bgt %s\n", i->label);
        break;

    case ARM_IFGE:
        printf("    bge %s\n", i->label);
        break;

    case ARM_OR:
        printf("    orr r%d,r%d,r%d\n", i->reg[0], i->reg[1], i->reg[2]);
        break;

    case ARM_AND:
        if(i->imm==1){
        printf("    and r%d,r%d,#%d\n",i->reg[0],i->reg[1],i->immOne);
        }else{
        printf("    and r%d,r%d,r%d\n", i->reg[0], i->reg[1], i->reg[2]);
        }
        break;

    case ARM_NEG:
        if(i->imm==1){
            printf("    rsb r%d,r%d,#%d\n", i->reg[0], i->reg[1],i->immOne);            
        }else{
            printf("    rsb r%d,r%d,r%d\n",i->reg[0],i->reg[1],i->reg[2]);
        }
        break;

    case ARM_GOTO:
        printf("    b %s\n", i->label);
        break;

    case ARM_CALL:
        printf("    bl %s\n", i->label);
        break;

    case ARM_RETURN:
        printf("    bx lr\n");
        break;

    case ARM_LABEL:
        printf("%s:\n", i->label);
        break;

    case ARM_FUNC:
        printFunc(i->name);
        break;

    case ARM_MOV_IMM:
        if (i->immOne > 4000 || i->immOne < -255)
        {
            movImm(i->reg[0], i->immOne);
        }
        else
        {
            printf("    mov r%d,#%d\n", i->reg[0], i->immOne);
        }
        break;

    case ARM_MOV_REG:
        if (i->reg[0] != i->reg[1])
        {
            printf("    mov r%d,r%d\n", i->reg[0], i->reg[1]);
        }
        break;

    case ARM_LOAD:
        if (i->imm == 1)
        {
            printf("    ldr r%d,[fp,#%d]\n", i->reg[0], i->immOne);
        }
        else
        {
            printf("    ldr r%d,[r%d]\n", i->reg[0], i->reg[1]);
        }
        break;

    case ARM_LOAD_THREE:
        if(i->immOne==1){
            printf("    ldr r%d,[fp,r%d]\n",i->reg[0],i->reg[1]);
        }else{
            printf("    ldr r%d,[fp,-r%d]\n",i->reg[0],i->reg[1]);
        }
        break;

    case ARM_STORE:
        if (i->imm == 1)
        {
            printf("    str r%d,[fp,#%d]\n", i->reg[0], i->immOne);
        }
        else
        {
            printf("    str r%d,[r%d]\n", i->reg[0], i->reg[1]);
        }
        break;

    case ARM_STORE_THREE:
        if(i->immOne==1){
            printf("    str r%d,[fp,r%d]\n",i->reg[0],i->reg[1]);
        }else{
            printf("    str r%d,[fp,-r%d]\n",i->reg[0],i->reg[1]);
        }
        break;

    case ARM_LOAD_ARR:
        printf("    ldr r%d,[r%d,r%d,lsl #2]\n", i->reg[0], i->reg[1], i->reg[2]);
        break;

    case ARM_STORE_ARR:
        printf("    str r%d,[r%d,r%d,lsl #2]\n", i->reg[0], i->reg[1], i->reg[2]);
        break;

    case ARM_LEA_ARR:
        printf("    add r%d,r%d,r%d,lsl #2\n", i->reg[0], i->reg[1], i->reg[2]);
        break;

    case ARM_LEA_GLOBAL:
        printf("    movw r%d,#:lower16:%s\n", i->reg[0], i->name);
        printf("    movt r%d,#:upper16:%s\n", i->reg[0], i->name);
        break;

    case ARM_LEA_STR:
        printf("    movw r%d,#:lower16:LC%d\n", i->reg[0], i->immOne);
        printf("    movt r%d,#:upper16:LC%d\n", i->reg[0], i->immOne);
        break;

    case ARM_LSL:
        printf("    lsl r%d,r%d,#%d\n", i->reg[0], i->reg[1], i->immOne);
        break;

    case ARM_DIV2:
        printf("    add r%d,r%d,r%d,lsr #31\n", i->reg[0], i->reg[1], i->reg[1]);
        printf("    asr r%d,r%d,#1\n",i->reg[0],i->reg[0]);
        if(i->immOne==0){
            printf("    rsb r%d,r%d,#0\n",i->reg[0],i->reg[0]);
        }
        break;

    case ARM_PUSH:
        printf("    push {");
        for(j=0;j<16;j++){
            if(i->reg[j]==1) printf("r%d,",j);
        }
        printf("fp");
        if (i->lr == 1)
        {
             printf(",lr");
        }
        printf("}\n");
        break;

    case ARM_POP:
        printf("    pop {");
        for(j=0;j<16;j++){
            if(i->reg[j]==1) printf("r%d,",j);
        }
        printf("fp");
        if (i->lr == 1)
        {
             printf(",pc");
        }
        printf("}\n");
        break;

    case ARM_GLOBAL_VAR:
        printf("	.global %s\n", i->name);
        printf("	.data\n");
        printf("	.align 2\n");
        printf("	.type %s,%%object\n", i->name);
        printf("	.size %s, 4\n", i->name);
        printf("%s:\n", i->name);
        printf("	.word %d\n", i->immOne);
        break;

    case ARM_GLOBAL_STR:
        printf("	.global LC%d\n", i->immOne);
        printf("	.data\n");
        printf("	.align 2\n");
        printf("	.type LC%d,%%object\n", i->immOne);
        printf("	.size LC%d, 4\n", i->immOne);
        printf("LC%d:\n", i->immOne);
        printf("	.ascii \"%s\\000\"\n", i->label);
        break;

    case ARM_GLOBAL_ARR:
        printf("	.global %s\n", i->name);
        printf("	.data\n");
        printf("	.align 2\n");
        printf("	.type %s,%%object\n", i->name);
        printf("	.size %s, %d\n", i->name, 4 * i->immOne);
        printf("%s:\n", i->name);
        break;

    case ARM_ZERO:
        printf("    .zero %d\n", 4 * i->immOne);
        break;

    case ARM_WORD:
        printf("    .word %d\n", i->immOne);
        break;

    case ARM_END:
        printf("	.size	%s, .-%s\n", i->name, i->name);
        break;

    case ARM_STORE_ACTUAL:
        printf("    str r%d,[sp,#%d]\n", i->reg[0], i->immOne);
        break;

    case ARM_MOVLT:
        printf("    movlt r%d,r%d\n",i->reg[0],i->reg[1]);
        break;

    case ARM_RSBLT:
        printf("    rsblt r%d,r%d,#%d\n",i->reg[0],i->reg[1],i->immOne);
        break;

    case ARM_ASR:
        printf("    asr r%d,r%d,#%d\n",i->reg[0],i->reg[1],i->immOne);
        if(i->imm==0){
            printf("    rsb r%d,r%d,#0\n",i->reg[0],i->reg[0]);
        }
        break;

    case ARM_LSR:
        printf("    lsr r%d,r%d,#%d\n",i->reg[0],i->reg[1],i->immOne);
        break;

    case ARM_SMULL:
        printf("    smull r%d,r%d,r%d,r%d\n",i->reg[1],i->reg[0],i->reg[1],i->reg[0]);
        break;

    case ARM_RSBS:
        printf("    rsbs r%d,r%d,#%d\n",i->reg[0],i->reg[1],i->immOne);
        break;

    case ARM_RSBPL:
        printf("    rsbpl r%d,r%d,#%d\n",i->reg[0],i->reg[1],i->immOne);
        break;

    default:
        printf("unknown ARM opcode\n");
        exit(18);
        break;
    }

    fflush(stdout);
}

#endif