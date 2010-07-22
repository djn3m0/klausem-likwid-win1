#ifdef WIN32

int isnan(double d) {
	return d != d;
}

#endif
