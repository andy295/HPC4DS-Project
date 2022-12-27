#include "include/huffman_coding.h"

int huffman_coding() {

	double time_spent = 0.0;
	clock_t begin = clock();

	// get the process text
	char *text = NULL;
	long total_text_length = readFilePortionForProcess(SRC_FILE, &text, 0, 1);

	CharFreqDictionary acp = {.number_of_chars = 0, .charFreqs = NULL};
	getCharFreqsFromText(&acp, text, total_text_length, 0);
	printf("Process %d: %ld characters read in bytes \n", 0, total_text_length);

	// sort the LetterFreqDictionary only in the master process
	sortCharFreqs(&acp);

	// create the Huffman tree
	LinkedListTreeNodeItem* root = createHuffmanTree(&acp);

	CharEncodingDictionary encodingsDict = {.number_of_chars = acp.number_of_chars, .charEncoding = NULL};
	getEncodingFromTree(&encodingsDict, &acp, root->item);

	printf("\nEncoding dictionary with %d chars:\n", encodingsDict.number_of_chars);
	printEncodings(&encodingsDict);

	printf("\nProcess 0 text:\n\t");
	for (int i = 0; i < strlen(text) + 1; i++)
		printFormattedChar(text[i]);
	printf("\n");

	// encode the text
	EncodingText encodingText = {.nr_of_dim = 0, .nr_of_bytes = 0, .nr_of_bits = 0, .dimensions = NULL, .encodedText = NULL};
	encodeStringToByteArray(&encodingText, &encodingsDict, text, total_text_length);

	printf("\nProcess 0 encoded text:\n\t");
	printf("number of bits: %hu\n\t", encodingText.nr_of_bits);
	printf("number of bytes: %hu\n\t", encodingText.nr_of_bytes);
	printf("number of dimensions: %hu\n", encodingText.nr_of_dim);
	for (int i = 0; i < encodingText.nr_of_dim; i++)
		printf("\tdimension[%d] = %d\n", i, encodingText.dimensions[i]);
	printEncodedText(encodingText.encodedText, encodingText.nr_of_bytes);
	printf("\n");

	freeBuffer(acp.charFreqs);

	// write the encoded text into the file
	printf("\nEncoding\n");

	// write an empty header to the file
	FileHeader fileHeader = {.byteStartOfDimensionArray = 0};
	BYTE *startPos = (BYTE*)&fileHeader;
	writeBufferToFile(ENCODED_FILE, startPos, sizeof(unsigned int) * FILE_HEADER_ELEMENTS, WRITE_B, 0);
	printf("Header size: %lu\n", FILE_HEADER_ELEMENTS * sizeof(unsigned int));
	printf("Encoded arrayPosStartPos: %d\n", fileHeader.byteStartOfDimensionArray);

	// write the encoded tree to the file
	int byteSizeOfTree; 
	BYTE* encodedTree = encodeTreeToByteArray(root->item, &byteSizeOfTree);
	writeBufferToFile(ENCODED_FILE, encodedTree, byteSizeOfTree, APPEND_B, 0);
	printf("Encoded tree size: %d\n", getByteSizeOfTree(root->item));

	int nodes = countTreeNodes(root->item);
	printf("Huffman tree nodes number: %d\n", nodes);
	// printHuffmanTree(root->item, 0);

	printf("Process 0 - number of bytes: %d\n", encodingText.nr_of_bytes);

	printf("\nProcess 0 encoded text:\n\n");
	printEncodedText(encodingText.encodedText, encodingText.nr_of_bytes);

	// write all the encoded text to file
	writeBufferToFile(ENCODED_FILE, encodingText.encodedText, encodingText.nr_of_bytes, APPEND_B, 0);
	printf("Encoded text size: %d\n", encodingText.nr_of_bytes);

	// write the dimensions array to file
	BYTE* dimensions = (BYTE*)encodingText.dimensions;
	writeBufferToFile(ENCODED_FILE, dimensions, encodingText.nr_of_dim * sizeof(unsigned short), APPEND_B, 0);
	printf("Dimension array size: %ld\n", encodingText.nr_of_dim * sizeof(short));

	for (int i = 0; i < encodingText.nr_of_dim; i++)
		printf("\tdimension[%d] = %d\n", i, encodingText.dimensions[i]);

	printf("\n");

	// write header to file
    fileHeader.byteStartOfDimensionArray = sizeof(FileHeader) + byteSizeOfTree + encodingText.nr_of_bytes;
	startPos = (BYTE*)&fileHeader; 
	printf("Total number of blocks: %d\n", encodingText.nr_of_dim);
    writeBufferToFile(ENCODED_FILE, startPos, sizeof(unsigned int) * FILE_HEADER_ELEMENTS, WRITE_B_AT, 0);

	printf("\nEncoded file size: %d\n", getFileSize(ENCODED_FILE));
	printf("Original file size: %d\n", getFileSize(SRC_FILE));

	freeBuffer(encodingText.dimensions);
	freeBuffer(encodingText.encodedText);
	freeLinkedList(root);

	clock_t end = clock();

	// calculate elapsed time by finding difference (end - begin) and
	// dividing the difference by CLOCKS_PER_SEC to convert to seconds
	time_spent += (double)(end - begin) / CLOCKS_PER_SEC;

	printf("The elapsed time is %f seconds\n", time_spent);

	return 0;
}

