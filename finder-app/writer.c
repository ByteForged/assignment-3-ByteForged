#include <syslog.h>
#include <stdio.h>

int main(int argc, const char* argv[]) {
    openlog(NULL, 0, LOG_USER);
    
    if (argc != 3) {
        syslog(LOG_ERR, "Must specify 2 arguements: ./writer <file> <string>");
        closelog();
        return 1;
    } else {
        // Assume the directory was created by the test script
        FILE* file = fopen(argv[1], "w");
        if (file) {
            syslog(LOG_DEBUG, "Writing %s to %s", argv[2], argv[1]);
            if (fprintf(file, "%s\n", argv[2]) < 0) {
                syslog(LOG_ERR, "Failed to write \"%s\" to \"%s\"", argv[2], argv[1]);
                fclose(file);
                closelog();
                return 1;
            }
            fclose(file);
        } else {
            syslog(LOG_ERR, "Failed to open \"%s\"", argv[1]);
            closelog();
            return 1;
        }
    }
    closelog();
    return 0;
}
