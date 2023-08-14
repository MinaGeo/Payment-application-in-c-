#pragma once
#ifndef CARD_H
#define CARD_H
#define CARD_INITIAL_SIZE 30
#define EXPIRY_DATE 12
#include <stdio.h>
#include <string.h>
#include < ctype.h >
#include <stdlib.h>
#include "stdint.h"

#define CLEASTREAM   fflush(stdout);\
					 fflush(stdin); \
					 fseek(stdin, 0, SEEK_END)


typedef unsigned char uint8_t;


typedef struct ST_cardData_t
{
    uint8_t cardHolderName[25];
    uint8_t primaryAccountNumber[20];
    uint8_t cardExpirationDate[6];
}ST_cardData_t;
typedef enum EN_cardError_t
{
    CARD_OK, WRONG_NAME, WRONG_EXP_DATE, WRONG_PAN
}EN_cardError_t;


EN_cardError_t getCardHolderName(ST_cardData_t* cardData);
EN_cardError_t getCardExpiryDate(ST_cardData_t* cardData);
EN_cardError_t getCardPAN(ST_cardData_t* cardData);

void getCardHolderNameTest(void);
void getCardPANTest(void);
void getCardExpiryDateTest(void);


#endif // !CARD_H