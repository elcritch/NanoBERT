#include "msgpck.h"
/*
   This is an example of message pack library use for Arduino uno.

   It writes some message pack serialized data on Serial.

   Expected output:

   0x86 0xa3 0x6e 0x69 0x6c 0xc0 0xa4 0x62
   0x6f 0x6f 0x6c 0xc2 0xa4 0x75 0x69 0x6e
   0x74 0x0c 0xa3 0x69 0x6e 0x74 0xd1 0xff
   0x00 0xa5 0x66 0x6c 0x6f 0x61 0x74 0xca
   0x3f 0xe0 0x00 0x00 0xa5 0x61 0x72 0x72
   0x61 0x79 0x93 0x05 0xcd 0x09 0xc4 0xd1
   0xfa 0x21
*/



void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(100);



}

uint8_t buf[128];
uint8_t rbuf[128];

void writeData()
{
  StreamBuff sb(buf, 128);

  msgpck_write_map_header(&sb, 6);

  msgpck_write_string(&sb, "nil", 3);
  msgpck_write_nil(&sb);

  msgpck_write_string(&sb, "bool", 4);
  msgpck_write_bool(&sb, true);

  msgpck_write_string(&sb, "uint", 4);
  msgpck_write_integer(&sb, 12);

  msgpck_write_string(&sb, "int", 3);
  msgpck_write_integer(&sb, -256);

  msgpck_write_string(&sb, "float", 5);
  msgpck_write_float(&sb, 1./3.);


  msgpck_write_string(&sb, "array", 5);
  msgpck_write_array_header(&sb, 3);
  uint8_t u0 = 5;
  uint16_t u1 = 2500;
  int32_t i0 = -1503;
  msgpck_write_integer(&sb, u0);
  msgpck_write_integer(&sb, u1);
  msgpck_write_integer(&sb, i0);

}

void readData()
{
  StreamBuff sb(buf, 128);

  uint8_t i;
  char sbuf[8];
  uint32_t map_size;
  uint32_t r_size;
  uint8_t pin;
  uint16_t uvalue;
  int svalue;
  float fvalue;
  bool level;
  bool res = true; 

//  msgpck_write_map_header(&sb, 6);

  res &= msgpck_map_next(&sb);

  res &= msgpck_read_map_size(&sb, &map_size);

  Serial.println("\n === 'map size' 6");
  Serial.print("msg unpack: '"); Serial.print(map_size); Serial.println("'"); 

//  msgpck_write_string(&sb, "nil", 3);
//  msgpck_write_nil(&sb);

  Serial.println("\n === 'nil'");
  res &= msgpck_read_string(&sb, sbuf, sizeof(sbuf), &r_size);
  sbuf[r_size] = '\0';
  Serial.print("msg unpack: '"); Serial.print(sbuf);  Serial.println("'"); 

  res &= msgpck_read_nil(&sb);
  Serial.print("msg unpack: "); Serial.println("got nil"); 

//  msgpck_read_string(&sb, "bool", 4);
//  msgpck_read_bool(&sb, false);

  Serial.println("\n === 'bool' -> true ");
  res &= msgpck_read_string(&sb, sbuf, sizeof(sbuf), &r_size);
  sbuf[r_size] = '\0';
  Serial.print("msg unpack: '"); Serial.print(sbuf);  Serial.println("'"); 

  msgpck_read_bool(&sb, &level);
  Serial.print("msg unpack: '"); Serial.print(level); Serial.println("'"); 

//  msgpck_read_string(&sb, "uint", 4);
//  msgpck_read_integer(&sb, 12);

  Serial.println("\n === 'uint' -> 12 ");

  res &= msgpck_read_string(&sb, sbuf, sizeof(sbuf), &r_size);
  sbuf[r_size] = '\0';
  Serial.print("msg unpack: '"); Serial.print(sbuf);  Serial.println("'"); 

  Serial.print("signed next: "); Serial.println(msgpck_signed_next(&sb));
  msgpck_read_integer(&sb, &pin, 4);
  Serial.print("msg unpack: '"); Serial.print(pin); Serial.println("'"); 

//  msgpck_read_string(&sb, "int", 3);
//  msgpck_read_integer(&sb, -256);

  Serial.println("\n === 'int' -> -256");

  res &= msgpck_read_string(&sb, sbuf, sizeof(sbuf), &r_size);
  sbuf[r_size] = '\0';
  Serial.print("msg unpack: '"); Serial.print(sbuf);  Serial.println("'"); 

  Serial.print("signed next: "); Serial.println(msgpck_signed_next(&sb));
  msgpck_read_integer(&sb, (byte*)&svalue, 4);
  Serial.print("msg unpack: '"); Serial.print(svalue); Serial.println("'"); 


//  msgpck_read_string(&sb, "float", 5);

  Serial.println("\n === 'float' -> 5 ");
  Serial.print("signed next: "); Serial.println(msgpck_signed_next(&sb));

  res &= msgpck_read_string(&sb, sbuf, sizeof(sbuf), &r_size);
  sbuf[r_size] = '\0';
  Serial.print("msg unpack: '"); Serial.print(sbuf);  Serial.println("'"); 

  Serial.print("signed next: "); Serial.println(msgpck_signed_next(&sb));
  Serial.print("float next: "); Serial.println(msgpck_float_next(&sb));
  msgpack_read_float(&sb, &fvalue);
  Serial.print("msg unpack: '"); Serial.print(fvalue); Serial.println("'"); 


//  msgpck_read_string(&sb, "array", 5);
//  msgpck_read_array_header(&sb, 3);

  Serial.println("\n === 'array size' 3 -> 5, 2500, -1503");

  res &= msgpck_read_string(&sb, sbuf, sizeof(sbuf), &r_size);
  sbuf[r_size] = '\0';
  Serial.print("msg unpack: '"); Serial.print(sbuf);  Serial.println("'"); 

  Serial.print("array next: "); Serial.println(msgpck_signed_next(&sb));

  res &= msgpck_array_next(&sb);

  res &= msgpck_read_array_size(&sb, &map_size);
  Serial.print("array size: '"); Serial.print(map_size); Serial.println("'"); 

  uint8_t u0 = 0;
  uint16_t u1 = 0;
  int32_t i0 = 0;

  Serial.print("signed next: "); Serial.println(msgpck_signed_next(&sb));
  msgpck_read_integer(&sb, &pin, 1);
  Serial.print("msg unpack: '"); Serial.print(pin); Serial.println("'"); 

  Serial.print("signed next: "); Serial.println(msgpck_signed_next(&sb));
  msgpck_read_integer(&sb, (byte*)&svalue, 4);
  Serial.print("msg unpack: '"); Serial.print(svalue); Serial.println("'"); 

  Serial.print("signed next: "); Serial.println(msgpck_signed_next(&sb));
  msgpck_read_integer(&sb, (byte*)&svalue, 4);
  Serial.print("msg unpack: '"); Serial.print(svalue); Serial.println("'"); 



  Serial.print("res = "); Serial.println(res);

  Serial.println("\n===== msg unpack done \n\n");
}

size_t readBuffer()
{

  memset(rbuf, 0, sizeof(rbuf));

  StreamBuff srb(rbuf, 128);

  while (Serial.available() > 0 && srb.available()) {
    // read the incoming byte:
    srb.write(Serial.read());
    // say what you got:
  }

  if (srb.pos > 0) {
    Serial.print("received: ");
    Serial.println((char*)rbuf);
    Serial.println();
    delay(1000);
  }

  return srb.pos;

}

void loop() {
  // put your main code here, to run repeatedly:

//  int rbufsize = readBuffer();

//  if (rbufsize > 0 ) {
//    readData();
//  }


  writeData();

  Serial.write(buf, 128);
  Serial.println("\n<-msg");

  readData();

  Serial.println("\n===== json \n");
  memset(rbuf, 0, sizeof(rbuf));

  StreamBuff bo(rbuf, 128);
  StreamBuff bi(buf, 128);

  msgpck_to_json(&bo, &bi);

  Serial.print("json: \n");
  Serial.write(rbuf, 128);
  Serial.print("\n\n");
  
  Serial.println("\n===== end json \n");

  delay(1000);
}
