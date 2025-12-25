// ネットワーク関連の機能を実装するファイル
// ネットワーク多重化+サーバー対応など将来的な拡張を見据えた設計とする
#include <bm_network.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <inttypes.h>
#include <time.h>
#include <sys/random.h>
#include <bm_protocol.h>               // struct messageの定義のため
#include <endian.h>                    // htobe64, htobe32などのため

int epfd = -1;

