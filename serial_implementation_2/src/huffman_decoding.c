#include "include/huffman_decoding.h"

static const int NUM_OF_PROCESSES_DEC = 3;

int roundUp(int numToRound, int multiple)
{
    if (multiple == 0)
        return numToRound;

    int remainder = abs(numToRound) % multiple;
    if (remainder == 0)
        return numToRound;

    if (numToRound < 0)
        return -(abs(numToRound) - remainder);
    else
        return numToRound + multiple - remainder;
}

void calculateBlockRange(int nrOfBlocks, int nrOfProcs, int pid, int *start, int *end) {
    int quotient = nrOfBlocks / nrOfProcs;
    int quoto =	nrOfBlocks % nrOfProcs;

    // printf("result: %d - rest: %d\n", result, rest);

	*start = (pid * quotient);
	if (quoto != 0 && pid > 0)
		*start += (pid > quoto) ? quoto : pid;

	*end = *start + quotient;
	if (quoto != 0 && (pid == 0 || pid < quoto))
		++(*end);
		
	// printf("cycle %d: start: %d - end: %d = %d\n", i, *start, *end - 1, *end - *start);
}

int huffman_decoding() {
	printf("\nDecoding\n");

	FILE *fp;
	unsigned short *dimensions = NULL;
	char *decodedText = NULL;
	char *tempText = NULL; 
	int decodedTextSize = 0;

	// unsigned short dimensions[] = {8, 7, 7, 9, 9, 4, 7 /*8*/, 7, 8, 7, 6, 3, 7, 7, 8, 7, 7, 3, 7, 7, 9, 8, 9, 8};

	for (int i = 0; i < NUM_OF_PROCESSES; i++) {
		FileHeader header = {.byteStartOfDimensionArray = 0};
		int start = 0;
		int end = 0;
		int nodes = 0;
		int treeByteSize = 0;
		int fileSize = 0;
		int number_of_blocks = 0;
		int oldDecodedTextSize = 0;
		int prevSize = 0;
		int startPoint = 0;

		switch (i) {
			case 0:
				fp = openFile(inputFile, READ_B, 0);

				parseHeader(&header, fp);
				printf("Header size: %lu\n", FILE_HEADER_ELEMENTS * sizeof(unsigned int));
				printf("Encoded arrayPosStartPos: %d\n", header.byteStartOfDimensionArray);

				TreeNode* root1 = malloc(sizeof(TreeNode));
				parseHuffmanTree(root1, fp);
				nodes = countTreeNodes(root1);
				treeByteSize = nodes * sizeof(TreeArrayItem);
				printf("Encoded tree size: %d\n", treeByteSize);
				printf("Huffman tree nodes number: %d\n", nodes);
				// printHuffmanTree(root, 0);

				fileSize = getFileSize(inputFile);
				number_of_blocks = (fileSize - header.byteStartOfDimensionArray) / sizeof(unsigned short);
				printf("\nTotal number of blocks: %d\n", number_of_blocks);
				printf("File size: %d\n", fileSize);

				dimensions = malloc(sizeof(unsigned short) * number_of_blocks);
				
				parseBlockLengths(dimensions, fp, number_of_blocks, header.byteStartOfDimensionArray);

				// for (int i = 0; i < number_of_blocks; i++)
				// 	printf("\tdimension[%d] = %d\n", i, dimensions[i]);

				// printf("\n");

				calculateBlockRange(number_of_blocks, NUM_OF_PROCESSES_DEC, 0, &start, &end);
				printf("Process 0 - Block range: %d - %d - Blocks nr: %d\n", start, end - 1, end - start);

				tempText = decodeFromFile(
					sizeof(FileHeader) + treeByteSize,
					dimensions,
					start,
					end - start,
					fp,
					root1);

				printf("decoded string: %s\n\n", tempText);

				decodedTextSize = strlen(tempText);
				decodedText = malloc(decodedTextSize + 1);
				strcpy(decodedText, tempText);

				freeBuffer(dimensions);
				freeBuffer(tempText);
				fclose(fp);
				break;

			case 1:
				fp = openFile(inputFile, READ_B, 0);

				parseHeader(&header, fp);
				printf("Header size: %lu\n", FILE_HEADER_ELEMENTS * sizeof(unsigned int));
				printf("Encoded arrayPosStartPos: %d\n", header.byteStartOfDimensionArray);

				TreeNode* root2 = malloc(sizeof(TreeNode));
				parseHuffmanTree(root2, fp);
				nodes = countTreeNodes(root2);
				treeByteSize = nodes * sizeof(TreeArrayItem);
				printf("Encoded tree size: %d\n", treeByteSize);
				printf("Huffman tree nodes number: %d\n", nodes);
				// printHuffmanTree(root, 0);

				fileSize = getFileSize(inputFile);
				number_of_blocks = (fileSize - header.byteStartOfDimensionArray) / sizeof(unsigned short);
				printf("\nTotal number of blocks: %d\n", number_of_blocks);
				printf("File size: %d\n", fileSize);

				dimensions = malloc(sizeof(unsigned short) * number_of_blocks);
				
				parseBlockLengths(dimensions, fp, number_of_blocks, header.byteStartOfDimensionArray);

				// for (int i = 0; i < number_of_blocks; i++)
				// 	printf("\tdimension[%d] = %d\n", i, dimensions[i]);

				// printf("\n");

				calculateBlockRange(number_of_blocks, NUM_OF_PROCESSES_DEC, 1, &start, &end);
				printf("Process 1 - Block range: %d - %d - Blocks nr: %d\n", start, end - 1, end - start);

				for (int i = 0; i < start; i++) {
					prevSize += dimensions[i];

					if (prevSize % BIT_8 != 0)	
						prevSize = roundUp(prevSize, BIT_8);
				}

				prevSize /= BIT_8;
				startPoint = sizeof(FileHeader) + treeByteSize + prevSize;

				tempText = decodeFromFile(
					startPoint,
					dimensions,
					start,
					end - start,
					fp,
					root2);

				printf("decoded string: %s\n", tempText);

				oldDecodedTextSize = decodedTextSize;
				decodedTextSize += strlen(tempText);
				decodedText = realloc(decodedText, decodedTextSize + 1);
				strcpy(decodedText + oldDecodedTextSize, tempText);

				freeBuffer(dimensions);
				freeBuffer(tempText);
				fclose(fp);
				break;
			
			case 2:
				fp = openFile(inputFile, READ_B, 0);

				parseHeader(&header, fp);
				printf("Header size: %lu\n", FILE_HEADER_ELEMENTS * sizeof(unsigned int));
				printf("Encoded arrayPosStartPos: %d\n", header.byteStartOfDimensionArray);

				TreeNode* root3 = malloc(sizeof(TreeNode));
				parseHuffmanTree(root3, fp);
				nodes = countTreeNodes(root3);
				treeByteSize = nodes * sizeof(TreeArrayItem);
				printf("Encoded tree size: %d\n", treeByteSize);
				printf("Huffman tree nodes number: %d\n", nodes);
				// printHuffmanTree(root, 0);

				fileSize = getFileSize(inputFile);
				number_of_blocks = (fileSize - header.byteStartOfDimensionArray) / sizeof(unsigned short);
				printf("\nTotal number of blocks: %d\n", number_of_blocks);

				dimensions = malloc(sizeof(unsigned short) * number_of_blocks);
				
				parseBlockLengths(dimensions, fp, number_of_blocks, header.byteStartOfDimensionArray);

				// for (int i = 0; i < number_of_blocks; i++)
				// 	printf("\tdimension[%d] = %d\n", i, dimensions[i]);

				// printf("\n");

				calculateBlockRange(number_of_blocks, NUM_OF_PROCESSES_DEC, 2, &start, &end);
				printf("Process 2 - Block range: %d - %d - Blocks nr: %d\n", start, end - 1, end - start);

				for (int i = 0; i < start; i++) {
					prevSize += dimensions[i];

					if (prevSize % BIT_8 != 0)	
						prevSize = roundUp(prevSize, BIT_8);
				}

				prevSize /= BIT_8;
				startPoint = sizeof(FileHeader) + treeByteSize + prevSize;

				tempText = decodeFromFile(
					startPoint,
					dimensions,
					start,
					end - start,
					fp,
					root2);

				printf("decoded string: %s\n", tempText);

				oldDecodedTextSize = decodedTextSize;
				decodedTextSize += strlen(tempText);
				decodedText = realloc(decodedText, decodedTextSize + 1);
				strcpy(decodedText + oldDecodedTextSize, tempText);

				freeBuffer(dimensions);
				freeBuffer(tempText);
				fclose(fp);
				break;

			default:
				break;
		}
	}

	printf("\nDecoded text: \n\n%s\n\n", decodedText);

	freeBuffer(decodedText);

	return 0;
}
