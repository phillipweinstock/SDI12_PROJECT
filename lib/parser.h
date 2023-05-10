#ifndef PARSER_H
#define PARSER_H
#include "macros.h"
#include "sensors.h"
#include <Arduino.h>
#include <DueFlashStorage.h>
#include <libsdi12.h>
#include <malloc.h>
/*
    Developed by Phillip Weinstock student ID 102101219
    Contact 102101219@student.swin.edu.au for all Academic/Business enquirys.
    Copyright 2023© ALL RIGHTS RESERVED.

    Thin wrapper around libsdi12 sensor API.
    Replaces the hand-rolled parser with the library's state machine.
*/
extern char _end;
extern "C" char *sbrk(int i);
char *ramstart = (char *)0x20070000;
char *ramend = (char *)0x20088000;

/* ── libsdi12 callbacks ─────────────────────────────────────────────── */

static DueFlashStorage flash;

static void sdi12_send(const char *data, size_t len, void *) {
  SEND();
  sdi12.write(data, len);
  sdi12.flush();
  CLR12();
}

static void sdi12_dir(sdi12_dir_t dir, void *) {
  if (dir == SDI12_DIR_TX) {
    SENDF;
  } else {
    RECEIVEF;
  }
}

static sdi12_value_t sdi12_read(uint8_t param_index, void *) {
  sdi12_value_t v = {0, 2};
  airoutput air = sensors.airreading();
  uint16_t lux = sensors.lightmeter.readLightLevel();

  switch (param_index) {
  case 0:
    v.value = (float)lux;
    v.decimals = 0;
    break;
  case 1:
    v.value = air.temp;
    v.decimals = 2;
    break;
  case 2:
    v.value = (float)air.pressure;
    v.decimals = 0;
    break;
  case 3:
    v.value = air.humidity;
    v.decimals = 2;
    break;
  case 4:
    v.value = (float)air.gasresistance;
    v.decimals = 0;
    break;
  }
  return v;
}

static void sdi12_save_addr(char address, void *) {
  __set_FAULTMASK(1);
  flash.write(1, address);
  __set_FAULTMASK(0);
}

static char sdi12_load_addr(void *) { return (char)flash.read(1); }

static sdi12_err_t sdi12_xcmd_reset(const char *, char *resp, size_t resp_size,
                                    void *) {
  rstc_start_software_reset(RSTC);
  /* never reached */
  (void)resp;
  (void)resp_size;
  return SDI12_OK;
}

static sdi12_err_t sdi12_xcmd_ramfree(const char *, char *resp,
                                      size_t resp_size, void *) {
  struct mallinfo mi = mallinfo();
  char *heapend = sbrk(0);
  register char *stack_ptr asm("sp");
  int freeram = stack_ptr - heapend + mi.fordblks;
  snprintf(resp, resp_size, "+RAMFREE: %d", freeram);
  return SDI12_OK;
}

/* ── Sensor context (global, initialised in setup) ──────────────────── */

static sdi12_sensor_ctx_t sensor_ctx;

void sdi12_sensor_setup(char address) {
  static const sdi12_ident_t ident = {"SWINBURN", "SDIBRG", "001", ""};

  sdi12_sensor_callbacks_t cb = {};
  cb.send_response = sdi12_send;
  cb.set_direction = sdi12_dir;
  cb.read_param = sdi12_read;
  cb.save_address = sdi12_save_addr;
  cb.load_address = sdi12_load_addr;

  sdi12_sensor_init(&sensor_ctx, address, &ident, &cb);

  /* Group 0: aM!/aC!/aD! — 5 parameters */
  sdi12_sensor_register_param(&sensor_ctx, 0, "RP", "1cd*sr/m2", 0); // Lux
  sdi12_sensor_register_param(&sensor_ctx, 0, "TA", "C", 2);         // Temp
  sdi12_sensor_register_param(&sensor_ctx, 0, "PA", "Kpa", 0);       // Pressure
  sdi12_sensor_register_param(&sensor_ctx, 0, "XR", "Humidity", 2);  // Humidity
  sdi12_sensor_register_param(&sensor_ctx, 0, "GR", "Gas Resistance", 0); // Gas

  /* Extended commands */
  sdi12_sensor_register_xcmd(&sensor_ctx, "RST", sdi12_xcmd_reset);
  sdi12_sensor_register_xcmd(&sensor_ctx, "V", sdi12_xcmd_ramfree);
}

void sdi12_sensor_handle(const char *cmd, size_t len) {
  sdi12_sensor_process(&sensor_ctx, cmd, len);
}

#endif