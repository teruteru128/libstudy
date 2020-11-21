
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdio.h>
#include "bm.h"
#include "bmapi.h"
#include "xmlrpc.h"
#include "base64.h"
#include <stdlib.h>

const endpointinfo_t server_addresses_list[] = {
    {"p2pquake.dyndns.info", "6910"},
    {"www.p2pquake.net", "6910"},
    {"p2pquake.dnsalias.net", "6910"},
    {"p2pquake.ddo.jp", "6910"},
    {"", ""}};

static int initflag = 0;
static xmlrpc_env env;
static xmlrpc_client *cp;
static xmlrpc_server_info *sinfo;

typedef struct bm_client_t bm_client;

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

bm_client *bm_client_new()
{
  if (initflag == 0)
  {
    return NULL;
  }
  bm_client *c = malloc(sizeof(bm_client));
  xmlrpc_client_create(&env, XMLRPC_CLIENT_NO_FLAGS, PACKAGE_NAME, PACKAGE_VERSION, NULL, 0, &c->cp);
  c->sinfo = xmlrpc_server_info_new(&env, "http://localhost:8442/");
  xmlrpc_server_info_set_user(&env, c->sinfo, "teruteru128", "testpassword");
  xmlrpc_server_info_allow_auth_basic(&env, c->sinfo);
  return c;
}

void bm_client_set_server(bm_client *c, const char *const url)
{
  c->sinfo = xmlrpc_server_info_new(&env, url);
}

void bm_client_set_user(bm_client *c, const char *const username, const char *const password)
{
  xmlrpc_server_info_set_user(&env, c->sinfo, "teruteru128", "testpassword");
}

void bm_client_free(bm_client *c)
{
  return;
}

int bmapi_init()
{
  if (initflag == 0)
  {
    xmlrpc_env_init(&env);
    xmlrpc_init(&env);
    xmlrpc_client_setup_global_const(&env);
    initflag = 1;
  }
  return 0;
}

int bmapi_cleanup()
{
  if (initflag == 1)
  {
    xmlrpc_client_setup_global_const(&env);
    xmlrpc_env_clean(&env);
    xmlrpc_term();
    initflag = 0;
  }
  return 0;
}

static void bmapi_call(const char *const methodName, xmlrpc_value *paramArray, xmlrpc_value **resultPP)
{
  xmlrpc_client_call2(&env, cp, sinfo, methodName, paramArray, resultPP);
}

/***/
const char *bmapi_helloWorld(const char *first, const char *second)
{
  if (initflag != 1)
  {
    return NULL;
  }

  // create array
  xmlrpc_value *paramArray = xmlrpc_array_new(&env);
  xmlrpc_value *f = xmlrpc_string_new(&env, first);
  xmlrpc_value *s = xmlrpc_string_new(&env, second);
  xmlrpc_array_append_item(&env, paramArray, f);
  xmlrpc_array_append_item(&env, paramArray, s);

  xmlrpc_value *resultP = NULL;
  bmapi_call("helloWorld", paramArray, &resultP);
  die_if_fault_occurred(&env);

  const char *msg = NULL;
  xmlrpc_read_string(&env, resultP, &msg);
  xmlrpc_DECREF(paramArray);
  xmlrpc_DECREF(resultP);

  return msg;
}

const char *bmapi_getStatus(const char *ackData)
{
  const char methodName[] = "getStatus";

  xmlrpc_value *paramArray = xmlrpc_array_new(&env);
  xmlrpc_value *ack_xml = xmlrpc_string_new(&env, ackData);
  xmlrpc_array_append_item(&env, paramArray, ack_xml);
  xmlrpc_value *resultP = NULL;
  bmapi_call(methodName, paramArray, &resultP);
  const char *msg = NULL;
  xmlrpc_read_string(&env, resultP, &msg);

  return msg;
}

const char *bmapi_simpleSendMessage(const char *toaddress, const char *fromaddress, const char *subject, const char *message)
{
  return bmapi_sendMessage(toaddress, fromaddress, subject, message, 2, 345600); // 4 * 24 * 60 * 60
}

const char *bmapi_sendMessage(const char *toaddress, const char *fromaddress, const char *subject, const char *message, int encodingType, int TTL)
{
  const char methodName[] = "sendMessage";

  xmlrpc_value *paramArray = NULL;
  paramArray = xmlrpc_array_new(&env);
  die_if_fault_occurred(&env);
  xmlrpc_value *toaddressV = NULL;
  toaddressV = xmlrpc_string_new(&env, toaddress);
  die_if_fault_occurred(&env);

  xmlrpc_value *fromaddressV = NULL;
  fromaddressV = xmlrpc_string_new(&env, fromaddress);
  die_if_fault_occurred(&env);

  char *subb64tmp = NULL;
  subb64tmp = base64encode(subject, strlen(subject));
  xmlrpc_value *subjectV = NULL;
  subjectV = xmlrpc_string_new(&env, subb64tmp);
  die_if_fault_occurred(&env);

  char *msgb64tmp = NULL;
  msgb64tmp = base64encode(message, strlen(message));
  xmlrpc_value *messageV = NULL;
  messageV = xmlrpc_string_new(&env, msgb64tmp);
  die_if_fault_occurred(&env);

  xmlrpc_value *encodingTypeP = NULL;
  encodingTypeP = xmlrpc_int_new(&env, encodingType);
  die_if_fault_occurred(&env);

  xmlrpc_value *TTLP = NULL;
  TTLP = xmlrpc_int_new(&env, TTL);
  die_if_fault_occurred(&env);

  xmlrpc_array_append_item(&env, paramArray, toaddressV);
  die_if_fault_occurred(&env);
  xmlrpc_array_append_item(&env, paramArray, fromaddressV);
  die_if_fault_occurred(&env);
  xmlrpc_array_append_item(&env, paramArray, subjectV);
  die_if_fault_occurred(&env);
  xmlrpc_array_append_item(&env, paramArray, messageV);
  die_if_fault_occurred(&env);
  xmlrpc_array_append_item(&env, paramArray, encodingTypeP);
  die_if_fault_occurred(&env);
  xmlrpc_array_append_item(&env, paramArray, TTLP);
  die_if_fault_occurred(&env);

  xmlrpc_value *resultP = NULL;
  bmapi_call(methodName, paramArray, &resultP);
  die_if_fault_occurred(&env);

  const char *msg = NULL;
  xmlrpc_read_string(&env, resultP, &msg);
  xmlrpc_DECREF(paramArray);
  xmlrpc_DECREF(resultP);

  die_if_fault_occurred(&env);

  return msg;
}

const char *bmapi_getDeterministicAddress(const char *pass, int addver, int stream)
{
  const char *methodName = "getDeterministicAddress";

  xmlrpc_value *paramArray = xmlrpc_array_new(&env);
  char *passb64 = base64encode(pass, strlen(pass));
  xmlrpc_value *passphrase = xmlrpc_string_new(&env, passb64);
  xmlrpc_value *add = xmlrpc_int_new(&env, addver);
  xmlrpc_value *s = xmlrpc_int_new(&env, stream);

  xmlrpc_array_append_item(&env, paramArray, passphrase);
  xmlrpc_array_append_item(&env, paramArray, add);
  xmlrpc_array_append_item(&env, paramArray, s);

  xmlrpc_value *resultP = NULL;
  bmapi_call(methodName, paramArray, &resultP);

  const char *msg = NULL;
  xmlrpc_read_string(&env, resultP, &msg);
  xmlrpc_DECREF(paramArray);
  xmlrpc_DECREF(resultP);
  return msg;
}

const char *bmapi_createChan(const char *passphrase)
{
  const char methodName[] = "sendMessage";
  const char *address = NULL;
  xmlrpc_value *encodingTypeP = NULL;
  char *adb64tmp = base64encode(passphrase, strlen(passphrase));
  printf("%s, %s\n", passphrase, adb64tmp);
  size_t len = strlen(adb64tmp);
  char *adbtmp2 = calloc(sizeof(char), len + 2);
  memcpy(adbtmp2, adb64tmp, len);
  strncat(adbtmp2 + len, "\n", 1);

  xmlrpc_value *passArray = NULL;
  passArray = xmlrpc_array_new(&env);
  die_if_fault_occurred(&env);

  xmlrpc_value *passV = NULL;
  passV = xmlrpc_string_new(&env, adbtmp2);
  die_if_fault_occurred(&env);
  //  encodingTypeP = xmlrpc_int_new(&env, 2);
  //  die_if_fault_occurred(&env);

  xmlrpc_array_append_item(&env, passArray, passV);
  die_if_fault_occurred(&env);
  //  xmlrpc_array_append_item(&env, passArray, encodingTypeP);
  //  die_if_fault_occurred(&env);
  xmlrpc_value *resultP = NULL;
  bmapi_call(methodName, passArray, &resultP);
  die_if_fault_occurred(&env);
  printf("%s\n", xmlrpc_type_name(xmlrpc_value_type(resultP)));
  xmlrpc_read_string(&env, resultP, &address);
  xmlrpc_DECREF(passArray);
  xmlrpc_DECREF(resultP);
  free(adb64tmp);
  free(adbtmp2);
  return address;
}
