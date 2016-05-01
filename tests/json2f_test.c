#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "../external/json2f.h"

#define FILENAME "./test.json"

int main(int argc, char **argv) {
    json2f_t jd;

    FILE *fd = fopen(FILENAME, "w");
    if (fd == NULL) {
        printf("Failed to open %s\n", FILENAME);
        return -1;
    }

    json2f_init(&jd, fd);

    json2f_obj(&jd);
        json2f_str(&jd, "key", "val");
        json2f_int(&jd, "int", 11);
        json2f_long(&jd, "long", -1L);
        json2f_bool(&jd, "boolT", 1);
        json2f_bool(&jd, "boolF", 0);
        json2f_namedarr(&jd, "emptyarr");
		    // empty array
        json2f_arr_end(&jd);
        json2f_namedarr(&jd, "arraY");
            int long il= 121231313L;
            json2f_arr_long(&jd, &il, 1);
            char *sp = "arr val 1";
            json2f_arr_str(&jd, &sp, 1);
            int i = -190;
            json2f_arr_int(&jd, &i, 1);
            json2f_obj(&jd);
                json2f_int(&jd, "nested.2", 2);
                json2f_str(&jd, "nested.1", "one");
            json2f_obj_end(&jd);
            json2f_arr(&jd);
                int x[] = {1, 2, 3};
                json2f_arr_int(&jd, x, 3);
            json2f_arr_end(&jd);

        json2f_arr_end(&jd);

    json2f_obj_end(&jd);


    if(json2f_geterr(&jd) != 0) {
        printf("Object generation error\n");
    }
    fclose(fd);

    if (system("python -m json.tool " FILENAME) != 0 ) {
        printf("Fail to execute python -m json.tool\n");
    }

    if (argc == 1) {
        printf("unlink %s\n", FILENAME);
        unlink(FILENAME);
    }

    return 0;
}
