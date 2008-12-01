void rb_exiv2_throw(const char *file, long unsigned int line, const char *fmt, ...) {
	char* message;
	va_list ap;
	va_start(ap, fmt);
	vasprintf(&message, fmt, ap);
	va_end(ap);
	
	char error_message[strlen(message) + 80];
	snprintf(error_message, sizeof(error_message), "%s. File: %s, line: %lu", message, file, line);
	free(message);
	rb_raise(eError, error_message);
}
