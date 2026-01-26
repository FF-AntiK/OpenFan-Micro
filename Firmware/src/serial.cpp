#ifdef SERIAL_ENABLE
#include "serial.h"

#include <Arduino.h>

#include "fan.h"

#define HEX_SZE(x) (2 * sizeof(x))
#define RX_START '>'
#define TX_START '<'

typedef enum {
  CMD_FAN_GET_RPM,
  CMD_FAN_SET_PWM,
  ERR_INVALID,
} cmd_t;

typedef struct __attribute__((packed)) {
  uint8_t cmd = CMD_FAN_GET_RPM;
} req_fan_get_rpm_t;

typedef struct __attribute__((packed)) {
  uint8_t cmd = CMD_FAN_SET_PWM;
  uint8_t pwm;
} req_fan_set_pwm_t;

typedef union __attribute__((packed)) {
  uint8_t cmd;
  req_fan_get_rpm_t fan_get_rpm;
  req_fan_set_pwm_t fan_set_pwm;
} req_t;

typedef struct __attribute__((packed)) {
  uint8_t cmd = CMD_FAN_GET_RPM;
  uint32_t rpm;
} res_fan_get_rpm_t;

typedef struct __attribute__((packed)) {
  uint8_t cmd = CMD_FAN_SET_PWM;
  uint8_t pwm;
} res_fan_set_pwm_t;

typedef union __attribute__((packed)) {
  uint8_t cmd;
  res_fan_get_rpm_t fan_get_rpm;
  res_fan_set_pwm_t fan_set_pwm;
} res_t;

typedef struct {
  char buf[HEX_SZE(req_t)];
  size_t pos = 0;
} rx_t;

static req_t parse(const rx_t *rx) {
  req_t req = {.cmd = ERR_INVALID};

  if (rx->pos < HEX_SZE(req.cmd)) {
    return req;
  }

  sscanf(rx->buf, "%2hhX", &req.cmd);

  switch (req.cmd) {
  case CMD_FAN_GET_RPM:
    if (rx->pos < HEX_SZE(req.fan_get_rpm)) {
      req.cmd = ERR_INVALID;
      return req;
    }

    break;
  case CMD_FAN_SET_PWM:
    if (rx->pos < HEX_SZE(req.fan_set_pwm)) {
      req.cmd = ERR_INVALID;
      return req;
    }

    sscanf(rx->buf, "%2hhX%2hhX", &req.fan_set_pwm.cmd, &req.fan_set_pwm.pwm);
    break;
  }

  return req;
}

static res_t process(const req_t *req) {
  res_t res = {.cmd = req->cmd};

  switch (req->cmd) {
  case CMD_FAN_GET_RPM:
    res.fan_get_rpm.rpm = fan_get_rpm();
    break;
  case CMD_FAN_SET_PWM:
    fan_set_pwm(req->fan_set_pwm.pwm);
    res.fan_set_pwm.pwm = fan_get_pwm();
    break;
  }

  return res;
}

static void send(const res_t *res) {
  switch (res->cmd) {
  case CMD_FAN_GET_RPM:
    Serial.printf("%c%02X%08X\r\n", TX_START, res->fan_get_rpm.cmd,
                  res->fan_get_rpm.rpm);
    return;
  case CMD_FAN_SET_PWM:
    Serial.printf("%c%02X%02X\r\n", TX_START, res->fan_set_pwm.cmd,
                  res->fan_set_pwm.pwm);
    return;
  }

  Serial.printf("%c%02X\r\n", TX_START, res->cmd);
}

void serial_loop(void) {
  static bool is_req_incoming = false;
  static rx_t rx;

  while (Serial.available() > 0) {
    const char chr = Serial.read();

    if (chr == RX_START) {
      is_req_incoming = true;
      rx.pos = 0;
      continue;
    }

    if (!is_req_incoming) {
      continue;
    }

    if (strchr("\r\n", chr)) {
      const req_t req = parse(&rx);
      const res_t res = process(&req);

      send(&res);
      is_req_incoming = false;
      continue;
    }

    if (rx.pos >= sizeof(rx.buf)) {
      is_req_incoming = false;
      continue;
    }

    rx.buf[rx.pos++] = chr;
  }
}
#endif /* SERIAL_ENABLE */