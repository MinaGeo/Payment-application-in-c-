#include "terminal.h"
/*helper function to print tests*/
void terminal_check(EN_terminalError_t result) {
	if (result == WRONG_DATE) {
		printf("Actual results: Transaction date is invalid. [WRONG_DATE]\n");
	}
	else if (result == TERMINAL_OK) {
		printf("Actual results: Successful operation, Transaction is valid [TERMINAL_OK]\n");
	}
	else if (result == EXPIRED_CARD) {
		printf("Actual results: Card is expired. [EXPIRED_CARD]\n");
	}
	else if (result == INVALID_AMOUNT) {
		printf("Actual results: Amount is invalid (<=0). [INVALID_AMOUNT]\n");
	}
	else if (result == EXCEED_MAX_AMOUNT) {
		printf("Actual results: Amount exceeds max. [EXCEED_MAX_AMOUNT]\n");
	}
	else if (result == INVALID_MAX_AMOUNT) {
		printf("Actual results: Max amount is invalid (<=0). [INVALID_MAX_AMOUNT]\n");
	}
	else {
		printf("Unknown return value.\n");
	}
}

EN_terminalError_t getTransactionDate(ST_terminalData_t* termData)
{
	uint8_t i = 0;
	uint8_t transactionDate[TRANSACTION_DATE] = { 0 };
	printf("Please Transcation date (DD/MM/YYYY): ");
	uint8_t x = 0;


	for (x = 0; x < TRANSACTION_DATE; x++)
	{
		transactionDate[x] = '\0';
	}
	//getStringWithSpaces(name,sizeof(name))
	CLEASTREAM;
	gets_s(transactionDate, TRANSACTION_DATE);
	CLEASTREAM;

	if (strlen(transactionDate) > 0 && transactionDate[strlen(transactionDate) - 1] == '\n') {
		transactionDate[strlen(transactionDate) - 1] = '\0';
	}
	if (transactionDate[0] == '\0' || strlen(transactionDate) != 10) {
		return WRONG_DATE;
	}
	if (transactionDate[2] != '/' || transactionDate[5] != '/' || (transactionDate[0] > '3') || ((transactionDate[0] == '3') && (transactionDate[1] > '1'))
		|| (transactionDate[3] > '1') || ((transactionDate[3] == '1') && (transactionDate[4] > '2'))) {
		return WRONG_DATE;
	}
	for (i = 0; i < strlen(transactionDate); i++) {
		termData->transactionDate[i] = transactionDate[i];
	}
	termData->transactionDate[i] = '\0';

	return TERMINAL_OK;
}

EN_terminalError_t isCardExpired(ST_cardData_t* cardData, ST_terminalData_t* termData)
{
	if ((cardData->cardExpirationDate[3] < termData->transactionDate[8]) || (cardData->cardExpirationDate[3] == termData->transactionDate[8]) && 
		(cardData->cardExpirationDate[4] < termData->transactionDate[9]))
	{
		return EXPIRED_CARD;
	}
	else if ((cardData->cardExpirationDate[3] == termData->transactionDate[8]) &&
		(cardData->cardExpirationDate[4] == termData->transactionDate[9]))
	{
		if ((cardData->cardExpirationDate[0] < termData->transactionDate[3]) || (cardData->cardExpirationDate[0] == termData->transactionDate[3] &&
			cardData->cardExpirationDate[1] < termData->transactionDate[4]))
		{
			return EXPIRED_CARD;
		}
	}
	return TERMINAL_OK;
}

EN_terminalError_t getTransactionAmount(ST_terminalData_t* termData)
{
	float transactionAmount = 0;
	printf("Please enter transaction amount: ");
	CLEASTREAM;
	scanf_s("%f",&transactionAmount);
	CLEASTREAM;
	if (transactionAmount <=0) {
		return INVALID_AMOUNT;
	}
	else
	{
		termData->transAmount = transactionAmount;
		return TERMINAL_OK;
	}

	
}

EN_terminalError_t isBelowMaxAmount(ST_terminalData_t* termData)
{
	if (termData->transAmount > termData->maxTransAmount)
	{

		return EXCEED_MAX_AMOUNT;
	}
	else
	{
		return TERMINAL_OK;
	}
}

EN_terminalError_t setMaxAmount(ST_terminalData_t* termData, float maxAmount)
{
	

	if (maxAmount <= 0) {
		return INVALID_MAX_AMOUNT;
	}
	else
	{
		termData->maxTransAmount = maxAmount;
		return TERMINAL_OK;
	}
}

void getTransactionDateTest(void)
{
	ST_terminalData_t termData;
	printf("Tester name: Mina George\n");
	printf("Function Name : getTransactionDate\n");
	printf("*-------------------------------------------------------------------------------*\n\n");
	printf("Test Case 1 : NULL date\n");
	printf("Input Data : NULL\n");
	printf("Expected Result : Transcation date is invalid. [WRONG_DATE]\n");
	terminal_check(getTransactionDate(&termData));

	printf("\n*-------------------------------------------------------------------------------*\n\n");
	printf("Test Case 2 : Transcation date have less characters\n");
	printf("Input Data : 12/03/201\n");
	printf("Expected Result : Transcation date is invalid. [WRONG_DATE]\n");
	terminal_check(getTransactionDate(&termData));
	printf("\n*-------------------------------------------------------------------------------*\n\n");
	printf("Test Case 3 : Transcation date have more characters\n");
	printf("Input Data : 05/12/20222\n");
	printf("Expected Result : Transcation date is invalid. [WRONG_DATE]\n");
	terminal_check(getTransactionDate(&termData));
	printf("\n*-------------------------------------------------------------------------------*\n\n");
	printf("Test Case 4 : Transcation date has inappropriate characters\n");
	printf("Input Data : 05#12#2022\n");
	printf("Expected Result : Transcation date is invalid. [WRONG_DATE]\n");
	terminal_check(getTransactionDate(&termData));
	printf("\n*-------------------------------------------------------------------------------*\n\n");
	printf("Test Case 5 : Transaction date has invalid Month\n");
	printf("Input Data : 29/13/2022\n");
	printf("Expected Result : Transcation date is invalid. [WRONG_DATE]\n");
	terminal_check(getTransactionDate(&termData));
	printf("\n*-------------------------------------------------------------------------------*\n\n");
	printf("Test Case 6 : Transaction date has invalid day\n");
	printf("Input Data : 49/10/2022\n");
	printf("Expected Result : Transcation date is invalid. [WRONG_DATE]\n");
	terminal_check(getTransactionDate(&termData));
	printf("\n*-------------------------------------------------------------------------------*\n\n");
	printf("Test Case 7 : (Happy case scenario) Expiry is valid\n");
	printf("Input Data : 14/10/2030\n");
	printf("Expected Result : Successful operation, Transcation is valid [TERMINAL_OK]\n");
	terminal_check(getTransactionDate(&termData));
}

void isCardExpriedTest(void)
{
	ST_terminalData_t termData;
	ST_cardData_t cardData;
	printf("Tester name: Mina George\n");
	printf("Function Name : isCardExpried");

	printf("\n*-------------------------------------------------------------------------------*\n");
	printf("Test Case 1 : Expired card (month)\n");
	printf("Input Data : Transaction Date: 22/06/2019, Exp date: 03/19\n");
	printf("Expected Result : Card is expired. [EXPIRED_CARD]\n\n");
	getTransactionDate(&termData);
	getCardExpiryDate(&cardData);
	terminal_check(isCardExpired(&cardData, &termData));
	printf("\n*-------------------------------------------------------------------------------*\n\n");
	printf("Test Case 2 : Expired card (Year)\n");
	printf("Input Data : Transaction Date: 22/02/2019,  Exp date: 03/18\n");
	printf("Expected Result : Card is expired. [EXPIRED_CARD]\n");
	getTransactionDate(&termData);
	getCardExpiryDate(&cardData);
	terminal_check(isCardExpired(&cardData, &termData));
	printf("\n*-------------------------------------------------------------------------------*\n\n");
	printf("Test Case 3 : Transcation date is valid (not expired)\n");
	printf("Input Data : Transaction Date: 22/03/2022, Exp date: 03/25\n");
	printf("Expected Result :  Successful operation, Transcation is valid [TERMINAL_OK]\n\n");
	getTransactionDate(&termData);
	getCardExpiryDate(&cardData);
	terminal_check(isCardExpired(&cardData, &termData));

	printf("\n*-------------------------------------------------------------------------------*\n\n");
	printf("Test Case 4 : Expiry date and transaction date the same\n");
	printf("Input Data : 22/03/2022, 03/22\n");
	printf("Expected Result :  Successful operation, Transcation is valid [TERMINAL_OK]\n\n");
	getTransactionDate(&termData);
	getCardExpiryDate(&cardData);
	terminal_check(isCardExpired(&cardData, &termData));

}

void getTransactionAmountTest(void)
{
	ST_terminalData_t termData;
	printf("Tester name: Mina George\n");
	printf("Function Name : getTransactionAmount\n");
	printf("*-------------------------------------------------------------------------------*\n\n");
	printf("Test Case 1 : negative amount\n");
	printf("Input Data : -1000\n");
	printf("Expected Result : amount is invalid (<=0). [INVALID_AMOUNT]\n");
	terminal_check(getTransactionAmount(&termData));
	printf("\n*-------------------------------------------------------------------------------*\n\n");
	printf("Test Case 2 : 0 amount\n");
	printf("Input Data : 0\n");
	printf("Expected Result : amount is invalid (<=0). [INVALID_AMOUNT]\n");
	terminal_check(getTransactionAmount(&termData));
	printf("\n*-------------------------------------------------------------------------------*\n\n");
	printf("Test Case 3 : correct amount (>0)\n");
	printf("Input Data : 1000\n");
	printf("Expected Result : Successful operation, Transcation is valid [TERMINAL_OK]\n");
	terminal_check(getTransactionAmount(&termData));


}

void isBelowMaxAmountTest(void)
{
	ST_terminalData_t termData;
	printf("Tester name: Mina George\n");
	printf("Function Name : isBelowMaxAmount\n");
	printf("*-------------------------------------------------------------------------------*\n\n");
	printf("Test Case 1 : Transaction amount less than Max Amount\n");
	printf("Input Data : Max amount: 1000, transaction amount: 500\n");
	printf("Expected Result :  Successful operation, Transcation is valid [TERMINAL_OK]\n");
	setMaxAmount(&termData, 1000);
	getTransactionAmount(&termData);
	terminal_check(isBelowMaxAmount(&termData));

	printf("\n*-------------------------------------------------------------------------------*\n\n");
	printf("Test Case 2 : Transaction amount equals Max Amount\n");
	printf("Input Data : Max amount: 1000, transaction amount: 1000\n");
	printf("Expected Result :  Successful operation, Transcation is valid [TERMINAL_OK]\n");
	setMaxAmount(&termData, 1000);
	getTransactionAmount(&termData);
	terminal_check(isBelowMaxAmount(&termData));

	printf("\n*-------------------------------------------------------------------------------*\n\n");
	printf("Test Case 3 : Transaction amount Greater than Max Amount\n");
	printf("Input Data : Max amount: 1000, transaction amount: 1500\n");
	printf("Expected Result : amount exceed max. [EXCEED_MAX_AMOUNT]\n");
	getTransactionAmount(&termData);
	terminal_check(isBelowMaxAmount(&termData));
}

void setMaxAmountTest(void)
{
	ST_terminalData_t termData;
	printf("Tester name: Mina George\n");
	printf("Function Name : setMaxAmountTest\n");
	printf("*-------------------------------------------------------------------------------*\n\n");
	printf("Test Case 1 : negative amount\n");
	printf("Input Data : -1000\n");
	printf("Expected Result : Max amount is invalid (<=0). [INVALID_MAX_AMOUNT ]\n");
	terminal_check(setMaxAmount(&termData,-1000));
	printf("\n*-------------------------------------------------------------------------------*\n\n");
	printf("Test Case 2 : 0 amount\n");
	printf("Input Data : 0\n");
	printf("Expected Result : Max amount is invalid (<=0). [INVALID_MAX_AMOUNT ]\n");
	terminal_check(setMaxAmount(&termData,0));
	printf("\n*-------------------------------------------------------------------------------*\n\n");
	printf("Test Case 3 : correct amount (>0)\n");
	printf("Input Data : 1000\n");
	printf("Expected Result :  Successful operation, Transcation is valid [TERMINAL_OK]\n");
	terminal_check(setMaxAmount(&termData,1000));

}





//int main(void)
//{
//		uint8_t input;
//	while (1) {
//		printf("Enter operation (t = getTransactionDateTest i = isCardExpriedTest e = getTransactionAmountTest\n s= setMaxAmountTest, b= isBelowMaxAmountTest, q=quit): ");
//		printf("\n.-----------------------------------------------------------------------.\n");
//		scanf_s("%c", &input,sizeof(input));
//		(void)getchar(); // consume any remaining characters in the input stream
//
//		switch (input) {
//		case 't':
//			getTransactionDateTest();
//			break;
//		case 'i':
//			isCardExpriedTest();
//			break;
//		case 'e':
//			getTransactionAmountTest();
//			break;
//		case 's':
//			setMaxAmountTest();
//			break;
//		case 'b':
//			isBelowMaxAmountTest();
//			break;
//		case 'q':
//			exit(0);
//		default:
//			printf("Error: Invalid input\n");
//			break;
//		}
//	}
//	return 0;
//}