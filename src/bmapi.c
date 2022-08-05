
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "base64.h"
#include "bm.h"
#include "bmapi.h"
#include "xmlrpc.h"
#include <stdio.h>
#include <stdlib.h>

const endpointinfo_t server_addresses_list[5]
    = { { "p2pquake.dyndns.info", "6910" },
        { "www.p2pquake.net", "6910" },
        { "p2pquake.dnsalias.net", "6910" },
        { "p2pquake.ddo.jp", "6910" },
        { "", "" } };

// error check function
void die_if_fault_occurred(xmlrpc_env *e)
{
    /* Check our error-handling environment for an XML-RPC fault. */
    if (e->fault_occurred)
    {
        fprintf(stderr, "XML-RPC Fault: %d %s (%d)\n", e->fault_occurred,
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
char *bmapi_helloWorld(xmlrpc_env *env, xmlrpc_client *clientP,
                       xmlrpc_server_info *serverP, const char *first,
                       const char *second)
{

    // create array
    xmlrpc_value *paramArray = xmlrpc_array_new(env);
    xmlrpc_value *f = xmlrpc_string_new(env, first);
    xmlrpc_value *s = xmlrpc_string_new(env, second);
    xmlrpc_array_append_item(env, paramArray, f);
    xmlrpc_array_append_item(env, paramArray, s);

    xmlrpc_value *resultP = NULL;
    xmlrpc_client_call2(env, clientP, serverP, HELLO_WORLD_METHOD_NAME,
                        paramArray, &resultP);
    die_if_fault_occurred(env);

    const char *msg = NULL;
    xmlrpc_read_string(env, resultP, &msg);
    xmlrpc_DECREF(paramArray);
    xmlrpc_DECREF(resultP);

    return strdup(msg);
}

#define GET_STATUS_METHOD_NAME "getStatus"

char *bmapi_getStatus(xmlrpc_env *env, xmlrpc_client *clientP,
                      xmlrpc_server_info *serverP, const char *ackData)
{
    xmlrpc_value *paramArray = xmlrpc_array_new(env);
    xmlrpc_value *ack_xml = xmlrpc_string_new(env, ackData);
    xmlrpc_array_append_item(env, paramArray, ack_xml);
    xmlrpc_value *resultP = NULL;
    xmlrpc_client_call2(env, clientP, serverP, GET_STATUS_METHOD_NAME,
                        paramArray, &resultP);
    const char *msg = NULL;
    xmlrpc_read_string(env, resultP, &msg);
    xmlrpc_DECREF(paramArray);
    xmlrpc_DECREF(ack_xml);
    xmlrpc_DECREF(resultP);

    return (char *)msg;
}

#if 0
const char *bmapi_simpleSendMessage(const char *toaddress,
                                    const char *fromaddress,
                                    const char *subject, const char *message)
{
    return bmapi_sendMessage(toaddress, fromaddress, subject, message, 2,
                             345600); // 4 * 24 * 60 * 60
}
#endif

#define SEND_MESSAGE_METHOD_NAME "sendMessage"

/**
 * @brief
 *
 * @param env error environment variable
 * @param clientP client object
 * @param serverP auth config object
 * @param toaddressV to address xmlrpc value
 * @param fromaddressV from address xmlrpc value
 * @param subjectV Base64 encoded subject xmlrpc value
 * @param messageV Base64 encoded message xmlrpc value
 * @param encodingTypeV encoding type xmlrpc value. If you specify this
 * parameter, the value is always 2.
 * @param TTLV ttl xmlrpc value. 3600 <= ttl <= 2419200
 * @param ackData If the ackData parameter is non-NULL, the ackData string(char *) will be set. This should be freed.
 */
__wur int bmapi_sendMessage(xmlrpc_env *env, xmlrpc_client *clientP,
                            xmlrpc_server_info *serverP,
                            xmlrpc_value *toaddressV,
                            xmlrpc_value *fromaddressV, xmlrpc_value *subjectV,
                            xmlrpc_value *messageV,
                            xmlrpc_value *encodingTypeV, xmlrpc_value *TTLV,
                            char **ackData)
{
    // create args array
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

    // call xmlrpc api
    xmlrpc_value *resultP = NULL;
    xmlrpc_client_call2(env, clientP, serverP, SEND_MESSAGE_METHOD_NAME,
                        paramArray, &resultP);
    die_if_fault_occurred(env);

    const char *msg = NULL;
    xmlrpc_read_string(env, resultP, &msg);
    die_if_fault_occurred(env);

    if (ackData != NULL)
    {
        *ackData = strdup(msg);
    }
    free((void *)msg);
    msg = NULL;

    xmlrpc_DECREF(paramArray);
    // xmlrpc_DECREF(toaddressV);
    // xmlrpc_DECREF(fromaddressV);
    // xmlrpc_DECREF(subjectV);
    // xmlrpc_DECREF(messageV);
    // xmlrpc_DECREF(encodingTypeV);
    // xmlrpc_DECREF(TTLV);
    xmlrpc_DECREF(resultP);

    return 0;
}

#define GET_DETERMINISTIC_ADDRESS_METHOD_NAME "getDeterministicAddress"

char *bmapi_getDeterministicAddress(xmlrpc_env *env, xmlrpc_client *clientP,
                                    xmlrpc_server_info *serverP,
                                    const char *pass, int addver, int stream)
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
    xmlrpc_client_call2(env, clientP, serverP,
                        GET_DETERMINISTIC_ADDRESS_METHOD_NAME, paramArray,
                        &resultP);

    const char *msg = NULL;
    xmlrpc_read_string(env, resultP, &msg);
    xmlrpc_DECREF(paramArray);
    xmlrpc_DECREF(resultP);
    return (char *)msg;
}

#define CREATE_CHAN_METHOD_NAME "createChan"

char *bmapi_createChan(xmlrpc_env *env, xmlrpc_client *clientP,
                       xmlrpc_server_info *serverP, const char *passphrase)
{
    const char *address = NULL;
    char *adb64tmp = base64encode(passphrase, strlen(passphrase));
    printf("%s, %s\n", passphrase, adb64tmp);
    size_t len = strlen(adb64tmp);
    char *adbtmp2 = calloc(sizeof(char), len + 2);
    memcpy(adbtmp2, adb64tmp, len);
    strncat(adbtmp2 + len, "\n", 2);

    xmlrpc_value *passArray = NULL;
    passArray = xmlrpc_array_new(env);
    die_if_fault_occurred(env);

    xmlrpc_value *passV = NULL;
    passV = xmlrpc_string_new(env, adbtmp2);
    die_if_fault_occurred(env);

    xmlrpc_array_append_item(env, passArray, passV);
    die_if_fault_occurred(env);
    xmlrpc_value *resultP = NULL;
    xmlrpc_client_call2(env, clientP, serverP, CREATE_CHAN_METHOD_NAME,
                        passArray, &resultP);
    die_if_fault_occurred(env);
    printf("%s\n", xmlrpc_type_name(xmlrpc_value_type(resultP)));
    xmlrpc_read_string(env, resultP, &address);
    xmlrpc_DECREF(passArray);
    xmlrpc_DECREF(resultP);
    free(adb64tmp);
    free(adbtmp2);
    return (char *)address;
}

#define SEND_BROADCAST_METHOD_NAME "sendBroadcast"

/**
 * @brief
 *
 * @param env
 * @param clientP
 * @param serverP
 * @param fromaddress
 * @param subject
 * @param message
 * @param encodingType
 * @param TTL
 * @param ackData
 * @return int
 */
int bmapi_sendBroadcast(xmlrpc_env *env, xmlrpc_client *clientP,
                        xmlrpc_server_info *serverP, const char *fromaddress,
                        const char *subject, const char *message,
                        int encodingType, int32_t TTL, char **ackData)
{
    // FIXME: 単純化 OR コンストラクタ呼び出し回数削減
    xmlrpc_value *fromaddressV = xmlrpc_string_new(env, fromaddress);
    die_if_fault_occurred(env);
    xmlrpc_value *subjectV = xmlrpc_string_new(env, subject);
    die_if_fault_occurred(env);
    xmlrpc_value *messageV = xmlrpc_string_new(env, message);
    die_if_fault_occurred(env);
    xmlrpc_value *encodingTypeV = xmlrpc_int_new(env, encodingType);
    die_if_fault_occurred(env);
    xmlrpc_value *TTLV = xmlrpc_int_new(env, TTL);
    // create args array
    xmlrpc_value *paramArray = xmlrpc_array_new(env);
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

    // call xmlrpc api
    xmlrpc_value *resultP = NULL;
    xmlrpc_client_call2(env, clientP, serverP, SEND_BROADCAST_METHOD_NAME,
                        paramArray, &resultP);
    die_if_fault_occurred(env);

    const char *msg = NULL;
    xmlrpc_read_string(env, resultP, &msg);
    die_if_fault_occurred(env);

    if (ackData != NULL)
    {
        *ackData = strdup(msg);
    }
    free((void *)msg);
    msg = NULL;

    xmlrpc_DECREF(paramArray);
    xmlrpc_DECREF(fromaddressV);
    xmlrpc_DECREF(subjectV);
    xmlrpc_DECREF(messageV);
    xmlrpc_DECREF(encodingTypeV);
    xmlrpc_DECREF(TTLV);
    xmlrpc_DECREF(resultP);
    return 0;
}
