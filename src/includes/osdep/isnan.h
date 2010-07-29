#ifdef WIN32

static int isnan(double d) {
	return d != d;
}

#endif
