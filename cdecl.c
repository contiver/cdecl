/* Based on the finite state machine implementation
 * on the book "Expert C Programming"
 */
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Macros */
#define NELEMS(array)          (sizeof(array)/sizeof(array[0]))
#define IS_PTR_OR_QUALIFIER(x) ((x) <= 2)
#define IS_QUALIFIER(x)        ((x) <= 1)
#define MAX_TOKENS             40
#define MAX_TOKEN_LEN          64

typedef struct Token {
	unsigned char kind;
	char string[MAX_TOKEN_LEN];
} Token;

enum kind {CONST, VOLATILE, POINTER, TYPE, IDENTIFIER};

/* function prototypes */
static enum kind classifytoken(void);
static void gettoken(void);
static void parseuntilidentifier(void);
static void processarray(void);
static void processpointer(void);
static void processparams(void);
static void processleftparen(void);
static void processtype(void);
static void arrangetype(void);
static void die(const char *errstr, ...);

/* variables */
static int top = -1;
static Token stack[MAX_TOKENS];
static Token currenttoken;
static void (*nextstate)(void) = parseuntilidentifier;
static char * const texts[] = {"read-only", "volatile", "pointer to"};
static char * const types[] = {
	"void", "char", "signed", "unsigned", "short",
	"int", "long", "float", "double", "struct", "union", "enum"
};

/* function implementations */
void
parseuntilidentifier(void){
	gettoken();
	while(currenttoken.kind != IDENTIFIER){
		if(++top >= MAX_TOKENS){
			die("error: too many tokens\n");
		}
		stack[top] = currenttoken;
		gettoken();
	}
	printf("%s is ", currenttoken.string);
	gettoken();
	nextstate = processarray;
}

void
gettoken(void){
	char *s = currenttoken.string;

	while((*s = getchar()) == ' ' || *s == '\t')
		; /* ignore starting whitespaces */

	if(isalnum(*s)){
		while(isalnum(*++s = getchar()))
			; /* parse token */
		ungetc(*s, stdin);
		*s = '\0';
		currenttoken.kind = classifytoken();
	} else if(*s == '*'){
		currenttoken.kind = POINTER;
	} else {
		currenttoken.string[1] = '\0';
		currenttoken.kind = *s;
	}
}

enum kind
classifytoken(void){
	char *s = currenttoken.string;
	if(strcmp(s, "const") == 0)
		return CONST;

	if(strcmp(s, "volatile") == 0)
		return VOLATILE;

	for(unsigned int i = 0; i < NELEMS(types); i++)
		if(strcmp(s, types[i]) == 0)
			return TYPE;

	return IDENTIFIER;
}

void
processarray(void){
	nextstate = processparams;
	while(currenttoken.kind == '['){
		printf("an array of ");
		gettoken(); /* number or ']' */
		if(isdigit(currenttoken.string[0])){
			printf("%d ", atoi(currenttoken.string));
			gettoken(); /* read the ']' */
		}
		gettoken();
		printf("of ");
		nextstate = processleftparen;
	}
}

void
processparams(void){
	if(currenttoken.kind == '('){
		while(currenttoken.kind != ')'){
			gettoken();
		}
		gettoken();
		printf("function returning ");
	}
	nextstate = processleftparen;
}

void
processleftparen(void){
	nextstate = processpointer;
	if(top >= 0){
		if(stack[top].kind == '('){
			top--;
			gettoken(); /* read past ')' */
			nextstate = processarray;
		}
	}
}

void
processpointer(void){
	int kind = stack[top].kind;
	if(IS_PTR_OR_QUALIFIER(kind)){
		printf("%s ", texts[kind]);
		top--;
		nextstate = processleftparen;
	} else {
		nextstate = processtype;
	}
}

void
processtype(void){
	arrangetype();
	printf("\n");
	nextstate = NULL;
}

void
arrangetype(void){
	if(top < 0)
		return;

	int t = top--;
	arrangetype();
	if(IS_QUALIFIER(stack[t].kind))
		printf("%s ", texts[stack[t].kind]);
	else
		printf("%s ", stack[t].string);
}

void
die(const char *errstr, ...) {
	va_list ap;

	va_start(ap, errstr);
	vfprintf(stderr, errstr, ap);
	va_end(ap);
	exit(EXIT_FAILURE);
}

int
main(void){
	while(nextstate)
		(*nextstate)();

	return EXIT_SUCCESS;
}
