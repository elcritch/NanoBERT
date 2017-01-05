#ifndef __NANO_BERT_RPC_H__
#define __NANO_BERT_RPC_H__

#include <Stream.h>
#include <msgpck.h>

#ifndef NANO_BERT_RPC_MAX_ATOM_LEN
#define NANO_BERT_RPC_MAX_ATOM_LEN 6
#endif

// =============== RPC Handlers ================ //

enum RpcMessageTypes : uint8_t
{
  Call = 0,
  Reply,
  Error,
  Cast,
  NoReply,
  Info,
  MAGIC_VERSION = 0x83,
};

enum RpcProtocolCodes : uint8_t
{
  Undesignated = 0,
  UnableToReadHeader = 1,
  UnableToReadData = 2,
  NoSuchModule = 3,
  NoSuchFunction = 4
};

// template<size_t T>
template<typename METADATA>
struct RpcMessageRaw
{
    uint8_t version;
    RpcMessageTypes type;
    uint8_t module;
    uint8_t method;
    METADATA metadata;

    StreamBuff * data;
};

struct EmbeddedRpcMetadata
{
  uint8_t sender_id;
  uint8_t recipient_id;
};


struct RpcMessage : RpcMessageRaw<EmbeddedRpcMetadata>
{

};

void copyStream(StreamBuff& outstream, StreamBuff& instream);

// int freeRam();


template<size_t T>
RpcMessage rpcMessageRead(StreamBuffStack<T>& outstream, StreamBuffStack<T>& instream)
{
  // Read message
  RpcMessage rpc_msg;
  rpc_msg.data = &outstream;

  bool res = true;
  bool res0 = true;
  bool res1 = true;
  bool res2 = true;
  bool res3 = true;
  bool res4 = true;

  uint32_t msg_arr_size;
  res &= msgpck_read_array_size(&instream, &msg_arr_size);
  res0 &= msgpck_read_integer(&instream, &rpc_msg.version, 1);
  res1 &= msgpck_read_integer(&instream, (uint8_t*)&rpc_msg.type, 1);
  res2 &= msgpck_read_integer(&instream, &rpc_msg.module, 1);
  res3 &= msgpck_read_integer(&instream, &rpc_msg.method, 1);
  res3 &= msgpck_read_bin(&instream, (byte*)&rpc_msg.metadata, sizeof(rpc_msg.metadata));
  res4 &= (rpc_msg.version == RpcMessageTypes::MAGIC_VERSION);

  if (!res) {
    // Serial.println(F("rpc msg err"));
    rpc_msg.type = RpcMessageTypes::Error;
    rpc_msg.method = RpcProtocolCodes::UnableToReadHeader;
    return rpc_msg;
  }

  // Remaining portion is the data packet
  rpc_msg.data->flush();
  copyStream(*rpc_msg.data, instream);

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
  msgpck_write_integer(outstream, (uint8_t)magic_version);
  msgpck_write_integer(outstream, (uint8_t)msg_type);
  copyStream(outstream, data);
}

template<size_t T>
void rpcNoReplyMessageHeader(StreamBuffStack<T>& outstream)
{
  uint8_t magic_version = MAGIC_VERSION;
  uint8_t msg_type = RpcMessageTypes::NoReply;
  msgpck_write_array_header(outstream, 2);
  msgpck_write_integer(outstream, (uint8_t)magic_version);
  msgpck_write_integer(outstream, (uint8_t)msg_type);
}

// template<size_t T>
// RpcMessageContainer<T> rpcMessageRead(StreamBuffStack<T>& instream);

template<class _ARG_WRITER>
void rpcMessageWrite(RpcMessage& rpc_msg, _ARG_WRITER _arg_writer)
{
  StreamBuff * ostream = rpc_msg.data;

  ostream->flush();

  // Read message
  bool res = true;
  uint8_t magic_version = RpcMessageTypes::MAGIC_VERSION;

  msgpck_write_array_header(ostream, 6);
  msgpck_write_integer(ostream, (uint8_t)magic_version);
  msgpck_write_integer(ostream, (uint8_t)rpc_msg.type);
  msgpck_write_integer(ostream, rpc_msg.module);
  msgpck_write_integer(ostream, rpc_msg.method);
  msgpck_write_bin(ostream, (byte*)&rpc_msg.metadata, sizeof(rpc_msg.metadata));
  // copyStream(ostream, rpc_msg->data);

  _arg_writer(ostream);
}


void rpcReplyMessageHeader(StreamBuff *outstream, StreamBuff *data);

void rpcNoReplyMessageHeader(StreamBuff *outstream);


/* ============ ATOMS ============= */

#define BASE 26

#define atom_t uint32_t

uint32_t ipow(uint32_t base, uint32_t exponent);

atom_t atom_parse(char *atomStr);

void atom_print(atom_t atom, StreamBuff *ostream);


#endif
