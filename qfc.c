#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 256
#define VERSION "0.0.1"
#define MAX_TOKENS_REPLACED 32
#define MAX_TOKEN_LENGTH 64

char StructsDeclared   [MAX_TOKENS_REPLACED][MAX_TOKEN_LENGTH] = { "" };
char StructsReplace[MAX_TOKENS_REPLACED][MAX_TOKEN_LENGTH] = { "" };
int NumTokensReplaced = 0;

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

void compileCompactCode(FILE* out, char* compactCode){
	// printf("%s\n", compactCode);
	if(beginsWith(compactCode, "import")){
		char* t = strtok(&compactCode[7], ",; ");

		while(t != NULL){
			fprintf(out, "#include <%s.h>\n", t);
			t = strtok(NULL, ",; ");
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
		
		char* t = compactCode;
		for(; *t; t++){
			switch(*t){
				case ',':
					fprintf(out, "; const "); break;
				case '(':
					fprintf(out, "{ const "); break;
				case ')':
					fprintf(out, "; }"); break;
				default:
					fprintf(out, "%c", *t); break;
			}
		}
		if(*(t-1) != ';'){
			fprintf(out, ";\n");
		}
		else{
			fprintf(out, "\n");
		}
	}
	
	else {
		char buffer[MAX_LINE_LENGTH];

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


		//apply const to function parameters
		int l = strlen(compactCode);
		if(compactCode[l-1] == '{'){
			char *f = &compactCode[l-2];
			while(*f == ' ') f--;
			if(*f == ')' && !beginsWith(compactCode, "switch") && !beginsWith(compactCode, "if")){
				char ins[] = " const ";
				char buffer[64];
				//This is definitely a function declaration we must const every argument if they aren't const yet
					// printf(compactCode);
				while(*f != '('){
					if(*f == ',' && !(beginsWith(f + 1, "const") || beginsWith(f + 1, " const"))){
						strcpy(buffer, f + 1);
						strcpy(f + 1, ins);
						strcpy(f + 1 + 7, buffer);
						f--;
					}
					
					f--;
				}
				if(*(f + 1) == ' ') f++;
				char * g = f + 1;
				while(*g == ' ') g++;
				if(*g != ')' && !beginsWith(f + 1, "const")){
					strcpy(buffer, f + 1);
					strcpy(f + 1, ins);
					strcpy(f + 1 + 7, buffer);
				}
					
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
		fprintf(out, "%s\n", compactCode);
	}

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

void main(int argc, char *argv[]){

	printf("\n");

	FILE *file = checkArgumentsAndOpenInputFile(argc, argv);
	FILE *out = fopen("code.c", "w");

	char line[MAX_LINE_LENGTH], compactLine[MAX_LINE_LENGTH];
	int isInComments = 0, compactLinePTR = 0;

    while (fgets(line, sizeof(line), file)) {

    	char* v = removeTabsNewLines(line);

    	if(*v == '/' && *(v + 1) == '*') isInComments = 1;
    	
    	if(! isInComments) {
    		v = filterSingleLineComments(v);
    		int k = strlen(v);
    		compactLinePTR += k;

    		if(shouldnewLine(v)){
				strncpy(compactLine + compactLinePTR - k, v, k);

    			compactLine[compactLinePTR] = '\0';

    			compileCompactCode(out, compactLine);
    			compactLinePTR = 0;

    		}
    		else{
    			fillSting(compactLine, v, 0, k, compactLinePTR - k);
    		}
    	}

    	if(*v == '*' && *(v + 1) == '/') isInComments = 0;

    }

    //All the code has been transpiled and the source now lies in ./code.c

    fclose(out);

    gcc();

    fclose(file);

}
