#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 256
#define VERSION "0.0.1"
#define MAX_TOKENS_REPLACED 32
#define MAX_TOKEN_LENGTH 64

char StructsDeclared   [MAX_TOKENS_REPLACED][MAX_TOKEN_LENGTH] = { "" };
char StructsReplace[MAX_TOKENS_REPLACED][MAX_TOKEN_LENGTH] = { "" };
int NumTokensReplaced = 0, numLambdasDefined = 0, isInComments = 0;

char *removeTabsNewLines(char *str){
  	while(*str == '\n' || *str == '\t' || *str == ' ') str++;

  	if(*str == 0)  return str; // All spaces?
    	
  	char *end = str + strlen(str) - 1;
  	while(end > str && (*end == '\n' || *end == '\t' || *end == ' ')) end--;
  	end[1] = '\0';

  	//Reduce Tab Spaces
  	char *f;
	while((f = strstr(str, "\t"))){
		strcpy(f, f + 1);
	}

  	// Reduce Multiple spaces in between
	while((f = strstr(str, "  "))){
  		int count = 2;
  		for (; *(f + count) == ' '; ++count);
  		strcpy(f + 1, f + count);
	}

  	return str;
}

int shouldnewLine(const char *source){
	//Assumes whitespace trimmed source
	int i = strlen(source) - 1;
	return (source[i] == '{' || source[i] == '}' || source[i] == ';');
}

char* filterSingleLineComments(char* line){
	char* res = line;
	char* i = strstr(line, "//");
	if(i == NULL) return res;
	if(i == line) {
		res = "";
	}
	else {
		while(*(i-1) == '\t' || *(i-1) == ' ') i--;
		res = strncpy(res, line, i - line);
		*(res + (i - line)) = '\0';
	}
	return res;
}

void fillSting(char* dest, const char* src, const int srcStart, const int srcLength, const int destStart){
	strncpy(dest + destStart, src + srcStart, srcLength);
}

int beginsWith(const char* haystack, const char* needle){
	//Assumes whiteSpace filetered String
	if(strlen(needle) > strlen(haystack)) return 0;
	
	for(int i = 0; needle[i]; i++) if(needle[i] != haystack[i]) return 0;

	return 1;
}

int isValidStructToken(const char *line, const char* ptr, const int structLength){
	int i = line == ptr, fl = structLength;
	const char* d = ptr;
	if(!i){
		i = (*(d - 1) == ' ' || *(d - 1) == '(' || *(d - 1) == ')' || *(d - 1) == '{' || *(d - 1) == '}' || *(d - 1) == ',' || d == line);
	}
	return i & (*(d + fl) == ' ' || *(d + fl) == '(' || *(d + fl) == ')' || *(d + fl) == '{' || *(d + fl) == '}' || *(d + fl) == ',');
}

void replaceAll(char* line, const char* StructsDeclared, const char *replace){
	const int fl = strlen(StructsDeclared), rl = strlen(replace);
	char buffer[MAX_LINE_LENGTH], *d = line;
	while((d = strstr(d, StructsDeclared))){
		if(isValidStructToken(line, d, fl)){
			strcpy(buffer, d + fl);
			strcpy(d, replace);
			strcpy(d + rl, buffer);
			d += rl;
		}
		else{
			d += fl;
		}
	}
}

void preformReplace(char* line){
	for (int i = 0; i < NumTokensReplaced; ++i)
	{
		replaceAll(line, StructsDeclared[i], StructsReplace[i]);
	}
}

void compileCompactCode(char* compactCode){
	char compiled[MAX_LINE_LENGTH] = "";

	if(beginsWith(compactCode, "import")){
		char* t = strtok(compactCode, ",; "); //Skip the First token as it is "import"

		while((t = strtok(NULL, ",; ")) != NULL){
			strcat(compiled, "#include <");
			strcat(compiled, t); 	
			strcat(compiled, ".h>\n");
		}
	}
	else if(beginsWith(compactCode, "struct")){
		int nameLength = strstr(compactCode, "(") - compactCode - 8;
		char structName[nameLength + 1];
		strncpy(structName, compactCode + 7, strstr(compactCode, "(") - compactCode);
		structName[nameLength] = '\0';

		strcpy(StructsDeclared[NumTokensReplaced], structName);
		strcpy(StructsReplace[NumTokensReplaced], "struct ");
		strcat(StructsReplace[NumTokensReplaced], structName);
		NumTokensReplaced++;

		preformReplace(compactCode + 7 + nameLength);
		strcpy(compiled, "struct ");
		int ptr = 7;
		
		char* t = compactCode + 7;
		for(; *t; t++){
			switch(*t){
				case ',':
					strcat(compiled, "; const "); ptr += 8; break;
				case '(':
					strcat(compiled, "{ const ");  ptr += 8; break;
				case ')':
					strcat(compiled, "; }");  ptr += 3; break;
				default:
					compiled[ptr++] = *t; break;
			}
		}
		if(*(t-1) != ';'){
			strcat(compiled, ";\n");
		}
		else{
			strcat(compiled, "\n");
		}
	}
	
	else {
		char buffer[MAX_LINE_LENGTH];

		//apply const to function parameters
		int l = strlen(compactCode);
		if(compactCode[l-1] == '{'){
			char *f = &compactCode[l-2];
			while(*f == ' ') f--;
			if(*f == ')' && !beginsWith(compactCode, "switch") && !beginsWith(compactCode, "if") && *(f - 1) != '('){
				char ins[] = " const ";
				char buffer[64];
				//This is definitely a function declaration we must const every argument if they aren't const yet
					// printf(compactCode);
				int isFunc = 0;
				while(f != compactCode){
					if(*f == '(') isFunc = 0;
					else if(*f == ',') isFunc = 1;
					if((*f == ',' || *f == '(') && !isFunc && !(beginsWith(f + 1, "const") || beginsWith(f + 1, " const")   )){
						strcpy(buffer, f + 1);
						strcpy(f + 1, ins);
						strcpy(f + 1 + 7, buffer);
						f--;
					}
					
					f--;
				}
			}
		}

		for (int i = 0; i < NumTokensReplaced; ++i)	{
			//Check if any new structs are created "Integer (6)" -> "(Integer) { 6 }"
			char* d = compactCode;
			while((d = strstr(d, StructsDeclared[i]))){
				int len = strlen(StructsDeclared[i]);
				if(isValidStructToken(compactCode, d, len)){
					char *e = d + len;
					while(*e == ' ') e++;
					//Constructor of the struct has been called
					if(*e == '('){
						strcpy(buffer, e);
						*d = '(';
						strcpy(d + 1, StructsDeclared[i]);
						*(d + 1 + len) = ')';
						strcpy(d + 2 + len, buffer);

						e = d + 2 + len;
						*e = '{';
						int count = 0;
						while(count >= 0) {
							if(*e == '(') {
								count++;
							}
							else if(*e == ')'){
								count--;
							} 
							e++;
						}
						*(e-1) = '}';

					}
				}
				d += len;
			}

		}	


		
		//Apply const to local variables find a single '=' sign

		char *f = strstr(compactCode, "=");
		if(f != NULL){
			if(*(f + 1) != '=' && *(f - 1) != '<' && *(f - 1) != '>' && *(f - 1) != '!' && *(f - 1) != '='){
				while(f > compactCode && *f != '(') f--;

				if(!beginsWith(f, "const")){
					char *g = (f == compactCode) ? f : f + 1;
					char buffer[MAX_LINE_LENGTH];
					strcpy(buffer, g);
					strcpy(g, " const ");
					strcpy(g + 7, buffer);
				}
			}
		}


		//  "(Integer) { 6 }" -> "(struct Integer) { 6 }"
		preformReplace(compactCode);
		strcpy(compiled, compactCode);
		strcat(compiled, "\n");
	}
	strcpy(compactCode, compiled);

}

FILE* checkArgumentsAndOpenInputFile(int argc, char *argv[]){
	if(argc == 1){
		printf("Please specify the File.bjs as argument.\n");
		exit(1);
	}

	FILE *file = fopen(argv[1], "r");

	if(file == NULL){
		printf("Error opening File.\n");
		exit(1);
	}
}

void gcc(){
	int i = system("gcc code.c");

	switch(i){
		case 0: printf("Compile Success.");
	}

}

void buildLambda(char* buffer, const char* line){
	//This function clears the line of the lambda, puts it in the buffer, and places
	//the pointer to the lambda in the character array of the line.

	//Line points to the Line where the Lambda began, it contains at least one "=>"
	char *ptr = strstr(line, "=>"), *Lptr = ptr;
	int bracketLvl = 1; //1 becuase we loop after we enter Lambda Prototype

	while(*ptr != '(') ptr--; //Enter Lambda Prototype

	while(1) {
		if(*ptr == ')') bracketLvl++;
		if(*ptr == '(') {
			if(bracketLvl == 1) break;
			bracketLvl--;
		}
		ptr--;
	}
	//ptr now points to the start of the argument list of the lambda

	ptr--;
	while(*ptr == ' ') ptr--;
	//ptr now points to the end of the return type of lambda
	char* r = ptr;
	while(*ptr != ' ') ptr--;
	//ptr now points to space before the return type

	char *start = ptr;

	strncpy(buffer, ptr, r - ptr + 1); 
	buffer[r - ptr + 1] = '\0';
	//Terminate the string for strcat() to work later

	ptr = r; //ptr pointrs to end of return type of lambda
	r = buffer + strlen(buffer); //r points to the null ending of buffer

	*r = ' '; r++; *r = '\0';

	char nameLambda[10];
	strcpy(nameLambda, "Lambda");
	sprintf(nameLambda + 6, "%d", numLambdasDefined++);
	strcat(buffer, nameLambda);
	r += strlen(nameLambda);

	strncpy(r, ptr + 1, Lptr - ptr);
	r += Lptr - ptr - 1;
	*r = '\0';
	//Lambda prototype is now built
	while(*Lptr != '{') Lptr++;
	ptr = Lptr;

	bracketLvl = 0;
	while(*Lptr){
		if(*Lptr == '{') bracketLvl++;
		if(*Lptr == '}') {
			if(bracketLvl == 1) break;
			bracketLvl --;
		}
		Lptr++;
	}
	//Now Lptr points to the end of body of the lambda and ptr points to its beginning

	Lptr++;
	char c = *Lptr;
	*Lptr = '\0';

	strcat(buffer, ptr);

	*Lptr = c;
	r += Lptr - ptr;
	*r = ';';
	*(r + 1) = '\0';
	//The entire Lambda is now built

	strcpy(start, nameLambda);
	strcpy(start + strlen(nameLambda), Lptr);
}

char *clearComments(char *line){
	char *res = line;
	char *t = strstr(line, "/*");
	if(t && !isInComments){
		isInComments = 1;
		*t = '\0';
	}
	else if(isInComments){
		t = strstr(line, "*/");
		if(t){
			res = t + 2;
			isInComments = 0;
		}
		else *res = '\0';
	}
	return res;
}

void main(int argc, char *argv[]){

	printf("\n");

	FILE *file = checkArgumentsAndOpenInputFile(argc, argv);
	FILE *out = fopen("code.c", "w");

	char line[MAX_LINE_LENGTH], compactLine[MAX_LINE_LENGTH];
	int compactLinePTR = 0;

    while (fgets(line, sizeof(line), file)) {

    	char* a = clearComments(line);
    	char* b = removeTabsNewLines(a);
    	char* v = filterSingleLineComments(b);
    	if(!*v) continue;

    	if(shouldnewLine(v)){
    		strcat(compactLine, v);

    		char *t = strstr(compactLine, "=>");
    		if(t++ != NULL){

    			int bracketLvl = 0;
    			char lambda[MAX_LINE_LENGTH * 10], buffer[MAX_LINE_LENGTH * 10]; lambda[0] = '\0';
    			strcat(lambda, v);
    			while(1){
    				if(*t == '{') bracketLvl++;
    				if(*t == '}') {
    					bracketLvl--;
    					if(bracketLvl <= 0) break;
    				}
    				t++;
    				if(*t == '\0'){ //Reached the end of line while building lambda
    					fgets(line, sizeof(line), file);
    					char* a = clearComments(line);
    					char* b = removeTabsNewLines(a);
    					char* v = filterSingleLineComments(b);
    					strcat(lambda, v);
    					t = line;
    				}
    			}
    			buildLambda(buffer, lambda);
	    		compileCompactCode(buffer);
	    		fprintf(out, "%s", buffer);
	    		compileCompactCode(lambda);
	    		fprintf(out, "%s", lambda);
    		}
    		else{
    			compileCompactCode(compactLine);
    			fprintf(out, "%s", compactLine);
    		}

    		*compactLine = '\0';
    	}
    	else{

    		strcat(compactLine, line);
    	}

    }

    //All the code has been transpiled and the source now lies in ./code.c

    fclose(out);

    gcc();

    fclose(file);

}