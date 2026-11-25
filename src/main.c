#define CCLI colc

#define CCLI_POS_ARGS(X, Y) \
    X(STRING, input,  "input",   "Input file",  "Path to the source file to process") \
    X(STRING, output, "output",  "Output file", "Path to the generated output file") \
    Y(8, STRING, files,  "file", "Extra files", "Additional input files to bundle")

#define CCLI_OPT_ARGS(X, Y) \
    X(STRING, out_dir, ".", Y("o", "out", "dir"), "Output dir", "Directory for generated files") \
    X(STRING, format,  "json", Y("f", "format"), "Format",      "Output format: json, yaml, toml") \
    X(UINT,   depth,   4,   Y("d", "depth"), "Max depth",       "Maximum recursion depth") \
    X(UINT,   limit,   100, Y("l", "limit"), "Limit",           "Maximum number of items to process")

#define CCLI_BOL_ARGS(X, Y) \
    X(help,      FALSE, Y("h", "help"),      "Help",      "Show this help message") \
    X(version,   FALSE, Y("v", "version"),   "Version",   "Show version information") \
    X(force,     FALSE, Y("F", "force"),     "Force",     "Overwrite existing output files") \
    X(recursive, FALSE, Y("r", "recursive"), "Recursive", "Process directories recursively") \
    X(verbose,   FALSE, Y("V", "verbose"),   "Verbose",   "Print detailed processing info")

#include "coloncli.h"
#include <stdio.h>

static void print_config(colc_args_t* cli) {
    char buf[4096];
    unsigned int n = colc_struct_str(cli, buf, sizeof(buf));
    fprintf(stdout, "Configuration:\n%.*s\n", n, buf);
}

int main(int argc, char** argv) {
    colc_args_t cli = colc_default();

    char err[4096];
    char buf[4096];
    unsigned int n;

    n = colc_parse(&cli, err, sizeof(err), argc, argv);
    if (n) {
        fprintf(stderr, "%.*s\n", n, err);
        return 2;
    }

    if (cli.help) {
        n = colc_help_str(buf, sizeof(buf));
        fprintf(stdout, "%.*s\n", n, buf);
        return 0;
    }

    if (cli.version) {
        fprintf(stdout, "colc 1.0.0 - Colon CLI Argument Parser\n");
        return 0;
    }

    if (!cli.ok) {
        return 1;
    }

    if (cli.input.len == 0) {
        fprintf(stderr, "Error: missing required positional argument 'input'\n");
        n = colc_help_str(buf, sizeof(buf));
        fprintf(stderr, "%.*s\n", n, buf);
        return 2;
    }

    print_config(&cli);

    if (cli.verbose) {
        fprintf(stdout, "[verbose] input=%.*s output=%.*s files=%u\n",
            (int)cli.input.len, cli.input.data,
            (int)cli.output.len, cli.output.data,
            cli.files.count);
    }

    for (unsigned int i = 0; i < cli.files.count; ++i) {
        if (cli.verbose) {
            fprintf(stdout, "[verbose] extra file %u: %.*s\n",
                i, (int)cli.files.items[i].len, cli.files.items[i].data);
        }
    }

    fprintf(stdout, "Processing complete: %.*s -> %.*s/%.*s\n",
        (int)cli.input.len, cli.input.data,
        (int)cli.out_dir.len, cli.out_dir.data,
        (int)cli.output.len, cli.output.data);

    return 0;
}
