#include "crypto.h"
#include "cppext/cppext.h"
#include <openssl/aes.h>
#include <openssl/rsa.h>
#include <openssl/sha.h>
void aes_encrypt(const void* key, void* data)
{
  AES_KEY mkey;
  AES_set_encrypt_key((unsigned char*)key,256,&mkey);
  AES_encrypt((unsigned char*)data,(unsigned char*)data,&mkey);
  
}
static BIGNUM* ReadBig(System::BStream& str) {
  uint16_t len;
  str.Read(len);
  return BN_bin2bn(str.Increment(len),len,0);
}
static void WriteBig(System::BStream& str, const BIGNUM* number) {
  uint16_t len = BN_num_bytes(number);
  str.Write(len);
  BN_bn2bin(number,str.ptr);
  str.Increment(len);
}

void* RSA_Key(unsigned char* data, size_t len)
{
  System::BStream str(data,len);
  try {
  RSA* msa = RSA_new();
  msa->n = ReadBig(str); //Public modulus
  msa->e = ReadBig(str); //Public exponent
  if(str.length) {
    msa->d = ReadBig(str); //Private exponent
    msa->p = ReadBig(str); //Secret prime factor
    msa->q = ReadBig(str); //Secret prime factor
    msa->dmp1 = ReadBig(str); //d mod (p-1)
    msa->dmq1 = ReadBig(str); //d mod (q-1)
    msa->iqmp = ReadBig(str); //q^-1 mod p
    return msa;
  }
  }catch(const char* err) {
    return 0;
  }
}

void RSA_Export(void* key, bool includePrivate, unsigned char** output, size_t* outlen)
{
  RSA* msa = (RSA*)key;
  if(includePrivate) {
    size_t len = 2+BN_num_bytes(msa->n)+2+BN_num_bytes(msa->e)+2+BN_num_bytes(msa->d)+2+BN_num_bytes(msa->p)+2+BN_num_bytes(msa->q)+2+BN_num_bytes(msa->dmp1)+2+BN_num_bytes(msa->dmq1)+2+BN_num_bytes(msa->iqmp);
    unsigned char* mander = new unsigned char[len];
    System::BStream str(mander,len);
    WriteBig(str,msa->n);
    WriteBig(str,msa->e);
    WriteBig(str,msa->d);
    WriteBig(str,msa->p);
    WriteBig(str,msa->q);
    WriteBig(str,msa->dmp1);
    WriteBig(str,msa->dmq1);
    WriteBig(str,msa->iqmp);
    *output = mander;
    *outlen = len;
  }else {
    size_t len = 2+BN_num_bytes(msa->n)+2+BN_num_bytes(msa->e);
    unsigned char* mander = new unsigned char[len];
    System::BStream str(mander,len);
    WriteBig(str,msa->n);
    WriteBig(str,msa->e);
    *output = mander;
    *outlen = len;
    
  }
}


void hash_generate(const unsigned char* data, size_t len, char* output)
{
  //Poor unsigned Charmander....
  unsigned char mander[64];
  SHA512(data,len,mander);
  const char* hex = "0123456789ABCDEF";
  size_t c = 0;
  for(size_t i = 0;i<16;i++) {
    output[c] = hex[mander[i] >> 4]; //Get lower 4 bits
    c++;
    output[c] = hex[((mander[i] << 4) & 0xff) >> 4];//Get upper 4 bits
    c++;
  }
  
}


void* RSA_GenKey(size_t bits)
{
 // BIGNUM* e = BN_new();
  //  BN_set_word(e, 65537);
  RSA* msa = RSA_generate_key(bits,65537,0,0);
  //BN_free(e);
  return msa;
}


void RSA_Free_Buffer(void* buffer)
{
  delete[] (unsigned char*)buffer;
}


void RSA_Free(void* key)
{
  RSA_free((RSA*)key);
}



void aes_decrypt(const void* key, void* data)
{
  
  AES_KEY mkey;
  AES_set_decrypt_key((unsigned char*)key,256,&mkey);
  AES_decrypt((unsigned char*)data,(unsigned char*)data,&mkey);
}