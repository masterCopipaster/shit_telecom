#ifndef _PROTOCOLS_H
#define _PROTOCOLS_H

#include <ctype.h>
#include <string.h>

template <int SMS_MAX_LEN = 50, int NUMBER_MAX_LEN = 12, int CODED_MAX_LEN = 113, char delimiter = '*'> class sms_dtmf_protocol
{
#define DECODING_TABLE_LEN 11	
#define CHAR_TO_NUM(c) (c - '0')
	public:
	char sms_buf[SMS_MAX_LEN];
	char num_buf[NUMBER_MAX_LEN];
	char coded_buf[CODED_MAX_LEN];
	bool valid = false;
	
	void encode()
	{
		int codedi = strlen(num_buf);
		int decodedi = 0;
		char c, cc, cn;
		strcpy(coded_buf, num_buf);
		coded_buf[codedi++] = '*';
		while((c = sms_buf[decodedi++]) != 0 && decodedi <= SMS_MAX_LEN)
		{
			c = toupper(c);
			for(int i = 0; i < DECODING_TABLE_LEN; i++)
			{
				char* ptr = strchr(decoding_table[i], c);
				if(ptr)
				{
					cc = *decoding_table[i];
					cn = (ptr - decoding_table[i]) + '1';
					coded_buf[codedi++] = cc;
					coded_buf[codedi++] = cn;
					break;
				}
			}
		}
		coded_buf[codedi++] = 0;
	}
	
	void decode()
	{
		char c, cn;
		int codedi = 0;
		int decodedi = 0;
		valid = false;
		
		while((c = coded_buf[codedi++]) != delimiter)
		{
			num_buf[decodedi++] = c;
		}
		num_buf[decodedi++] = 0;
		decodedi = 0;
		
		while((c = coded_buf[codedi++]) != 0 && (cn = coded_buf[codedi++]) != 0 && isdigit(cn))
		{
			valid = false;
			for(int i = 0; i < DECODING_TABLE_LEN; i++)
			{
				if(*(decoding_table[i]) == c)
				{
					int n = CHAR_TO_NUM(cn);
					if(strlen(decoding_table[i]) >= n && n)
					{
						sms_buf[decodedi++] = decoding_table[i][n - 1];
						valid = true;
						break;
					}
				}
			}
			if(!valid) break;
		}
		sms_buf[decodedi++] = 0;
		if(c)
			valid = false;
	}
	private:
	char* decoding_table[DECODING_TABLE_LEN] = {"0", "1_,@", "2ABC", "3DEF", "4GHI", "5JKL", "6MNO", "7PQRS", "8TUV", "9WXYZ", "* "};
};	

#endif 