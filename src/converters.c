
#include "converters.h"
#include <math.h>
#include "assembler.h"

static void reverseArray(char arr[], int start, int end);


const char machineCodeBase32[] = {
				  '!', '@', '#', '$', '%', '^', '&',
                     		  '*', '<', '>', 'a', 'b', 'c', 'd', 'e', 'f',
                      		  'g', 'h', 'i', 'j','k','l','m','n','o','p',
                   		  'q','r','s','t','u','v'};


int decimalToBinary(int N){
    long B_Number = 0;
    int cnt = 0;
    while (N != 0) {
        int rem = N % 2;
        long c = pow(10, cnt);
        B_Number += rem * c;
        N /= 2;
        cnt++;
    }
    return B_Number;
}
char* convertDecimalTo32(int n){
    char *base32 = (char*)calloc(3, sizeof(char));
    int reminder, i=0;
    if (n == 0){
        return "!!";
    }
    while(n != 0){
        reminder = n % 32;
        n  /= 32;
        base32[i] = machineCodeBase32[reminder];
        i++;
    }
    if(base32[1] == '\0'){
        base32[1] = '!';
        base32[2] = '\0';
    }else{
        base32[i] = '\0';
    }
    reverseArray(base32, 0, 1);
    return base32;
}

static void reverseArray(char arr[], int start, int end){
    char temp;
    while (start < end)
    {
        temp = arr[start];
        arr[start] = arr[end];
        arr[end] = temp;
        start++;
        end--;
    }
}