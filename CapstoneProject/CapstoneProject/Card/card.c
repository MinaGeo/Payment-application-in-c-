#include "card.h"


void card_check(EN_cardError_t result) {
	if (result == CARD_OK) {
		printf("Actual results : Successful operation, Card is ok. [CARD_OK]\n");
	}
	else if (result == WRONG_EXP_DATE) {
		printf("Actual results: Expiry date is invalid [WRONG_EXP_DATE]\n");
	}
	else if (result == WRONG_PAN) {
		printf("Actual results : Card PAN is invalid [WRONG_PAN].\n");
	}
	else if (result == WRONG_NAME) {
		printf("Actual results : Card holder name is invalid. [WRONG_NAME].\n");
	}
	else {
		printf("Unknown return value.\n");
	}
}


EN_cardError_t getCardHolderName(ST_cardData_t* cardData)
{
	uint8_t i = 0;
	uint8_t x = 0;
	uint8_t name[CARD_INITIAL_SIZE];


	for (x = 0; x < CARD_INITIAL_SIZE; x++)
	{
		name[x] = '\0';
	}
	printf("Please enter card holder name (Only up to 4 names): ");
	//getStringWithSpaces(name,sizeof(name))
	CLEASTREAM;
	gets_s(name,CARD_INITIAL_SIZE);
	CLEASTREAM;

	if (name == NULL || strlen(name) < 20 || strlen(name) > 24) {   
		return WRONG_NAME;
	}
	else 
	{
		uint8_t spacesCount = 0;
		for (i = 0; i < strlen(name); i++) { //Check if the name is 4 or not (checking spaces).
			uint8_t c = name[i];
			if (c == ' ') {
				spacesCount++;
			}
			if (((!(c >= 'a' && c <= 'z')) && (!(c >= 'A' && c <= 'Z')) && (c != ' ')) && (c!='\n') || (spacesCount > 3))
			{
				return WRONG_NAME;
			}
			else
			{
				cardData->cardHolderName[i] = name[i];
			}
		}
	}
		cardData->cardHolderName[i] = '\0';
		return CARD_OK;


}

EN_cardError_t getCardExpiryDate(ST_cardData_t* cardData) {
	uint8_t i = 0;
	uint8_t expiryDate[EXPIRY_DATE] = { 0 };
	printf("Please enter card expiry date (MM/YY): ");
	uint8_t x = 0;


	for (x = 0; x < EXPIRY_DATE; x++)
	{
		expiryDate[x] = '\0';
	}
	//getStringWithSpaces(name,sizeof(name))
	CLEASTREAM;
	gets_s(expiryDate, EXPIRY_DATE);
	CLEASTREAM;



	if (strlen(expiryDate) > 0 && expiryDate[strlen(expiryDate) - 1] == '\n') {
		expiryDate[strlen(expiryDate) - 1] = '\0';
	}
	if (expiryDate[0] == '\0' || !cardData || strlen(expiryDate) != 5) {
		return WRONG_EXP_DATE;
	}
	if (expiryDate[2] != '/' || (expiryDate[0] > '1') || ((expiryDate[0] == '1') && (expiryDate[1] > '2'))) {
		return WRONG_EXP_DATE;
	}
	for (i = 0; i < strlen(expiryDate); i++) {
		cardData->cardExpirationDate[i] = expiryDate[i];
	}
	cardData->cardExpirationDate[i] = '\0';

	return CARD_OK;
}

EN_cardError_t getCardPAN(ST_cardData_t* cardData)
{
	uint8_t i = 0;
	uint8_t cardPan[CARD_INITIAL_SIZE] = { 0 };
	printf("Please enter card pan: ");
	uint8_t x = 0;


	for (x = 0; x < CARD_INITIAL_SIZE; x++)
	{
		cardPan[x] = '\0';
	}
	//getStringWithSpaces(name,sizeof(name))
	CLEASTREAM;
	gets_s(cardPan, CARD_INITIAL_SIZE);
	CLEASTREAM;

	if (cardPan == NULL || strlen(cardPan) < 16 || strlen(cardPan) > 19) {
		return WRONG_PAN;
	}
	else
	{
		uint8_t spacesCount = 0;
		for (i = 0; i < strlen(cardPan); i++) { //Check if the name is 4 or not (checking spaces).
			uint8_t c = cardPan[i];
			if (c == ' ') {
				spacesCount++;
			}
			if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (spacesCount > 3))
			{
				return WRONG_PAN;

			}
			else
			{
				cardData->primaryAccountNumber[i] = cardPan[i];
			}
		}
	}
	cardData->primaryAccountNumber[i] = '\0';
	return CARD_OK;
}

void getCardHolderNameTest(void)
{
	ST_cardData_t cardData;
	printf("Tester name: Mina George\n");
	printf("Function Name : getCardHolderName\n");
	printf("*-------------------------------------------------------------------------------*\n\n");
	printf("Test Case 1 : NULL name\n");
	printf("Input Data : NULL\n");
	printf("Expected Result :Card holder name is invalid. [WRONG_NAME]\n");
	card_check(getCardHolderName(&cardData));
	printf("\n*-------------------------------------------------------------------------------*\n\n");
	printf("Test Case 2 : Name less than 24 characters\n");
	printf("Input Data : Mina George\n");
	printf("Expected Result : Card holder name is invalid. [WRONG_NAME]\n");
	card_check(getCardHolderName(&cardData));
	printf("\n*-------------------------------------------------------------------------------*\n\n");
	printf("Test Case 3 : Name more than 24 characters\n");
	printf("Input Data : Mohammed Mahmoud Abdulrahman\n");
	printf("Expected Result : Card holder name is invalid. [WRONG_NAME]\n");
	card_check(getCardHolderName(&cardData));
	printf("\n*-------------------------------------------------------------------------------*\n\n");
	printf("Test Case 4 : Name with inappropriate characters\n");
	printf("Input Data : Most7fa Moham3ed Khaled\n");
	printf("Expected Result : Card holder name is invalid. [WRONG_NAME]\n");
	card_check(getCardHolderName(&cardData));
	printf("\n*-------------------------------------------------------------------------------*\n\n");
	printf("Test Case 5 : Name with with too many spaces (more than 4)\n");
	printf("Input Data : Mi khaled Leo Tah Mi Op\n");
	printf("Expected Result : Card holder name is invalid. [WRONG_NAME]\n");
	card_check(getCardHolderName(&cardData));
	printf("\n*-------------------------------------------------------------------------------*\n\n");
	printf("Test Case 6 : Happy Case Scenario (Correct name)\n");
	printf("Input Data : Micheal Fady George Max\n");
	printf("Expected Result : Successful operation, Card is ok. [CARD_OK]\n");
	card_check(getCardHolderName(&cardData));

}
void getCardPANTest(void)
{
	ST_cardData_t cardData;
	printf("Tester name: Mina George\n");
	printf("Function Name : getCardPAN\n");
	printf("*-------------------------------------------------------------------------------*\n\n");
	printf("Test Case 1 : NULL Pan\n");
	printf("Input Data : NULL\n");
	printf("Expected Result : Card PAN is invalid [WRONG_PAN].\n");
	card_check(getCardPAN(&cardData));
	printf("\n*-------------------------------------------------------------------------------*\n\n");
	printf("Test Case 2 : Pan less than 16 characters\n");
	printf("Input Data : 505035050\n");
	printf("Expected Result : Card PAN is invalid [WRONG_PAN].\n");
	card_check(getCardPAN(&cardData));
	printf("\n*-------------------------------------------------------------------------------*\n\n");
	printf("Test Case 3 : Card Pan more than 19 characters\n");
	printf("Input Data : 89893746154368513333333\n");
	printf("Expected Result : Card PAN is invalid [WRONG_PAN].\n");
	card_check(getCardPAN(&cardData));
	printf("\n*-------------------------------------------------------------------------------*\n\n");
	printf("Test Case 4 : Card pan contains alphabets\n");
	printf("Input Data : 89893746154Mina13333333\n");
	printf("Expected Result : Card PAN is invalid [WRONG_PAN].\n");
	card_check(getCardPAN(&cardData));
	printf("\n*-------------------------------------------------------------------------------*\n\n");
	printf("Test Case 5 : Card pan contains too many spaces (more than 4)\n");
	printf("Input Data : 5050 5050 50 50 50\n");
	printf("Expected Result : Card PAN is invalid [WRONG_PAN].\n");
	card_check(getCardPAN(&cardData));
	printf("\n*-------------------------------------------------------------------------------*\n\n");
	printf("Test Case 6 : Happy Case Scenario (correct Pan)\n");
	printf("Input Data : 8989374615436851\n");
	printf("Expected Result : Successful operation, Card is ok. [CARD_OK]\n");
	card_check(getCardPAN(&cardData));
}

void getCardExpiryDateTest(void)
{
	ST_cardData_t cardData;
	printf("Tester name: Mina George\n");
	printf("Function Name : getCardExpiryDate\n");
	printf("*-------------------------------------------------------------------------------*\n\n");
	printf("Test Case 1 : NULL date\n");
	printf("Input Data : NULL\n");
	printf("Expected Result : Expiry date is invalid [WRONG_EXP_DATE]\n");
	card_check(getCardExpiryDate(&cardData));

	printf("\n*-------------------------------------------------------------------------------*\n\n");
	printf("Test Case 2 : Expiry date less than 5 characters\n");
	printf("Input Data : 02/2\n");
	printf("Expected Result :Expiry date is invalid [WRONG_EXP_DATE]\n");
	card_check(getCardExpiryDate(&cardData));

	printf("\n*-------------------------------------------------------------------------------*\n\n");
	printf("Test Case 3 : Expiry date more than 5 characters\n");
	printf("Input Data : 05/222\n");
	printf("Expected Result :Expiry date is invalid [WRONG_EXP_DATE]\n");
	card_check(getCardExpiryDate(&cardData));

	printf("\n*-------------------------------------------------------------------------------*\n\n");
	printf("Test Case 4 : Expiry date has inappropriate characters\n");
	printf("Input Data : 05#22\n");
	printf("Expected Result : Expiry date is invalid [WRONG_EXP_DATE]\n");
	card_check(getCardExpiryDate(&cardData));

	printf("\n*-------------------------------------------------------------------------------*\n\n");
	printf("Test Case 5 : Expiry date has invalid Month\n");
	printf("Input Data : 13/22\n");
	printf("Expected Result : Expiry date is invalid [WRONG_EXP_DATE]\n");
	card_check(getCardExpiryDate(&cardData));

	printf("\n*-------------------------------------------------------------------------------*\n\n");
	printf("Test Case 6 : (Happy case scenario) Expiry is valid\n");
	printf("Input Data : 10/30\n");
	printf("Expected Result : Successful operation, Card is ok. [CARD_OK]\n");
	card_check(getCardExpiryDate(&cardData));

}


//int main(void)
//{
//	uint8_t input;
//	while (1) {
//		printf("Enter operation (n = Card Holder name test, p = Card pan test, e = Card expiry date test, q=quit): ");
//		scanf_s("%c", &input,sizeof(input));
//		(void)getchar(); // consume any remaining characters in the input stream
//		switch (input) {
//		case 'n':
//			getCardHolderNameTest();
//			break;
//		case 'p':
//			getCardPANTest();
//			break;
//		case 'e':
//			getCardExpiryDateTest();
//			break;
//		case 'q':
//			exit(0);
//		default:
//			printf("Error: Invalid input\n");
//			break;
//		}
//	}
//	return 0;
//
//}