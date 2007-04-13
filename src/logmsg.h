/*
 * Copyright (c) 2002, 2003, 2004 BalaBit IT Ltd, Budapest, Hungary
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 *
 * Note that this permission is granted for only version 2 of the GPL.
 *
 * As an additional exemption you are allowed to compile & link against the
 * OpenSSL libraries as published by the OpenSSL project. See the file
 * COPYING for details.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef LOG_H_INCLUDED
#define LOG_H_INCLUDED

#include <glib.h>

#include "gsockaddr.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>

#define LP_NOPARSE         0x0001
#define LP_INTERNAL        0x0002
#define LP_LOCAL           0x0004
#define LP_CHECK_HOSTNAME  0x0008
#define LP_STRICT	   0x0010

#define LF_UNPARSED 0x0001
#define LF_INTERNAL 0x0002
#define LF_LOCAL    0x0004
#define LF_MARK     0x0008

/* timestamp formats */
#define TS_FMT_BSD   0
#define TS_FMT_ISO   1


struct _LogSourceGroup;
struct _LogMessage;

typedef void (*LMAckFunc)(struct _LogMessage *lm, gpointer user_data);

typedef struct _LogAckBlock
{
  LMAckFunc ack;
  gint req_ack_cnt;
  gint ack_cnt;
  gpointer ack_user_data;
} LogAckBlock;

typedef struct _LogStamp
{
  gboolean frac_present;
  struct timeval time;
  /* zone offset in seconds, add this to UTC to get the time in local */
  glong zone_offset;
} LogStamp;

glong log_stamp_get_zone_offset(LogStamp *stamp, gint tz_convert, glong custom_zone_offset);
void log_stamp_format(LogStamp *stamp, GString *target, gint ts_format, glong zone_offset);

typedef struct _LogMessage
{
  guint ref_cnt;
  /* meta information */
  struct _LogSourceGroup *source_group;
  GSockAddr *saddr;
  guint32 flags;
  
  GSList *ack_blocks;

  /* message parts */
  guint pri;
  LogStamp stamp;
  LogStamp recvd;
  GString *date, *host, *program, *msg;
} LogMessage;

LogMessage *log_msg_ref(LogMessage *m);
void log_msg_unref(LogMessage *m);

LogMessage *log_msg_new(gchar *msg, gint length, GSockAddr *saddr, guint flags);
LogMessage *log_msg_new_mark(void);

void log_msg_ack_block_inc(LogMessage *m);
void log_msg_ack_block_start(LogMessage *m, LMAckFunc func, gpointer user_data);
void log_msg_ack_block_end(LogMessage *m);
void log_msg_ack(LogMessage *msg);
void log_msg_drop(LogMessage *msg, guint path_flags);

#endif
