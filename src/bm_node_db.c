// SQLiteへの保存・読み込みロジックを担当。
// BMノードデータベースの管理。
#include <bm_node_db.h>
#include <stdio.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

sqlite3 *db = NULL;

int bm_db_init(const char *path)
{
    int rc = sqlite3_open("bm.db", &db);
    if (rc)
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

typedef enum
{
    BM_ADDR_TYPE_IPV4 = 1,
    BM_ADDR_TYPE_IPV6 = 2,
    BM_ADDR_TYPE_ONION_V3 = 3
} bm_addr_type_t;

// struct addr_messageはバイナリ形式なのでテキスト形式に変換しなければならない
typedef struct addr
{
    bm_addr_type_t type;
    union
    {
        uint8_t ipv6[16];     // IPv4はIPv6射影アドレスとして保持
        uint8_t onion_v3[35]; // v3アドレスの公開鍵バイナリデータ
    } addr;
    uint16_t port;
    uint64_t services;
    int64_t last_seen;
    int stream_number;
} bm_node_t;

typedef struct addr_list
{
    uint64_t count;
    bm_node_t *nodes;
} bm_node_list_t;

int bm_db_add_node(bm_node_t *addr_msg)
{
    sqlite3_stmt *res = NULL;
    sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
    const char *sql = "INSERT OR REPLACE INTO KNOWN_NODES(ADDRESS, PORT, address_type, LAST_SEEN, stream) VALUES(?, ?, ?, ?, ?)";
    sqlite3_prepare_v2(db, sql, -1, &res, 0);
    sqlite3_bind_text(res, 1, "", -1, SQLITE_STATIC);
    sqlite3_bind_int(res, 2, -1);
    sqlite3_bind_int(res, 3, -1);
    sqlite3_bind_int64(res, 3, time(NULL));
    sqlite3_step(res);
    // sqlite3_reset(res);
    // sqlite3_clear_bindings(res);
    sqlite3_finalize(res);
    sqlite3_exec(db, "COMMIT;", NULL, NULL, NULL);
    return EXIT_SUCCESS;
}

int bm_db_get_best_nodes()
{
    return EXIT_SUCCESS;
}

void bm_db_close()
{
    sqlite3_finalize(db); // 解放
    db = NULL;
}
