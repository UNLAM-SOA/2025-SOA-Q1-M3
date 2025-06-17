#pragma once
#include <stdint.h>
#include <string.h>
#include <ArduinoJson.h>

#define JSON_QUEUE_CAPACITY 5
#define JSON_DOC_SIZE 256

typedef struct
{
 StaticJsonDocument<JSON_DOC_SIZE> docs[JSON_QUEUE_CAPACITY];
 int front;
 int rear;
 int count;
} JsonQueue;

void json_queue_init(JsonQueue *q)
{
 q->front = 0;
 q->rear = 0;
 q->count = 0;
}

bool json_queue_enqueue(JsonQueue *q, const StaticJsonDocument<JSON_DOC_SIZE> &doc)
{
 if (q->count >= JSON_QUEUE_CAPACITY)
  return false;

 q->docs[q->rear] = doc;
 q->rear = (q->rear + 1) % JSON_QUEUE_CAPACITY;
 q->count++;
 return true;
}

bool json_queue_dequeue(JsonQueue *q, StaticJsonDocument<JSON_DOC_SIZE> &out)
{
 if (q->count <= 0)
  return false;

 out = q->docs[q->front];
 q->front = (q->front + 1) % JSON_QUEUE_CAPACITY;
 q->count--;
 return true;
}

bool json_queue_is_empty(const JsonQueue *q)
{
 return q->count == 0;
}

bool json_queue_is_full(const JsonQueue *q)
{
 return q->count == JSON_QUEUE_CAPACITY;
}

int json_queue_size(const JsonQueue *q)
{
 return q->count;
}

bool json_queue_peek(const JsonQueue *q, StaticJsonDocument<JSON_DOC_SIZE> &out)
{
 if (q->count <= 0)
  return false;

 out = q->docs[q->front];
 return true;
}