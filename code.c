#include <stdio.h>
#include <string.h>
struct Integer { const  int val ; };
struct Integer sum( const struct Integer a, const  struct Integer b) {
return (struct Integer){a.val + b.val};
}
struct Integer product( const struct Integer c, const struct Integer d) {
return (struct Integer){c.val * d.val};
}
struct Integer minus( const struct Integer a, const  struct Integer b){
return (struct Integer){a.val - b.val};
}
struct Integer divide( const struct Integer a, const  struct Integer b){
return (struct Integer){a.val / b.val};
}
void print( const struct Integer a, const  struct Integer b){
if(a.val >= b.val) return;
printf("%d\n", a.val);
print((struct Integer){a.val + 1}, b);
}
void main() {
 const struct Integer a = (struct Integer){5}, b = (struct Integer){10};
print(a, b);
printf("%d\n", sum(a, b).val);
printf("%d\n", product(a, b).val);
printf("%d\n", minus(a, b).val);
printf("%d\n", divide(a, b).val);
}
