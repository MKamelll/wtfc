#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<assert.h>
#include<stdbool.h>

#define USAGE "usage: wtf <Bin name>"

#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"

FILE * run_command(const char * cmd, const char * bin, size_t cmd_len, size_t bin_len)
{
    const char * redirect = " 2>&1 ";
    size_t max_len = cmd_len + bin_len + strlen(redirect) + 1;
    char cmd_with_redirected_error[max_len];

    memset(cmd_with_redirected_error, 0, max_len * sizeof(char));
    
    strncpy(cmd_with_redirected_error, cmd, cmd_len);
    strncat(cmd_with_redirected_error, bin, bin_len);
    strncat(cmd_with_redirected_error, redirect, strlen(redirect));
    
    FILE *fp = popen(cmd_with_redirected_error, "r");

    if (fp == NULL)
    {
        fprintf(stderr, "%s%s%s\n", "Couldn't run ", cmd, " command");
        exit(1);
    }

    return fp;
}

void print_cmd(const char * cmd, const char * bin_name)
{
    
    char * line = NULL;
    size_t line_len = 124;

    FILE * fp = run_command(cmd, bin_name, strlen(cmd), strlen(bin_name));
   
    assert(fp != NULL);
    
    printf(ANSI_COLOR_GREEN);
    printf("* %s", cmd);
    printf(ANSI_COLOR_RESET);
    printf("=> \n");
 
    while (getline(&line, &line_len, fp) > 0) {
        
        assert(line != NULL);
        if (strlen(line) != 0) {
            printf("\t%s", line);
        }

    }

    pclose(fp);

    if (line) free(line);
}

void print_description_from_man(const char * bin_name)
{

    const char * cmd = "man ";
    
    char * line = NULL;
    size_t line_len = 124;


    FILE * fp = run_command(cmd, bin_name, strlen(cmd), strlen(bin_name));
    
    assert(fp != NULL);
    
    printf(ANSI_COLOR_GREEN);
    printf("* Description");
    printf(ANSI_COLOR_RESET);
    printf(" => \n");

    const char * description = "DESCRIPTION";
    size_t offset = strlen(description) + 1;
    bool found_man_page = false;

    while (getline(&line, &line_len, fp) > 0) {
        if (strstr(line, description) != NULL) {
            fseek(fp, offset, SEEK_CUR);
            found_man_page = true;
            break;
        }
    }

    if (line) free(line);

    line = NULL;

    if (!found_man_page) {
        printf("\tNo manual entries for %s\n", bin_name);
        return;
    }
       
    while (getline(&line, &line_len, fp) > 0) {

        if (strlen(line) <= 1) {
            break;
        }
       
       printf("%s", line);

    }

    printf("\n");

    if (line) free(line);

    pclose(fp);

}

int main(int argc, char * argv[])
{
    if (argc < 2 || argc > 2) {
        fprintf(stderr, "%s\n", USAGE);
        exit(1);
    }

    if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--h") == 0) {
        printf("%s\n", USAGE);
        exit(0);
    }

    const char * bin_name = argv[1];
    
    const char * cmd_which = "which ";
    const char * cmd_where = "whereis ";

    
    print_cmd(cmd_which, bin_name);
    print_cmd(cmd_where, bin_name);
    print_description_from_man(bin_name);
 
    return 0;
}
