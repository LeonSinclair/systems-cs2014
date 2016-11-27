#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>



bool arrayContains(char array[], char key, int size) // searchs an array for a key and then returns a boolean answer
{
	for(int i =0; i<size; i++)
	{
		if(array[i]==key)
		{
			return true;
		}
		if(array[i]=='\0')
		{
			return false; //if the character is a zeroed character then the rest of the array afterwards should be as well, so no point continuing on and wasting time
		}
	}
	return false;

}


void zeroArray(char a[], int size) // takes an array of characters and sets all of them to null, allows for simple checking of when there is nothing else left in an array (not certain if c does this automatically but best to be sure)
{
	int i;
	for(i=0; i<size; i++)
	{
		a[i] = '\0';
	}
}


int operatorsGetSize() //simple getter
{
	char ops[] = "+-*/^()\0";
	return sizeof(ops);
}

void writeOut(char a[]) //Not certain how necessary this is but it looks nice to have a function for it
{
	printf("%s\n", a);
}

void convertToReversePolishNotation(char input[], char operators[], char output[]) // converts infix to RPN
{
	char convertStack[256]; // our "stack" is just going to be an array in Empty Ascending Order ( I think that's right it's been a while)
	zeroArray(convertStack,256);
	int sizeOfOperators = operatorsGetSize();
	int i =0; //for finding place in input
	int stackIndex=0; //indexes for stack and output string
	int outputIndex=0;
	char nextChar; 
	while(input[i]!='\0') // this is why we zero the arrays
	{
		nextChar = input[i];
		if(arrayContains(operators, nextChar, sizeOfOperators))//if the token is an operator push it to the stack
		{
			switch(nextChar)
			{
				 
				//check if the array contains an operator of higher precedence (this is messy I'm sorry)
				case '+':
				case '-':while(arrayContains(convertStack,'^', 256) || arrayContains(convertStack, '*', 256) || arrayContains(convertStack, '/', 256))
					{
						char tmp = convertStack[stackIndex-1]; //pop the item off the stack then append to the output string
						convertStack[stackIndex-1] = '\0';
						stackIndex--;
						output[outputIndex] = tmp;
						outputIndex++;
					}
					break;
				case '*':	
				case '/':while(arrayContains(convertStack,'^', 256))
					{
						char tmp = convertStack[stackIndex-1]; //pop the item off the stack then append to the output string
						convertStack[stackIndex-1] = '\0';
						stackIndex--;
						output[outputIndex] = tmp;
						outputIndex++;
					}
					break;
				case '^': //just push to the stack, no other work needed since there is no operator with higher precedence
				case '(': 
					break;
				//only remaining case is if it's equal to ')' since we know from the if statement that it's in the list of operators
				default: while((convertStack[stackIndex-1]!='('))
					{
						//while the top of the stack is not a left bracket, pop and append to output string
						char tmp = convertStack[stackIndex-1];
						convertStack[stackIndex-1] = '\0';
						stackIndex--;
						output[outputIndex] = tmp;
						outputIndex++;
					}	
					if(convertStack[stackIndex-1]=='(')
					{
						convertStack[stackIndex-1] = '\0'; // pop the left bracket and don't append to string
						stackIndex--;
					}
					break;

			}
			if(nextChar!=')') // we don't want the right bracket to be put on the stack so we ignore it
			{	
				convertStack[stackIndex] = nextChar; //push to the stack
				stackIndex++;
			}			
		}
		else // if it's a number just append it to the output string
		{
			output[outputIndex] = nextChar;
			outputIndex++;
		}
		i++;
		if(input[i]=='\0') //if there is nothing left to add
		{
				while(stackIndex>=0)
				{	//pop remaining items from stack and add to output string
					char tmp = convertStack[stackIndex-1];
					convertStack[stackIndex] = '\0';
					stackIndex--;
					output[outputIndex] = tmp;
					outputIndex++;
				}
		}
	}

}


double calculateReversePolishNotation(char input[], char operators[])
{
	
	double operandStack[256]; // need a way to store numbers once they're converted from tokens and a stack for the operands to be placed on 
	char doubleStorage[256];
	zeroArray(doubleStorage,256);
	int stackPointer = 0; // just initialising some variables, nothing fancy here
	int storagePointer = 0;
	int sizeOfOperators = operatorsGetSize();
	double operand1 = 0;
	double operand2 = 0;
	double operandToPush = 0;
	double answer = 0;
	double result = 0;
	int i = 0;
	while(input[i]!='\0')
	{
		char nextChar = input[i];
		if(arrayContains(operators, nextChar, sizeOfOperators))
		{	//operands are in this order because it's popping off of a stack and this is the simplest way of doing it
			operand2 = operandStack[stackPointer-1];
			stackPointer--;
			operand1 = operandStack[stackPointer-1];
			stackPointer--;
			switch(nextChar) // perform different operations depending on what the operator is
			{
				case '+': result = operand1 + operand2;
					break;
				case '-': result = operand1 - operand2;
					break;
				case '*': result = operand1 * operand2;
					break;
				case '/': result = operand1 / operand2;
					break;
				default: result = pow(operand1,operand2);
					break;
			}
			operandStack[stackPointer] = result; // add result to the stack
			stackPointer++;
		}
		else if(nextChar != ' ') // strips out whitespace
		{
			doubleStorage[storagePointer] = nextChar;
			storagePointer++;
			if(input[i+1]==' ') // if the next character is whitespace then the token must be over
			{
				//convert token to double and push to the stack
				operandToPush = strtod(doubleStorage, NULL);
				zeroArray(doubleStorage,256);
				storagePointer = 0;
				operandStack[stackPointer] = operandToPush;
				stackPointer++;
			}
		}
		i++;
		
		
	}
	//pop last element on stack and boom
	return operandStack[0];
	
}

int tokenise(char str[], int start, char result[])
{
	int j =0;
	while(result[j]!='\0') // skip to the next free space in result
	{
		j++;
	}
	int i = start;
	bool tokenFinished = false;
	while(tokenFinished == false)
	{
		char nextChar = str[i];
		if(nextChar!=' ')
		{
			result[j] = nextChar;
			j++;
		}
		else if(i+1>strlen(str))
		{
			return -1;
		}
		else
		{
			result[j] = ',';
			j++;
			i+=3; //skip to next token that isn't an operator
			return i;
		}
		i++;
	}
}

int tokeniseOps(char str[], int start, char result[], char operators[])
{
	int j=0;
	while(result[j]!='\0') // skip to the next free space in result
	{
		j++;
	}
	int i = start;
	int sizeOfOperators = operatorsGetSize();
	bool tokenFinished = false;
	while(tokenFinished == false)
	{
		char nextChar = str[i];
		if(arrayContains(operators,nextChar, sizeOfOperators)) // if it's an operator, previous token is assumed to have ended, add whitespace because that comes in handy later and we can't assume the user has put any in
		{
			result[j]=' ';
			j++;
			result[j] = nextChar;
			j++;
			result[j]=' ';
			j++;
			tokenFinished=true;
		}
		else if(nextChar!='\n')
		{
			
			result[j] = nextChar;
			j++;
		}
		if(arrayContains(operators, str[i+1], sizeOfOperators)) //if we've read in some characters and the next char is an operator the previous token must be finished
		{
			tokenFinished=true;
		}
		if(i+1>strlen(str))
		{
			return -1;
		}
		i++;
	}
	return i;
}





int main()
{
	const int MAX_STRING_LENGTH = 256;
	char input[MAX_STRING_LENGTH];
	zeroArray(input, MAX_STRING_LENGTH);
	char choice[MAX_STRING_LENGTH];
	zeroArray(choice, MAX_STRING_LENGTH);
	//need to ask for input here TODO
	printf ("Choose either 'infix' or 'postfix': ");
	fgets(choice, MAX_STRING_LENGTH, stdin); // take in user choise
	printf ("Please enter an expression of the previously chosen type: ");
	fgets(input, MAX_STRING_LENGTH, stdin);
	char result[MAX_STRING_LENGTH];
	zeroArray(result,MAX_STRING_LENGTH);
	int start=0;
	char ops[] = "+-*/^()\0";
	while(start!=-1)
	{
		//start = tokenise(input, start, result);
  		start = tokeniseOps(input, start, result, ops);
		//writeOut(result); fairly certain this line just adds clutter to the output so let's not
	}
	char output[MAX_STRING_LENGTH]; 
	zeroArray(output,MAX_STRING_LENGTH);
	double answer = 0;
	if(arrayContains(choice, 'n', MAX_STRING_LENGTH) || arrayContains(choice, 'N', MAX_STRING_LENGTH) ) // if the input contains an N of some sort it probably means infix, otherwise it's RPN already
	{
		convertToReversePolishNotation(result, ops, output);
		answer = calculateReversePolishNotation(output, ops);
	}
	else
	{
		convertToReversePolishNotation(result, ops, output);
		answer = calculateReversePolishNotation(output, ops);
	}
	printf("%f\n", answer);
}


/*
	rough running order of program
	- take infix string - that's done, just reading in input
	- convert to RPN string - first tokenise? in your tokenise ops it should also push to stack of some sort, do stack as an array, have it zeroed and when you pop it puts it back to zero, only need one stack for operators 
	- numbers just get added to the string, we then retokenise it for the calculations bit like was said earlier
	- do calculations on RPN string
	- 
	dealing with reverse polish notation - only need one stack for numbers

	if(!operator)
	{
		convert to int from token and push to stack
	}
	else
	{
		pop two items from stack
		use switch to decide what operation to use
		do operation, push result to stack
	}







*/
