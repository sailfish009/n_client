// n_client.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include "define.h"

// using external libraries nanomsg 
///////////////////////////////////////////////////
#include <Windows.h>
#include <stdio.h>
#include <errno.h>

#include <nanomsg/nn.h>
#include <nanomsg/reqrep.h>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "wsock32.lib")
#pragma comment(lib, "../ext/lib/nanomsg.lib")

const int COMMAND_LENGTH = 2;
const std::string RESULT_OK = "OK";
const std::string RESULT_NG = "NG";

template<typename T>
byte * toB(T& t)
{
  return (byte*)&t;
};

template<typename T>
void fromB(T& t, byte *bytes)
{
  t = *(T*)bytes;
};
///////////////////////////////////////////////////

/*  Return the UNIX time in milliseconds.  You'll need a working
    gettimeofday(), so this won't work on Windows.  */
#include <chrono>
uint64_t milliseconds(void)
{
#ifdef _WIN32
  // SYSTEMTIME st;
  // GetSystemTime(&st);
  return std::chrono::duration_cast<std::chrono::milliseconds>
    (std::chrono::steady_clock::now().time_since_epoch()).count();
#else
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (((uint64_t)tv.tv_sec * 1000) + ((uint64_t)tv.tv_usec / 1000));
#endif
}

/*  The client runs just once, and then returns. */
int client(const char *url, const char *msecstr)
{
  int fd;
  int rc;
  uint8_t msg[BUFFER_SIZE] = "hello, world!";
  unsigned msec;

  // uint64_t start;
  // uint64_t end;

  // measure data
  double data = 0;

  msec = atoi(msecstr);

  fd = nn_socket(AF_SP, NN_REQ);
  if (fd < 0) 
  {
    fprintf(stderr, "nn_socket: %s\n", nn_strerror(nn_errno()));
    return (-1);
  }

  if (nn_connect(fd, url) < 0) 
  {
    fprintf(stderr, "nn_socket: %s\n", nn_strerror(nn_errno()));
    nn_close(fd);
    return (-1);
  }

#if false
  data = htonl(data);
  memcpy(msg, &data, sizeof(data));
#endif

  // msec = htonl(msec);
  // memcpy(msg, &msec, sizeof(msec));
  // start = milliseconds();

  if (nn_send(fd, msg, sizeof(msg), 0) < 0) 
  {
    fprintf(stderr, "nn_send: %s\n", nn_strerror(nn_errno()));
    nn_close(fd);
    return (-1);
  }

  rc = nn_recv(fd, &msg, sizeof(msg), 0);
  if (rc < 0) 
  {
    fprintf(stderr, "nn_recv: %s\n", nn_strerror(nn_errno()));
    nn_close(fd);
    return (-1);
  }

  printf("rc: %d\n", rc);

  nn_close(fd);

  // end = milliseconds();

  // printf("Request took %u milliseconds.\n", (uint32_t)(end - start));
  return (0);
}


int main()
{
  client("tcp://127.0.0.1:5555", "323");

#if false
  double a = 1838023.08202;

  byte *b = toB(a);
  for(int i=0; i<8; ++i){ printf("b: %d ", b[i]);}
  printf("\n");

  double c = 0; 
  fromB(c, b);
  printf("c: %0.5lf\n", c);
#endif

  return 0;
}

