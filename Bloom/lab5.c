#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>


typedef struct
{
	int arraySize;
	unsigned *arrayP;

} Bitset;


int checkBit(unsigned num, int desiredBit)
{
// remember that bits are 0 indexed!
	desiredBit = 1<<desiredBit; //left shift 1 into the place of the desired bit, AND it with the number, if it is >1 then the desired bit is 1
	num = num & desiredBit;
	if(num>1) return 1;
	return 0;

}
Bitset createBitset (int size) //creates a new bitset of the given size
{
	Bitset newBitset;
	newBitset.arraySize = size;
	newBitset.arrayP = (unsigned*)calloc(size, sizeof(unsigned));
	return newBitset;
}

int bitsetLookup(Bitset this, int item) //takes a bitset and finds a specific item in it
{//remember that bits are 0 indexed
	int arrayIndex = item/(sizeof(unsigned)*8);
	int pos = item%(sizeof(unsigned)*8);
	unsigned tmp = this.arrayP[arrayIndex];
	return(checkBit(tmp, pos));

}

int bitsetAdd(Bitset this, int item) //adds an item to a bitset
{
	if(item>this.arraySize*8*sizeof(unsigned)) //if the item would be greater than the potential size of the array, return 0 as an error code
	{
		return 0;
	}
	int arrayIndex = item/(sizeof(unsigned)*8);
        int pos = item%(sizeof(unsigned)*8);
        unsigned tmp = this.arrayP[arrayIndex];
	int desiredBit = 1<<pos;
	tmp = tmp | desiredBit;
	this.arrayP[arrayIndex] = tmp;
	return 1;
}

int bitsetRemove (Bitset this, int item) //removes an item from a bitset (not normally used due to collisions making this unsafe)
{

   if(item>this.arraySize*8*sizeof(unsigned)) //if the item would be greater than the potential size of the array, returns 0 as an error code
        {
                return 0;
        }
        int arrayIndex = item/(sizeof(unsigned)*8);
        int pos = item%(sizeof(unsigned)*8);
        unsigned tmp = this.arrayP[arrayIndex];
        int desiredBit = 1<<pos;
	desiredBit = ~desiredBit; //set all bits to 1 except the desired bit
        tmp = tmp & desiredBit; //and with 1 is always what was there, and with 0 is always false
        this.arrayP[arrayIndex] = tmp;
        return 1;

}


void bitsetUnion(Bitset src1, Bitset src2, Bitset dest) //union is implemented via bitwise OR
{
	for(int i = 0; i<dest.arraySize; i++)
	{
		dest.arrayP[i] = src1.arrayP[i] | src2.arrayP[i];
	}

}


void bitsetIntersection(Bitset src1, Bitset src2, Bitset dest) //intersection is implemented via bitwise AND
{
        for(int i = 0; i<dest.arraySize; i++)
        {
                dest.arrayP[i] = src1.arrayP[i] & src2.arrayP[i];
        }

}

void printBitsetCharacters(Bitset input) //to print the characters in a bitset you can just find their index value and print that as a character since no hashing is done to them
{
	for(int i =0; i<input.arraySize; i++)
	{
		if(bitsetLookup(input, i)==1)
		{
			printf("%c\n", i);
		}
	}

}

void zeroBitsetArray(Bitset bitset) //initialises a bitset array to all zeroes, don't think it's necessary, but useful if you need to wipe a bitset?
{
	for(int i = 0; i<bitset.arraySize; i++)
	{
		bitset.arrayP[i] = 0;
	}
}

int hashFunction1(char *string, int hashTableSize) //multiplies the key by a prime and then adds the value of the character, simple but effective
{
	int key = 0;
	for(int i = 0; i<strlen(string); i++)
	{
		key = key*101 + string[i];
	}
	return key%(hashTableSize*8*sizeof(unsigned));
}


int hashFunction2(char *string, int hashTableSize) // effectively the same thing but with a different prime number
{
	int key = 0;
	for(int i = 0; i<strlen(string); i++)
	{
		key = key*31 + string[i]; //horner multiplication method of hashing
	}
	return key%(hashTableSize*8*sizeof(unsigned));

}


int checkString(char *string, Bitset bitset)
{
	int key1 = hashFunction1(string, bitset.arraySize); // finds the two keys associated with the string, if both locations are 1 then we can assume it is in the set
	int key2 = hashFunction2(string, bitset.arraySize);
	if(bitsetLookup(bitset, key1)>0 && bitsetLookup(bitset, key2)>0) return 1; //reduces collisions by checking multiple hash functions
	return 0; //0 is our false value, 1 is our true value

}
int main(int argc, char *argv[])
{
	if(argc!=4) // unless user inputs the correct number of arguments the program will not run
	{
		printf("Incorrect number of arguments, please try again \n");
		return 1;
	}
	printf("Program name %s\n", argv[0]); // prints the program name and mode you have chosen
	printf("Mode %s\n", argv[1]);

	if(strncmp(argv[1], "Bloom", strlen(argv[1]))==0) //if you put in bloom then it runs in bloom mode, otherwise it runs as if you put in bitvector
	{
		FILE *firstFileP; // open both files in read only mode
		FILE *secondFileP;

		firstFileP = fopen(argv[2],"r");
		secondFileP = fopen(argv[3],"r");
		Bitset seen = createBitset(1400);  //initialise bitsets
		Bitset written = createBitset(1400);
		char *buff = (char*)calloc(256, sizeof(char)); //buffer for reading in strings
		while(fscanf(firstFileP, "%s", buff)!=EOF) // condition needs to keep on reading until first file is empty
		{
			int key1 = hashFunction1(buff, seen.arraySize); // hash the string and add the two keys to the bitset
			int key2 = hashFunction2(buff, seen.arraySize);
			if(bitsetAdd(seen, key1)==0) printf("Error adding first item \n"); //if the item key is too large just give an error
			if(bitsetAdd(seen, key2)==0) printf("Error adding second item \n");
		}
		int j =0; //used for counting number of matches
		while(fscanf(secondFileP, "%s", buff)!=EOF) // while file isn't empty effectively
		{
			int key1 = hashFunction1(buff, written.arraySize); // hash the string
			int key2 = hashFunction2(buff, written.arraySize);
			if(checkString(buff, written)==0) //if it's not in written and it is in seen, put it in written and write it out
			{
				if(checkString(buff, seen)>0)
				{
					j++;
					bitsetAdd(written, key1);
					bitsetAdd(written, key2);
					printf("%s\n", buff);
				}
			}
		}
		printf("Number of words printed: %d\n", j);

	}
	else
	{
		char *input1P = (char*)calloc(strlen(argv[2]), sizeof(char)); //take the arguments and turn them into arrays of characters that are more easily manipulated
		input1P = argv[2];
		char *input2P = (char*)calloc(strlen(argv[3]), sizeof(char));
		input2P = argv[3];
		Bitset input1 = createBitset(256); //initialise all the bitsets needed
		Bitset input2 = createBitset(256);
		Bitset intersection = createBitset(256);
		Bitset unionSet = createBitset(256);


		for(int i =0; i<strlen(input1P); i++) //put characters from argument into the bitset
		{
			if(bitsetAdd(input1, input1P[i])==0)
			{
				printf("Error adding first item \n");
			}
		}
		printf("Characters in first input arg \n"); //print all the charcters in the first input argument
		printBitsetCharacters(input1);


		for(int i =0; i<strlen(input2P); i++) //put characters from argument into the bitset
		{
			if(bitsetAdd(input2, input2P[i])==0)
			{
				printf("Error adding second item \n");
			}
		}
		printf("Characters in second input arg \n"); //print all the charcters in the second input argument
		printBitsetCharacters(input2);


		bitsetIntersection(input1, input2, intersection); //find the intersection of both bitsets then print it
		printf("Characters in intersection \n");
		printBitsetCharacters(intersection);


		bitsetUnion(input1, input2, unionSet); //find the union of both bitsets then print it
		printf("Characters in union \n");
		printBitsetCharacters(unionSet);
	}
	return 0;
}

