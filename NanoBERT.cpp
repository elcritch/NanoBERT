#include "NanoBERT.h"

// =============== RPC Handlers ================ //

void copyStream(StreamBuff *outstream, StreamBuff *instream)
{
  while (outstream->available()) {
    outstream->write(instream->read());
  }
}

template<typename HandleRpcMessageRecieve, typename HandleRpcMethods, typename CallReplyFunction>
bool handle_rpc(HandleRpcMessageRecieve msg_recv_func, StreamBuff& rpc_arg_stream, CallReplyFunction msg_reply_func)
{
  uint8_t len = RH_RF95_MAX_MESSAGE_LEN; // always reset to max length, will truncate msg otherwise


  StreamBuff * rpc_msg_stream = msg_recv_func();

    msgpck_to_json( &Serial, &rpc_msg_stream.reset()); Serial.println(F("!"));

    StreamBuff rpc_stream(rpc_msg_stream.data, len);
    RpcMessage rpc_msg = rpcMessageRead(&rpc_stream, &rpc_arg_stream);

    StreamBuff json_stream(rpc_msg_stream.data, len);
    msgpck_to_json( &Serial, &json_stream); Serial.println();

    /* --------- Handle NanoBertRpc Call's ---------- */
    // rpc_msg_stream.flush();

    switch (rpc_msg.method) {
      case RpcMethods::GetSensorData:
        handle_call_get_sensor_data(&rpc_arg_stream);

        break;
      case RpcMethods::ReadSensorData:
        handle_call_read_sensors();
        break;
    }

    /* --------- Handle NanoBertRpc Cast's ---------- */

    if (rpc_msg.type == RpcMessageTypes::Call) {

      Serial.print(F("-->")); msgpck_to_json( &Serial, &rpc_arg_stream.reset()); Serial.print(F(":")); Serial.println(rpc_arg_stream.max_position);

      delay(10);

      msg_reply_func(&rpc_arg_stream);

    } else if (rpc_msg.type == RpcMessageTypes::Cast) {
    }
  }
}



RpcMessage rpcMessageRead(StreamBuff *instream, StreamBuff *outstream)
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
  res4 &= (rpc_msg.version == RpcMessageTypes::MAGIC_VERSION);

  if (!res) {
    Serial.println(F("rpc msg err"));
    rpc_msg.type = RpcMessageTypes::Error;
    rpc_msg.method = RpcProtocolCodes::UnableToReadHeader;
    return rpc_msg;
  }

  // Remaining portion is the data packet
  outstream->flush();
  copyStream(outstream, instream);
  rpc_msg.data = outstream;

  if (!res) {
    Serial.println(F("msg err"));
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
  uint8_t magic_version = RpcMessageTypes::MAGIC_VERSION;

  msgpck_write_array_header(ostream, 5);
  msgpck_write_bin(ostream, (uint8_t*)&magic_version, 1);
  msgpck_write_bin(ostream, (uint8_t*)&rpc_msg->type, 1);
  msgpck_write_bin(ostream, &rpc_msg->module, 1);
  msgpck_write_bin(ostream, &rpc_msg->method, 1);
  copyStream(ostream, rpc_msg->data);
}


void rpcReplyMessageHeader(StreamBuff *outstream, StreamBuff *data)
{
  uint8_t magic_version = MAGIC_VERSION;
  uint8_t msg_type = RpcMessageTypes::Reply;
  msgpck_write_array_header(outstream, 3);
  msgpck_write_bin(outstream, &magic_version,1);
  msgpck_write_bin(outstream, &msg_type,1);
  copyStream(outstream, data);
}

void rpcNoReplyMessageHeader(StreamBuff *outstream)
{
  uint8_t magic_version = MAGIC_VERSION;
  uint8_t msg_type = RpcMessageTypes::NoReply;
  msgpck_write_array_header(outstream, 2);
  msgpck_write_bin(outstream, &magic_version,1);
  msgpck_write_bin(outstream, &msg_type,1);
}


/* ============ ATOMS ============= */

#define BASE 36

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
