//decoding
//need to read in the next byte from the file, then take the bits in it and pass them individually
//need a counter of how many bits we've used
//keep taking bits till you reach a leaf, then we want to write that character to the file

//encoding
//read bits from the tree until you fill a byte, then dump that byte to the file
//need a counter of how many bits we've used

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>


// need to define my own objects here
// basically need an array of pointers
// can just only use compounds instead of huffnodes, and then have the left and rights point to compounds which could be null?


struct huffnode
{
	long freq;
	bool isLeaf;
	unsigned char c;
	struct huffnode *left;
	struct huffnode *right;

};

void addFrequency(unsigned char c, struct huffnode **list)
{
	list[c]->freq++;
}

struct huffnode** createHuffnodeList(int size, struct huffnode **huffnodeList)
{
	huffnodeList = malloc (size * sizeof(struct huffnode));
	for(int i = 0; i<size; i++)
	{
		struct huffnode *a = malloc(sizeof(struct huffnode)); //default nodes are all leaves 
		a->left = NULL;
		a->right = NULL;
		a->isLeaf = true;
		a->freq = 0;
		a->c=i;
		huffnodeList[i] = a; 
	}
	huffnodeList[size] = NULL;
	return huffnodeList;
}

int getSize(struct huffnode **huffnodeList)
{
	int i = 0;
	while(huffnodeList[i]!=NULL) 
	{
		i++;
	}
	return i;
}

void sort(int size, struct huffnode **huffnodeList)
{	
  	for(int i = 0; i<(size-1); i++)
	{
    		for(int j = 0; j<size-i-1; j++)
		{
     			if(huffnodeList[j]->freq < huffnodeList[j+1]->freq) //simple bubble sort for the huffnodes
			{
        			struct huffnode *tmp = huffnodeList[j];
        			huffnodeList[j] = huffnodeList[j+1];
        			huffnodeList[j+1] = tmp;
			}
			else if((huffnodeList[j]->freq == huffnodeList[j+1]->freq) && (huffnodeList[j]->c > huffnodeList[j+1]->c))
			{
				struct huffnode *tmp = huffnodeList[j];
        			huffnodeList[j] = huffnodeList[j+1];
        			huffnodeList[j+1] = tmp;
			}	
		}
	}
}

struct huffnode* deleteSmallest(int size, struct huffnode **huffnodeList)
{
	int smallestIndex = 0;
	for(int i = 0; i < size; i ++)
	{
		if(huffnodeList[i]->freq < huffnodeList[smallestIndex]->freq)
		{
			smallestIndex = i;
		}	
		else if((huffnodeList[i]->freq == huffnodeList[smallestIndex]->freq) && (huffnodeList[i]->c > huffnodeList[smallestIndex]->c))
		{
			smallestIndex = i;
		}
	}
	struct huffnode *result = huffnodeList[smallestIndex];
	huffnodeList[smallestIndex] = huffnodeList[size-1];
	huffnodeList[size-1] = NULL;
	return result;
}

void buildHuffmanTree(struct huffnode **huffnodeList)
{
	//array should be sorted
	//in a loop
	// remove smallest 2, combine them (needs deleteSmallest, build does the combining)
	// add compound into list
	// repeat until there is only 1 left
	int size = getSize(huffnodeList);
	printf("%d\n", size);
	int nfreqs = 255;
	for(int i = 0; i < nfreqs; i++)
	{
		struct huffnode *compoundNode = malloc (sizeof(struct huffnode));
		struct huffnode *smallest1 = deleteSmallest(size, huffnodeList);
		compoundNode->left = smallest1;
		size--;
		struct huffnode *smallest2 = deleteSmallest(size, huffnodeList);
		compoundNode->right = smallest2;
		size--;
		compoundNode->freq = compoundNode->right->freq + compoundNode->left->freq;
		compoundNode->c = -1;
		compoundNode->isLeaf = false;
		huffnodeList[size] = compoundNode;
		size++;
	}
}

//this is going to have to be recursive

void toStringHuff(char *path, struct huffnode *huffNode, char ** paths)
{ 
	//every time it takes a left it adds a 0 to a string, every time it takes a right it adds a 1
	//then every time it finds a leaf it prints the string and the number 
	if(huffNode==NULL) // back up
	{
		return;
	}
	if(huffNode->isLeaf)
	{
		//printf("%c", huffNode->c);		
		//printf("%s", " is encoded as ");
		//printf("%s\n", path);
		paths[huffNode->c] = path;
	}
	else
	{	
		char *left = malloc(strlen(path)+2);
		strcpy(left,path);
		char *right = malloc(strlen(path)+2);
		strcpy(right,path);
		if(huffNode->left!=NULL)
		{
			left = strcat(left, "0");
			toStringHuff(left, huffNode->left, paths);		
		}
		if(huffNode->right!=NULL)
		{	
			right = strcat(right, "1");
			toStringHuff(right, huffNode->right, paths);		
		}
	}

}


int main(int argc, char ** argv)
{
	unsigned char c;
	if(argc!=5)
	{
		fprintf(stderr, "Incorrect number of arguments\n");
		exit(1);
	}
	FILE *fp;
	fp = fopen(argv[2],"r");
	assert (fp!=NULL);
	c = fgetc(fp);
	
	struct huffnode **huffNodeArray = createHuffnodeList(256, huffNodeArray);
	while(!feof(fp))
	{
		//printf("%c", c);
		addFrequency(c, huffNodeArray);
		c = fgetc(fp);
	}

	for(int i = 0; i<256; i++)
	{
		if(huffNodeArray[i]->freq == 0)
		{
			huffNodeArray[i]->freq = 1;
		}
	}
	buildHuffmanTree(huffNodeArray);
	char pass[] = "";
	char ** paths = malloc(256);
	toStringHuff(pass, huffNodeArray[0], paths);
	fclose(fp);
	FILE *input;
	input = fopen(argv[3], "r");
	assert(input!=NULL);
	c = fgetc(input);
	FILE *output;	
	output = fopen(argv[4], "w");
	if(strncmp(argv[1], "encode", strlen(argv[1]))==0)
	{
		while(!feof(input))
		{
			char *put = paths[c];
			int out;
			
			for(int i = 0; i<strlen(put); i++)
			{
				int j = 2;
				j = j >> put[i];
				if(j!=2)
				{
					out += 1;
					out = out << 1;
				}
			}
			fputc(out, output);
			c = fgetc(input);
		}
		char *put = paths[5]; //put an EOF into the file
		int out;
		for(int i = 0; i<strlen(put); i++)
		{
			int j = 2;
			j = j >> put[i];
			if(j!=2)
			{
				out = out << 1;
				out = out & 1;
				
			}
			else
			{
				out = out << 1;
			}
		}
		fputc(out, output);
	}
	else
	{
		bool reading = true;
		char put;
		struct huffnode *traverse = huffNodeArray[0];
		//take in a character, read all the bits in it and use those to traverse the tree
		//then if you hit a leaf you can write that character to a file and return to the root?
		while(reading)
		{
			while(!traverse->isLeaf)
			{
				if(c&1)
				{
					traverse = traverse->right;				
				}
				else
				{
					traverse = traverse->left;
				}
				if(traverse->isLeaf)
				{
					put = traverse->c;
				}
				else
				{
					c = c >> 1;
				}
				if(c==0)
				{
					c = fgetc(input);
				}
			}
			if(put == *paths[5])
			{
				fputc(put, output);
				reading = false;
			}
			else
			{
				fputc(put, output);
				traverse = huffNodeArray[0];
			}
		}
	}
	return 0;
}
