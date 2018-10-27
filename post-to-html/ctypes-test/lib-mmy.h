/*
    lib-mmy.h

    000. 
    (a) Typedefs
    (b) Useful macros
    (c) Assert/Debug macros
    (d) Logging macros

    001.
    (a) void xmemset(unsigned char *ptr, unsigned char value, u64 size)
    (b) void xmemcpy(unsigned char *dst, unsigned char *src, u64 size)
    (c) void* xmalloc(size_t num_bytes)
    (d) void* xcalloc(size_t nitems, size_t num_bytes)
    (e) void* xrealloc(void *ptr, size_t num_bytes)

    002. 
    Copied from https://github.com/nothings/stb/ (public domain). 
    Uses Meresenne Twister and LCG to seed. Changed so automatically 
    seeded with time(NULL) if srand() hasn't been called.
    (a) seeds the random number generator. 
    (b) returns a random number between 0 and ULONG_MAX. 
    (c) returns a random number between 0 and 1. 
    (a) unsigned long stb_srand(unsigned long seed)
    (b) unsigned long stb_rand()
    (c) double stb_frand()

    003.
    Math operations. a,b use intrinsics. c,d,e: 
    https://graphics.stanford.edu/%7Eseander/bithacks.html
    (a) float mth_sqrt(float input)
    (b) double mth_sqrt(double input)
    (c) int mth_min(int a, int b)
    (d) int mth_max(int a, int b)
    (e) int mth_abs(int a)
    (f) int mth_pow(int num, int pow)
    
    004. 
    ANSI string operations.
    (a) int str_len(char* str)
    (b) int str_equal(char* a, char* b)
    (c) char* str_copy(char *s)
    (d) int str_beginswith(char* str, char* start)
    (e) int str_endswith(char* str, char* end)
    (f) char* str_concat(char* str, char* addition)
    (g) void str_lower(char* str)
    (h) void str_upper(char* str)
    (i) int str_isalpha(char* str)
    (j) int str_isint(char* str)
    (k) char* str_lstrip(char* str, char tostrip)
    (l) char* str_rstrip(char* str, char tostrip)
    (m) char* str_strip(char* str, char* tostrip)
    (n) void str_sort(char* str)
    (o) char** str_split(char* str, char c, int* size)
    (p) int str_toint(char* str)
    (q) char* str_inttostr(int num)
    
    005.
    Dynamic array. 
    Copied from https://github.com/pervognsen/bitwise/blob/master/ion/common.c (public domain)
    
    006.
    Hash table.

    007.
    Memory debug stuff.

*/

// 000. START 
#if 1

#include <stdio.h>
#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8; 
typedef int16_t s16; 
typedef int32_t s32; 
typedef int64_t s64; 

typedef float f32;
typedef double f64;

#define kilobytes(value) ((value)*(u64)1024)
#define megabytes(value) (kilobytes(value)*1024)
#define gigabytes(value) (megabytes(value)*1024)
#define terabytes(value) (gigabytes(value)*1024)

#ifdef TEST
#define dbg(msg, ...) fprintf(stderr, "[DEBUG] (%s:%d) " msg "\n", \
        __FILE__, __LINE__, ##__VA_ARGS__)
int shouldAssert = 0;
int assertFired = 0;
#define assert(expr) if(!(expr)) { \
        assertFired = 1; \
        if(!shouldAssert) { dbg("Assert failed: " #expr); } }
#define shouldAssert(expr) shouldAssert = 1; assertFired = 0; \
        assert(expr) \
        shouldAssert = 0; \
        if(!assertFired) { log_err("Assert didn't fail: " #expr); }
#elif DEBUG
#define dbg(msg, ...) fprintf(stderr, "[DEBUG] (%s:%d) " msg "\n", \
        __FILE__, __LINE__, ##__VA_ARGS__)
#define assert(expr) if(!(expr)) { dbg("Assert failed: " #expr); *(int*)0 = 0; }
#define shouldAssert(expr) log_warn("shouldAssert should only be used for testing?");
#else 
#define dbg(msg, ...)
#define assert(expr)
#define shouldAssert(expr) log_warn("shouldAssert should only be used for testing?");
#endif

#define log_err(msg, ...) fprintf(stderr, "[ERROR] (%s:%d) " msg "\n", \
        __FILE__, __LINE__, ##__VA_ARGS__) 
#define log_warn(msg, ...) fprintf(stderr, "[WARN] (%s:%d) " msg "\n", \
        __FILE__, __LINE__, ##__VA_ARGS__)
#define log_info(msg, ...) fprintf(stderr, "[INFO] (%s:%d) " msg "\n", \
        __FILE__, __LINE__, ##__VA_ARGS__)

#endif
// 000. END

// 001. START
#if 1
#include <stdlib.h> // for malloc, realloc, calloc
void xmemset(unsigned char *ptr, unsigned char value, u64 size) {
    for(u64 i = 0; i < size; i++) {
        *ptr = value;
        ptr++;
    }
}

void xmemcpy(unsigned char *dst, unsigned char *src, u64 size) {
    while(size > 0) {
        *dst = *src;
        src++, dst++;
        size--;
    }
}

void *xmalloc(size_t num_bytes) {
    void *ptr = malloc(num_bytes); 
    if (!ptr) {
        perror("xmalloc failed");
        exit(1);
    }
    return ptr;
}

void *xcalloc(size_t nitems, size_t num_bytes) {
    void *ptr = calloc(nitems, num_bytes);
    if (!ptr) {
        perror("xcalloc failed");
        exit(1);
    }
    return ptr;
}

void *xrealloc(void *ptr, size_t num_bytes) {
    ptr = realloc(ptr, num_bytes);
    if (!ptr) {
        perror("xrealloc failed");
        exit(1);
    }
    return ptr;
}
#endif
// 001. END

// 002. START
#if 1
#include <time.h> // for time()
#include <limits.h> // for ULONG_MAX

typedef struct { char d[4]; } stb__4;
typedef struct { char d[8]; } stb__8;

// optimize the small cases, though you shouldn't be calling this for those!
void stb_swap(void *p, void *q, size_t sz)
{
    char buffer[256];
    if (p == q) return;
    if (sz == 4) {
        stb__4 temp    = * ( stb__4 *) p;
        * (stb__4 *) p = * ( stb__4 *) q;
        * (stb__4 *) q = temp;
        return;
    } else if (sz == 8) {
        stb__8 temp    = * ( stb__8 *) p;
        * (stb__8 *) p = * ( stb__8 *) q;
        * (stb__8 *) q = temp;
        return;
    }

    while (sz > sizeof(buffer)) {
        stb_swap(p, q, sizeof(buffer));
        p = (char *) p + sizeof(buffer);
        q = (char *) q + sizeof(buffer);
        sz -= sizeof(buffer);
    }

    xmemcpy(buffer, p     , sz);
    xmemcpy(p     , q     , sz);
    xmemcpy(q     , buffer, sz);
}

static unsigned long stb__rand_seed=0;

unsigned long stb_srandLCG(unsigned long seed)
{
    unsigned long previous = stb__rand_seed;
    stb__rand_seed = seed;
    return previous;
}

unsigned long stb_randLCG(void)
{
    stb__rand_seed = stb__rand_seed * 2147001325 + 715136305; // BCPL generator
    // shuffle non-random bits to the middle, and xor to decorrelate with seed
    return 0x31415926 ^ ((stb__rand_seed >> 16) + (stb__rand_seed << 16));
}

void stb_shuffle(void *p, size_t n, size_t sz, unsigned long seed)
{
    char *a;
    unsigned long old_seed;
    int i;
    if (seed)
        old_seed = stb_srandLCG(seed);
    a = (char *) p + (n-1) * sz;

    for (i=n; i > 1; --i) {
        int j = stb_randLCG() % i;
        stb_swap(a, (char *) p + j * sz, sz);
        a -= sz;
    }
    if (seed)
        stb_srandLCG(old_seed);
}

void stb_reverse(void *p, size_t n, size_t sz)
{
    int i,j = n-1;
    for (i=0; i < j; ++i,--j) {
        stb_swap((char *) p + i * sz, (char *) p + j * sz, sz);
    }
}

// public domain Mersenne Twister by Michael Brundage
#define STB__MT_LEN       624

int stb__mt_index = STB__MT_LEN*sizeof(unsigned long)+1;
unsigned long stb__mt_buffer[STB__MT_LEN];

int srandcalled = 0;

void stb_srand(unsigned long seed)
{
    srandcalled = 1;

    int i;
    unsigned long old = stb_srandLCG(seed);
    for (i = 0; i < STB__MT_LEN; i++)
        stb__mt_buffer[i] = stb_randLCG();
    stb_srandLCG(old);
    stb__mt_index = STB__MT_LEN*sizeof(unsigned long);
}

#define STB__MT_IA           397
#define STB__MT_IB           (STB__MT_LEN - STB__MT_IA)
#define STB__UPPER_MASK      0x80000000
#define STB__LOWER_MASK      0x7FFFFFFF
#define STB__MATRIX_A        0x9908B0DF
#define STB__TWIST(b,i,j)    ((b)[i] & STB__UPPER_MASK) | ((b)[j] & STB__LOWER_MASK)
#define STB__MAGIC(s)        (((s)&1)*STB__MATRIX_A)

unsigned long stb_rand()
{
    if(!srandcalled) stb_srand(time(NULL));

    unsigned long * b = stb__mt_buffer;
    int idx = stb__mt_index;
    unsigned long s,r;
    int i;

    if (idx >= STB__MT_LEN*sizeof(unsigned long)) {
        if (idx > STB__MT_LEN*sizeof(unsigned long))
            stb_srand(0);
        idx = 0;
        i = 0;
        for (; i < STB__MT_IB; i++) {
            s = STB__TWIST(b, i, i+1);
            b[i] = b[i + STB__MT_IA] ^ (s >> 1) ^ STB__MAGIC(s);
        }
        for (; i < STB__MT_LEN-1; i++) {
            s = STB__TWIST(b, i, i+1);
            b[i] = b[i - STB__MT_IB] ^ (s >> 1) ^ STB__MAGIC(s);
        }

        s = STB__TWIST(b, STB__MT_LEN-1, 0);
        b[STB__MT_LEN-1] = b[STB__MT_IA-1] ^ (s >> 1) ^ STB__MAGIC(s);
    }
    stb__mt_index = idx + sizeof(unsigned long);

    r = *(unsigned long *)((unsigned char *)b + idx);

    r ^= (r >> 11);
    r ^= (r << 7) & 0x9D2C5680;
    r ^= (r << 15) & 0xEFC60000;
    r ^= (r >> 18);

    return r;
}

double stb_frand(void)
{
    return (double) stb_rand() / ((double) ULONG_MAX);
    //return (double) stb_rand() / ((double) (1 << 16) * (1 << 16)); // NOTE: This stopped working?
}
#endif
// 002. END

// 003. START
#if 1

#include <emmintrin.h>

//float mth_sqrt(float input) {
//    float result = 0.0f;
//    __m128 one = _mm_set_ss(input);
//    __m128 two = _mm_sqrt_ss(one); // SSE
//    result = _mm_cvtss_f32(two);
//
//    return result;
//}

double mth_sqrt(double input) {
    double result = 0.0f;
    __m128d one = _mm_set_sd(input);
    __m128d two = _mm_sqrt_pd(one); // SSE2
    result = _mm_cvtsd_f64(two);

    return result;
}

int mth_min(int a, int b) {
    int result = b ^ ((a ^ b) & -(a < b));

    return result;
}

int mth_max(int a, int b) {
    int result = a ^ ((a ^ b) & -(a < b));

    return result;
}

int mth_abs(int a) {
    unsigned int result;
    int const mask = a >> sizeof(int) * 8 - 1;
    result = (a + mask) ^ mask;

    return result;
}

int mth_pow(int num, int pow) {
    int result = 1;
    for(int i = 0; i < pow; i++) {
        result *= num;
    }
    return result;
}

#endif
// 003. END

// 004. START
#if 1
int str_len(char *str) {
    char* ptr = str;
    while(*ptr != '\0')
        ptr++;
    return ptr - str;
}

int str_equal(char *a, char *b) {
    while((*a != '\0') && (*b != '\0') && (*a == *b)) {
        a++, b++;
    }
    return ((*a == '\0') && (*b == '\0'));
}

char* str_copy(char *s) {
    char* copy = (char*)xmalloc(sizeof(char)*(str_len(s)+1));
    char* copyPtr = copy;
    while(*s != '\0') {
        *copyPtr = *s;
        s++, copyPtr++;
    }
    *copyPtr = '\0';
    return copy;
}

int str_beginswith(char* str, char* start) {
    assert(str_len(start) <= str_len(str));
    while((*start != '\0') && (*start == *str)) {
        start++, str++;
    }
    return *start == '\0';
}

int str_endswith(char* str, char* end) {
    int endLength = str_len(end);
    assert(endLength <= str_len(str));
    while(*end != '\0') { end++; }
    while(*str != '\0') { str++; }

    while(*str == *end && endLength > 0) {
        str--, end--;
        endLength--;
    }
    return *str == *end;
}

char* str_concat(char* str, char* addition) {
    int newLength = str_len(str) + str_len(addition) + 1;
    str = (char*)xrealloc(str, sizeof(char) * newLength);
    char* strPtr = str;
    while(*strPtr != '\0') { strPtr++; }
    while(*addition != '\0') {
        *strPtr = *addition;
        strPtr++, addition++;
    }
    *strPtr = '\0';
    return str;
}

void str_lower(char* str) {
    while(*str != '\0') {
        if(*str >= 'A' && *str <= 'Z') {
            *str += 'a' - 'A';
        }
        str++;
    }
}

void str_upper(char* str) {
    while(*str != '\0') {
        if(*str >= 'a' && *str <= 'z') {
            *str -= 'a' - 'A';
        }
        str++;
    }
}

int str_isalpha(char* str) {
    while(*str != '\0') {
        if((*str < 'a' || *str > 'z') && (*str < 'A' || *str > 'Z')) {
            return 0;
        }
        str++;
    }
    return 1;
}

int str_isint(char* str) {
    if(*str == '-') {
        str++;
    }
    if(*str == '0' || *str == '\0') return 0; // shouldn't begin with 0 or have no digits
    while(*str != '\0') {
        if(*str < '0' || *str > '9') {
            return 0;
        }
        str++;
    }
    return 1;
}

char* str_lstrip(char* str, char tostrip) {
    assert(tostrip != '\0');
    while(*str == tostrip) {
        str++;
    }
    return str;
}

char* str_rstrip(char* str, char tostrip) {
    assert(tostrip != '\0');
    if(*str == '\0') return str;
    char* strPtr = str;
    while(*strPtr != '\0') {
        strPtr++;
    }
    strPtr--;
    while(*strPtr == tostrip) {
        *strPtr = '\0';
        strPtr--;
    }
    return str;
}

char* str_strip(char* str, char* tostrip) {
    while(1) {
        int length = str_len(str);
        for(int i = 0; i < str_len(tostrip); i++) {
            str = str_lstrip(str, tostrip[i]);
            str = str_rstrip(str, tostrip[i]);
        }
        if(length == str_len(str)) return str;
    }
}

void str_sort(char* str) {
    int len = str_len(str);
    for(int i = 0; i < len; i++) {
        for(int j = i+1; j < len; j++) {
            if(str[i] > str[j]) {
                char temp = str[i];
                str[i] = str[j];
                str[j] = temp;
            }
        }
    }
}

char** str_split(char* str, char c, int* size) {
    int numStrs = 1;
    char* strPtr = str;
    while(*strPtr != '\0') {
        if(*strPtr == c) {
            *strPtr = '\0';
            numStrs++;
        }
        strPtr++;
    }
    char** result = (char**)xmalloc(sizeof(char*)*numStrs);
    strPtr = str;
    int i = 0;
    while(numStrs > 0) {
        if(*strPtr == '\0') {
            numStrs--;
            result[i] = str;
            str = strPtr + 1;
            i++;
        }
        strPtr++;
    }
    *size = i;
    return result;
}

int str_toint(char *str) {
    int result = 0;
    char *strPtr = str;

    int length = str_len(str);

    while (length > 0) {
        assert((*strPtr >= '0' && *strPtr <= '9') || (length == str_len(str) && *strPtr == '-'));
        length--;
        if(*strPtr == '-') { strPtr++; continue; }

        // Calculate value based on position (i.e. value * 10^position)
        int exponent = 1;
        for (int i = 0; i < length; i++) {
            exponent *= 10;
        }
        result += (*strPtr - 48) * exponent;
        strPtr++;
    }
    if (str[0] == '-') { result = -result; }
    return result;
}

char* str_inttostr(int num) {
    int negative = 0;
    if (num < 0) {
        negative = 1;
        num = -num;
    }

    int len = 1;
    int temp = num;
    while (temp >= 10) {
        len++;
        temp /= 10;
    }

    char *res = (char *)xmalloc(sizeof(char) *
            (len + negative + 1)); // len + '-' + '\0'

    char *resPtr = res;
    if (negative) {
        *resPtr = '-';
        resPtr++;
    }
    while (len > 0) {
        int mod = 1;
        int templen = len;
        while (templen > 0) {
            mod *= 10;
            templen--;
        }
        *resPtr = '0' + ((num % mod) / (mod / 10)); // isolate wanted digit
        resPtr++;
        len--;
    }

    *resPtr = '\0';
    return res;
}
#endif
// 004. END

#if 1
// 005. START
#define offsetof(st, m) ((size_t)&(((st *)0)->m))
#define CLAMP_MIN(x, min) mth_max(x, min)

typedef struct ArrHdr {
    size_t len;
    size_t cap;
    char buf[];
} ArrHdr;

#define arr__hdr(b) ((ArrHdr *)((char *)(b) - offsetof(ArrHdr, buf)))

#define arr_len(b) ((b) ? arr__hdr(b)->len : 0)
#define arr_cap(b) ((b) ? arr__hdr(b)->cap : 0)
#define arr_end(b) ((b) + arr_len(b))
#define arr_sizeof(b) ((b) ? arr_len(b)*sizeof(*b) : 0)

#define arr_free(b) ((b) ? (free(arr__hdr(b)), (b) = NULL) : 0)
#define arr_fit(b, n) ((n) <= arr_cap(b) ? 0 : ((b) = arr__grow((b), (n), sizeof(*(b)))))
#define arr_push(b, ...) (arr_fit((b), 1 + arr_len(b)), (b)[arr__hdr(b)->len++] = (__VA_ARGS__))
#define arr_clear(b) ((b) ? arr__hdr(b)->len = 0 : 0)

void *arr__grow(const void *buf, size_t new_len, size_t elem_size) {
    assert(arr_cap(buf) <= (SIZE_MAX - 1)/2);
    size_t new_cap = CLAMP_MIN(2*arr_cap(buf), mth_max(new_len, 16));
    assert(new_len <= new_cap);
    assert(new_cap <= (SIZE_MAX - offsetof(ArrHdr, buf))/elem_size);
    size_t new_size = offsetof(ArrHdr, buf) + new_cap*elem_size;
    ArrHdr *new_hdr;
    if (buf) {
        new_hdr = xrealloc(arr__hdr(buf), new_size);
    } else {
        new_hdr = xmalloc(new_size);
        new_hdr->len = 0;
    }
    new_hdr->cap = new_cap;
    return new_hdr->buf;
}
// 005. END
#endif

#if 1
// 006. START
typedef struct HashTable {
    u32 len;
    u32 cap;
    char **keys;
    void **values;
} HashTable;

HashTable *ht_create() {
    HashTable *ht = xcalloc(1, sizeof(HashTable));
    ht->len = 0;
    ht->cap = 1024;
    ht->keys = (char**)xcalloc(1, sizeof(char*) * ht->cap); // calloc to memset to zero
    ht->values = (void**)xcalloc(1, sizeof(void*) * ht->cap); // calloc to memset to zero
    return ht;
}

void ht_free(HashTable *ht) {
    for(int i = 0; i < ht->cap; i++) {
        if(ht->keys[i]) {
            free(ht->keys[i]);
            free(ht->values[i]);
        }
    }
    free(ht->keys);
    free(ht->values);
    free(ht);
}

int ht_hash(HashTable *ht, char *key) {
    // djb2 hash function
    u64 hash = 5381;
    s32 c;
    while(c = *key++) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }

    return hash % ht->cap;
}

void ht_insert(HashTable *ht, char *key, void *value);

void ht_grow(HashTable *ht) {
    int oldCap = ht->cap;
    ht->cap = ht->cap * 2;
    char **oldKeys = ht->keys;
    void **oldValues = ht->values;
    ht->keys = (char**)xcalloc(1, sizeof(char*) * ht->cap); // calloc to memset to zero
    ht->values = (void**)xcalloc(1, sizeof(char*) * ht->cap); // calloc to memset to zero
    ht->len = 0; // because ht_insert increments len
    for(int i = 0; i < oldCap; i++) {
        if(!oldKeys[i]) { continue; }
        else {
            ht_insert(ht, oldKeys[i], oldValues[i]);
            free(oldKeys[i]);
        }
    }
    free(oldKeys);
    free(oldValues);
}

void ht_insert(HashTable *ht, char *key, void *value) {
    assert(ht->keys);
    assert(ht->values);
    if((ht->len + 1) * 2 >= ht->cap) {
        ht_grow(ht);
    }

    int index = ht_hash(ht, key);
    while(1) {
        if(!ht->keys[index]) {
            char *keyCopy = str_copy(key);
            ht->keys[index] = keyCopy;
            ht->values[index] = value;
            ht->len++;
            return;
            // Overwrite existing values of the same key
        } else if(str_equal(ht->keys[index], key)) {
            free(ht->values[index]);
            ht->values[index] = value;
            return;
        } else {
            index = (index + 1) % ht->cap;
        }
    }
}

void *ht_search(HashTable *ht, char *key) {
    assert(ht->keys);
    assert(ht->values);
    u32 index = ht_hash(ht, key);
    while(1) {
        if(ht->keys[index]) {
            if(str_equal(ht->keys[index], key)) {
                return ht->values[index];
            } else {
                index = (index + 1) % ht->cap;
            }
        } else { 
            return 0;
        }
    }
}

void ht_delete(HashTable *ht, char *key) {
    u64 hash = ht_hash(ht, key);
    while(1) {
        if(!ht->keys[hash]) {
            break;
        } else if(str_equal(ht->keys[hash], key)) {
            ht->keys[hash] = 0;
            free(ht->values[hash]);
            ht->values[hash] = 0;
            ht->len--;
            // NOTE: Need to reinsert all records until the next empty slot
            while(ht->keys[(hash + 1) % ht->cap]) {
                hash = (hash + 1) % ht->cap;
                char* tmpKey = ht->keys[hash];
                void* tmpValue = ht->values[hash];
                ht->keys[hash] = 0;
                ht->values[hash] = 0;
                ht_insert(ht, tmpKey, tmpValue);
                free(tmpKey); // because insert mallocs for the key
                ht->len--; // because insert increments len
            }
            break;
        } else {
            hash = (hash + 1) % ht->cap;
        }
    }
}
// 006. END
#endif

#if 1
// 007. START

#ifdef MEM_DEBUG
typedef struct AllocRecord {
    void *ptr;
    u32 size;
    char *file;
    u32 line;
    int freed;
    char* freeFile;
    int freeLine;
} AllocRecord;

AllocRecord *allocRecords = 0;

void *mem_malloc(u32 size, char *file, u32 line) {
    void *ptr = malloc(size);
    if (!ptr) {
        perror("mem_malloc failed");
        exit(1);
    }

    AllocRecord ar; 
    //ar = malloc(sizeof(ar));
    ar.ptr = ptr;
    ar.size = size;
    ar.file = str_copy(file);
    ar.line = line;
    ar.freed = 0;
    ar.freeFile = 0;
    ar.freeLine = 0;
    arr_push(allocRecords, ar);
        
    return ptr;
}

void *mem_calloc(u32 num, u32 size, char* file, int line) {
    void *ptr = calloc(num, size);
    if (!ptr) {
        perror("mem_calloc failed");
        exit(1);
    }

    AllocRecord ar; 
    //ar = malloc(sizeof(ar));
    ar.ptr = ptr;
    ar.size = num * size;
    ar.file = str_copy(file);
    ar.line = line;
    ar.freed = 0;
    ar.freeFile = 0;
    ar.freeLine = 0;
    arr_push(allocRecords, ar);
        
    return ptr;
}

void *mem_realloc(void *ptr, u32 size, char* file, int line) {
    for(int i = 0; i < arr_len(allocRecords); i++) {
        if(allocRecords[i].ptr == ptr) {
            allocRecords[i].size = size;
            free(allocRecords[i].file);
            allocRecords[i].file = str_copy(file);
            allocRecords[i].line = line;

            void *res = realloc(ptr, size);
            if (!ptr) {
                perror("mem_realloc failed");
                exit(1);
            }

            allocRecords[i].ptr = res;

            return res;
        }
    }

}

void mem_free(void *ptr, char* file, int line) {
    for(int i = 0; i < arr_len(allocRecords); i++) {
        if(allocRecords[i].ptr == ptr) {
            if(allocRecords[i].freed == 0) { 
                allocRecords[i].freed = 1;
                allocRecords[i].freeFile = str_copy(file);
                allocRecords[i].freeLine = line;
                free(ptr);
                break;
            }
            else {
                log_err("xfree at %s:%d was already freed!", file, line);
                log_err("alloced at %s:%d", allocRecords[i].file, allocRecords[i].line);
                log_err("previous freed at %s:%d", allocRecords[i].freeFile, allocRecords[i].freeLine);
                break;
            }
        }
    }
}

#define xmalloc(n) mem_malloc(n, __FILE__, __LINE__)
#define xcalloc(s, n) mem_calloc(s, n, __FILE__, __LINE__)
#define xrealloc(p, n) mem_realloc(p, n, __FILE__, __LINE__)
#define xfree(p) mem_free(p, __FILE__, __LINE__)
#endif

// 007. END
#endif
