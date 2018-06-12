/*
班級:資訊二甲
姓名:黃羿豪
學號:D0542487
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SIZE_OP 50   //OPTable
#define SIZE_SYM 20 //SymTable
#define SIZE_LIT 20 //LitTable
#define SIZE_USE 5 //Use Blocks
#define invalid 1 //無效操作碼, 記錄錯誤
#define repeat 2 //重複符號, 記錄錯誤
#define false 0

typedef struct {
    int opcode;
    int format;
    char key[6];
}t_op;
t_op OPTAB[SIZE_OP];


typedef struct {
    char key[8];
    int loc;
}t_sym;
t_sym SYMTAB[SIZE_SYM];

typedef struct {
    char name[3];
	int id;
}t_reg;

t_reg REGTAB[] = {
    {"A", 0},
	{"X", 1},
	{"L", 2},
	{"B", 3},
	{"S", 4},
	{"T", 5},
	{"F", 6},
	{"PC", 8},
	{"SW", 9}
};

typedef struct {
    char name[10];
    int loc;
    int put;
}t_lit;
t_lit LITTAB[SIZE_LIT];

typedef struct {
    char name[10];
    int index;
}t_use;
t_use USE[SIZE_USE];

int Hash(char *c, int size) {
    int tmp = 0;
    int i;
    for(i = 0; i < strlen(c); i++){
        tmp += c[i];
    }
    return tmp%size;
}
void insert(char* c, int o, int f) {
    int i = Hash(c, SIZE_OP);
    while(OPTAB[i].opcode != -1){
        i = (i + 1)%SIZE_OP;
    }
    strcpy(OPTAB[i].key, c);
    OPTAB[i].opcode = o;
    OPTAB[i].format = f;
}
void initOpTable() {
    int i;
    for(i = 0; i < SIZE_OP; i++){
        OPTAB[i].key[0] = '\0';
        OPTAB[i].opcode = -1;
        OPTAB[i].format = 0;
    }
}
void insertOpTable() {
    initOpTable(OPTAB);
    insert("ADD", 0x18, 3);
    insert("ADDR", 0x90, 2);
    insert("AND", 0x40, 3);
    insert("COMP", 0xA0, 3);
    insert("CLEAR", 0xB4, 2);
    insert("COMPR", 0xA0, 2);
    insert("DIV", 0x24, 3);
    insert("DIVR", 0x9C, 2);
    insert("J", 0x3C, 3);
    insert("JEQ", 0x30, 3);
    insert("JGT", 0x34, 3);
    insert("JLT", 0x38, 3);
    insert("JSUB", 0x48, 3);
    insert("LDA", 0x00, 3);
    insert("LDB", 0x68, 3);
    insert("LDCH", 0x50, 3);
    insert("LDL", 0x08, 3);
    insert("LDS", 0x6C, 3);
    insert("LDT", 0x74, 3);
    insert("LDX", 0x04, 3);
    insert("LDT", 0x74, 3);
    insert("LDCH", 0x50, 3);
    insert("MUL", 0x20, 3);
    insert("OR", 0x44, 3);
    insert("RD", 0xD8, 3);
    insert("RSUB", 0x4C, 3);
    insert("STA", 0x0C, 3);
    insert("STCH", 0x54, 3);
    insert("STL", 0x14, 3);
    insert("STSW", 0xE8, 3);
    insert("STX", 0x10, 3);
    insert("SUB", 0x1C, 3);
    insert("TD", 0x10, 3);
    insert("TIX", 0x2C, 3);
    insert("TIXR", 0xB8, 2);
    insert("WD", 0xDC, 3);
}
int searchOpTable(char* c) {
    int i = Hash(c, SIZE_OP);
    for(int j = 0; j < SIZE_OP; j++){
        if(strcmp(c, OPTAB[i].key) == 0){
            return i;
            break;
        }
        i = (i + 1)%SIZE_OP;
    }
    return -1;
}

void initSymTable() {
    for(int i = 0; i < SIZE_SYM; i++){
        SYMTAB[i].key[0] = '\0';
        SYMTAB[i].loc = 0;
    }
}
void insertSymTable(char* c, int loc) {
    int i = Hash(c, SIZE_SYM);
    while(SYMTAB[i].key[0] != '\0') {
        i = (i + 1)%SIZE_SYM;
    }
    strcpy(SYMTAB[i].key, c);
    SYMTAB[i].loc = loc;
}
int searchSym(char* c) {
    int i = Hash(c, SIZE_SYM);
    for(int j = 0; j < SIZE_SYM; j++){
        if(strcmp(c, SYMTAB[i].key) == 0){
            return i;
        }
        i = (i + 1)%SIZE_SYM;
    }
    return -1;
}
void initLitTable(){
    for(int i = 0; i < SIZE_LIT; i++) {
        LITTAB[i].loc = 0;
        LITTAB[i].name[0] = '\0';
        LITTAB[i].put = -1;
    }
}
int searchLitTable(char* c) {
    int i = Hash(c, SIZE_LIT);
    for(int j = 0; j < SIZE_LIT; j++) {
        if(strcmp(c, LITTAB[i].name) == 0){
            return i;
        }
        i = (i + 1)%SIZE_LIT;
    }
    return -1;
}
void insertLitTable(char* c) {
    if(searchLitTable(c) == -1) {
        int i = Hash(c, SIZE_LIT);
        while(LITTAB[i].name[0] != '\0') {
            i = (i + 1)%SIZE_LIT;
        }
        strcpy(LITTAB[i].name, c);
        LITTAB[i].put = 0;
    }
}

void initUse() {
    for(int i = 0; i < SIZE_USE; i++) {
        USE[i].index = 0;
        USE[i].name[0] = '\0';
    }
}

void insertUse(char* c, int* i) {
    static int USE_i = 0 ;
    USE_i++;
    USE[USE_i].index = USE_i;
    strcpy(USE[USE_i].name, c);
    (*i) = USE_i;
}
int searchUse(char* c) {
    for(int i = 0; i < SIZE_USE; i++) {
        if(strcmp(c, USE[i].name) == 0) {
            return i;
        }
    }
    return -1;
}
int countSize(char *c) {
    if(c[1] == 'X'){
        int count = 0;
        int size = 0;
        int i = 2;
        while(count < 2) {
            if(c[i] == 39) {
                count++;
            } else {
                size++;
            }
            i++;
        }
        return size/2;
    } else if (c[1] == 'C'){
        int count = 0;
        int size = 0;
        int i = 2;
        while(count < 2) {
            if(c[i] == 39) {
                count++;
            } else {
                size++;
            }
            i++;
        }
        return size;
    }
    return 0;
}

void setLitTable(int* loc) {
    for(int i = 0; i < SIZE_LIT; i++) {
        if(LITTAB[i].put == 0) {
            printf("      %04X   *       %-10s\n", (*loc), LITTAB[i].name);
            LITTAB[i].loc = (*loc);
            (*loc) =  (*loc) + countSize(LITTAB[i].name);
            LITTAB[i].put = 1;
        }
    }
}


void printOpTable() {
    int line = 1;
    printf("\nOPTAB");
    printf("\n------------------------------\n");
    printf("Line  Name  Format  Opcode\n");
    for(int i = 0; i < SIZE_OP; i++){
        if(OPTAB[i].key[0] == '\0') continue;
        if(OPTAB[i].format == 3){
            printf("%-2d    %-5s   %-3s     %02X\n", line++ , OPTAB[i].key, "3/4", OPTAB[i].opcode);
        } else {
            printf("%-2d    %-5s   %-3d     %02X\n", line++ , OPTAB[i].key, OPTAB[i].format, OPTAB[i].opcode);
        }
    }
    printf("------------------------------\n");
}
void printSymTable() {
    int line = 1;
    printf("\nSymTable");
    printf("\n------------------------------\n");
    printf("Line  Label   Loc\n");
    for(int i = 0; i < SIZE_SYM; i++){
        if(strlen(SYMTAB[i].key) > 0)
            printf("%-2d    %-6s  %04X\n", line++, SYMTAB[i].key, SYMTAB[i].loc);
    }
    printf("------------------------------\n");
}
void printRegTable() {
    int line = 1;
    printf("\nREGTAB");
    printf("\n------------------------------\n");
    printf("Line  Name  Num\n");
    for(int i = 0; i < 9; i++){
        printf("%-2d    %-2s    %d\n", line++, REGTAB[i].name, REGTAB[i].id);
    }
    printf("------------------------------\n");
}
void printLitTable() {
    int line = 1;

    printf("\nLITTAB");
    printf("\n------------------------------\n");
    printf("Line  Name      Loc\n");
    for(int i = 0; i < SIZE_LIT; i++) {
        if(LITTAB[i].put != -1)
            printf("%-2d    %-7s   %04X\n", line++, LITTAB[i].name, LITTAB[i].loc);
    }
    printf("------------------------------\n");
}

void deleteSpace(char *s) {
    char *t = s;
    while(*s != '\0'){
        if(*s != ' ' && *s != '+'){
            *t++ = *s;
        }
        ++s;
    }
    *t = '\0';
}
void deleteEnter(char *s) {
    char *t = s;
    int length = strlen(t);
    if(t[length - 1] == '\n')
        t[length - 1] = '\0';
}
void readLine(char *s, char *label, char *opcode, char* operand) {

    for(int i = 0; i < 7; i++) {
        label[i] = '\0';
    }
    for(int i = 0; i < 9; i++) {
        opcode[i] = '\0';
    }
    for(int i = 0; i < 10; i++) {
        operand[i] = '\0';
    }

    for(int j = 0; j < 7; j++) {
        if(s[j] != ' ') label[j] = s[j];
    }
    for(int j = 0; j < 7; j++) {
        opcode[j] = s[8 + j];
    }
    for(int j = 0; j < 9; j++) {
        operand[j] = s[15 + j];
    }
    deleteSpace(label);
    deleteSpace(opcode);
    deleteSpace(operand);
}
int byteSize(char *c) {
    if(c[0] == 'X'){
        int count = 0;
        int size = 0;
        int i = 1;
        while(count < 2) {
            if(c[i] == 39) {
                count++;
            } else {
                size++;
            }
            i++;
        }
        return size/2;
    } else {
        int count = 0;
        int size = 0;
        int i = 1;
        while(count < 2) {
            if(c[i] == 39) {
                count++;
            } else {
                size++;
            }
            i++;
        }
        return size;
    }
}
int main() {
    insertOpTable();
    initLitTable();
    FILE *fptr = fopen("srcpro2.11.txt", "r");
    char str[50] = {'\0'};
    char label[7] = {'\0'};
    char opcode[9] = {'\0'};
    char operand[10] = {'\0'};
    int locctr[SIZE_USE] = {0};
    int INIT_ADDR = 0;
    int error = false;
    int line = 1;
    int loc_i = 0;
    //讀第一道敘述
    fgets(str, 50, fptr);
    deleteEnter(str);
    readLine(str, label, opcode, operand);//讀入字串分割
    printf("Origin Program\n");
    printf("----------------------------------------\n");
    printf("Line  Loc    Code \n");
    if(strcmp(opcode, "START") == 0){
        INIT_ADDR = atoi(operand);
        locctr[loc_i] = INIT_ADDR;
    } else {
        locctr[loc_i] = 0;
    }
    printf("%-2d    %04X   %-55s\n", line++, locctr[loc_i], str);
    //第二道開始
    while(fgets(str, 50, fptr) != NULL) {
        deleteEnter(str);
        readLine(str, label, opcode, operand);//讀入字串分割
        if(strcmp(opcode, "END") != 0){//若非END
            if(str[0] != '.'){ //若非註解
                if(strcmp(opcode, "BASE") == 0 || strcmp(opcode, "LTORG") == 0) {
                    printf("%-2d           %-55s\n", line++, str);
                } else if (strcmp(opcode, "USE") == 0 || strcmp(opcode, "EQU") == 0) {

                } else {
                    printf("%-2d    %04X   %-55s\n", line++, locctr[loc_i], str);
                }
                if(searchSym(label) != -1 && strlen(label) > 0) {//若符號表已存在
                    error = repeat;
                } else {
                    insertSymTable(label, locctr[loc_i]);
                }
                if(searchOpTable(opcode) != -1){ // 在OPTAB裡面
                    int op = searchOpTable(opcode);
                    if(str[7] == '+') locctr[loc_i]++;// 有+號
                    locctr[loc_i] += OPTAB[op].format;
                } else if (strcmp(opcode, "WORD") == 0){
                    locctr[loc_i] += 3;
                } else if (strcmp(opcode, "RESW") == 0){
                   locctr[loc_i] += (3*atoi(operand));
                } else if (strcmp(opcode, "RESB") == 0){
                    locctr[loc_i] += atoi(operand);
                } else if (strcmp(opcode, "BYTE") == 0){
                    locctr[loc_i] += byteSize(operand);
                } else if (strcmp(opcode, "BASE") == 0){

                } else if (strcmp(opcode, "USE") == 0){
                    if(searchUse(operand) == -1 && strlen(operand) < 7) {
                        insertUse(operand, &loc_i);
                    } else if(searchUse(operand) != -1 && strlen(operand) < 7) {
                        int u = searchUse(operand);
                        loc_i = USE[u].index;
                    } else {
                        loc_i = 0;
                    }
                    printf("%-2d    %04X   %-55s\n", line++, locctr[loc_i], str);
                } else if (strcmp(opcode, "EQU") == 0){
                    if(strlen(operand) > 1) { // BUFFEND - BUFFER
                        int bufend = SYMTAB[searchSym("BUFEND")].loc;
                        int buffer = SYMTAB[searchSym("BUFFER")].loc;
                        printf("%-2d    %04X   %-55s\n", line++, bufend - buffer, str);
                    } else {
                        printf("%-2d    %04X   %-55s\n", line++, locctr[loc_i], str);
                    }
                } else if (strcmp(opcode, "LTORG") == 0){
                    setLitTable(&locctr[loc_i]);
                } else {
                    //printf("Error opcode is %s ", opcode);
                    error = invalid;
                }
                if(operand[0] == '=') {
                    insertLitTable(operand);
                }

            } //若非註解 結束
            else {
                printf("%-2d           %-55s\n", line++, str);
            }
        } else { //若非END 結束
            printf("%-2d           %-55s\n", line++, str);
            setLitTable(&locctr[loc_i]);
        }
    }//第一輪END
    printf("----------------------------------------\n");
    //printf("ERROR = %d\n", error);
    printOpTable();
    printSymTable();
    printLitTable();
    printRegTable();
    return 0;
}
