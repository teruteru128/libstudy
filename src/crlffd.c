
#include <iconv.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int convertutf8tosjis(char *utf8buf, size_t utf8buflen, char *sjisbuf,
                      size_t sjisbuflen)
{
    char *work_newbuf = utf8buf;
    size_t work_newbuflen = utf8buflen;
    char *work_sjisbuf = sjisbuf;
    size_t work_sjisbuflen = sjisbuflen;

    iconv_t ic = iconv_open("SJIS", "UTF-8");
    if (ic == (iconv_t)-1)
    {
        perror("iconv_open");
        return 1;
    }
    iconv(ic, &work_newbuf, &work_newbuflen, &work_sjisbuf, &work_sjisbuflen);
    iconv_close(ic);

    return 0;
}

/**
 * @brief line を SJISに変換して、 CRLF とともに fd に書き込みます。
 * XXX: return
 * @param fd
 * @param line
 * @return int succeess: 0, fail: 1
 */
int writeline(int fd, const char *line)
{
    size_t oldstrlen = strlen(line);
    // concat crlf
    size_t crlfbuflen = oldstrlen + 3;
    char *crlfbuf = malloc(crlfbuflen);
    crlfbuf[0] = '\0';
    strcat(crlfbuf, line);
    strcat(crlfbuf + oldstrlen, "\r\n");

    // ここで文字コード変換
    char *sjisbuf = malloc(crlfbuflen);
    convertutf8tosjis(crlfbuf, crlfbuflen, sjisbuf, crlfbuflen);
    char *minimizedbuf = realloc(sjisbuf, strlen(minimizedbuf) + 1);
    if (minimizedbuf != NULL)
    {
        sjisbuf = minimizedbuf;
    }

    // write で送信
    ssize_t writesize = write(fd, sjisbuf, strlen(sjisbuf));

    // 後片付け
    free(crlfbuf);
    free(sjisbuf);

    return writesize >= 0 ? 0 : 1;
}
