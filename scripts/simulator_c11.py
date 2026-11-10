"""Use a pre-C23 C dialect for the native simulator's QRCode dependency."""

Import("env")

env.Append(CFLAGS=["-std=gnu11"])
