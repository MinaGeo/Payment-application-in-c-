#include "app.h"



void appStart(void)
{

	printf("Welcome to Mina's payment app!\n");

	ST_cardData_t cardData;
	ST_terminalData_t termData;
	ST_transaction_t transData;

	printf("*------------------------- Obtaining Card Details -------------------------*\n");
	while (getCardHolderName(&cardData) != CARD_OK)
	{
		printf("Please enter a valid name.\n");
	}

	while (getCardExpiryDate(&cardData) != CARD_OK)
	{
		printf("Please enter a valid exp date format (MM/YY).\n");

	}
	while (getCardPAN(&cardData) != CARD_OK)
	{
		printf("please enter a valid PAN.\n");
	}
	printf("\nsuccessfully obtained card details.\n");
	
	printf("*------------------------- Obtaining Transaction Details -------------------------*\n");

	while (getTransactionDate(&termData) != TERMINAL_OK)
	{
		printf("Please enter a valid transaction date (DD/MM/YYYY).\n");
	}
	if (isCardExpired(&cardData, &termData) == EXPIRED_CARD)
	{
		printf("Declind Expired Card.");
		return EXPIRED_CARD;
	}
	printf("Setting maximum transaction amount: 5000\n");
	setMaxAmount(&termData, TRANSACTION_MAX_AMOUNT);

	while (getTransactionAmount(&termData) != TERMINAL_OK)
	{
		printf("Please enter a valid amount. (>0)\n");
	}

	if (isBelowMaxAmount(&termData) == EXCEED_MAX_AMOUNT)
	{
		printf("Declined Amount Exceeding Limit.\n");
		return EXCEED_MAX_AMOUNT;
	}

	transData.cardHolderData = cardData;
	transData.terminalData = termData;
	EN_transState_t result = recieveTransactionData(&transData);
	if (result == FRAUD_CARD)
	{
		printf("Declind Invalid Account.\n");
		return FRAUD_CARD;
	}
	if (result == DECLINED_INSUFFECIENT_FUND)
	{
		printf("Declined Insufficient funds.\n");
		return DECLINED_INSUFFECIENT_FUND;
	}
	if (result == DECLINED_STOLEN_CARD)
	{
		printf("Declined Account blocked.\n");
			return DECLINED_STOLEN_CARD;

	}
	if (result == APPROVED)
	{
		printf("Approved.\n");
	}

}

//int main(void)
//{
//	appStart();
//
//	return 0;
//}
