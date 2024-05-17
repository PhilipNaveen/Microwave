%{
#include "microwave.tab.hpp"
%}

%option noyywrap

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
[ \t]   ; // Ignore whitespace
\n      ; // Ignore newlines
.       ; // Ignore unrecognized characters
%%

