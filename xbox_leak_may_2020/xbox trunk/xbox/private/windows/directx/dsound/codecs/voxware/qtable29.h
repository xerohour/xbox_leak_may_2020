#ifndef QTABLE29_H
#define QTABLE29_H

unsigned short voxQuanFillLsfTableRT29(const float ***phLsfTable);
unsigned short voxQuanFreeLsfTableRT29(const float ***phLsfTable);
unsigned short voxQuanFillAmpTableRT29(const float ****phAmpTable);
unsigned short voxQuanFreeAmpTableRT29(const float ****phAmpTable);
const int* voxQuanGetLevelsRT29(void);
const float* voxQuanGetPvTableRT29(void);
const float* voxQuanGetEnergyTableRT29(void);

#endif /* QTABLE29_H */


