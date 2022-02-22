#include <stdio.h>

// Prints line of specified length
void draw_line(int length)
{
    if (length < 0){
        printf("Non-valid length!");
    }
    char a = '#';
    while(length > 0){
        printf("%c", a);
        length -= 1;
    }
    printf("%c", '\n');
}

void draw_square(int size)
{
    int i;
    int j;
    for (i = 0; i < size; i++){
        for (j = 0; j < size; j++){
            putchar('#');
            putchar('#');
        }
        putchar('\n');
    }
}

void draw_rectangle(int a, int b, char fill)
{
    int i;
    int j;
    for (i = 0; i < b; i++){
        for (j = 0; j < a; j++){
            putchar(fill);
        }
        putchar('\n');
    }
}

void draw_circle(int radius, char fill, char space)
{
    int i;
    int j;
    for (i = 0; i < 2*radius; i++){
        for (j = 0; j <= 2*radius; j++){
            if ((i-radius)*(i-radius) + (j-radius)*(j-radius) >= radius*radius){
                putchar(space);
                putchar(space);
            }
            else {
                putchar(fill);
                putchar(fill);
            }
        }
        putchar('\n');
    }
}

void draw_ellipse(int aAxis, int bAxis, char fill, char space)
{
    if (aAxis < 0 || bAxis < 0){
        printf("Non-valid axis!");
    }

    int i;
    int j;
    for (i = 0; i < 2*bAxis; i++){ // i je y, j je x suradnica
        for (j = 0; j <= 2*aAxis; j++){
            if ( ((j - aAxis)*(j - aAxis))*(bAxis*bAxis) + ((i - bAxis)*(i - bAxis))*(aAxis*aAxis)
            >= bAxis*bAxis*aAxis*aAxis){
                putchar(space);
                putchar(space);
            }
            else {
                putchar(fill);
                putchar(fill);
            }
        }
        putchar('\n');
    }
}

int main(void)
{
    //int x = 0;
    //scanf(" %d", &x);
    //draw_line(x);
    //scanf("%d", &x);
    //draw_square(x);
    //int a = 0;
    //int b = 0;
    //char fill = '#';
    //scanf(" %d", &a);
    //scanf(" %d", &b);
    //scanf(" %c", &fill);
    //draw_rectangle(a, b, fill);
    //int r = 0;
    //char outside = '^';
    //char inside = '#';
    //scanf(" %d", &r);
    //scanf(" %c", &inside);
    //scanf(" %c", &outside);
    //draw_circle(r, inside, outside);
    int a = 0;
    int b = 0;
    char outside = '^';
    char inside = '#';
    scanf(" %d", &a);
    scanf(" %d", &b);
    scanf(" %c", &inside);
    scanf(" %c", &outside);
    draw_ellipse(a, b, inside, outside);
    return 0;
}
