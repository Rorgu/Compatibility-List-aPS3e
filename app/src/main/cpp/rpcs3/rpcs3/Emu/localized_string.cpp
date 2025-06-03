#include "stdafx.h"
#include "localized_string.h"
#include "System.h"

std::string get_localized_string(localized_string_id id, const char* args)
{
	return Emu.GetCallbacks().get_localized_string(id, args);
}

/*std::u32string get_localized_u32string(localized_string_id id, const char* args)
{
    const std::string utf8_str=Emu.GetCallbacks().get_localized_string(id, args);
    std::u32string result;
    size_t i = 0;
    while (i < utf8_str.size()) {
        uint32_t codepoint = 0;
        unsigned char c = utf8_str[i++];
        if (c <= 0x7F) {  // 1-byte sequence (0xxxxxxx)
            codepoint = c;
        } else if ((c & 0xE0) == 0xC0) {  // 2-byte sequence (110xxxxx)
            codepoint = (c & 0x1F) << 6;
            codepoint |= (utf8_str[i++] & 0x3F);
        } else if ((c & 0xF0) == 0xE0) {  // 3-byte sequence (1110xxxx)
            codepoint = (c & 0x0F) << 12;
            codepoint |= (utf8_str[i++] & 0x3F) << 6;
            codepoint |= (utf8_str[i++] & 0x3F);
        } else if ((c & 0xF8) == 0xF0) {  // 4-byte sequence (11110xxx)
            codepoint = (c & 0x07) << 18;
            codepoint |= (utf8_str[i++] & 0x3F) << 12;
            codepoint |= (utf8_str[i++] & 0x3F) << 6;
            codepoint |= (utf8_str[i++] & 0x3F);
        } else {
            // 非法 UTF-8 序列（可选：抛出异常或替换为 U+FFFD）
            codepoint = 0xFFFD;
        }
        result.push_back(codepoint);
    }
    return result;

	//return Emu.GetCallbacks().get_localized_u32string(id, args);
}*/
