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
struct RpcMessage
{
    uint8_t version;
    RpcMessageTypes type;
    uint8_t module;
    uint8_t method;
    StreamBuff * data;


    virtual StreamBuff * data_ptr() { return data; }
};

template<size_t T>
struct RpcMessageContainer : public RpcMessage
{
    StreamBuffStack<T> static_data;

    virtual StreamBuff * data_ptr() { return &static_data; }
};


void copyStream(StreamBuff& outstream, StreamBuff& instream);


template<size_t T>
RpcMessageContainer<T> rpcMessageRead(StreamBuffStack<T>& instream)
{
  // Read message
  RpcMessageContainer<T> rpc_msg;

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

  {
    Serial.print("instream read: pos: "); Serial.println(instream.pos);
    Serial.print("instream read: max_position: "); Serial.println(instream.len);
  }


  // Remaining portion is the data packet
  rpc_msg.static_data.flush();
  copyStream(rpc_msg.static_data, instream);
  // rpc_msg.data = outstream;

  {
    Serial.print("instream' read: pos: "); Serial.println(instream.pos);
    Serial.print("instream' read: max_position: "); Serial.println(instream.len);
  }

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

// template<size_t T>
// RpcMessageContainer<T> rpcMessageRead(StreamBuffStack<T>& instream);

template<class _ARG_WRITER>
void rpcMessageWrite(RpcMessage& rpc_msg, _ARG_WRITER _arg_writer)
{
  StreamBuff * ostream = rpc_msg.data_ptr();

  ostream->flush();

  // Read message
  bool res = true;
  uint8_t magic_version = RpcMessageTypes::MAGIC_VERSION;

  msgpck_write_array_header(ostream, 5);
  msgpck_write_bin(ostream, (uint8_t*)&magic_version, 1);
  msgpck_write_bin(ostream, (uint8_t*)&rpc_msg.type, 1);
  msgpck_write_bin(ostream, &rpc_msg.module, 1);
  msgpck_write_bin(ostream, &rpc_msg.method, 1);
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
