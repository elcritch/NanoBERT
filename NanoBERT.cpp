#include "NanoBERT.h"

// =============== RPC Handlers ================ //

void copyStream(StreamBuff& outstream, StreamBuff& instream)
{
  while (outstream.available()) {
    outstream.write(instream.read());
  }
}

template<size_t T>
RpcMessage rpcMessageRead(StreamBuffStack<T>& instream)
{
  // Read message
  RpcMessage rpc_msg = RpcMessageContainer<T>();

  bool res = true;
  bool res0 = true;
  bool res1 = true;
  bool res2 = true;
  bool res3 = true;
  bool res4 = true;

  uint32_t msg_arr_size;
  res &= msgpck_read_array_size(&instream, &msg_arr_size);
  res0 &= msgpck_read_bin(&instream, &rpc_msg.version, 1);
  res1 &= msgpck_read_bin(&instream, (uint8_t*)&rpc_msg.type, 1);
  res2 &= msgpck_read_bin(&instream, &rpc_msg.module, 1);
  res3 &= msgpck_read_bin(&instream, &rpc_msg.method, 1);
  res4 &= (rpc_msg.version == RpcMessageTypes::MAGIC_VERSION);

  if (!res) {
    // Serial.println(F("rpc msg err"));
    rpc_msg.type = RpcMessageTypes::Error;
    rpc_msg.method = RpcProtocolCodes::UnableToReadHeader;
    return rpc_msg;
  }

  // Remaining portion is the data packet
  rpc_msg.data.flush();
  copyStream(rpc_msg.data, instream);
  // rpc_msg.data = outstream;

  if (!res) {
    // Serial.println(F("msg err"));
    rpc_msg.type = RpcMessageTypes::Error;
    rpc_msg.method = RpcProtocolCodes::UnableToReadHeader;
    return rpc_msg;
  }


  return rpc_msg;
}


template<size_t T>
void rpcReplyMessageHeader(StreamBuffStack<T>& outstream, StreamBuffStack<T>& data)
{
  uint8_t magic_version = MAGIC_VERSION;
  uint8_t msg_type = RpcMessageTypes::Reply;
  msgpck_write_array_header(&outstream, 3);
  msgpck_write_bin(&outstream, &magic_version,1);
  msgpck_write_bin(&outstream, &msg_type,1);
  copyStream(outstream, data);
}

template<size_t T>
void rpcNoReplyMessageHeader(StreamBuffStack<T>& outstream)
{
  uint8_t magic_version = MAGIC_VERSION;
  uint8_t msg_type = RpcMessageTypes::NoReply;
  msgpck_write_array_header(outstream, 2);
  msgpck_write_bin(outstream, &magic_version,1);
  msgpck_write_bin(outstream, &msg_type,1);
}


/* ============ ATOMS ============= */

// #define BASE 36

#define atom_t uint32_t


uint32_t ipow(uint32_t base, uint32_t exponent)
{
    uint32_t result = 1;
    while (exponent)
    {
        if (exponent & 1)
            result *= base;
        exponent >>= 1;
        base *= base;
    }

    return result;
}

atom_t atom_parse(char *atomStr)
{
  unsigned int i;
  unsigned int c;
  unsigned int x,y;

  int n = strnlen(atomStr,NANO_BERT_RPC_MAX_ATOM_LEN);

  y = 0;
  for (i = 0; i < n; ++i) {
    x = (atomStr[i]-'a') % BASE;
    y += x * ipow(BASE,i);
  }

  return y;
}

void atom_print(StreamBuff *ostream, atom_t atom) {


  size_t i;
  char c;

  for (i = 0; i < 6 && atom; ++i) {
    c = atom % BASE + 'a';
    ostream->write(c);
    atom = atom / BASE;
  }
}
