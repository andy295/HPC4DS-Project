#include "include/huffman_coding.h"

int huffman_coding() {

	double time_spent = 0.0;
	clock_t begin = clock();

	// get the processes' portion of text
	char *text0 = NULL;
	char *text1 = NULL;
	char *text2 = NULL;
	char *text3 = NULL;

	long total_text_length0;
	long total_text_length1;
	long total_text_length2;
	long total_text_length3;

	CharFreqDictionary acp0 = {.number_of_chars = 0, .charFreqs = NULL};
	CharFreqDictionary acp1 = {.number_of_chars = 0, .charFreqs = NULL};
	CharFreqDictionary acp2 = {.number_of_chars = 0, .charFreqs = NULL};
	CharFreqDictionary acp3 = {.number_of_chars = 0, .charFreqs = NULL};

	LinkedListTreeNodeItem* root = NULL;

	CharEncodingDictionary encodingsDict0 = {.number_of_chars = 0, .charEncoding = NULL};
	CharEncodingDictionary encodingsDict1 = {.number_of_chars = 0, .charEncoding = NULL};
	CharEncodingDictionary encodingsDict2 = {.number_of_chars = 0, .charEncoding = NULL};
	CharEncodingDictionary encodingsDict3 = {.number_of_chars = 0, .charEncoding = NULL};

	EncodingText encodingText0 = {.nr_of_dim = 0, .nr_of_bytes = 0, .nr_of_bits = 0, .dimensions = NULL, .encodedText = NULL};
	EncodingText encodingText1 = {.nr_of_dim = 0, .nr_of_bytes = 0, .nr_of_bits = 0, .dimensions = NULL, .encodedText = NULL};
	EncodingText encodingText2 = {.nr_of_dim = 0, .nr_of_bytes = 0, .nr_of_bits = 0, .dimensions = NULL, .encodedText = NULL};
	EncodingText encodingText3 = {.nr_of_dim = 0, .nr_of_bytes = 0, .nr_of_bits = 0, .dimensions = NULL, .encodedText = NULL};

	for (int i = 0; i < NUM_OF_PROCESSES; i++) {
		printf("Process %d\n", i);	

		// get characters frequencies for the processes' portion of text
		switch (i) {
		case 0:
			total_text_length0 = readFilePortionForProcess(fileName, &text0, i, NUM_OF_PROCESSES);
			getCharFreqsFromText(&acp0, text0, total_text_length0, 0);
			printf("Process %d: %ld characters read in bytes \n", i, total_text_length0);
			break;
		case 1:
			total_text_length1 = readFilePortionForProcess(fileName, &text1, i, NUM_OF_PROCESSES);
			getCharFreqsFromText(&acp1, text1, total_text_length1, 1);
			printf("Process %d: %ld characters read in bytes \n", i, total_text_length1);
			break;
		case 2:
			total_text_length2 = readFilePortionForProcess(fileName, &text2, i, NUM_OF_PROCESSES);
			getCharFreqsFromText(&acp2, text2, total_text_length2, 2);
			printf("Process %d: %ld characters read in bytes \n", i, total_text_length2);
			break;
		case 3:
			total_text_length3 = readFilePortionForProcess(fileName, &text3, i, NUM_OF_PROCESSES);
			getCharFreqsFromText(&acp3, text3, total_text_length3, 3);
			printf("Process %d: %ld characters read in bytes \n", i, total_text_length3);
			break;

		default:
			printf("Process %d unknown\n", i);
		}
	}

	// send CharFreqDictionary to master process
	int bufferSize = 0;

	BYTE *buffer1 = NULL;
	BYTE *buffer2 = NULL;
	BYTE *buffer3 = NULL;

	for (int i = 1; i < NUM_OF_PROCESSES; i++) {
		switch (i) {
		case 1:
			bufferSize = 0;
			buffer1 = getMessage(&acp1, MSG_DICTIONARY, &bufferSize);
			break;
		case 2:
			bufferSize = 0;
			buffer2 = getMessage(&acp2, MSG_DICTIONARY, &bufferSize);
			break;
		case 3:
			bufferSize = 0;
			buffer3 = getMessage(&acp3, MSG_DICTIONARY, &bufferSize);
			break;

		default:
			printf("Process %d unknown\n", i);
		}
	}

	// master process receives all the messages from the slaves
	// and merges the dictionaries
	acp1.number_of_chars = 0;
	freeBuffer(acp1.charFreqs);
	acp2.number_of_chars = 0;
	freeBuffer(acp2.charFreqs);
	acp3.number_of_chars = 0;
	freeBuffer(acp3.charFreqs);

	// printf("\nBefore merge dictionary 0 with %d chars:\n", acp0.number_of_chars);
	// printCharFreqs(&acp0);

	for (int i = 1; i < NUM_OF_PROCESSES; i++) {
		switch (i) {
		case 1:
			setMessage(&acp1, buffer1);
			mergeCharFreqs(&acp0, acp1.charFreqs, acp1.number_of_chars);
			break;
		case 2:
			setMessage(&acp2, buffer2);
			mergeCharFreqs(&acp0, acp2.charFreqs, acp2.number_of_chars);
			break;
		case 3:
			setMessage(&acp3, buffer3);
			mergeCharFreqs(&acp0, acp3.charFreqs, acp3.number_of_chars);
			break;

		default:
			printf("Process %d unknown\n", i);
		}
	}

	freeBuffer(buffer1);
	freeBuffer(buffer2);
	freeBuffer(buffer3);

	freeBuffer(acp1.charFreqs);
	freeBuffer(acp2.charFreqs);
	freeBuffer(acp3.charFreqs);

	// sort the LetterFreqDictionary only in the master process
	sortCharFreqs(&acp0);

	// create the Huffman tree
	root = createHuffmanTree(&acp0);

	encodingsDict0.number_of_chars = acp0.number_of_chars;
	getEncodingFromTree(&encodingsDict0, &acp0, root->item);

	// send the encoding dict to all processes
	for (int i = 1; i < NUM_OF_PROCESSES; i++) {
		switch (i) {
		case 1:
			bufferSize = 0;
			buffer1 = getMessage(&encodingsDict0, MSG_ENCODING_DICTIONARY, &bufferSize);
			break;
		case 2:
			bufferSize = 0;
			buffer2 = getMessage(&encodingsDict0, MSG_ENCODING_DICTIONARY, &bufferSize);
			break;
		case 3:
			bufferSize = 0;
			buffer3 = getMessage(&encodingsDict0, MSG_ENCODING_DICTIONARY, &bufferSize);
			break;

		default:
			printf("Process %d unknown\n", i);
		}
	}

	printf("\nReceived encoding dictionary with %d chars:\n", encodingsDict0.number_of_chars);
	printEncodings(&encodingsDict0);

	printf("\nProcess 0 text:\n\t");
	for (int i = 0; i < strlen(text0) + 1; i++)
		printFormattedChar(text0[i]);
	printf("\n");

	printf("\nProcess 1 text:\n\t");
	for (int i = 0; i < strlen(text1) + 1; i++)
		printFormattedChar(text1[i]);
	printf("\n");

	printf("\nProcess 2 text:\n\t");
	for (int i = 0; i < strlen(text2) + 1; i++)
		printFormattedChar(text2[i]);
	printf("\n");

	printf("\nProcess 3 text:\n\t");
	for (int i = 0; i < strlen(text3) + 1; i++)
		printFormattedChar(text3[i]);
	printf("\n");

	// process 0 encodes its piece of text
	encodeStringToByteArray(&encodingText0, &encodingsDict0, text0, total_text_length0);

	printf("\nProcess 0 encoded text:\n\t");
	// printf("number of bits: %hu\n\t", encodingText0.nr_of_bits);
	// printf("number of bytes: %hu\n\t", encodingText0.nr_of_bytes);
	// printf("number of dimensions: %hu\n", encodingText0.nr_of_dim);
	// for (int i = 0; i < encodingText0.nr_of_dim; i++)
	// 	printf("\tdimension[%d] = %d\n", i, encodingText0.dimensions[i]);
	printEncodedText(encodingText0.encodedText, encodingText0.nr_of_bytes);
	// printf("\n");

	freeBuffer(acp0.charFreqs);

	// processes receive the encoding dict
	for (int i = 1; i < NUM_OF_PROCESSES; i++) {
		switch (i) {
		case 1:
			setMessage(&encodingsDict1, buffer1);
			encodeStringToByteArray(&encodingText1, &encodingsDict1, text1, total_text_length1);

			freeBuffer(buffer1);

			printf("\nProcess 1 encoded text:\n\t");
			// printf("number of bits: %hu\n\t", encodingText1.nr_of_bits);
			// printf("number of bytes: %hu\n\t", encodingText1.nr_of_bytes);
			// printf("number of dimensions: %hu\n", encodingText1.nr_of_dim);
			// for (int i = 0; i < encodingText1.nr_of_dim; i++)
			// 	printf("\tdimension[%d] = %d\n", i, encodingText1.dimensions[i]);
			printEncodedText(encodingText1.encodedText, encodingText1.nr_of_bytes);
			// printf("\n");

			bufferSize = 0;
			buffer1 = getMessage(&encodingText1, MSG_ENCODING_TEXT, &bufferSize);
			
			freeBuffer(encodingText1.dimensions);
			freeBuffer(encodingText1.encodedText);

			for (int i = 0; i < encodingsDict1.number_of_chars; i++)
				freeBuffer(encodingsDict1.charEncoding[i].encoding);

			freeBuffer(encodingsDict1.charEncoding);
			break;
		case 2:
			setMessage(&encodingsDict2, buffer2);
			encodeStringToByteArray(&encodingText2, &encodingsDict2, text2, total_text_length2);

			freeBuffer(buffer2);

			bufferSize = 0;
			buffer2 = getMessage(&encodingText2, MSG_ENCODING_TEXT, &bufferSize);

			printf("\nProcess 2 encoded text:\n\t");
			// printf("number of bits: %hu\n\t", encodingText2.nr_of_bits);
			// printf("number of bytes: %hu\n\t", encodingText2.nr_of_bytes);
			// printf("number of dimensions: %hu\n", encodingText2.nr_of_dim);
			// for (int i = 0; i < encodingText2.nr_of_dim; i++)
			// 	printf("\tdimension[%d] = %d\n", i, encodingText2.dimensions[i]);
			printEncodedText(encodingText2.encodedText, encodingText2.nr_of_bytes);
			// printf("\n");

			freeBuffer(encodingText2.dimensions);
			freeBuffer(encodingText2.encodedText);

			for (int i = 0; i < encodingsDict2.number_of_chars; i++)
				freeBuffer(encodingsDict2.charEncoding[i].encoding);

			freeBuffer(encodingsDict2.charEncoding);
			break;
		case 3:
			setMessage(&encodingsDict3, buffer3);
			encodeStringToByteArray(&encodingText3, &encodingsDict3, text3, total_text_length3);

			freeBuffer(buffer3);

			bufferSize = 0;
			buffer3 = getMessage(&encodingText3, MSG_ENCODING_TEXT, &bufferSize);

			printf("\nProcess 3 encoded text:\n\t");
			// printf("number of bits: %hu\n\t", encodingText3.nr_of_bits);
			// printf("number of bytes: %hu\n\t", encodingText3.nr_of_bytes);
			// printf("number of dimensions: %hu\n", encodingText3.nr_of_dim);
			// for (int i = 0; i < encodingText3.nr_of_dim; i++)
			// 	printf("\tdimension[%d] = %d\n", i, encodingText3.dimensions[i]);
			printEncodedText(encodingText3.encodedText, encodingText3.nr_of_bytes);
			// printf("\n");

			freeBuffer(encodingText3.dimensions);
			freeBuffer(encodingText3.encodedText);

			for (int i = 0; i < encodingsDict3.number_of_chars; i++)
				freeBuffer(encodingsDict3.charEncoding[i].encoding);

			freeBuffer(encodingsDict3.charEncoding);
			break;

		default:
			printf("Process %d unknown\n", i);
		}
	}

	freeBuffer(text0);
	freeBuffer(text1);
	freeBuffer(text2);
	freeBuffer(text3);

	// master process receives the encoded text from each process
	// and writes it to the file

	printf("\nEncoding\n");

	// write an empty header to the file
	FileHeader fileHeader = {.byteStartOfDimensionArray = 0};
	BYTE *startPos = (BYTE*)&fileHeader;
	writeBufferToFile(encodedFileName, startPos, sizeof(unsigned int) * FILE_HEADER_ELEMENTS, WRITE_B, 0);
	printf("Header size: %lu\n", FILE_HEADER_ELEMENTS * sizeof(unsigned int));
	printf("Encoded arrayPosStartPos: %d\n", fileHeader.byteStartOfDimensionArray);

	// write the encoded tree to the file
	int byteSizeOfTree; 
	BYTE* encodedTree = encodeTreeToByteArray(root->item, &byteSizeOfTree);
	writeBufferToFile(encodedFileName, encodedTree, byteSizeOfTree, APPEND_B, 0);
	printf("Encoded tree size: %d\n", getByteSizeOfTree(root->item));

	int nodes = countTreeNodes(root->item);
	printf("Huffman tree nodes number: %d\n", nodes);
	// printHuffmanTree(root->item, 0);

	printf("Process 0 - number of bytes: %d\n", encodingText0.nr_of_bytes);

	// receive the encoded text from each process
	// store in unique buffer
	for (int i = 1; i < NUM_OF_PROCESSES; i++) {
		EncodingText temp = {.nr_of_dim = 0, .nr_of_bytes = 0, .nr_of_bits = 0, .dimensions = NULL, .encodedText = NULL};

		switch (i) {
		case 1:
			setMessage(&temp, buffer1);

			// not sure if this leaves spaces between bytes... probably yes
			// but we may make it work with the block sizes	
			mergeEncodedText(&encodingText0, &temp);
		
			printf("\nProcess 1 - number of bytes: %d\n", temp.nr_of_bytes);

			freeBuffer(temp.dimensions);
			freeBuffer(temp.encodedText);
			freeBuffer(buffer1);
			break;
		case 2:
			setMessage(&temp, buffer2);

			// not sure if this leaves spaces between bytes... probably yes
			// but we may make it work with the block sizes	
			mergeEncodedText(&encodingText0, &temp);

			printf("\nProcess 2 - number of bytes: %d\n", temp.nr_of_bytes);

			freeBuffer(temp.dimensions);
			freeBuffer(temp.encodedText);
			freeBuffer(buffer2);
			break;
		case 3:
			setMessage(&temp, buffer3);

			// not sure if this leaves spaces between bytes... probably yes
			// but we may make it work with the block sizes	
			mergeEncodedText(&encodingText0, &temp);

			printf("\nProcess 3 - number of bytes: %d\n", temp.nr_of_bytes);

			freeBuffer(temp.dimensions);
			freeBuffer(temp.encodedText);
			freeBuffer(buffer3);
			break;

		default:
			printf("Process %d unknown\n", i);
		}
	}

	printf("\nProcess 0 complete encoded text:\n\n");
	printEncodedText(encodingText0.encodedText, encodingText0.nr_of_bytes);

	// write all the encoded text to file
	writeBufferToFile(encodedFileName, encodingText0.encodedText, encodingText0.nr_of_bytes, APPEND_B, 0);
	printf("Encoded text size: %d\n", encodingText0.nr_of_bytes);

	// write the dimensions array to file
	BYTE* dimensions = (BYTE*)encodingText0.dimensions;
	writeBufferToFile(encodedFileName, dimensions, encodingText0.nr_of_dim * sizeof(unsigned short), APPEND_B, 0);
	printf("Dimension array size: %ld\n", encodingText0.nr_of_dim * sizeof(short));

	for (int i = 0; i < encodingText0.nr_of_dim; i++)
		printf("\tdimension[%d] = %d\n", i, encodingText0.dimensions[i]);

	printf("\n");

	// write header to file
    fileHeader.byteStartOfDimensionArray = sizeof(FileHeader) + byteSizeOfTree + encodingText0.nr_of_bytes;
	startPos = (BYTE*)&fileHeader; 
	printf("Total number of blocks: %d\n", encodingText0.nr_of_dim);
    writeBufferToFile(encodedFileName, startPos, sizeof(unsigned int) * FILE_HEADER_ELEMENTS, WRITE_B_AT, 0);

	printf("\nEncoded file size: %d\n", getFileSize(encodedFileName));
	printf("Original file size: %d\n", getFileSize(fileName));

	freeBuffer(encodingText0.dimensions);
	freeBuffer(encodingText0.encodedText);
	freeLinkedList(root);

	clock_t end = clock();

	// calculate elapsed time by finding difference (end - begin) and
	// dividing the difference by CLOCKS_PER_SEC to convert to seconds
	time_spent += (double)(end - begin) / CLOCKS_PER_SEC;

	printf("The elapsed time is %f seconds\n", time_spent);

	return 0;
}
