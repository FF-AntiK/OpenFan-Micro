#ifdef WIFI_ENABLE
#ifndef WIFI_H
#define WIFI_H

const char *wifi_get_mac();
void wifi_loop();
void wifi_reset();
void wifi_setup();

#endif /* WIFI_H */
#endif /* WIFI_ENABLE */