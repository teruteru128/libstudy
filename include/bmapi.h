
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
const char *bmapi_helloWorld(const char *, const char *);
int bmapi_add(int, int);
int bmapi_statusBar();
int bmapi_listAddresses2();
int bmapi_createRandomAddress();
const char *bmapi_getDeterministicAddress(const char *, int, int);
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
const char *bmapi_simpleSendMessage(const char *, const char *, const char *, const char *);
const char *bmapi_sendMessage(const char *, const char *, const char *, const char *, const int, int);
int bmapi_sendBroadcast();
const char *bmapi_getStatus(const char *);
int bmapi_listSubscriptions();
int bmapi_addSubscription();
int bmapi_deleteSubscriptions();
int bmapi_listAddressBookEntries();
int bmapi_addAddressBookEntry();
int bmapi_deleteAddressBookEntry();
const char *bmapi_createChan(const char *);
int bmapi_deleteAddress();
int bmapi_decodeAddress();
int bmapi_addAddressToBlackWhiteList();
int bmapi_removeAddressFromBlackWhiteList();
int bmapi_clientStatus();
#endif
