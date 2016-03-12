#include "rle.h"

//XStream fRLE("anal", XS_OUT);

/*static int G_len = 0;
static int G_all_len = 0;*/

int RLE_ANALISE(uchar* _buf, int len, uchar*& out){
	int i = 0;
	int pack_len = 0;
	uchar c_len = 0;
	uchar* buf = _buf;
	uchar *_out = new uchar[len*2];
	uchar* p = _out;
	uchar _ch = *buf++;

	while( i < len) {
		while( (i < len) && (_ch == *buf) &&  (c_len < 127)) {
			c_len++;
			buf++;
			i++;
		}

		if (c_len){
			*p++ = c_len;
			*p++ = _ch;

			_ch = *buf++;

			pack_len += 2;
			c_len = 0;
			i++;
		}

		while( (i < len) && (_ch != *buf) && (c_len < 127)) {
			c_len++;
			_ch = *buf++;
			i++;
		}

		if (c_len){
			*p++ = 128 + (c_len-1);
			memcpy( p, buf - c_len - 1, c_len);
			p += c_len;
			pack_len += c_len + 1;
			c_len = 0;
		}
	}//  end while

	/*G_len += pack_len;
	G_all_len += len;

	fRLE <= G_len < "  ==>  " <= G_all_len < "\n";
	fRLE <= pack_len < "  ==>  " <= len < "\n";
	if ( where ) fRLE < "\n";*/

	out = new uchar[pack_len];
	memcpy( out, _out, pack_len);
	/*RLE_UNCODE( _out, len,  out);

	for( i = 0; i < len; i++)
		if (_out[i] != _buf[i] )
			ErrH.Abort("'Bad restore");*/

	delete[] _out;
	
	return pack_len;
}			

void RLE_UNCODE(uchar* _buf, int len, uchar* out){
	uchar* buf = _buf;
	uchar c_len = 0;
	uchar* p = out;

	int i = 0;
	while( i < len){
		c_len = *p++;

		if (c_len & 128){
			c_len ^= 128;
			memcpy(	buf, p, ++c_len);

			i += c_len;
			p += c_len;
			buf += c_len;
		} else {
			memset(buf, *p++, ++c_len);
			i += c_len;
			buf += c_len;
		}//  end if
	}//  end while
}
