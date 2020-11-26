
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#include "bm.h"
#include "bmapi.h"
#include "xmlrpc.h"
#include "base64.h"

const endpointinfo_t server_addresses_list[5] = {
    {"p2pquake.dyndns.info", "6910"},
    {"www.p2pquake.net", "6910"},
    {"p2pquake.dnsalias.net", "6910"},
    {"p2pquake.ddo.jp", "6910"},
    {"", ""}};

void die_if_fault_occurred(xmlrpc_env *e)
{
  /* Check our error-handling environment for an XML-RPC fault. */
  if (e->fault_occurred)
  {
    fprintf(stderr, "XML-RPC Fault: %s (%d)\n",
            e->fault_string, e->fault_code);
    exit(1);
  }
}

#define HELLO_WORLD_METHOD_NAME "helloWorld"

/**
 * @brief 
 * 
 * @param first 
 * @param second 
 * @return const char* 
 */
const char *bmapi_helloWorld(xmlrpc_env *env, xmlrpc_client *clientP, xmlrpc_server_info *serverP, const char *first, const char *second)
{

  // create array
  xmlrpc_value *paramArray = xmlrpc_array_new(env);
  xmlrpc_value *f = xmlrpc_string_new(env, first);
  xmlrpc_value *s = xmlrpc_string_new(env, second);
  xmlrpc_array_append_item(env, paramArray, f);
  xmlrpc_array_append_item(env, paramArray, s);

  xmlrpc_value *resultP = NULL;
  xmlrpc_client_call2(env, clientP, serverP, HELLO_WORLD_METHOD_NAME, paramArray, &resultP);
  die_if_fault_occurred(env);

  const char *msg = NULL;
  xmlrpc_read_string(env, resultP, &msg);
  xmlrpc_DECREF(paramArray);
  xmlrpc_DECREF(resultP);

  return msg;
}

#define GET_STATUS_METHOD_NAME "getStatus"

const char *bmapi_getStatus(xmlrpc_env *env, xmlrpc_client *clientP, xmlrpc_server_info *serverP, const char *ackData)
{
  xmlrpc_value *paramArray = xmlrpc_array_new(env);
  xmlrpc_value *ack_xml = xmlrpc_string_new(env, ackData);
  xmlrpc_array_append_item(env, paramArray, ack_xml);
  xmlrpc_value *resultP = NULL;
  xmlrpc_client_call2(env, clientP, serverP, GET_STATUS_METHOD_NAME, paramArray, &resultP);
  const char *msg = NULL;
  xmlrpc_read_string(env, resultP, &msg);

  return msg;
}

/*
const char *bmapi_simpleSendMessage(const char *toaddress, const char *fromaddress, const char *subject, const char *message)
{
  return bmapi_sendMessage(toaddress, fromaddress, subject, message, 2, 345600); // 4 * 24 * 60 * 60
}
*/

#define SEND_MESSAGE_METHOD_NAME "sendMessage"

const char *bmapi_sendMessage(xmlrpc_env *env, xmlrpc_client *clientP, xmlrpc_server_info *serverP, xmlrpc_value *toaddressV, xmlrpc_value *fromaddressV, xmlrpc_value *subjectV, xmlrpc_value *messageV, xmlrpc_value *encodingTypeV, xmlrpc_value *TTLV)
{
  xmlrpc_value *paramArray = xmlrpc_array_new(env);
  die_if_fault_occurred(env);

  xmlrpc_array_append_item(env, paramArray, toaddressV);
  die_if_fault_occurred(env);
  xmlrpc_array_append_item(env, paramArray, fromaddressV);
  die_if_fault_occurred(env);
  xmlrpc_array_append_item(env, paramArray, subjectV);
  die_if_fault_occurred(env);
  xmlrpc_array_append_item(env, paramArray, messageV);
  die_if_fault_occurred(env);
  xmlrpc_array_append_item(env, paramArray, encodingTypeV);
  die_if_fault_occurred(env);
  xmlrpc_array_append_item(env, paramArray, TTLV);
  die_if_fault_occurred(env);

  xmlrpc_value *resultP = NULL;
  xmlrpc_client_call2(env, clientP, serverP, SEND_MESSAGE_METHOD_NAME, paramArray, &resultP);
  die_if_fault_occurred(env);

  const char *msg = NULL;
  xmlrpc_read_string(env, resultP, &msg);
  die_if_fault_occurred(env);

  xmlrpc_DECREF(paramArray);
  //xmlrpc_DECREF(toaddressV);
  //xmlrpc_DECREF(fromaddressV);
  //xmlrpc_DECREF(subjectV);
  //xmlrpc_DECREF(messageV);
  //xmlrpc_DECREF(encodingTypeV);
  //xmlrpc_DECREF(TTLV);
  //xmlrpc_DECREF(resultP);

  return msg;
}

#define GET_DETERMINISTIC_ADDRESS_METHOD_NAME "getDeterministicAddress"

const char *bmapi_getDeterministicAddress(xmlrpc_env *env, xmlrpc_client *clientP, xmlrpc_server_info *serverP, const char *pass, int addver, int stream)
{
  xmlrpc_value *paramArray = xmlrpc_array_new(env);
  char *passb64 = base64encode(pass, strlen(pass));
  xmlrpc_value *passphrase = xmlrpc_string_new(env, passb64);
  xmlrpc_value *add = xmlrpc_int_new(env, addver);
  xmlrpc_value *s = xmlrpc_int_new(env, stream);

  xmlrpc_array_append_item(env, paramArray, passphrase);
  xmlrpc_array_append_item(env, paramArray, add);
  xmlrpc_array_append_item(env, paramArray, s);

  xmlrpc_value *resultP = NULL;
  xmlrpc_client_call2(env, clientP, serverP, GET_DETERMINISTIC_ADDRESS_METHOD_NAME, paramArray, &resultP);

  const char *msg = NULL;
  xmlrpc_read_string(env, resultP, &msg);
  xmlrpc_DECREF(paramArray);
  xmlrpc_DECREF(resultP);
  return msg;
}

#define CREATE_CHAN_METHOD_NAME "createChan"

const char *bmapi_createChan(xmlrpc_env *env, xmlrpc_client *clientP, xmlrpc_server_info *serverP, const char *passphrase)
{
  const char *address = NULL;
  char *adb64tmp = base64encode(passphrase, strlen(passphrase));
  printf("%s, %s\n", passphrase, adb64tmp);
  size_t len = strlen(adb64tmp);
  char *adbtmp2 = calloc(sizeof(char), len + 2);
  memcpy(adbtmp2, adb64tmp, len);
  strncat(adbtmp2 + len, "\n", 1);

  xmlrpc_value *passArray = NULL;
  passArray = xmlrpc_array_new(env);
  die_if_fault_occurred(env);

  xmlrpc_value *passV = NULL;
  passV = xmlrpc_string_new(env, adbtmp2);
  die_if_fault_occurred(env);

  xmlrpc_array_append_item(env, passArray, passV);
  die_if_fault_occurred(env);
  xmlrpc_value *resultP = NULL;
  xmlrpc_client_call2(env, clientP, serverP, CREATE_CHAN_METHOD_NAME, passArray, &resultP);
  die_if_fault_occurred(env);
  printf("%s\n", xmlrpc_type_name(xmlrpc_value_type(resultP)));
  xmlrpc_read_string(env, resultP, &address);
  xmlrpc_DECREF(passArray);
  xmlrpc_DECREF(resultP);
  free(adb64tmp);
  free(adbtmp2);
  return address;
}
