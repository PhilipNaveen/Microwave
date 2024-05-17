%{
#include "microwave.tab.h"
%}

%%

mmm     { return mmm; }
mMm     { return mMm; }
mmM     { return mmM; }
mMM     { return mMM; }
Mmm     { return Mmm; }
MMm     { return MMm; }
MmM     { return MmM; }
MMM     { return MMM; }
beep    { return beep; }
Beep    { return Beep; }
beeP    { return beeP; }
BEEP    { return BEEP; }
\n      { /* ignore newlines */ }
[ \t]   { /* ignore whitespace */ }
.       { /* ignore unrecognized characters */ }

%%

int yywrap(void) {
    return 1;
}
