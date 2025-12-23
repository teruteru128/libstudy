// ストレージ関連の機能を実装するファイル
// 例えば、データの保存、読み込み、管理など
// ここでは、基本的なファイル操作やデータ構造の管理を行う関数を実装します。
// ストレージ処理は重いためネットワーク処理とは別スレッドとしたい
#include <bm_storage.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bm_protocol.h> // struct messageの定義のため
#include <endian.h>      // htobe64, htobe32などのため
// ここにストレージ関連の関数を実装します。
