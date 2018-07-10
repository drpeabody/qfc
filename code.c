#include <stdio.h>
#include <string.h>
struct Integer { const  int val ; };
struct Integer sum( const struct Integer a, struct Integer b) {
return (struct Integer){a.val + b.val};
}
struct Integer product( const struct Integer c, const struct Integer d) {
return (struct Integer){c.val * d.val};
}
struct Integer minus( const struct Integer a, struct Integer b){
return (struct Integer){a.val - b.val};
}
struct Integer divide( const struct Integer a, struct Integer b){
return (struct Integer){a.val / b.val};
}
void printIf( const struct Integer a, int pred( const struct Integer b)){
if(pred(a)) printf("%d\n", a.val);
}
void main() {
int c;
 const c = 6;
 const struct Integer a = (struct Integer){5}, b = (struct Integer){10};
 int Lambda0 ( const struct Integer a) { struct Integer c = sum(a, b); return c.val % 2 == 0; };
printIf(a,Lambda0);
printf("%d\n", sum(a, b).val);
printf("%d\n", product(a, b).val);
printf("%d\n", minus(a, b).val);
printf("%d\n", divide(a, b).val);
}
