#ifndef CATFLAGS_H
#define CATFLAGS_H

void CatSetTable(const char *table[static 256]);
void CatSetEnd(const char *table[static 256]);
void CatSetTAB(const char *table[static 256]);
void CatSetNonPrintable(const char *table[static 256]);

#endif