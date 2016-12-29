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
    StreamBuff data;

    RpcMessage() : data() {}

    RpcMessage(StreamBuff db) : data(db) {}
};

template<size_t S>
struct RpcMessageContainer : public RpcMessage
{

  RpcMessageContainer() : RpcMessage(StreamBuffStack<S>()) {}

};


void copyStream(StreamBuff& outstream, StreamBuff& instream);

template<size_t T>
RpcMessage rpcMessageRead(StreamBuffStack<T>& instream);

template<class _ARG_WRITER, size_t T>
void rpcMessageWrite(RpcMessage& rpc_msg, _ARG_WRITER _arg_writer)
{
  StreamBuff * ostream = &rpc_msg.data;

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
