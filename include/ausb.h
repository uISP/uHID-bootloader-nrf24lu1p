
#define EP_IN(ep) (0<<7 | (ep))
#define EP_OUT(ep) (1<<7 | (ep))

bool ausb_ep_ready(uint8_t ep);
bool ausb_pullup_ctl(uint8_t ep);


int ausb_epout_get(uint8_t ep, void *data, uint8_t len);
int ausb_epin_put(uint8_t ep, const void *data, uint8_t len);



void *ausb_platform_get_descriptor(int *dlen) {

}
