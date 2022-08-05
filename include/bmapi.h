
#ifndef BMAPI_H
#define BMAPI_H
#include <stdarg.h>
#include <string.h>
#include <xmlrpc.h>
#include <xmlrpc_client.h>

/** TODO: 実装をinternalへ移す */
typedef struct bm_client_t
{
  xmlrpc_client *cp;
  xmlrpc_server_info *sinfo;
} bm_client;

bm_client *bm_client_new();
int bmapi_init(void);
int bmapi_cleanup();
char *bmapi_helloWorld(xmlrpc_env *, xmlrpc_client *, xmlrpc_server_info *, const char *, const char *);
int bmapi_add(int, int);
int bmapi_statusBar();
int bmapi_listAddresses2();
int bmapi_createRandomAddress();
char *bmapi_getDeterministicAddress(xmlrpc_env *, xmlrpc_client *, xmlrpc_server_info *, const char *, int, int);
int bmapi_getAllInboxMessages();
int bmapi_getAllInboxMessageIDs();
int bmapi_getSentMessageByAckData();
int bmapi_getAllSentMessages();
int bmapi_getSentMessageByID();
int bmapi_getSentMessagesBySender();
int bmapi_trashMessages();
/**
  simpleSendMessage(toaddress, fromaddress, subject, message)
  sendMessage(toAddress, fromAddress, subject, message, encodingType, TTL)
*/
#if 0
char *bmapi_simpleSendMessage(const char *, const char *, const char *, const char *);
#endif
__wur int bmapi_sendMessage(xmlrpc_env *, xmlrpc_client *, xmlrpc_server_info *, xmlrpc_value *, xmlrpc_value *, xmlrpc_value *, xmlrpc_value *, xmlrpc_value *, xmlrpc_value *, char **ackData);
int bmapi_sendBroadcast(xmlrpc_env *env, xmlrpc_client *clientP,
                        xmlrpc_server_info *serverP, const char *fromaddress,
                        const char *subject, const char *message,
                        int encodingType, int32_t TTL, char **ackData);
char *bmapi_getStatus(xmlrpc_env *, xmlrpc_client *, xmlrpc_server_info *, const char *) __wur;
int bmapi_listSubscriptions();
int bmapi_addSubscription();
int bmapi_deleteSubscriptions();
int bmapi_listAddressBookEntries();
int bmapi_addAddressBookEntry();
int bmapi_deleteAddressBookEntry();
char *bmapi_createChan(xmlrpc_env *, xmlrpc_client *, xmlrpc_server_info *, const char *) __wur;
int bmapi_deleteAddress();
int bmapi_decodeAddress();
int bmapi_addAddressToBlackWhiteList();
int bmapi_removeAddressFromBlackWhiteList();
int bmapi_clientStatus();

extern void die_if_fault_occurred(xmlrpc_env *env);
#endif
