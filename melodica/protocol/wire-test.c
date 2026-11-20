#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h>
extern int melodica_wire_blocks(uint8_t *, size_t, const uint8_t *, size_t, int);
int main(void) {
    const uint8_t key[8] = {0}, expected[8] = {0x4e,0xf9,0x97,0x45,0x61,0x98,0xdd,0x78};
    uint8_t buf[16] = {0}, clear[16] = {0};
    assert(melodica_wire_blocks(buf,16,key,8,0));
    assert(!memcmp(buf,expected,8) && !memcmp(buf+8,expected,8));
    assert(melodica_wire_blocks(buf,16,key,8,1) && !memcmp(buf,clear,16));
    assert(!melodica_wire_blocks(buf,7,key,8,0));
    assert(!melodica_wire_blocks(buf,8,key,0,0));
    return 0;
}
