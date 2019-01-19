#ifndef VALIDATOR_H
#define VALIDATOR_H
int validateInt(const char *str) {
    int len = 0;
    while (str[len] != '\0') {
        len++;
    }

    for (int i = 0; i < len; ++i) {
        if (!(str[i] <= '9' && str[i] >= '0')) {
            return 0;
        }
    }

    return 1;
}

int validateFile(const char *pathname) {
    FILE *file = fopen(pathname, "r");
    if (file == NULL) {
        return 0;
    }
	fclose(file);
    return 1;
}

int validateFileWrite(const char *pathname) {
    FILE *file = fopen(pathname, "w");
    if (file == NULL) {
        return 0;
    }
    fclose(file);
    return 1;
}

#endif