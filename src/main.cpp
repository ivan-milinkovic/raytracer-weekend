#include <iostream>
#include "rwmain.h"
#include "stdout_redir.h"

int main(int argc, const char * argv[]) {
    redirect_stdout();
    rwmain();
}
