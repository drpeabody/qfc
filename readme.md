# QFC
QFC is short for Quick Functional C. It's a simple parser that automatically reads and does some basic operations on your code, making writing functional Code in C easier.

You will need a gcc refernce in your path to compile the C code that QFC generates. QFC automatically compiles the code it generates.

## How to Use

### Using prebuilt binary

1) Clone this repo
2) `cd qfc`
3) `qfc <yourcode.qfc>`

### Compiling qfc

1) Clone this repo
2) `cd qfc`
3) `gcc qfc.c -o qfc.exe` OR `gcc qfc.c -o qfc.out` If you are on Lunux.
4) `qfc <yourcode.qfc>`

## What QFC Actually Does

QFC simply generates C code from easily written Functional-like Code.
It automatically puts `struct` keyword everywhere, automatically resolves contructor calls, and automatically marks all variables, all function arguments as `const`.

QFC | Generated C
------------ | -------------
Imports <br/>`import stdio, string;` | `#include <stdio.h> `<br/>` #include <string.h>`
Struct Declarations <br/>`struct String(`<br/>`  int length,`<br/>`  char val[MAX_LENGTH]`<br/>`);`  | `struct String{`<br/>`  const int length;`<br/>`  const char val[MAX_LENGTH];`<br/>`};`
Assuming Above String Struct is defined, <br/> `String t = String(5, "YOLO");` <br/><br/> | `const struct String t = (struct String) {5, "YOLO"};`
Function declarations <br/> `int plus(int a, int b){` <br/> `return a + b;` <br/> `}` | `int plus(const int a, const int b){` <br/> `return a + b;` <br/> `}`
A more Functional Aproach: <br/> `struct Integer( int val );`<br/><br/>`Integer plus(Integer a, Integer b){`<br/>`return Integer(a.val + b.val);`<br/>`}`|`struct Integer{ const int val; };`<br/><br/>`struct Integer plus(const struct Integer a, const struct Integer b){`<br/>`return (struct Integer){a.val + b.val};`<br/>`}`

Struct Declarations like these are designed to look like contructor calls. All local variables are automatically declared const.
This is to make your code as immutable as possible, you can't dedclare a variable and then assign it somehting. 
The following is not a valid syntax for QFC.  

## Seperate declaration and Assignment

Splitting primitive variables into declaration and assignment is not allowed and will lead to errors. This happens because strust 
types are not allowed to have seperate assignments, so declaring mutable primitives could be done. Therefore, it makes sense to me that
this should be not allowed in general.

`int a;` <br/> `a = 5;`

Such a splitting of declaration and assignment is not encouraged and hence its support is not planned.

## Lambdas

Check the `test.qfc` to see an a live example of a Lambda Expression. Also check `code.c` 
to see how it gets parsed.

A function that takes a Function as parameter is declared as follows:   
```
void mapIntToFloat(float mapper(int)){
  mapper(5); //Call given function
}

//somewhere else in the code

mapIntToFloat(float (int a) => { return (float)a; });

//Generated C Code:

float Lambda0(int a) { return (float)a; };
mapIntToFloat(Lambda0);

```

##

Also note that all local variables are automatically declared `const`, you practically cannot use any loops like `for` and `while`.
Instead, you are required to write purely functional recursive functions to replace your loops. For example, consider the follwing.

# C Code that uses a loop
```
void print(){
  int a = 0;
  while(a < 10) {
    printf("%d\n", a);
    a++;
  }
}
```
 
# QFC Code wrapped in a Function
```
void print(int a){
  if(a < 10) {
    printf("%d\n", a);
    print(a + 1);
  }
}

void print(){
  print(0);
}
```
In Both the above cases, calling function `print()` will print numbers starting at 0 ending at 9.


