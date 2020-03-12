I found this struct definition lying in plaintext in the firmware image. I'm not
sure exactly what it's used for:

    struct {
      uint32_t TS;
      uint8_t  Fingerprint[16];
    }
