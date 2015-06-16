/* 
 *  libzhuyin
 *  Library to deal with zhuyin.
 *  
 *  Copyright (C) 2011 Peng Wu <alexepico@gmail.com>
 *  
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 * 
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef PINYIN_PARSER2_H
#define PINYIN_PARSER2_H

#include <glib.h>
#include "novel_types.h"
#include "chewing_key.h"
#include "zhuyin_custom2.h"

namespace zhuyin{

typedef struct {
    const char * m_hanyu_pinyin;
    const char * m_bopomofo;
    const char * m_luoma_pinyin;
    const char * m_secondary_bopomofo;
    ChewingKey   m_chewing_key;
} content_table_item_t;

typedef struct {
    const char * m_pinyin_input;
    guint32      m_flags;
    guint16      m_table_index;
} pinyin_index_item_t;

typedef struct {
    const char * m_chewing_input;
    guint32      m_flags;
    guint16      m_table_index;
} chewing_index_item_t;

typedef struct {
    const char m_input;
    const char * m_chewing;
} chewing_symbol_item_t;

typedef struct {
    const char m_input;
    const char m_tone;
} chewing_tone_item_t;

typedef GArray * ParseValueVector;


/**
 * PhoneticParser2:
 *
 * Parse the ascii string into an array of the struct ChewingKeys.
 *
 */
class PhoneticParser2
{
public:
    /**
     * PhoneticParser2::~PhoneticParser2:
     *
     * The destructor of the PhoneticParser2.
     *
     */
    virtual ~PhoneticParser2() {}

public:
    /**
     * PhoneticParser2::parse_one_key:
     * @options: the pinyin options.
     * @key: the parsed result of struct ChewingKey.
     * @str: the input of the ascii string.
     * @len: the length of the str.
     * @returns: whether the entire string is parsed as one key.
     *
     * Parse only one struct ChewingKey from a string.
     *
     */
    virtual bool parse_one_key(pinyin_option_t options, ChewingKey & key, const char *str, int len) const = 0;

    /**
     * PhoneticParser2::parse:
     * @options: the pinyin options.
     * @keys: the parsed result of struct ChewingKeys.
     * @str: the input of the ascii string.
     * @len: the length of the str.
     * @returns: the number of chars were actually used.
     *
     * Parse the ascii string into an array of struct ChewingKeys.
     *
     */
    virtual int parse(pinyin_option_t options, ChewingKeyVector & keys, ChewingKeyRestVector & key_rests, const char *str, int len) const = 0;

};


/**
 * FullPinyinParser2:
 *
 * Parses the full pinyin string into an array of struct ChewingKeys.
 *
 */
class FullPinyinParser2 : public PhoneticParser2
{
protected:
    /* Note: some internal pointers to full pinyin table. */
    const pinyin_index_item_t * m_pinyin_index;
    size_t m_pinyin_index_len;

protected:
    ParseValueVector m_parse_steps;

    int final_step(size_t step_len, ChewingKeyVector & keys,
                   ChewingKeyRestVector & key_rests) const;

public:
    FullPinyinParser2();
    virtual ~FullPinyinParser2() {
        g_array_free(m_parse_steps, TRUE);
    }

    virtual bool parse_one_key(pinyin_option_t options, ChewingKey & key, const char *str, int len) const;

    /* Note:
     *   the parse method will use dynamic programming to drive parse_one_key.
     */
    virtual int parse(pinyin_option_t options, ChewingKeyVector & keys, ChewingKeyRestVector & key_rests, const char *str, int len) const;

public:
    bool set_scheme(ZhuyinScheme scheme);
};

/**
 * ChewingParser2:
 *
 * Parse the chewing input string into an array of struct ChewingKeys.
 *
 */
class ChewingParser2 : public PhoneticParser2
{
public:
    virtual ~ChewingParser2() {}

public:
    /**
     * ChewingParser2::in_chewing_scheme:
     * @options: the pinyin options.
     * @key: the user input ascii character.
     * @symbol: the corresponding chewing symbol.
     * @returns: whether the character is in the chewing scheme.
     *
     * Check whether the input character is in the chewing keyboard mapping.
     *
     */
    virtual bool in_chewing_scheme(pinyin_option_t options, const char key, gchar ** & symbols) const = 0;
};


 /**
 * ChewingSimpleParser2:
 *
 * Parse the chewing string into an array of struct ChewingKeys.
 *
 * Several keyboard scheme are supported:
 * * CHEWING_STANDARD  Standard ZhuYin keyboard, which maps 1 to Bo(ㄅ), q to Po(ㄆ) etc.
 * * CHEWING_IBM       IBM ZhuYin keyboard, which maps 1 to Bo(ㄅ), 2 to Po(ㄆ) etc.
 * * CHEWING_GINYIEH   Gin-Yieh ZhuYin keyboard.
 * * CHEWING_ETEN      Eten (倚天) ZhuYin keyboard.
 * * CHEWING_STANDARD_DVORAK      Standard Dvorak ZhuYin keyboard
 *
 */

class ChewingSimpleParser2 : public ChewingParser2
{
    /* internal options for chewing parsing. */
    pinyin_option_t m_options;

    /* Note: some internal pointers to chewing scheme table. */
protected:
    const chewing_symbol_item_t * m_symbol_table;
    const chewing_tone_item_t   * m_tone_table;

public:
    ChewingSimpleParser2() {
        m_symbol_table = NULL; m_tone_table = NULL;
        set_scheme(CHEWING_DEFAULT);
    }

    virtual ~ChewingSimpleParser2() {}

    virtual bool parse_one_key(pinyin_option_t options, ChewingKey & key, const char *str, int len) const;

    virtual int parse(pinyin_option_t options, ChewingKeyVector & keys, ChewingKeyRestVector & key_rests, const char *str, int len) const;

public:
    bool set_scheme(ZhuyinScheme scheme);
    virtual bool in_chewing_scheme(pinyin_option_t options, const char key, gchar ** & symbols) const;
};


/**
 * ChewingDiscreteParser2:
 *
 * Parse the chewing string into an array of struct ChewingKeys.
 *
 * Initially will support HSU, HSU Dvorak and ETEN26.
 *
 */

class ChewingDiscreteParser2 : public ChewingParser2
{
protected:
    /* internal options for chewing parsing. */
    pinyin_option_t m_options;

    /* some internal pointers to chewing scheme table. */
    const chewing_index_item_t * m_chewing_index;
    size_t m_chewing_index_len;
    const chewing_symbol_item_t * m_initial_table;
    const chewing_symbol_item_t * m_middle_table;
    const chewing_symbol_item_t * m_final_table;
    const chewing_tone_item_t   * m_tone_table;

public:
    ChewingDiscreteParser2() {
        m_options = 0;
        m_chewing_index = NULL; m_chewing_index_len = 0;
        m_initial_table = NULL; m_middle_table = NULL;
        m_final_table   = NULL; m_tone_table = NULL;
        set_scheme(CHEWING_HSU);
    }

    virtual ~ChewingDiscreteParser2() {}

    virtual bool parse_one_key(pinyin_option_t options, ChewingKey & key, const char *str, int len) const;

    virtual int parse(pinyin_option_t options, ChewingKeyVector & keys, ChewingKeyRestVector & key_rests, const char *str, int len) const;

public:
    bool set_scheme(ZhuyinScheme scheme);
    virtual bool in_chewing_scheme(pinyin_option_t options, const char key, gchar ** & symbols) const;
};


class ChewingDaChenCP26Parser2 : public ChewingParser2
{
    /* some internal pointers to chewing scheme table. */
    const chewing_index_item_t * m_chewing_index;
    size_t m_chewing_index_len;
    const chewing_symbol_item_t * m_initial_table;
    const chewing_symbol_item_t * m_middle_table;
    const chewing_symbol_item_t * m_final_table;
    const chewing_tone_item_t   * m_tone_table;

public:
    ChewingDaChenCP26Parser2();

    virtual ~ChewingDaChenCP26Parser2() {}

    virtual bool parse_one_key(pinyin_option_t options, ChewingKey & key, const char *str, int len) const;

    virtual int parse(pinyin_option_t options, ChewingKeyVector & keys, ChewingKeyRestVector & key_rests, const char *str, int len) const;

public:
    virtual bool in_chewing_scheme(pinyin_option_t options, const char key, gchar ** & symbols) const;
};


/* Direct Parser for Chewing table load. */
class ChewingDirectParser2 : public PhoneticParser2
{
    const chewing_index_item_t * m_chewing_index;
    size_t m_chewing_index_len;

public:
    ChewingDirectParser2();

    virtual ~ChewingDirectParser2() {}

    virtual bool parse_one_key(pinyin_option_t options, ChewingKey & key, const char *str, int len) const;

    virtual int parse(pinyin_option_t options, ChewingKeyVector & keys, ChewingKeyRestVector & key_rests, const char *str, int len) const;
};

/* compare pinyins with chewing internal representations. */
inline int pinyin_compare_initial2(pinyin_option_t options,
                                   ChewingInitial lhs,
                                   ChewingInitial rhs) {
    if (lhs == rhs)
        return 0;

    if ((options & ZHUYIN_AMB_C_CH) &&
        ((lhs == CHEWING_C && rhs == CHEWING_CH) ||
         (lhs == CHEWING_CH && rhs == CHEWING_C)))
        return 0;

    if ((options & ZHUYIN_AMB_S_SH) &&
        ((lhs == CHEWING_S && rhs == CHEWING_SH) ||
         (lhs == CHEWING_SH && rhs == CHEWING_S)))
        return 0;

    if ((options & ZHUYIN_AMB_Z_ZH) &&
        ((lhs == CHEWING_Z && rhs == CHEWING_ZH) ||
         (lhs == CHEWING_ZH && rhs == CHEWING_Z)))
        return 0;

    if ((options & ZHUYIN_AMB_F_H) &&
        ((lhs == CHEWING_F && rhs == CHEWING_H) ||
         (lhs == CHEWING_H && rhs == CHEWING_F)))
        return 0;

    if ((options & ZHUYIN_AMB_L_N) &&
        ((lhs == CHEWING_L && rhs == CHEWING_N) ||
         (lhs == CHEWING_N && rhs == CHEWING_L)))
        return 0;

    if ((options & ZHUYIN_AMB_L_R) &&
        ((lhs == CHEWING_L && rhs == CHEWING_R) ||
         (lhs == CHEWING_R && rhs == CHEWING_L)))
        return 0;

    if ((options & ZHUYIN_AMB_G_K) &&
        ((lhs == CHEWING_G && rhs == CHEWING_K) ||
         (lhs == CHEWING_K && rhs == CHEWING_G)))
        return 0;

    return (lhs - rhs);
}


inline int pinyin_compare_middle_and_final2(pinyin_option_t options,
                                            ChewingMiddle middle_lhs,
                                            ChewingMiddle middle_rhs,
                                            ChewingFinal final_lhs,
                                            ChewingFinal final_rhs) {
    if (middle_lhs == middle_rhs && final_lhs == final_rhs)
        return 0;

    /* both pinyin and chewing incomplete options will enable this. */
    if (options & (PINYIN_INCOMPLETE | CHEWING_INCOMPLETE)) {
        if (middle_lhs == CHEWING_ZERO_MIDDLE &&
            final_lhs == CHEWING_ZERO_FINAL)
            return 0;
        if (middle_rhs == CHEWING_ZERO_MIDDLE &&
            final_rhs == CHEWING_ZERO_FINAL)
            return 0;
    }

    /* compare chewing middle first. */
    int middle_diff = middle_lhs - middle_rhs;
    if (middle_diff)
        return middle_diff;

    if ((options & ZHUYIN_AMB_AN_ANG) &&
        ((final_lhs == CHEWING_AN && final_rhs == CHEWING_ANG) ||
         (final_lhs == CHEWING_ANG && final_rhs == CHEWING_AN)))
        return 0;

    if ((options & ZHUYIN_AMB_EN_ENG) &&
        ((final_lhs == CHEWING_EN && final_rhs == CHEWING_ENG) ||
         (final_lhs == CHEWING_ENG && final_rhs == CHEWING_EN)))
        return 0;

    if ((options & ZHUYIN_AMB_IN_ING) &&
        ((final_lhs == PINYIN_IN && final_rhs == PINYIN_ING) ||
         (final_lhs == PINYIN_ING && final_rhs == PINYIN_IN)))
        return 0;

    return (final_lhs - final_rhs);
}


inline int pinyin_compare_tone2(pinyin_option_t options,
                                ChewingTone lhs,
                                ChewingTone rhs) {
#if 0
    if (lhs == rhs)
        return 0;
#endif
    if (options & FORCE_TONE)
        return (lhs - rhs);
    if (lhs == CHEWING_ZERO_TONE)
        return 0;
    if (rhs == CHEWING_ZERO_TONE)
        return 0;
    return (lhs - rhs);
}


};

#endif
