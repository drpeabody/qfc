#include <stdio.h>
#include <string.h>
struct vec2 {int x; int y;};
void sum(struct vec2* a,struct vec2* b){ a->x += b->x;
a->y += b->y;
} int main(){ struct vec2 a, b;
a.x = 9;
b.x = 4;
sum(&a, &b);
printf("%d\n", a.x);
};
