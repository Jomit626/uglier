#include <stdio.h>
#include <stdint.h>

void dump_obj(FILE *f, char* name, void *obj, size_t n){
    uint64_t *d64 = (uint64_t*)obj,byte64 = 0, n64byte = n >> 3;
    //uint8_t *dbyte = obj, byte, nbyte = n & (size_t)0x7;
    int i;
    fprintf(f, "uint64_t %s[]={\n",name);
    for(i=0;i<n64byte+1;i++){
        fprintf(f, "0x%lX,\n",d64[i]);
    }
    //dbyte += n64byte<<3;
    // big-endian
    // TODO: little-endian
    //if(nbyte){
    //    for(i=0;i<nbyte;i++){
    //        byte64 >>= 8;
    //        byte64 |= (uint64_t)dbyte[i] << (64-8);
    //    }
    //    fprintf(f, "0x%lX,\n",byte64);
    //}
    fprintf(f, "};\n");
}