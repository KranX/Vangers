#include "rle.h"

#include <climits>
#include <cstddef>
#include <memory>
#include <stdexcept>

int RLE_ANALISE(uchar *_buf, int len, uchar *&out) {
	out = nullptr;
	if (!_buf || len <= 0)
		return 0;

	// Each packet needs at most two encoded bytes per input byte.
	std::unique_ptr<uchar[]> packed(new uchar[static_cast<std::size_t>(len) * 2]);
	std::size_t pack_len = 0;
	int i = 0;
	while (i < len) {
		const int start = i++;
		while (i < len && i - start < 128 && _buf[i] == _buf[start])
			++i;

		// The low seven bits store count - 1; bit 7 marks a literal block.
		if (i - start > 1) {
			packed[pack_len++] = static_cast<uchar>(i - start - 1);
			packed[pack_len++] = _buf[start];
		} else {
			// Stop before the next repeated pair, or include the final singleton.
			while (i < len && i - start < 128 && (i + 1 == len || _buf[i] != _buf[i + 1]))
				++i;
			const int count = i - start;
			packed[pack_len++] = static_cast<uchar>(128 + count - 1);
			memcpy(packed.get() + pack_len, _buf + start, count);
			pack_len += count;
		}
	}

	if (pack_len > static_cast<std::size_t>(INT_MAX))
		throw std::length_error("RLE encoded data is too large");
	out = new uchar[pack_len];
	memcpy(out, packed.get(), pack_len);

	return static_cast<int>(pack_len);
}

void RLE_UNCODE(uchar *_buf, int len, uchar *out) {
	uchar *buf = _buf;
	uchar c_len = 0;
	uchar *p = out;

	int i = 0;
	while (i < len) {
		c_len = *p++;

		if (c_len & 128) {
			c_len ^= 128;
			memcpy(buf, p, ++c_len);

			i += c_len;
			p += c_len;
			buf += c_len;
		} else {
			memset(buf, *p++, ++c_len);
			i += c_len;
			buf += c_len;
		} //  end if
	} //  end while
}
