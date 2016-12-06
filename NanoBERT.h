#ifndef __NANO_BERT_RPC_H__
#define __NANO_BERT_RPC_H__

#include <msgpck.h>

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

void copyStream(StreamBuff *outstream, StreamBuff *instream);

RpcMessage rpcMessageRead(StreamBuff *instream);

void rpcMessageWrite(StreamBuff *ostream, RpcMessage *rpc_msg);

void rpcReplyMessageHeader(StreamBuff *outstream, StreamBuff *data);

void rpcNoReplyMessageHeader(StreamBuff *outstream);


/* ============ ATOMS ============= */

#define BASE 26

#define atom_t uint32_t

uint32_t ipow(uint32_t base, uint32_t exponent);

atom_t atom_parse(char *atomStr);

void atom_print(atom_t atom, StreamBuff *ostream);


#endif
