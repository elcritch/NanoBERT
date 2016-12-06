#include "NanoBERT.h"

uint8_t magive_version_bert_rpc = 0x83;

// =============== RPC Handlers ================ //

enum RpcMessageTypes : uint8_t
{
  Call = 0,
  Reply,
  Error,
  Cast,
  NoReply,
  Info
};

enum RpcProtocolCodes : uint8_t
{
  Undesignated = 0,
  UnableToReadHeader = 1,
  UnableToReadData = 2,
  NoSuchModule = 3,
  NoSuchFunction = 4
};


struct RpcMessage
{
    uint8_t version;
    RpcMessageTypes type;
    uint8_t module;
    uint8_t method;
    StreamBuff *data;
};

uint8_t __rpc_arg_stream[RH_RF95_MAX_MESSAGE_LEN]; // <-- Dont put this on the stack
StreamBuff rpc_arg_stream(__rpc_arg_stream, RH_RF95_MAX_MESSAGE_LEN);

void copyStream(StreamBuff *outstream, StreamBuff *instream)
{
  while (outstream->available()) {
    outstream->write(instream->read());
  }
}

RpcMessage rpcMessageRead(StreamBuff *instream)
{
  // Read message
  RpcMessage rpc_msg;

  bool res = true;
  bool res0 = true;
  bool res1 = true;
  bool res2 = true;
  bool res3 = true;
  bool res4 = true;

  uint32_t msg_arr_size;
  res &= msgpck_read_array_size(instream, &msg_arr_size);
  res0 &= msgpck_read_bin(instream, &rpc_msg.version, 1);
  res1 &= msgpck_read_bin(instream, (uint8_t*)&rpc_msg.type, 1);
  res2 &= msgpck_read_bin(instream, &rpc_msg.module, 1);
  res3 &= msgpck_read_bin(instream, &rpc_msg.method, 1);
  res4 &= (rpc_msg.version == magive_version_bert_rpc);

  if (!res) {
    Serial.println("rpc msg err");
    rpc_msg.type = RpcMessageTypes::Error;
    rpc_msg.method = RpcProtocolCodes::UnableToReadHeader;
    return rpc_msg;
  }

  // Remaining portion is the data packet
  rpc_arg_stream.flush();
  copyStream(&rpc_arg_stream, instream);
  rpc_msg.data = &rpc_arg_stream;

  if (!res) {
    Serial.println("msg err");
    rpc_msg.type = RpcMessageTypes::Error;
    rpc_msg.method = RpcProtocolCodes::UnableToReadHeader;
    return rpc_msg;
  }


  return rpc_msg;
}

void rpcMessageWrite(StreamBuff *ostream, RpcMessage *rpc_msg)
{
  // Read message

  bool res = true;
  msgpck_write_array_header(ostream, 5);
  msgpck_write_bin(ostream, &magive_version_bert_rpc, 1);
  msgpck_write_bin(ostream, (uint8_t*)&rpc_msg->type, 1);
  msgpck_write_bin(ostream, &rpc_msg->module, 1);
  msgpck_write_bin(ostream, &rpc_msg->method, 1);
  copyStream(ostream, rpc_msg->data);
}


void rpcReplyMessageHeader(StreamBuff *outstream, StreamBuff *data)
{
  msgpck_write_array_header(outstream, 3);
  msgpck_write_integer(outstream, magive_version_bert_rpc);
  msgpck_write_integer(outstream, RpcMessageTypes::Reply);
  copyStream(outstream, data);
}

void rpcNoReplyMessageHeader(StreamBuff *outstream)
{
  msgpck_write_array_header(outstream, 2);
  msgpck_write_integer(outstream, magive_version_bert_rpc);
  msgpck_write_integer(outstream, RpcMessageTypes::NoReply);
}


/* ============ ATOMS ============= */

#define BASE 26

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

  int n = strlen(atomStr);

  y = 0;
  for (i = 0; i < n; ++i) {
    x = (atomStr[i]-'a') % BASE;
    y += x * ipow(BASE,i);
  }

  return y;
}


void atom_print(atom_t atom, StreamBuff *ostream) {

  size_t i;
  char c;

  for (i = 0; i < 6 && atom; ++i) {
    c = atom % BASE + 'a';
    ostream->write(c);
    atom = atom / BASE;
  }
}
