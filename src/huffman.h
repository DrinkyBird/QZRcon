// Version 5, released 5/13/2009. Compatible with Zandronum/Skulltag launchers and servers.
// Modified by Korshun to be const-correct.
// Also added Qt wrappers here.

#ifndef __HUFFMAN_H__
#define __HUFFMAN_H__

#include "common.h"

//*****************************************************************************
//	STRUCTURES

typedef struct huffnode_s
{
	struct huffnode_s *zero;
	struct huffnode_s *one;
	unsigned char val;
	float freq;

} huffnode_t;

typedef struct
{
	unsigned int bits;
	int len;

} hufftab_t;

//*****************************************************************************
//	PROTOTYPES

void HUFFMAN_Construct( void );
void HUFFMAN_Destruct( void );

void HUFFMAN_Encode(const unsigned char *in, unsigned char *out, int inlen, int *outlen );
void HUFFMAN_Decode(const unsigned char *in, unsigned char *out, int inlen, int *outlen );

#ifdef _DEBUG
void huffman_ZeroFreq( void );
#endif

//*****************************************************************************
//	QT WRAPPERS

QByteArray huffmanEncode(QByteArray data);
QByteArray huffmanDecode(QByteArray data);


#endif // __HUFFMAN_H__
