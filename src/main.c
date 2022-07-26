//
// ReactorMQ Command-Line Interface
// main.c
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <hiredis/hiredis.h>
#include "../deps/crossline.h"

#define DEFAULT_HOST    "127.0.0.1"
#define DEFAULT_PORT    9876
#define COMMAND_MAX_LEN 1023
#define PROMPT_MAX_LEN  63

static inline redisContext *connect(const char *host, int port);
static inline void print_reply(redisReply *reply);

static inline redisContext *connect(const char *host, int port)
{
  redisContext *ctx = redisConnect(host, port);
  if (!ctx)
  {
    fprintf(stderr, "Error: out of memory\n");
    exit(EXIT_FAILURE);
  }
  if (ctx->err)
  {
    fprintf(stderr, "Error: %s\n", ctx->errstr);
    exit(EXIT_FAILURE);
  }
  return ctx;
}

static inline void print_reply(redisReply *reply)
{
  switch (reply->type)
  {
  case REDIS_REPLY_STRING:
  case REDIS_REPLY_STATUS:
    printf("\"%.*s\"", (int) reply->len, reply->str);
    break;
  case REDIS_REPLY_ARRAY:
    {
      printf("[");
      size_t count = reply->elements;
      for (size_t i = 0; i < count; ++i)
      {
        if (i > 0)
          printf(", ");
        print_reply(reply->element[i]);
      }
      printf("]");
    }
    break;
  case REDIS_REPLY_INTEGER:
    printf("%lld", reply->integer);
    break;
  case REDIS_REPLY_NIL:
    printf("null");
    break;
  case REDIS_REPLY_ERROR:
    printf("(error) %.*s", (int) reply->len, reply->str);
    break;
  case REDIS_REPLY_DOUBLE:
    printf("%f", reply->dval);
    break;
  case REDIS_REPLY_BOOL:
    printf("%s", reply->integer ? "true" : "false");
    break;
  case REDIS_REPLY_MAP:
    {
      printf("{");
      size_t count = reply->elements;
      for (size_t i = 0; i < count; i += 2)
      {
        if (i > 0)
          printf(", ");
        print_reply(reply->element[i]);
        printf(": ");
        print_reply(reply->element[i + 1]);
      }
      printf("}");
    }
    break;
  default:
    fprintf(stderr, "Error: unsupported reply type\n");
    exit(EXIT_FAILURE);
  }
}

int main(int argc, const char *argv[])
{
  const char *host = argc > 1 ? argv[1] : DEFAULT_HOST;
  int port = argc > 2 ? atoi(argv[2]) : DEFAULT_PORT;

  redisContext *ctx = connect(host, port);

  char prompt[PROMPT_MAX_LEN + 1];
  snprintf(prompt, PROMPT_MAX_LEN, "%s:%d> ", host, port);

  char command[COMMAND_MAX_LEN + 1];

  while (crossline_readline(prompt, command, COMMAND_MAX_LEN))
  {
    if (!strlen(command))
      continue;

    if (!strcmp(command, "exit") || !strcmp(command, "quit"))
      break;

    redisReply *reply = redisCommand(ctx, command);
    if (!reply)
    {
      fprintf(stderr, "Error: %s\n", ctx->errstr);
      exit(EXIT_FAILURE);
    }

    print_reply(reply);
    printf("\n");
    freeReplyObject(reply);
  }

  redisFree(ctx);
  return EXIT_SUCCESS;
}
