#include "server.h"
uint32_t transactionCounter = 0;

//balance, state, Pan
ST_accountsDB_t accountsDB[ACCOUNT_DB_SIZE] = {
  {
    2000.0,
    RUNNING,
    "8989374615436851"
  }, //running account
  {
    100000.0,
    BLOCKED,
    "5807007076043875"
  }, //stolen account (blocked)
  {
    50000.0,
    RUNNING,
    "98765432112345678"
  }, //running account
  {
    70000.0,
    BLOCKED,
    "123451234567896789"
  }, //blocked
  {
    84300.0,
    RUNNING,
    "768594351263456789"
  }, //running (Expired card but running account)
  {
    62345.0,
    BLOCKED,
    "65543211277778888"
  }, //blocked
  {
    10345.0,
    RUNNING,
    "9149997985121373374"
  }, //running (Exceed the maximum amount)
  {
    240000.0,
    BLOCKED,
    "2532552789043566666"
  }, //blocked (stolen)
  {
    800500.0,
    RUNNING,
    "369369369369369369"
  }, //running (insufficient fund)
  {
    439000.0,
    BLOCKED,
    "95195195195195189"
  } //blocked
};

ST_transaction_t transactionDB[TRANSACTION_DB_SIZE] = {
  0
};

//HELPER FUNCTION TO HELP WITH THE PRINT OF TEST
const char* transStateStrings[] = {
  "APPROVED",
  "DECLINED_INSUFFICIENT_FUND",
  "DECLINED_STOLEN_CARD",
  "FRAUD_CARD",
  "INTERNAL_SERVER_ERROR"
};
//HELPER FUNCTION TO HELP WITH THE PRINT OF TEST
void checkAndPrintMsg(EN_transState_t result) {
    // Use if-else statements to print messages based on the result
    if (result == FRAUD_CARD && strcmp(transStateStrings[FRAUD_CARD], "FRAUD_CARD") == 0) {
        printf("Actual results : Account doesn't Exist.[FRAUD_CARD]\n");
    }
    else if (result == APPROVED && strcmp(transStateStrings[APPROVED], "APPROVED") == 0) {
        printf("Actual resuts: Approved.[APPROVED]\n");
    }
    else if (result == DECLINED_INSUFFECIENT_FUND && strcmp(transStateStrings[DECLINED_INSUFFECIENT_FUND], "DECLINED_INSUFFICIENT_FUND") == 0) {
        printf("Actual results : Insufficient fund. [DECLINED_INSUFFICIENT_FUND]\n");
    }
    else if (result == DECLINED_STOLEN_CARD && strcmp(transStateStrings[DECLINED_STOLEN_CARD], "DECLINED_STOLEN_CARD") == 0) {
        printf("Actual results : Stolen card. [DECLINED_STOLEN_CARD]\n");
    }
    else if (result == INTERNAL_SERVER_ERROR && strcmp(transStateStrings[INTERNAL_SERVER_ERROR], "INTERNAL_SERVER_ERROR") == 0) {
        printf("Actual results : Saving failed.[INTERNAL_SERVER_ERROR]\n");
    }
    else {
        printf("Unknown return value.\n");
    }
}
//HELPER FUNCTION TO HELP WITH THE PRINT OF TEST
void check_server_error(EN_serverError_t result) {
    if (result == BLOCKED_ACCOUNT) {
        printf("Actual results : Account BLOCKED.[BLOCKED_ACCOUNT]\n");
    }
    else if (result == ACCOUNT_NOT_FOUND) {
        printf("Actual results : Account doesn't Exist.[ACCOUNT_NOT_FOUND]\n");
    }
    else if (result == SAVING_FAILED) {
        printf("Actual results : saving failed. [SAVING_FAILED]\n");
    }
    else if (result == LOW_BALANCE) {
        printf("Actual results : Low balance, Amount is not available. [LOW_BALANCE]\n");
    }
    else if (result == SERVER_OK) {
        printf("Actual results : successful operation. [SERVER_OK]\n");
    }
    else {
        printf("Unknown return value.\n");
    }
}

EN_transState_t recieveTransactionData(ST_transaction_t* transData) {

    ST_accountsDB_t accountReference;

    if (isValidAccount(&(transData->cardHolderData), &accountReference) == ACCOUNT_NOT_FOUND) {
        transData->transState = FRAUD_CARD;
        saveTransaction(transData);
        return FRAUD_CARD;
    }
    else if (isAmountAvailable(&(transData->terminalData), &accountReference) == LOW_BALANCE) {
        transData->transState = DECLINED_INSUFFECIENT_FUND;
        saveTransaction(transData);
        return DECLINED_INSUFFECIENT_FUND;
    }
    else if (isBlockedAccount(&accountReference) == BLOCKED_ACCOUNT) {
        transData->transState = DECLINED_STOLEN_CARD;
        saveTransaction(transData);
        return DECLINED_STOLEN_CARD;
    }
    else
    {
        transData->transState = APPROVED;
        
        if (saveTransaction(transData) == SAVING_FAILED) {
            transData->transState = INTERNAL_SERVER_ERROR;
            saveTransaction(transData);
            return INTERNAL_SERVER_ERROR;
        }
        else {
            accountReference.balance -= transData->terminalData.transAmount;
            printf("Account new balance is: %f\n", accountReference.balance);
            return APPROVED;
        }

    }
 
}

EN_serverError_t isValidAccount(ST_cardData_t* cardData, ST_accountsDB_t* accountRefrence) {
    uint32_t i = 0;
    for (i = 0; i < sizeof(accountsDB) / sizeof(accountsDB[0]) || accountsDB[i].primaryAccountNumber == NULL; i++) {
        if (strlen(cardData->primaryAccountNumber) > 0 && cardData->primaryAccountNumber[strlen(cardData->primaryAccountNumber) - 1] == '\n') {
            cardData->primaryAccountNumber[strlen(cardData->primaryAccountNumber) - 1] = '\0';
        }
        if (strcmp(cardData->primaryAccountNumber, accountsDB[i].primaryAccountNumber) == 0) {
            accountRefrence->balance = accountsDB[i].balance;
            accountRefrence->state = accountsDB[i].state;
            strcpy_s(accountRefrence->primaryAccountNumber, sizeof(accountsDB[i].primaryAccountNumber), accountsDB[i].primaryAccountNumber);
            return SERVER_OK;
        }
    }
    accountRefrence = NULL;
    return ACCOUNT_NOT_FOUND;
}

EN_serverError_t isBlockedAccount(ST_accountsDB_t* accountRefrence) {

    if (accountRefrence->state == BLOCKED) {
        return BLOCKED_ACCOUNT;
    }
    else {
        return SERVER_OK;
    }
}

EN_serverError_t isAmountAvailable(ST_terminalData_t* termData, ST_accountsDB_t* accountRefrence) {
    if (accountRefrence->balance < termData->transAmount) {
        return LOW_BALANCE;
    }
    else {
        return SERVER_OK;
    }
}

EN_serverError_t saveTransaction(ST_transaction_t* transData) {
    if (transactionCounter < TRANSACTION_DB_SIZE && transactionDB[transactionCounter].transactionSequenceNumber < TRANSACTION_DB_SIZE) {

        strcpy_s(transactionDB[transactionCounter].cardHolderData.primaryAccountNumber, sizeof(transData->cardHolderData.primaryAccountNumber), transData->cardHolderData.primaryAccountNumber);
        strcpy_s(transactionDB[transactionCounter].cardHolderData.cardExpirationDate, sizeof(transData->cardHolderData.cardExpirationDate), transData->cardHolderData.cardExpirationDate);
        strcpy_s(transactionDB[transactionCounter].cardHolderData.cardHolderName, sizeof(transData->cardHolderData.cardHolderName), &(transData->cardHolderData.cardHolderName));
        transactionDB[transactionCounter].terminalData.transAmount = transData->terminalData.transAmount;
        transactionDB[transactionCounter].terminalData.maxTransAmount = transData->terminalData.maxTransAmount;
        strcpy_s(transactionDB[transactionCounter].terminalData.transactionDate, sizeof(transData->terminalData.transactionDate), transData->terminalData.transactionDate);
        transactionDB[transactionCounter].transState = transData->transState;
        transactionDB[transactionCounter].transactionSequenceNumber = transactionCounter + 1;
        transactionCounter++;

        return SERVER_OK;
    }
    else {
        return SAVING_FAILED;
    }

}

void listSavedTransactions(void) {
    uint32_t i = 0;
    for (i = 0; i < sizeof(transactionDB) / sizeof(transactionDB[0]) && transactionDB[i].cardHolderData.cardHolderName[0] != '\0'; i++) {
        printf("#########################\n");
        printf("Transaction Sequence Number : %d\n", transactionDB[i].transactionSequenceNumber);
        printf("Transaction Date : %s\n", transactionDB[i].terminalData.transactionDate);
        printf("Transaction Amount : %f\n", transactionDB[i].terminalData.transAmount);
        printf("Transaction State : %s\n", transStateStrings[transactionDB[i].transState]);
        printf("Terminal Max Amount : %f\n", transactionDB[i].terminalData.maxTransAmount);
        printf("Cardholder Name : %s\n", transactionDB[i].cardHolderData.cardHolderName);
        printf("PAN : %s\n", transactionDB[i].cardHolderData.primaryAccountNumber);
        printf("Card Expiration Date : %s\n", transactionDB[i].cardHolderData.cardExpirationDate);
        printf("#########################\n");
    }
}

void recieveTransactionDataTest(void) {
    ST_transaction_t transData = {
      0
    };
    printf("Tester name: Mina George\n");
    printf("Function Name : recieveTransactionDataTest\n");
    printf("*-------------------------------------------------------------------------------*\n\n");

    printf("*-------------------------------------------------------------------------------*\n\n");
    printf("Test Case 1 : Happy case scenario");
    printf("Input Data : everything works correctly  PAN: 369369369369369369 \n");
    printf("*-------------------------------------------------------------------------------*\n");
    printf("Expected Result : Approved.[APPROVED]\n");
    printf("*-------------------------------------------------------------------------------*\n");
    if (getCardHolderName(&(transData.cardHolderData)) != WRONG_NAME && getCardExpiryDate(&(transData.cardHolderData)) != WRONG_EXP_DATE && getCardPAN(&(transData.cardHolderData)) != WRONG_PAN && getTransactionAmount(&(transData.terminalData)) != INVALID_AMOUNT && setMaxAmount(&(transData.terminalData), TRANSACTION_MAX_AMOUNT) != INVALID_MAX_AMOUNT && getTransactionDate(&(transData.terminalData)) != WRONG_DATE) {

        checkAndPrintMsg(recieveTransactionData(&transData));
    }

    printf("\n*-------------------------------------------------------------------------------*\n\n");
    printf("Test Case 2 : Account doesn't exist\n");
    printf("Input Data : Non-exisiting PAN.. ( 50505050505330540 )\n");
    printf("*-------------------------------------------------------------------------------*\n");
    printf("Expected Result : Account doesn't Exist.[FRAUD_CARD]\n");
    printf("*-------------------------------------------------------------------------------*\n");
    if (getCardHolderName(&(transData.cardHolderData)) != WRONG_NAME && getCardExpiryDate(&(transData.cardHolderData)) != WRONG_EXP_DATE && getCardPAN(&(transData.cardHolderData)) != WRONG_PAN && getTransactionAmount(&(transData.terminalData)) != INVALID_AMOUNT && setMaxAmount(&(transData.terminalData), TRANSACTION_MAX_AMOUNT) != INVALID_MAX_AMOUNT && getTransactionDate(&(transData.terminalData)) != WRONG_DATE) {

        checkAndPrintMsg(recieveTransactionData(&transData));
    }

    printf("\n*-------------------------------------------------------------------------------*\n\n");
    printf("Test Case 3 : Invalid balance\n");
    printf("Input Data : Transcation amount greater than bank account (2000.0) PAN: 8989374615436851\n");
    printf("*-------------------------------------------------------------------------------*\n");
    printf("Expected Result :  Insufficient fund. [DECLINED_INSUFFECIENT_FUND]\n");
    printf("*-------------------------------------------------------------------------------*\n");
    if (getCardHolderName(&(transData.cardHolderData)) != WRONG_NAME && getCardExpiryDate(&(transData.cardHolderData)) != WRONG_EXP_DATE && getCardPAN(&(transData.cardHolderData)) != WRONG_PAN && getTransactionAmount(&(transData.terminalData)) != INVALID_AMOUNT && setMaxAmount(&(transData.terminalData), TRANSACTION_MAX_AMOUNT) != INVALID_MAX_AMOUNT && getTransactionDate(&(transData.terminalData)) != WRONG_DATE) {

        checkAndPrintMsg(recieveTransactionData(&transData));
    }

    printf("\n*-------------------------------------------------------------------------------*\n\n");
    printf("Test Case 4 : Stolen card\n");
    printf("Input Data : stolen account (2000.0) PAN: 5807007076043875\n");
    printf("*-------------------------------------------------------------------------------*\n");
    printf("Expected Result :  Stolen card. [DECLINED_STOLEN_CARD]\n");
    printf("*-------------------------------------------------------------------------------*\n");
    if (getCardHolderName(&(transData.cardHolderData)) != WRONG_NAME && getCardExpiryDate(&(transData.cardHolderData)) != WRONG_EXP_DATE && getCardPAN(&(transData.cardHolderData)) != WRONG_PAN && getTransactionAmount(&(transData.terminalData)) != INVALID_AMOUNT && setMaxAmount(&(transData.terminalData), TRANSACTION_MAX_AMOUNT) != INVALID_MAX_AMOUNT && getTransactionDate(&(transData.terminalData)) != WRONG_DATE) {

        checkAndPrintMsg(recieveTransactionData(&transData));
    }
}

void isValidAccountTest(void) {

    ST_cardData_t cardData;
    ST_accountsDB_t accountRefrence;
    printf("Tester name: Mina George\n");
    printf("Function Name : isValidAccountTest\n");
    printf("*-------------------------------------------------------------------------------*\n\n");
    printf("Test Case 1 : Account valid\n");
    printf("Input Data : An exisiting PAN = 8989374615436851\n");
    printf("Expected Result : successful operation. [SERVER_OK]\n");
    if (getCardPAN(&cardData) == WRONG_PAN) {
        printf("Invalid Pan!");
    }
    else {
        check_server_error(isValidAccount(&cardData, &accountRefrence));
    }
    printf("\n*-------------------------------------------------------------------------------*\n\n");
    printf("Test Case 2 : Account invalid\n");
    printf("Input Data : A non exisiting PAN = 102030405060708090\n");
    printf("Expected Result : Account doesn't Exist.[ACCOUNT_NOT_FOUND]\n");
    if (getCardPAN(&cardData) == WRONG_PAN) {
        printf("Invalid Pan!");
    }
    else {
        check_server_error(isValidAccount(&cardData, &accountRefrence));
    }
}

void isBlockedAccountTest(void) {
    ST_cardData_t cardData;
    ST_accountsDB_t accountRefrence;
    printf("Tester name: Mina George\n");
    printf("Function Name : isBlockedAccountTest\n");
    printf("*-------------------------------------------------------------------------------*\n\n");
    printf("Test Case 1 : [RUNNING] Account\n");
    printf("Input Data : 	{ 2000.0,RUNNING, 8989374615436851 }\n");
    printf("Expected Result : successful operation. [SERVER_OK]\n");
    if (getCardPAN(&cardData) != WRONG_PAN) {
        if (isValidAccount(&cardData, &accountRefrence) != ACCOUNT_NOT_FOUND) {

            check_server_error(isBlockedAccount(&accountRefrence));
        }
    }

    printf("\n*-------------------------------------------------------------------------------*\n\n");
    printf("Test Case 2 : [BLOCKED] Account\n");
    printf("Input Data : 	{ 100000.0, BLOCKED, 5807007076043875 }\n");
    printf("Expected Result : Account BLOCKED.[BLOCKED_ACCOUNT]\n");
    if (getCardPAN(&cardData) != WRONG_PAN) {
        if (isValidAccount(&cardData, &accountRefrence) != ACCOUNT_NOT_FOUND) {

            check_server_error(isBlockedAccount(&accountRefrence));
        }
    }
}

void isAmountAvailableTest(void) {
    ST_terminalData_t termData;
    ST_cardData_t cardData;
    ST_accountsDB_t accountRefrence;
    printf("Tester name: Mina George\n");
    printf("Function Name : isAmountAvailableTest\n");
    printf("*-------------------------------------------------------------------------------*\n\n");
    printf("Test Case 1 : Amount is Available\n");
    printf("Input Data : 	{ 2000.0,RUNNING, 8989374615436851 }, Transaction amount = 500.0\n");
    printf("Expected Result : successful operation. [SERVER_OK]\n");
    if (getCardPAN(&cardData) != WRONG_PAN) {
        if (isValidAccount(&cardData, &accountRefrence) != ACCOUNT_NOT_FOUND) {

            if (isBlockedAccount(&accountRefrence) != BLOCKED_ACCOUNT) {
                if (getTransactionAmount(&termData) != INVALID_AMOUNT);
                {
                    printf("*-------------------------------isAmountAvailableTEST RESULTS-------------------------------*\n");
                    check_server_error(isAmountAvailable(&termData, &accountRefrence));

                }
            }

        }

    }

    printf("\n*-------------------------------------------------------------------------------*\n\n");
    printf("Test Case 2 : Amount is not Available\n");
    printf("Input Data : 	{ 2000.0,RUNNING, 8989374615436851 }, Transaction amount = 2500.0\n");
    printf("Expected Result : Low balance, Amount is not available. [LOW_BALANCE]\n");

    if (getCardPAN(&cardData) != WRONG_PAN) {
        if (isValidAccount(&cardData, &accountRefrence) != ACCOUNT_NOT_FOUND) {

            if (isBlockedAccount(&accountRefrence) != BLOCKED_ACCOUNT) {
                if (getTransactionAmount(&termData) != INVALID_AMOUNT);
                {
                    printf("*-------------------------------isAmountAvailableTEST RESULTS-------------------------------*\n");
                    check_server_error(isAmountAvailable(&termData, &accountRefrence));
                }
            }

        }

    }

}

void saveTransactionTest(void) {
    ST_transaction_t transaction;
    snprintf(transaction.cardHolderData.cardHolderName, sizeof(transaction.cardHolderData.cardHolderName), "Mina George Fawzy Gir");
    snprintf(transaction.cardHolderData.primaryAccountNumber, sizeof(transaction.cardHolderData.primaryAccountNumber), "1234567890123456");
    snprintf(transaction.cardHolderData.cardExpirationDate, sizeof(transaction.cardHolderData.cardExpirationDate), "12/25");

    // Populate terminalData
    transaction.terminalData.transAmount = 100.0;
    transaction.terminalData.maxTransAmount = TRANSACTION_MAX_AMOUNT;
    snprintf(transaction.terminalData.transactionDate, sizeof(transaction.terminalData.transactionDate), "2023-08-13");

    // Set transState and transactionSequenceNumber
    transaction.transState = APPROVED;
    transaction.transactionSequenceNumber = 1;
    printf("Tester name: Mina George\n");
    printf("Function Name : saveTransactionTest\n");
    printf("*-------------------------------------------------------------------------------*\n\n");
    printf("Test Case 1 : save succeed\n");
    printf("Input Data : correct data \n");
    printf("Expected Result : successful operation. [SERVER_OK]\n");
    check_server_error(saveTransaction(&transaction));

    transactionDB[transactionCounter].transactionSequenceNumber = 260;
    printf("\n*-------------------------------------------------------------------------------*\n\n");
    printf("Test Case 2 : save failed\n");
    printf("Input Data : correct data but transactionSequenceNumber = 260> 255 \n");
    printf("Expected Result : saving failed. [SAVING_FAILED]\n");
    check_server_error(saveTransaction(&transaction));

}

void listSavedTransactionsTest(void) {

    ST_transaction_t transData = {
      0
    };

    printf("Tester name: Mina George\n");
    printf("Function Name : listSavedTransactionsTest\n");
    printf("*-------------------------------------------------------------------------------*\n\n");
    printf("Test Case 1 :printing data correctly\n");
    printf("Input Data : correct data \n");
    printf("Expected Result : correctly printed data\n");

    listSavedTransactions();

}

int main(void) {

    		uint8_t input;
	while (1) {
		printf("\nEnter operation (t = isValidAccountTest i = isAmountAvailableTest e = recieveTransactionDataTest\n s= saveTransactionTest, b= isBlockedAccountTest, l = listSavedTransactionsTest, q=quit): ");
		printf("\n.-----------------------------------------------------------------------.\n");
		scanf_s("%c", &input,sizeof(input));
		(void)getchar(); // consume any remaining characters in the input stream

		switch (input) {
		case 't':
            isValidAccountTest();
			break;
		case 'i':
            isAmountAvailableTest();
			break;
		case 'e':
            recieveTransactionDataTest();
			break;
		case 's':
            saveTransactionTest();
			break;
		case 'b':
            isBlockedAccountTest();
			break;
        case 'l':
            listSavedTransactionsTest();
            break;
		case 'q':
			exit(0);
		default:
			printf("Error: Invalid input\n");
			break;
		}
	}
	return 0;


}